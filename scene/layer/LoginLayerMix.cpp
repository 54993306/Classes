/******************************************************
*文件名称:	LoginLayerMix.cpp
*编写日期:	2016-6-29-18:19
*编写作者:	YPF
*功能描述:	facebook登录界面
*******************************************************/
#include "LoginLayerMix.h"
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
#include "SDK/FaceBookSDK.h"
#include "jsonCpp/json.h"
#include "UserDefaultData.h"
#include "update/CDownloadPackage.h"
#include "ApplicationDefine.h"
#include "Battle/AnimationManager.h"
#include "Battle/EffectObject.h"
#include "Resources.h"
#include "common/RollLabel.h"
#include "tools/ShowTexttip.h"
#include "../LoginScene.h"
#include "SDK/GamePlatformManager.h"
#include "SDK/GamePlatfomDefine.h"
#include "ApplicationDefine.h"
#include "SDK/GoogleLoginSDK.h"

using namespace BattleSpace;

LoginLayerMix::LoginLayerMix():
	m_ui(nullptr),
	m_pArmature(nullptr),
	m_LayoutRoot(nullptr),
	m_iCurrentType(0),
	m_iSelectServerIndex(0),
	m_pTableView(nullptr),
	m_pLoading(nullptr),
	m_pCell(nullptr),
	m_pLoginSelectLay(nullptr),
	m_pEnterGameLay(nullptr),
	m_iLoginType(LoginTypeNull)
	,m_pClose(nullptr)
{
	m_FaceBookUserID = CCUserDefault::sharedUserDefault()->getStringForKey(FACEBOOK_ID);
	//m_FaceBookUserID = "688438407925979";
}

LoginLayerMix::~LoginLayerMix()
{
	CC_SAFE_RELEASE(m_pCell);
}


bool LoginLayerMix::init()
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
	m_pArmature = CCArmature::create("opening");
	m_pArmature->setPosition(VCENTER);
	m_LayoutRoot->addChild(m_pArmature);
	//绑定动画事件
	m_pArmature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(LoginLayerMix::movementCallBack));
	m_pArmature->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(LoginLayerMix::frameEventCallBack));
	m_pArmature->getAnimation()->playWithIndex(0);

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.8f), CCCallFunc::create(this, callfunc_selector(LoginLayerMix::callBackForSoundUp))));

	return true;
}

void LoginLayerMix::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
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
				CCLOG("LoginLayeFaceBook::movementCallBack::opening");

				m_pLoading->setVisible(true);

				showLabelRoll(GETLANGSTR(1244));

				//拉取服务器列表
				if (!CNetClient::getShareInstance()->isConnected())
				{
					if(CJniHelper::getInstance()->checkNetWork())
					{
						LinkedServerListNet(); 
					}
					else
					{
						ShowPopTextTip(GETLANGSTR(1189));
					}
					schedule(schedule_selector(LoginLayerMix::scheduleForConnect), 5.0f); 
				}
				else 
				{
					m_pLoading->setVisible(false);
					initUI();
				}
			}
			else if(strcmp(sData, "enter")==0)//动作2
			{
				//保存游戏服务器名称
				const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
				CCUserDefault::sharedUserDefault()->setStringForKey(SERVER_FOR_GAME, gameServer.server_name());
				CCUserDefault::sharedUserDefault()->flush();
				//存储登录方式
				CCUserDefault::sharedUserDefault()->setIntegerForKey(LOGIN_TYPE, m_iLoginType);
				//写入
				CCUserDefault::sharedUserDefault()->flush();

				m_LayoutRoot->setVisible(false);
				hideLableRoll();

				this->runAction(CCCallFunc::create(this, callfunc_selector(LoginLayerMix::callBackInGame)));
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}


void LoginLayerMix::initUI()
{
	CCLOG("LoginLayeFaceBook::initUI");

	if(m_ui!=nullptr)
	{
		return;
	}

	initLogin();

	BackLayer* bklayer = BackLayer::create();
	bklayer->setZOrder(10);
	this->addChild(bklayer);

	PlayBackgroundMusic(BGM_Login,true);

	DataCenter::sharedData()->getUser()->clear();
}


void LoginLayerMix::removeLogin()
{
	m_ui->removeFromParentAndCleanup(true);
	m_ui = nullptr;
}



bool LoginLayerMix::checkFacebookAuthor()
{
	if ( m_FaceBookUserID == "" )
	{
		return false;
	}
	return true;
}

void LoginLayerMix::onFacebookAuthor( )
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	FaceBookSDK::sharedInstance()->openAuthor();
#else
	m_FaceBookUserID = "688438407925979";
	CCUserDefault::sharedUserDefault()->setStringForKey(FACEBOOK_ID,m_FaceBookUserID);
	switchLayer();
#endif

}

void LoginLayerMix::onFacebookLogin( )
{
	if (!LinkedGameServer())
	{
		return;
	}

	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginLayerMix::doFacebookLogin),0.5);
	}
	else
	{
		GetTcpNet->sendLogin("", "", "", false, 0, m_FaceBookUserID, G_PLATFORM_TARGET);
		DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(true);
	}
}

void LoginLayerMix::doFacebookLogin(float delt)
{
	GetTcpNet->sendLogin("", "", "", false, 0, m_FaceBookUserID, G_PLATFORM_TARGET);
	DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(true);

	this->unschedule(schedule_selector(LoginLayerMix::doFacebookLogin));
}


bool LoginLayerMix::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
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
bool LoginLayerMix::ProcessMsg(int type, google::protobuf::Message *msg)
{
	m_iCurrentType = type;
	switch (type)
	{
		//网关信息
	case ServerListMsg:
		{
			unschedule(schedule_selector(LoginLayerMix::scheduleForConnect));

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
				initUI();
			}

		}
		break;
		//登录游戏
	case LoginResponseMsg:
		{
			LoginResponse *loginResponse = (LoginResponse *)msg;
			printf("%d,%d,",loginResponse->authrecode(),loginResponse->hasrole());
			printf("\n name = %s ,...", loginResponse->GetDescriptor()->name().c_str());
			int res =loginResponse->authrecode();			//登录成功,
			if (res==0)
			{
				//已有角色
				if (loginResponse->hasrole())
				{
					for (int i=0; i<loginResponse->rolelist_size();i++)
					{
						protos::common::Role role = loginResponse->rolelist(i);
						printf("role id:%d  rolename:%s, rolelv: %d", role.roleid(), U8(role.rolename().c_str()),role.rolelv());
						//请求进入游戏（去主城）
						CNetClient::getShareInstance()->sendInGameReq(role.roleid());
						break;
					}
				}
				//没有角色
				else
				{
					//播动作，等回调进引导（去战斗引导）
					m_pArmature->getAnimation()->play("enter");
					this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginLayerMix::callbackForSound))));
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
			m_pArmature->getAnimation()->play("enter");
			this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginLayerMix::callbackForSound))));
			removeLogin();
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void LoginLayerMix::onEnter()
{
	BaseLayer::onEnter();

	CNetClient::getShareInstance()->registerMsgHandler(this);
	FaceBookSDK::sharedInstance()->setCallbackForAuthor(this, callfuncO_selector(LoginLayerMix::faceBookAuthorCallback));
	FaceBookSDK::sharedInstance()->setCallbackForUserInfo(this, callfuncO_selector(LoginLayerMix::faceBookUserInfoCallback));
	GoogleLoginSDK::getInstance()->setCallbackForUserInfo(this, callfuncO_selector(LoginLayerMix::googleUserInfoCallback));

	CJniHelper::getInstance()->setJniListener(this);

	initLoadingEffect();
}


void LoginLayerMix::onExit()
{
	BaseLayer::onExit();

	//释放骨骼，释放图片
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("LoginLayeFaceBook/opening.ExportJson");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	FaceBookSDK::sharedInstance()->setCallbackForAuthor(nullptr,nullptr);
	FaceBookSDK::sharedInstance()->setCallbackForUserInfo(nullptr,nullptr);
	GoogleLoginSDK::getInstance()->setCallbackForUserInfo(nullptr, nullptr);

	CJniHelper::getInstance()->setJniListener(nullptr);
	CNetClient::getShareInstance()->unRegisterMsgHandler(this);
}


void LoginLayerMix::callBackInGame()
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
				pop->setButtonLisener(this, ccw_click_selector(LoginLayerMix::onClickDownloadPackage));
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

void LoginLayerMix::faceBookAuthorCallback(CCObject *object)
{
	CCString *strResult=dynamic_cast<CCString *>(object);
	if(strResult!=NULL)
	{
		CCLog("onFaceBookAuthorChange:%s",strResult->getCString());
		string str = strResult->getCString();
	}
}
void LoginLayerMix::faceBookUserInfoCallback(CCObject *object)
{
	CCLOG("LoginLayeFaceBook::faceBookUserInfoCallback");

	//如果m_FaceBookUserID已经有值，不再接受回调
	if (checkFacebookAuthor())
	{
		return;	
	}

	CCString *strResult=dynamic_cast<CCString *>(object);
	if(strResult!=NULL)
	{
		Json::Value jsValue;
		CCLog("onFaceBookUserInfoGet:%s",strResult->getCString());
		if(Json::Reader().parse(strResult->getCString(),jsValue))
		{
			m_FaceBookAccount=jsValue["name"].asCString();
			m_FaceBookUserID =jsValue["id"].asCString();
			CCUserDefault::sharedUserDefault()->setStringForKey(FACEBOOK_ID,m_FaceBookUserID);

			CCLOG("save FACEBOOK_ID %s", m_FaceBookUserID.c_str());

			//跳转到游戏层
			switchLayer();
		}
	}
}


void LoginLayerMix::googleUserInfoCallback( CCObject *object )
{
	//如果m_email已经有值，不再接受回调
	if (checkGoogleAuthor())
	{
		return;	
	}

	CCString *strResult=dynamic_cast<CCString *>(object);

	if(strResult!=nullptr)
	{
		Json::Value jsValue;
		CCLog("onFaceBookUserInfoGet:%s",strResult->getCString());
		if(Json::Reader().parse(strResult->getCString(),jsValue))
		{
			string sAccount = jsValue["name"].asCString();
			string sPassword =jsValue["id"].asCString();
			CCUserDefault::sharedUserDefault()->setStringForKey(GOOGLE_USER_NAME, sAccount);
			CCUserDefault::sharedUserDefault()->setStringForKey(GOOGLE_PASSWORD, sPassword);
			CCUserDefault::sharedUserDefault()->flush();
			CCLOG("save google username %s", sAccount.c_str());

			//跳转到游戏层
			switchLayer();
		}
	}
}


void LoginLayerMix::jniResult( int nReqCode, int nResult )
{
	if(nReqCode==SHOW_VIDEO)
	{
		if(nResult==0)
		{
			LGResume(this);
			CCLog("LoginLayeFaceBook::jniResult----LGResume(this);");
			//CCDirector::sharedDirector()->resume();
		}
	}
}


void LoginLayerMix::frameEventCallBack( CCBone* pBone, const char* sEvent, int iTag1, int iTag2 )
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

void LoginLayerMix::showVideo()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CJniHelper::getInstance()->showVideo(0);
	LGPause(this);
	//CCDirector::sharedDirector()->pause();
	//runAction(CCCallFunc::create(CCDirector::sharedDirector(), callfunc_selector(CCDirector::pause)));
#endif
}

void LoginLayerMix::scheduleForConnect( float dt )
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

bool LoginLayerMix::LinkedGameServer( )
{
	//点击按钮才连接游戏服务器
	const Server& server = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	//检查服务器状态
	if(!checkServerStatus(server))
	{ 
		return false;
	}

	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->close();
	tcp->setAddress(server.server_host().c_str(), server.server_port(),true);
	if (!tcp->isConnected()&&!tcp->getIsCloseConn())
	{
		tcp->connect();
	}

	return true;
}
//连接网关服务器，连接成功后，网关服务器会自动推送服务器列表的消息过来
void LoginLayerMix::LinkedServerListNet()
{
	GateServer gateServer = LoginScene::getGateServer();
	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->setAddress(gateServer.sHost.c_str(), gateServer.iPort,false);
	if (!tcp->isConnected())
	{
		tcp->connect();
	}
}

void LoginLayerMix::onClickDownloadPackage( CCObject* pSender )
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

void LoginLayerMix::downloadPackageEnd( bool bAnswer )
{
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
}

void LoginLayerMix::callbackForSound()
{
	PlayEffectSound(SFX_539);
}

void LoginLayerMix::callBackForSoundUp()
{
	PlayEffectSound(SFX_541);
}

int LoginLayerMix::getServerIndex( const std::string& sName )
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

void LoginLayerMix::updateSelectServer()
{
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	CLabel* pServerSelect = (CLabel*)m_ui->findWidgetById("server_select");
	pServerSelect->setString(gameServer.server_name().c_str());
	CLabel* pServerSelectStatus = (CLabel*)m_ui->findWidgetById("status_select");
	int iShowText[5] = {2002, 1184, 1185, 1186, 1187};
	pServerSelectStatus->setString(GETLANGSTR(iShowText[gameServer.server_status()]));
}

CCObject* LoginLayerMix::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void LoginLayerMix::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
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
				pBoard->setOnClickListener(this, ccw_click_selector(LoginLayerMix::selectServer));
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

void LoginLayerMix::selectServer( CCObject* pObj )
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

void LoginLayerMix::showServerList()
{
	m_pTableView->reloadData();
	m_pTableView->setVisible(true);

	CButton *pLogin = (CButton*)m_pEnterGameLay->findWidgetById("login");
	pLogin->setVisible(false);

	m_pClose->setVisible(false);

	m_ui->findWidgetById("check")->setVisible(false);
}

void LoginLayerMix::hideServerList()
{
	m_pTableView->setVisible(false);

	CButton *pLogin = (CButton*)m_pEnterGameLay->findWidgetById("login");
	pLogin->setVisible(true);

	m_pClose->setVisible(true);

	m_ui->findWidgetById("check")->setVisible(true);
}

void LoginLayerMix::initLoadingEffect()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/loading.plist");
	AnimationManager::sharedAction()->ParseAnimation("loading");

	//loading贴图
	EffectObject* pLoadEffect = EffectObject::create("loading",sPlayType::eRepeat);
	pLoadEffect->setPosition(VCENTER);
	pLoadEffect->setEffAnchorPoint(0.5f,0.5f);
	pLoadEffect->setScale(0.6f);
	this->addChild(pLoadEffect, 999);
	m_pLoading = pLoadEffect;
	m_pLoading->setVisible(false);
}

void LoginLayerMix::initLogin()
{
	//生成UI
	m_ui = LoadComponent("EnterGame.xaml");
	m_ui->setPosition(VCENTER);
	m_LayoutRoot->addChild(m_ui, 2);

	//版本做相对位置处理
	const string verStr(VERSION);
	const string verStr2 = "V" + verStr;
	CLabel* pLabel = (CLabel *)m_ui->findWidgetById("version");
	pLabel->setAnchorPoint(ccp(1.0f, 0.0f));
	pLabel->setPosition(ccp(VRIGHT-20, VBOTTOM+20));
	CC_SAFE_RETAIN(pLabel);
	pLabel->removeFromParentAndCleanup(true);
	this->addChild(pLabel);
	CC_SAFE_RELEASE(pLabel);


	//显示本地的版本号。全部都是这样
	std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
	std::string sCurrentVersion = CCUserDefault::sharedUserDefault()->getStringForKey(VERSION, sDefalutVersion);
	pLabel->setString(CCString::createWithFormat("Version:%s", sCurrentVersion.c_str())->getCString());


	initSelectLoginLayer();

	initEnterGameLayer();


	//是否有选择登录类型成功登录过
	m_iLoginType = (LoginType)CCUserDefault::sharedUserDefault()->getIntegerForKey(LOGIN_TYPE, LoginTypeNull);
	if ( m_iLoginType == LoginTypeNull)
	{
		//之前没有成功登录过
		m_pLoginSelectLay->setVisible(true);
		m_pEnterGameLay->setVisible(false);
	}
	else
	{
		//有成功登录过
		m_pLoginSelectLay->setVisible(false);
		m_pEnterGameLay->setVisible(true);
	}
}

void LoginLayerMix::initSelectLoginLayer()
{
	m_pLoginSelectLay = (CLayout *)m_ui->findWidgetById("select_login");

	//游客登录
	CButton *pTouristLogin = (CButton*)m_pLoginSelectLay->findWidgetById("tour_login");
	pTouristLogin->setTag(LoginTypeTourist);
	pTouristLogin->setOnClickListener(this,ccw_click_selector(LoginLayerMix::onSelectLogin));

	//facebook登录
	CButton *pFacebookLogin = (CButton*)m_pLoginSelectLay->findWidgetById("fb_login");
	pFacebookLogin->setTag(LoginTypeFacebook);
	pFacebookLogin->setOnClickListener(this,ccw_click_selector(LoginLayerMix::onSelectLogin));

	//google登录
	CButton *pGoogleLogin = (CButton*)m_pLoginSelectLay->findWidgetById("google_login");
	pGoogleLogin->setTag(LoginTypeGoogle);
	pGoogleLogin->setOnClickListener(this,ccw_click_selector(LoginLayerMix::onSelectLogin));

}

void LoginLayerMix::initEnterGameLayer()
{
	m_pEnterGameLay = (CLayout *)m_ui->findWidgetById("enter_game");

	//进入游戏
	CButton *pEnterGame = (CButton*)m_pEnterGameLay->findWidgetById("login");
	pEnterGame->setTag(LoginTypeFacebook);
	pEnterGame->getSelectedImage()->setScale(1.15f);
	pEnterGame->setOnClickListener(this,ccw_click_selector(LoginLayerMix::onEnterGame));


	//服务器选区cell
	CLayout* pLayout = (CLayout*)m_ui->findWidgetById("serverCell");
	pLayout->retain();
	m_ui->removeChild(pLayout);
	m_pCell = pLayout;

	//获取空的滑动列表Scroll
	m_pTableView = (CTableView*)m_pEnterGameLay->findWidgetById("serverScroll");
	m_pTableView->setDirection(eScrollViewDirectionVertical);
	m_pTableView->setSizeOfCell(m_pCell->getContentSize());
	m_pTableView->setCountOfCell(m_serverInfo.server_list().size());
	m_pTableView->setBounceable(true);
	m_pTableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(LoginLayerMix::tableviewDataSource));
	m_pTableView->reloadData();


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_pEnterGameLay->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setOnClickListener(this,ccw_click_selector(LoginLayerMix::onReSelectLogin));
	m_pEnterGameLay->addChild(pClose, 999);
	m_pClose = pClose;


	//默认服务器
	updateSelectServer();

	//隐藏服务器列表
	showServerList();
	hideServerList();
}


void LoginLayerMix::onSelectLogin( CCObject *pObj )
{
	CCNode *pNode = (CCNode *)pObj;
	int iTag = pNode->getTag();

	m_iLoginType = (LoginType)iTag;

	switch (m_iLoginType)
	{
	case LoginTypeTourist:
		{
			//跳转到游戏层
			switchLayer();

		}break;
	case LoginTypeFacebook:
		{
			//验证或者跳转到进入游戏层
			if (checkFacebookAuthor())
			{
				//跳转到游戏层
				switchLayer();
			}
			else
			{
				//授权验证
				onFacebookAuthor();
			}
		}break;
	case LoginTypeGoogle:
		{
			//验证或者跳转到进入游戏层
			//TODO
			if (checkGoogleAuthor())
			{
				//跳转到游戏层
				switchLayer();
			}
			else
			{
				//授权验证
				onGoogleAuthor();
			}
			
		}break;
	default:
		break;
	}
}


void LoginLayerMix::onEnterGame( CCObject *pObj )
{
	switch (m_iLoginType)
	{
	case LoginTypeTourist:
		{
			//直接根据机器ID创建账号密码登录
			onTouristLogin();
		}
		break;
	case LoginTypeFacebook:
		{
			//点击按钮才连接游戏服务器
			onFacebookLogin();
		}
	case LoginTypeGoogle:
		{
			//点击按钮才连接游戏服务器
			onGoogleLogin();
		}
		break;
	default:
		break;
	}
}

void LoginLayerMix::onTouristLogin( )
{
	if (!LinkedGameServer())
	{
		return;
	}

	//初始默认本地的游客账号和密码
	std::string sInfo = CJniHelper::getInstance()->getDeviceId();

	CCLOG("Jni--%s", sInfo.c_str());

	CCString *pStr1 = CCString::createWithFormat("t%s", sInfo.c_str());
	CCString *pStr2 = CCString::createWithFormat("%s", sInfo.c_str());
	string sUserName = pStr1->m_sString;
	string sPassword = pStr2->m_sString;

	CCUserDefault::sharedUserDefault()->setStringForKey( USER_NAME, sUserName );
	CCUserDefault::sharedUserDefault()->setStringForKey( PASSWORD, sPassword );
	CCUserDefault::sharedUserDefault()->flush();

	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginLayerMix::doTouristLogin), 0.5);
	}
	else
	{
		string sOpenId = CCUserDefault::sharedUserDefault()->getStringForKey(FACEBOOK_ID);

		CNetClient::getShareInstance()->sendLogin( sUserName, sPassword, "", false, 0,  sOpenId, G_PLATFORM_TARGET);
		DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(false);

	}	

	PlayEffectSound(SFX_LoginClick);

}

void LoginLayerMix::doTouristLogin( float delt )
{
	string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey( USER_NAME );
	string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey( PASSWORD );
	string sOpenId = CCUserDefault::sharedUserDefault()->getStringForKey(FACEBOOK_ID);

	CNetClient::getShareInstance()->sendLogin( sUserName, sPassword, "", false, 0,  sOpenId, G_PLATFORM_TARGET);
	DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(false);

	this->unschedule(schedule_selector(LoginLayerMix::doTouristLogin));
}

void LoginLayerMix::switchLayer()
{
	CCLOG("switchLayer");

	m_pLoginSelectLay->setVisible(!m_pLoginSelectLay->isVisible());
	m_pEnterGameLay->setVisible(!m_pEnterGameLay->isVisible());
}


void LoginLayerMix::updateForCheckDay( float dt )
{
	if(CheckDay(LoginDayFaceBook))
	{
		hideLableRoll();
		unschedule(schedule_selector(LoginLayerMix::updateForCheckDay));
	}
}

void LoginLayerMix::showLabelRoll( const char* sInfo )
{
	if(CheckDay(LoginDayFaceBook))
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

	schedule(schedule_selector(LoginLayerMix::updateForCheckDay), 1.0f);
}

void LoginLayerMix::hideLableRoll()
{
	this->removeChildByTag(88888);
}


void LoginLayerMix::onGameExit(CCObject* obj)
{
	CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
	CCDirector::sharedDirector()->end();
}



bool LoginLayerMix::checkServerStatus(const Server &server)
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

void LoginLayerMix::onReSelectLogin( CCObject *pObj )
{
	switchLayer();
}

bool LoginLayerMix::checkGoogleAuthor()
{
	string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(GOOGLE_USER_NAME, "");
	if ( sUserName == "" )
	{
		return false;
	}
	return true;
}


void LoginLayerMix::onGoogleAuthor()
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	GoogleLoginSDK::getInstance()->openAuthor();
#else
	CCUserDefault::sharedUserDefault()->setStringForKey(GOOGLE_USER_NAME, "GO-ypfsoul@gmail.com");
	CCUserDefault::sharedUserDefault()->setStringForKey(GOOGLE_PASSWORD, "GO-ypfsoul@gmail.com");
	switchLayer();
#endif
}

void LoginLayerMix::onGoogleLogin()
{
	if (!LinkedGameServer())
	{
		return;
	}


	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginLayerMix::doGoogleLogin), 0.5);
	}
	else
	{
		//初始默认本地的游客账号和密码
		string sAccount = CCUserDefault::sharedUserDefault()->getStringForKey( GOOGLE_USER_NAME, "" );
		string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey( GOOGLE_PASSWORD, "" );

		CNetClient::getShareInstance()->sendLogin( sAccount, sPassword, "", false, 0,  "", G_PLATFORM_TARGET);
		DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(false);

	}	

	PlayEffectSound(SFX_LoginClick);
}

void LoginLayerMix::doGoogleLogin( float delt )
{
	string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey( GOOGLE_USER_NAME );
	string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey( GOOGLE_PASSWORD );

	CNetClient::getShareInstance()->sendLogin( sUserName, sPassword, "", false, 0,  "", G_PLATFORM_TARGET);
	DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(false);

	this->unschedule(schedule_selector(LoginLayerMix::doGoogleLogin));
}