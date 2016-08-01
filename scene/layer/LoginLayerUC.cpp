/******************************************************
*文件名称:	LoginLayerUC.cpp
*编写日期:	2016-6-29-22:35
*编写作者:	YPF
*功能描述:	UC登录
*******************************************************/

#include "LoginLayerUC.h"
#include "Global.h"
#include "tools/ToolDefine.h"
#include "model/DataCenter.h"
#include "scene/layer/WaitLayer.h"
#include "tools/ShowTexttip.h"
#include "common/color.h"
#include "scene/CPopTip.h"
#include "scene/layer/LayerManager.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "common/PixelSprite.h"
#include "tools/commonDef.h"
#include "BackLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "update/CUpdateLayer.h"
#include "jsonCpp/json.h"
#include "UserDefaultData.h"
#include "update/CDownloadPackage.h"
#include "ApplicationDefine.h"
#include "Battle/AnimationManager.h"
#include "Resources.h"
#include "common/RollLabel.h"
#include "tools/ShowTexttip.h"
#include "scene/LoginScene.h"
#include "Battle/EffectObject.h"
#include "update/CUpdateData.h"
#include "model/DataCenter.h"
#include "ApplicationDefine.h"
#include "SDK/GamePlatformManager.h"

using namespace BattleSpace;

LoginLayerUC::LoginLayerUC():
	m_ui(nullptr),
	m_pLoginLay(nullptr),
	m_armature(nullptr),
	m_LayoutRoot(nullptr),
	m_iCurrentType(0),
	m_iSelectServerIndex(0),
	m_pTableView(nullptr),
	m_pLoading(nullptr),
	m_pCell(nullptr),
	m_pCurrentServerUI(nullptr)
{

}


LoginLayerUC::~LoginLayerUC()
{
	CC_SAFE_RELEASE(m_pCell);
}


bool LoginLayerUC::init()
{
	BaseLayer::init();

	m_ui = nullptr;
	m_iCurrentType = LoginResponseMsg;
	m_LayoutRoot = CWidgetWindow::create();
	m_LayoutRoot->setTag(10000);
	this->addChild(m_LayoutRoot);
	CCLayerColor* pColor = CCLayerColor::create(ccc4(255, 255, 255, 255));
	pColor->setContentSize(WINSIZE);
	pColor->setPosition(CCPointZero);
	m_LayoutRoot->addChild(pColor, -1);

	//加载
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("loginScene/opening.ExportJson");
	//生成骨骼
	m_armature = CCArmature::create("opening");
	m_armature->setPosition(VCENTER);
	m_LayoutRoot->addChild(m_armature);
	//绑定动画事件
	m_armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(LoginLayerUC::movementCallBack));
	m_armature->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(LoginLayerUC::frameEventCallBack));
	m_armature->getAnimation()->playWithIndex(0);

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.8f), CCCallFunc::create(this, callfunc_selector(LoginLayerUC::callBackForSoundUp))));

	return true;
}

void LoginLayerUC::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
{
	switch (type)
	{
	case cocos2d::extension::START:
		break;
	case cocos2d::extension::COMPLETE:
		{
			//动作1
			if(strcmp(sData, "opening")==0)
			{
				CCLOG("LoginLayerUC::movementCallBack::opening");

				m_pLoading->setVisible(true);

				//暂时不需要
				//showLabelRoll(GETLANGSTR(1244));

				//拉取服务器列表
				if(CJniHelper::getInstance()->checkNetWork())
				{
					LinkedServerListNet(); 
				}
				else
				{
					ShowPopTextTip(GETLANGSTR(1189));
				}
				schedule(schedule_selector(LoginLayerUC::scheduleForConnectGateServer), 5.0f);

			}
			else if(strcmp(sData, "enter")==0)//动作2
			{
				//执行动作
				//保存游戏服务器名称
				const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
				CCUserDefault::sharedUserDefault()->setStringForKey(SERVER_FOR_GAME, gameServer.server_name());
				CCUserDefault::sharedUserDefault()->flush();

				m_LayoutRoot->setVisible(false);
				hideLableRoll();
				this->runAction(CCCallFunc::create(this, callfunc_selector(LoginLayerUC::callBackInGame)));
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}


void LoginLayerUC::initLogin()
{
	CCLOG("LoginLayerUC::initLogin");

	if(m_ui!=nullptr)
	{
		return;
	}

	//播放背景音乐
	PlayBackgroundMusic(BGM_Login,true);

	m_ui = LoadComponent("EnterGameUC.xaml");
	m_ui->setTag(1);
	m_ui->setPosition(VCENTER);
	m_LayoutRoot->addChild(m_ui, 2);

	//自己的登陆框隐藏
	m_pLoginLay = (CLayout *)m_ui->getChildByTag(10);
	m_pLoginLay->setVisible(false);

	//选择好的服务器列表
	m_pCurrentServerUI = (CLayout *)m_ui->findWidgetById("current_server");
	m_pCurrentServerUI->setVisible(true);


	//默认服务器
	updateSelectServer();

	//服务器选区
	CLayout* pLayout = (CLayout*)m_ui->findWidgetById("serverCell");
	pLayout->retain();
	m_ui->removeChild(pLayout);
	m_pCell = pLayout;

	//获取空的滑动列表Scroll
	m_pTableView = (CTableView*)m_ui->findWidgetById("serverScroll");
	m_pTableView->setDirection(eScrollViewDirectionVertical);
	m_pTableView->setSizeOfCell(m_pCell->getContentSize());
	m_pTableView->setCountOfCell(m_serverInfo.server_list().size());
	m_pTableView->setBounceable(true);
	m_pTableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(LoginLayerUC::tableviewDataSource));
	m_pTableView->reloadData();


	//按钮打开服务器列表
	CImageView* pBtnServer = (CImageView*)m_ui->findWidgetById("serverBtn");


	//快速登录按钮
	CButton * pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setOnClickListener(this,ccw_click_selector(LoginLayerUC::onQuickLogin));
	CCActionInterval *scaleSmall = CCScaleTo::create(0.9f,0.98f);
	CCFadeTo *out = CCFadeTo::create(0.9f,60);
	CCSpawn *spawn = CCSpawn::createWithTwoActions(scaleSmall,out);
	CCActionInterval *scaleBig = CCScaleTo::create(1.2f,1.0f);
	CCFadeTo *in = CCFadeTo::create(1.2f,255);
	CCSpawn *spawn1 = CCSpawn::createWithTwoActions(scaleBig,in);
	CCSequence *seque = CCSequence::createWithTwoActions(spawn,spawn1);
	CCRepeatForever *action = CCRepeatForever::create(seque);
	pQuickLogin->runAction(action);
	pQuickLogin->setVisible(true);

	//清除用户信息
	DataCenter::sharedData()->getUser()->clear();

	//版本做相对位置处理
	CLabel* pLabel = (CLabel *)m_ui->findWidgetById("version");
	pLabel->setAnchorPoint(ccp(1.0f, 0.0f));
	pLabel->setPosition(ccp(VRIGHT-20, VBOTTOM+20));
	CC_SAFE_RETAIN(pLabel);
	pLabel->removeFromParentAndCleanup(true);
	this->addChild(pLabel);
	CC_SAFE_RELEASE(pLabel);
	pLabel->setString(CCString::createWithFormat("Version:%s", m_serverInfo.game_version().c_str())->getCString());


	//隐藏服务器列表
	showServerList();
	hideServerList();


	//TODO
	//初始化UC_SDK？-放在最前面
	//还是点击登录再初始化？
	//点击登录再初始化吧
	//初始化完直接请求登录


	//先连接连接服务器
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);

	//检查服务器是否可用
	if(!checkServerStatus(gameServer))	
	{
		return;
	}

	LinkedGameServer(gameServer);
}


void LoginLayerUC::removeLogin()
{
	m_ui->removeFromParentAndCleanup(true);
	m_ui = nullptr;
}

void LoginLayerUC::onExit()
{
	BaseLayer::onExit();

	//释放骨骼，释放图片
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("LoginLayerUC/opening.ExportJson");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CNetClient::getShareInstance()->unRegisterMsgHandler(this);

	NOTIFICATION->removeAllObservers(this);

	CJniHelper::getInstance()->setJniListener(nullptr);
}

void LoginLayerUC::onGameExit(CCObject* obj)
{
	CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
	CCDirector::sharedDirector()->end();
}


void LoginLayerUC::onQuickLogin(CCObject* obj)
{
	PlayEffectSound(SFX_LoginClick);

	//如果输入框显示状态，不快速login
	if(m_pLoginLay->isVisible())
	{
		return;
	}

	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginLayerUC::reConnectLogin),0.5);
	}
	else
	{
		linkedGameServerSuccess();
	}
}

void LoginLayerUC::reConnectLogin(float dt)
{
	linkedGameServerSuccess();
	this->unschedule(schedule_selector(LoginLayerUC::reConnectLogin));
}


void LoginLayerUC::linkedGameServerSuccess()
{
	//TODO
	//在这里修改登录
	click_login(nullptr);
	//loginMyGame(nullptr);
}

bool LoginLayerUC::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!m_ui)return false;

	CCPoint pos = convertToNodeSpace(pTouch->getLocation());

	if(m_pTableView->isVisible())
	{
		return false;
	}

	//按钮
	CImageView* pBtnServer = (CImageView*)m_ui->findWidgetById("serverBtn");
	if(pBtnServer->boundingBox().containsPoint(pos) && m_pTableView->getCountOfCell()>1)
	{
		showServerList();
		return true;
	}

	return false;
}

//连接网关服务器成功后,服务器自动推送消息，返回网关服务器列表
bool LoginLayerUC::ProcessMsg(int type, google::protobuf::Message *msg)
{
	m_iCurrentType = type;
	switch (type)
	{
	case ServerListMsg:
		{
			unschedule(schedule_selector(LoginLayerUC::scheduleForConnectGateServer));

			CCLOG("ServerListMsg");
			ServerList* serverInfo = (ServerList*)msg;
			m_serverInfo = *serverInfo;
			m_pLoading->setVisible(false);

			//没有服务器列表弹框提示
			if(m_serverInfo.server_list().size()<=0)
			{
				ShowPopTextTip(GETLANGSTR(207));
				return false;
			}

			//读取上一次登录选择的服务器
			std::string sServerGame = CCUserDefault::sharedUserDefault()->getStringForKey(SERVER_FOR_GAME);
			m_iSelectServerIndex = getServerIndex(sServerGame);


			//读取更新服务器列表
			std::string sUpdateServer = serverInfo->update_server();
			if(sUpdateServer.compare("") != 0)
			{
				CCUserDefault::sharedUserDefault()->setStringForKey(SERVER_FOR_UPDATE, sUpdateServer);
				CCUserDefault::sharedUserDefault()->flush();
			}

			//版本更新
			bool bUpdate = GamePlatformMgr->VersionUpdateWithPlatform( serverInfo->game_version() );
			if(!bUpdate)
			{
				initLogin();
			}
		}
		break;
	case LoginResponseMsg:
		{
			LoginResponse *loginResponse = (LoginResponse *)msg;
			printf("%d,%d,",loginResponse->authrecode(),loginResponse->hasrole());
			printf("\n name = %s ,...", loginResponse->GetDescriptor()->name().c_str());
			int res =loginResponse->authrecode();			//登录成功,
			if (res==0)
			{
				if (loginResponse->hasrole())
				{
					for (int i=0; i<loginResponse->rolelist_size();i++)
					{
						protos::common::Role role = loginResponse->rolelist(i);
						printf("role id:%d  rolename:%s, rolelv: %d", role.roleid(), U8(role.rolename().c_str()),role.rolelv());
						CNetClient::getShareInstance()->sendInGameReq(role.roleid());
						break;
					}
				}
				else
				{
					m_armature->getAnimation()->play("enter");
					this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginLayerUC::callbackForSound))));
					removeLogin();
				}
			}
			//0x01服务器人数上限，0x02账号禁止登录，0x03账号验证错误)
			else if (res==1)
			{
				ShowTexttip(GETLANGSTR(227),RGB_RED,0,ccp(VCENTER.x,VCENTER.y+150),0.0f,2.0f,0.0f,20.0f);
			}
			else if (res==2)
			{
				ShowTexttip(GETLANGSTR(228),RGB_RED,0,ccp(VCENTER.x,VCENTER.y+150),0.0f,2.0f,0.0f,20.0f);
			}
			else if (res==3)
			{
				ShowTexttip(GETLANGSTR(165),RGB_RED,0,ccp(VCENTER.x,VCENTER.y+150),0.0f,2.0f,0.0f,20.0f);
			}
			else if(res==4)
			{
				ShowTexttip(GETLANGSTR(1155),RGB_RED,0,ccp(VCENTER.x,VCENTER.y+150),0.0f,2.0f,0.0f,20.0f);
			}
			return true;
		}
		break;
		//进入游戏
	case InGameResponseMsg:
		{
			InGameResponse *inRespon = (InGameResponse*)msg;
			protos::common::Role role = inRespon->myrole();
			printf("role id:%d  rolename:%s, rolelv: %d", role.roleid(), U8(role.rolename().c_str()),role.rolelv());

			UserData* userData = DataCenter::sharedData()->getUser()->getUserData();
			userData->setRaceType(role.rolenation());
			userData->setAliveID(role.roleid());
			userData->setRoleID(role.roleid());
			userData->setLevel(role.rolelv());
			userData->setName(role.rolename().c_str());
			userData->setCoin(role.rolecoin());
			userData->setExp(role.roleexp());
			userData->setNextExp(role.nextexp());
			userData->setRoleAction(role.roleaction());
			userData->setRoleGold(role.rolegold());
			userData->read(role);
			m_armature->getAnimation()->play("enter");
			this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginLayerUC::callbackForSound))));
			removeLogin();
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void LoginLayerUC::onEnter()
{
	BaseLayer::onEnter();

	CNetClient::getShareInstance()->registerMsgHandler(this);

	CJniHelper::getInstance()->setJniListener(this);

	NOTIFICATION->addObserver(this, callfuncO_selector(LoginLayerUC::loginMyGame), "UC_LOGIN_SUCCESS", nullptr);

	initLoadingEffect();
}

void LoginLayerUC::callBackInGame()
{
	switch (m_iCurrentType)
	{
	case LoginResponseMsg://首次登陆，进引导
		{
			//						//此处拉0关卡信息
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			CPlayerControl::getInstance().sendEnterStage(0,0,0,0, 0);		//新手引导关卡
#else
			CPlayerControl::getInstance().sendEnterStage(0,0,0,0, 0);		//新手引导关卡
#endif
			//CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(SelectHeroScene));
		}break;
	case InGameResponseMsg://旧账号登陆，进主城
		{
#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
			////战斗引导完了，弹框告知是否下载更新包
			if(CDownloadPackage::checkIsNeedPackage())
			{
				CPopTip *pop = CPopTip::create();
				LayerManager::instance()->pop();
				pop->setVisible(true);
				pop->addContentTip(GETLANGSTR(1156));
				pop->setTouchEnabled(true);
				pop->setTouchPriority(-100);
				pop->setButtonLisener(this, ccw_click_selector(LoginLayerUC::onClickDownloadPackage));
				CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 899);
				return;
			}
#endif
			CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
		}
		break;
	default:
		break;
	}
}


void LoginLayerUC::jniResult( int nReqCode, int nResult )
{
	if(nReqCode==SHOW_VIDEO)
	{
		if(nResult==0)
		{
			LGResume(this);
			CCLog("LoginLayerUC::jniResult----LGResume(this);");
			//CCDirector::sharedDirector()->resume();
		}
	}
}


void LoginLayerUC::frameEventCallBack( CCBone* pBone, const char* sEvent, int iTag1, int iTag2 )
{
	if(strcmp(sEvent, "video") == 0)
	{
		showVideo();
		return;
	}
	if(strcmp(sEvent, "545") == 0)
	{
		PlayEffectSound(SFX_545);
		return;
	}
	if(strcmp(sEvent, "546") == 0)
	{
		PlayEffectSound(SFX_546);
		return;
	}
}

void LoginLayerUC::showVideo()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CJniHelper::getInstance()->showVideo(0);
	LGPause(this);
	//CCDirector::sharedDirector()->pause();
	//runAction(CCCallFunc::create(CCDirector::sharedDirector(), callfunc_selector(CCDirector::pause)));
#endif
}

void LoginLayerUC::scheduleForConnectGateServer( float dt )
{
	//拉取服务器列表
	if(CJniHelper::getInstance()->checkNetWork())
	{
		LinkedServerListNet();
	}
	else
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		ShowPopTextTip(GETLANGSTR(1189));
	}
}


void LoginLayerUC::LinkedGameServer(const Server& server)
{
	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->setAddress(server.server_host().c_str(), server.server_port(),true);
	if (!tcp->isConnected()&&!tcp->getIsCloseConn())
	{
		tcp->connect();
	}
}

//连接网关服务器，连接成功后，网关服务器会自动推送服务器列表的消息过来
void LoginLayerUC::LinkedServerListNet()
{
#if DIRECT_CONNECT_GAMESERVER
	directConnectGameServer();
	return;
#endif
	GateServer gateServer = LoginScene::getGateServer();
	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->setAddress(gateServer.sHost.c_str(), gateServer.iPort,false);
	if (!tcp->isConnected())
	{
		tcp->connect();
	}
}

void LoginLayerUC::onClickDownloadPackage( CCObject* pSender )
{
	CButton* pBtn = (CButton*)pSender;

	if(pBtn->getTag()==PopTipConfirm)
	{
		CDownloadPackage* pLayer = CDownloadPackage::create();
		pLayer->setDownloadPacakgeDelegate(this);
		CCDirector::sharedDirector()->getRunningScene()->addChild(pLayer, 1147);
		pLayer->downLoadPackage();
	}
	else
	{
		//不更新，重启游戏，重复引导
		//CJniHelper::getInstance()->restartGame();
		CCDirector::sharedDirector()->end();
	}

	((CPopTip*)(pBtn->getParent()->getParent()))->removeFromParentAndCleanup(true);
}

void LoginLayerUC::downloadPackageEnd( bool bAnswer )
{
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
}

void LoginLayerUC::callbackForSound()
{
	return;
	PlayEffectSound(SFX_539);
}

void LoginLayerUC::callBackForSoundUp()
{
	return;
	PlayEffectSound(SFX_541);
}

int LoginLayerUC::getServerIndex( const std::string& sName )
{
	int iSize = m_serverInfo.server_list().size();
	for(int i=0; i<iSize; i++)
	{
		const Server& server = m_serverInfo.server_list().Get(i);
		if(server.server_name().compare(sName.c_str())==0)
		{
			return i;
		}
	}
	//没有的话默认选取第一个
	return NULL;
}

void LoginLayerUC::updateSelectServer()
{
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	CLabel* pServerSelect = (CLabel*)m_pCurrentServerUI->findWidgetById("server_select");
	pServerSelect->setString(gameServer.server_name().c_str());
	CLabel* pServerSelectStatus = (CLabel*)m_pCurrentServerUI->findWidgetById("status_select");
	int iShowText[5] = {2002, 1184, 1185, 1186, 1187};
	pServerSelectStatus->setString(GETLANGSTR(iShowText[gameServer.server_status()]));

	UCSdk::s_serverid = 0;
	UCSdk::s_servername = gameServer.server_name();
}

CCObject* LoginLayerUC::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

void LoginLayerUC::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
{
	CLayout *lay = UICloneMgr::cloneLayout(m_pCell);

	const Server& server = m_serverInfo.server_list().Get(uIdx);

	for (unsigned int i=1; i<=4; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		child->setTag(i);
		pCell->addChild(child);
		pCell->setTag(uIdx);

		switch (i)
		{
		case 1:
			{
				CImageView* pBoard = (CImageView*)child;
				pBoard->setTouchEnabled(true);
				pBoard->setOnClickListener(this, ccw_click_selector(LoginLayerUC::selectServer));
			}
			break;
		case 3:
			{
				CLabel* pLabel = (CLabel*)child;
				pLabel->setString(server.server_name().c_str());
			}
			break;
		case 4:
			{
				CLabel* pLabel = (CLabel*)child;
				int iShowText[5] = {2002, 1184, 1185, 1186, 1187};
				pLabel->setString(GETLANGSTR(iShowText[server.server_status()]));
			}
			break;
		default:
			break;
		}
	}
}

void LoginLayerUC::selectServer( CCObject* pObj )
{
	PlayEffectSound(SFX_Button);
	CImageView* pImageView = (CImageView*)pObj;
	if(m_iSelectServerIndex != pImageView->getParent()->getTag())
	{
		m_iSelectServerIndex = pImageView->getParent()->getTag();

		CNetClient *tcp = CNetClient::getShareInstance();
		if (tcp->isConnected())
		{
			tcp->close();
		}
	}

	updateSelectServer();
	hideServerList();
}

void LoginLayerUC::showServerList()
{
	m_pTableView->reloadData();
	m_pTableView->setVisible(true);
	CButton* pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setVisible(false);
	m_pCurrentServerUI->setVisible(false);
}

void LoginLayerUC::hideServerList()
{
	m_pTableView->setVisible(false);
	CButton* pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setVisible(true);
	m_pCurrentServerUI->setVisible(true);
}

void LoginLayerUC::initLoadingEffect()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/loading.plist");
	AnimationManager::sharedAction()->ParseAnimation("loading");

	//loading贴图
	EffectObject* pLoadEffect = EffectObject::create("loading", BattleSpace::sPlayType::eRepeat);
	pLoadEffect->setPosition(VCENTER);
	pLoadEffect->setEffAnchorPoint(0.5f,0.5f);
	pLoadEffect->setScale(0.6f);
	this->addChild(pLoadEffect, 999);
	m_pLoading = pLoadEffect;
	m_pLoading->setVisible(false);
}

void LoginLayerUC::directConnectGameServer()
{
	CCLOG("directConnectGameServer");

	m_pLoading->setVisible(false);

	Server serverBase;
	serverBase.set_server_host("103.31.20.79");
	serverBase.set_server_port(5204);
	Server* server = m_serverInfo.add_server_list();
	server->CopyFrom(serverBase);

	initLogin();
}

void LoginLayerUC::showLabelRoll( const char* sInfo )
{
	if(CheckDay(LoginDayUC))
	{
		return;
	}

	this->removeChildByTag(88888);
	CCNode *rollNode = CCNode::create();
	this->addChild(rollNode,0,88888);
	CCSprite *rollBg = CCSprite::create("mainCity/bulletin-board.png");
	rollBg->setPosition(ccp(VCENTER.x,VCENTER.y+200));
	rollBg->setTag(1200);
	rollNode->addChild(rollBg);
	CRollLabel *rollLab = CRollLabel::create(sInfo, "Arial",20,
		CCRectMake(rollBg->getPositionX()-rollBg->getContentSize().width/2,rollBg->getPositionY()-rollBg->getContentSize().height/2+8,rollBg->getContentSize().width,rollBg->getContentSize().height));
	rollLab->setRollTime(20);
	rollNode->addChild(rollLab);

	schedule(schedule_selector(LoginLayerUC::updateForCheckDay), 1.0f);
}

void LoginLayerUC::hideLableRoll()
{
	this->removeChildByTag(88888);
}

void LoginLayerUC::updateForCheckDay( float dt )
{
	if(CheckDay(LoginDayUC))
	{
		hideLableRoll();
		unschedule(schedule_selector(LoginLayerUC::updateForCheckDay));
	}
}

bool LoginLayerUC::checkServerStatus(const Server &server)
{
	switch (server.server_status())
	{
		//未开启状态
		case 0:
			{
				tm  tm ;  
				time_t  timep = server.open_time()/1000;
				tm  = *localtime(&timep);  
				int  year =  tm.tm_year + 1900;  
				int  month =  tm.tm_mon + 1;  
				int  day =  tm.tm_mday;  
				int  hour= tm.tm_hour;  
				int minute = tm.tm_min;
				int second = tm.tm_sec;

				CCString* sDay = CCString::createWithFormat("%d/%s%d/%s%d-%s%d:%s%d:%s%d", 
					year, month>9?"":"0", month, day>9?"":"0", day, hour>9?"":"0", hour, minute>9?"":"0", minute, second>9?"":"0", minute);

				CCString* pStr = CCString::createWithFormat(GETLANGSTR(2001), sDay->getCString());
				ShowPopTextTip(pStr->getCString());
				return false;
			}break;
		case 4:
			{
				//服务器是维护状态，不连接，给提示
				ShowPopTextTip(GETLANGSTR(1243));
				return false;
			}break;	
	default:
		break;
	}

	return true;
}



void LoginLayerUC::uc_sdk_init()
{
	if(UCSdk::s_inited)
	{
		CCLOG("has init");
	}else{
		CUCGameSdk::setLogLevel(UCSdk::s_loglevel); CCLOG("1");
		CUCGameSdk::setOrientation(UCSdk::s_orientation);
		CUCGameSdk::setLoginUISwitch(UCSdk::s_loginUISwitch);
		CUCGameSdk::setLogoutCallback(UCSdkCallback::logout_callback);
		CUCGameSdk::initSDK(UCSdk::s_debugMode, UCSdk::s_loglevel,UCSdk::s_cpid, UCSdk::s_gameid, UCSdk::s_serverid,UCSdk::s_servername.c_str(), UCSdk::s_enablePayHistory,UCSdk::s_enableLogout, UCSdkCallback::init_callback);
	}
}

bool LoginLayerUC::check_login_status()
{
	if(!UCSdk::s_logined)
	{
		CUCGameSdk::login(false, "", UCSdkCallback::login_callback,UCSdkCallback::gameUserLogin_callback); //此方法可支持UC账号和游戏老账号登录
		return false;
	}
	return true;
}


void LoginLayerUC::click_enter_uc(CCObject* pSender){
	CCLOG("enterUserCenter");
	if(check_login_status())
	{
		CUCGameSdk::enterUserCenter(UCSdkCallback::userCenter_callback);
	}

}

void LoginLayerUC::click_hide_fb(CCObject* pSender){
	CCLOG("hide float button");
	CUCGameSdk::showFloatButton(0, 0, false);
}

void LoginLayerUC::click_logout(CCObject* pSender){
	CCLOG("logout");
	if(check_login_status()){
		CUCGameSdk::logout();
		ShowPopTextTip("has logout!");
	}
}

void LoginLayerUC::click_login(CCObject* pSender){
	CCLOG("login");
	if(check_login_status())
	{
		//重新发自家服务器
		updateForLogin(0);
		//ShowPopTextTip("has logined!");
	}

}

void LoginLayerUC::click_pay(CCObject* pSender){
	CCLOG("pay");
	if(check_login_status())
	{
		CUCGameSdk::pay(
			false, 
			0,
			UCSdk::s_serverid, 
			CCString::createWithFormat("%d", DataCenter::sharedData()->getUser()->getUserData()->getRoleID())->getCString(),	//角色标识
			DataCenter::sharedData()->getUser()->getUserData()->getName().c_str(),																		//姓名
			CCString::createWithFormat("%d", DataCenter::sharedData()->getUser()->getUserData()->getLevel())->getCString(),	//等级
			CCString::createWithFormat("%d", DataCenter::sharedData()->getUser()->getUserData()->getRoleID())->getCString(),	//自定义信息
			"http://103.31.20.79/payment/ucPay/",
			"10",
			UCSdkCallback::pay_callback);
	}
}

void LoginLayerUC::click_show_fb(CCObject* pSender){
	CCLOG("show floatbutton");
	CUCGameSdk::showFloatButton(100, 20, true);
}

void LoginLayerUC::click_submit_ed(CCObject* pSender){
	CCLOG("submit extend data");
	if(check_login_status()){
		// loginGameRole 类型必接（必须提交），其中，extData中除“roleLevelMTime”外的字段都必填
		std::string extData ="{\"roleId\":\"R0010\",\"roleName\":\"令狐一冲\",\"roleLevel\":\"99\",\"zoneId\":192825,\"zoneName\":\"游戏一区-逍遥谷\",\"roleCTime\":1456397360,\"roleLevelMTime\":1456397361}";
		CUCGameSdk::submitExtendData("loginGameRole", extData.c_str());
		ShowPopTextTip( "has submited!" );
	}
}

void LoginLayerUC::click_upoint(CCObject* pSender){
	CCLOG("uPoing charge");
	if(check_login_status()){
		CUCGameSdk::uPointCharge(UCSdkCallback::uPointCharge_callback);
	}
}

void LoginLayerUC::click_bbs(cocos2d::CCObject* pSender)
{
	CCLog("enter bbs");
	if(check_login_status())
	{
		CUCGameSdk::enterUI("bbs", UCSdkCallback::enterUI_callback);
	}
}

void LoginLayerUC::loginMyGame( CCObject* pObj )
{
	scheduleOnce(schedule_selector(LoginLayerUC::updateForLogin), 0.0f);
}

void LoginLayerUC::updateForLogin( float dt )
{
	LOGIN_GAME(UCSdk::s_sid);
	CCLOG("LoginLayerUC::updateForLogin( float dt )__________%s", UCSdk::s_sid.c_str());
}

void LoginLayerUC::exit_with_uc()
{
	CUCGameSdk::exitSDK(UCSdkCallback::exit_callback);
}

