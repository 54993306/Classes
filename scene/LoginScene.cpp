#include "LoginScene.h"
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
#include "layer/BackLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "update/CUpdateLayer.h"
#include "SDK/FaceBookSDK.h"
#include "jsonCpp/json.h"
#include "UserDefaultData.h"
#include "update/CDownloadPackage.h"
#include "ApplicationDefine.h"
#include "scene/AnimationManager.h"
#include "scene/effect/EffectObject.h"
#include "Resources.h"
#include "ApplicationDefine.h"
#include "common/RollLabel.h"
#include "tools/ShowTexttip.h"

#define getLayer() (CLayout*)this->getChildByTag(10000)->getChildByTag(1)
//网关服务器
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
//const GateServer GATE_SERVER = {"125.227.163.145",	5203};//李辉服务器
const GateServer GATE_SERVER = {"192.168.0.9",			7303};//大毛服务器
//const GateServer GATE_SERVER = {"127.0.0.1",					5203};//本机服务器
//const GateServer GATE_SERVER = {"192.168.1.2",			5203};//局域网服务器
//const GateServer GATE_SERVER = {"192.168.0.2",			5203};//香港局域网服务器 
//const GateServer GATE_SERVER = {"103.31.20.79",			7303};//外网服务器
//const GateServer GATE_SERVER = {"54.251.138.176",			5203};//泰国-境外 
#else
//const GateServer GATE_SERVER = {"125.227.163.145",	5203};//李辉服务器
//const GateServer GATE_SERVER = {"192.168.0.9",			7303};//大毛服务器
//const GateServer GATE_SERVER = {"127.0.0.1",					5203};//本机服务器
//const GateServer GATE_SERVER = {"192.168.1.2",			5203};//局域网服务器
//const GateServer GATE_SERVER = {"192.168.0.2",			5203};//香港局域网服务器 
//const GateServer GATE_SERVER = {"103.31.20.79",			7303};//外网服务器
const GateServer GATE_SERVER = {"54.251.138.176",			5203};//泰国-境外 
#endif

LoginScene::LoginScene():
	m_ui(nullptr),
	m_logLay(nullptr),
	m_armature(nullptr),
	m_LayoutRoot(nullptr),
	m_iCurrentType(0),
	m_accessCodeText(nullptr),
	m_isRegister(false),
	m_iSelectServerIndex(0),
	m_pTableView(nullptr),
	m_pLoading(nullptr),
	m_pCell(nullptr)
{

}

using namespace BattleSpace;

LoginScene::~LoginScene()
{
	CC_SAFE_RELEASE(m_pCell);
}


void LoginScene::onCreate()
{
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
	m_armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(LoginScene::movementCallBack));
	m_armature->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(LoginScene::frameEventCallBack));
	m_armature->getAnimation()->playWithIndex(0);

	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.8f), CCCallFunc::create(this, callfunc_selector(LoginScene::callBackForSoundUp))));
}

void LoginScene::movementCallBack( CCArmature* pAramture, MovementEventType type, const char* sData )
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
				CCLOG("LoginScene::movementCallBack::opening");

				m_pLoading->setVisible(true);

				showLabelRoll(GETLANGSTR(1244));

				//拉取服务器列表
				if(CJniHelper::getInstance()->checkNetWork())
				{
					LinkedServerListNet(); 
				}
				else
				{
					ShowPopTextTip(GETLANGSTR(1189));
				}
				schedule(schedule_selector(LoginScene::scheduleForConnect), 5.0f);

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
				this->runAction(CCCallFunc::create(this, callfunc_selector(LoginScene::callBackInGame)));
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}


void LoginScene::initLogin()
{
	CCLOG("LoginScene::initLogin");

	if(m_ui!=nullptr)
	{
		return;
	}

	m_ui = LoadComponent("EnterGame.xaml");
	m_ui->setTag(1);
	m_ui->setPosition(VCENTER);
	m_LayoutRoot->addChild(m_ui, 2);

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
	m_pTableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(LoginScene::tableviewDataSource));
	m_pTableView->reloadData();


	//按钮打开服务器列表
	CImageView* pBtnServer = (CImageView*)m_ui->findWidgetById("serverBtn");

	//登陆框
	m_logLay = (CLayout *)m_ui->getChildByTag(10);

	const string verStr(VERSION);
	const string verStr2 = "V" + verStr;

	CButton * pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setVisible(false);
	pQuickLogin->setOnClickListener(this,ccw_click_selector(LoginScene::onQuickLogin));
	CCActionInterval *scaleSmall = CCScaleTo::create(0.9f,0.98f);
	CCFadeTo *out = CCFadeTo::create(0.9f,60);
	CCSpawn *spawn = CCSpawn::createWithTwoActions(scaleSmall,out);
	CCActionInterval *scaleBig = CCScaleTo::create(1.2f,1.0f);
	CCFadeTo *in = CCFadeTo::create(1.2f,255);
	CCSpawn *spawn1 = CCSpawn::createWithTwoActions(scaleBig,in);
	CCSequence *seque = CCSequence::createWithTwoActions(spawn,spawn1);
	CCRepeatForever *action = CCRepeatForever::create(seque);
	pQuickLogin->runAction(action);


	string userName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	if(userName != "")
	{
		CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
		name->setString(userName.c_str());
	}

	//添加输入框
	addLoginField();

	BackLayer* bklayer = BackLayer::create();
	bklayer->setZOrder(10);
	this->addChild(bklayer);

	//登录按钮
	CButton* button = (CButton*)m_logLay->getChildByTag(1);
	button->setOnClickListener(this,ccw_click_selector(LoginScene::onLogin));
	button->setId("logBtn");

	CButton *btn = (CButton*)m_ui->getChildByTag(11);
	btn->setOnClickListener(this, ccw_click_selector(LoginScene::showOrHideLogin));

	CButton* reg = (CButton*)m_logLay->findWidgetById("register");
	reg->setOnClickListener(this,ccw_click_selector(LoginScene::onRegister));
    reg->setId("regBtn");

	CButton* facebook = (CButton*)m_ui->findWidgetById("fb_login");
	facebook->setOnClickListener(this,ccw_click_selector(LoginScene::onFacebookLogin));

	PlayBackgroundMusic(BGM_Login,true);

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

	if(userName != "")
	{
		pQuickLogin->setVisible(true);
		m_logLay->setVisible(false);
	}
	else
	{
		pQuickLogin->setVisible(false);
		m_logLay->setVisible(true);
	}
}


void LoginScene::removeLogin()
{
	m_ui->removeFromParentAndCleanup(true);
	m_ui = nullptr;
}

void LoginScene::onExit()
{
	CNetClient::getShareInstance()->unRegisterMsgHandler(this);
	CScene::onExit();

	//释放骨骼，释放图片
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("loginScene/opening.ExportJson");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
	FaceBookSDK::sharedInstance()->setCallbackForAuthor(nullptr,nullptr);
	FaceBookSDK::sharedInstance()->setCallbackForUserInfo(nullptr,nullptr);

	CJniHelper::getInstance()->setJniListener(nullptr);
}

void LoginScene::onGameExit(CCObject* obj)
{
	CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
	CCDirector::sharedDirector()->end();
}

void LoginScene::onFacebookLogin(CCObject* obj)
{
	FaceBookSDK::sharedInstance()->openAuthor();
	//FaceBookSDK::sharedInstance()->onOpenPickFriends();

	//C2DXShareSDK::authorize(C2DXPlatTypeFacebook,authorLogin);
}

void LoginScene::onRegister(CCObject* obj)
{
	CImageViewScale9* rect_3 = (CImageViewScale9*)m_logLay->findWidgetById("rect_3");
	CLabel* code = (CLabel*)m_logLay->findWidgetById("code");
	rect_3->setVisible(true);
	code->setVisible(true);
	((CCNode*)obj)->setVisible(false);
	m_accessCodeText->setVisible(true);
}

void LoginScene::onLogin(CCObject* obj)
{
	//点击按钮才连接游戏服务器
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);

	//服务器是维护状态，不连接，给提示
	if(gameServer.server_status()==4)
	{
		ShowPopTextTip(GETLANGSTR(1243));
		return;
	}

	LinkedGameServer(gameServer);

	CLayout* layer = getLayer();
	DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(false);

	CButton *btn = (CButton*)obj; 
	if (strcmp(btn->getId(),"regBtn")==0)
	{
		m_isRegister = true;
	}
	else
	{
		m_isRegister = false;
	}

	CursorTextField* input = (CursorTextField*)m_logLay->getChildByTag(100);
	if(!input||strcmp(input->getTextInput()->c_str(),""))
	{
		CCUserDefault::sharedUserDefault()->setStringForKey(USER_NAME, input->getTextInput()->c_str());
	}
	
	CursorTextField *pwd = (CursorTextField*)m_logLay->getChildByTag(101);
	if(strcmp(pwd->getTextInput()->c_str(),""))
	{
		CCUserDefault::sharedUserDefault()->setStringForKey(PASSWORD,pwd->getTextInput()->c_str());
	}

	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginScene::doLogin),0.5);
	}
	else
	{
		if (strcmp(input->getTextInput()->c_str(),"")&&strcmp(pwd->getTextInput()->c_str(),""))
		{
			printf("%s",pwd->getTextInput()->c_str());
			string code;
			if (m_isRegister)
			{
				code = *(m_accessCodeText->getTextInput());
			}
			else
			{
				code ="";
			}
			CNetClient::getShareInstance()->sendLogin(input->getTextInput()->c_str(),pwd->getTextInput()->c_str(),m_accessCodeText->getTextInput()->c_str());
		}
		else
		{
			CCMessageBox(GETLANGSTR(1004), GETLANGSTR(1005));
		}
	}	

	PlayEffectSound(SFX_LoginClick);
}

void LoginScene::doFacebookLogin(float delt)
{
	DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(true);
	GetTcpNet->sendLogin("","", "",false,0,m_FaceBookUserID);
}

void LoginScene::doLogin(float delt)
{
	if(!m_ui)return;

	CursorTextField* input = (CursorTextField*)m_logLay->getChildByTag(100);
	CursorTextField *pwd = (CursorTextField*)m_logLay->getChildByTag(101);
	
	if (strcmp(input->getString(),"")&&strcmp(pwd->getString(),""))
	{
		string code;
		if (m_isRegister)
		{
			code = *(m_accessCodeText->getTextInput());
		}
		else
		{
			code ="";
		}
		CNetClient::getShareInstance()->sendLogin(input->getTextInput()->c_str(),pwd->getTextInput()->c_str(),m_accessCodeText->getTextInput()->c_str());
	}
	else
	{
		CCMessageBox(GETLANGSTR(1004), GETLANGSTR(1005));
	}
	this->unschedule(schedule_selector(LoginScene::doLogin));
}

void LoginScene::showOrHideLogin(CCObject* obj)
{
	CButton* btn  = (CButton*)obj;
	int  tag = btn->getTag();
	if (tag==11)
	{
		if (!m_logLay->isVisible())
		{
			m_logLay->setVisible(true);
			if (!m_logLay->getChildByTag(100)&&!m_logLay->getChildByTag(101))
			{
				addLoginField();
			}
			CButton *login = (CButton*)m_ui->getChildByTag(1);
		    login->setVisible(false);
		}
		else
		{
			m_logLay->setVisible(false);
			if (CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME)!="")
			{
				CButton *login = (CButton*)m_ui->getChildByTag(1);
				login->setVisible(true);
			}
		}
		
	}
}

void LoginScene::onQuickLogin(CCObject* obj)
{
	PlayEffectSound(SFX_LoginClick);

	//如果输入框显示状态，不快速login
	if(m_logLay->isVisible())
	{
		return;
	}

	//点击按钮才连接服务器
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	
	//服务器是维护状态，不连接，给提示
	if(gameServer.server_status()==4)
	{
		ShowPopTextTip(GETLANGSTR(1243));
		return;
	}

	LinkedGameServer(gameServer);

	string userName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	string psw = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
	if (!CNetClient::getShareInstance()->isConnected())
	{
		CNetClient::getShareInstance()->connect();
		this->scheduleOnce(schedule_selector(LoginScene::reConnectLogin),0.5);
	}
	else if (userName!=""&&psw!="")
	{
		CNetClient::getShareInstance()->sendLogin(userName,psw);
	}
}

void LoginScene::reConnectLogin(float dt)
{
	string userName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	string psw = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
	CNetClient::getShareInstance()->sendLogin(userName,psw);
	this->unschedule(schedule_selector(LoginScene::reConnectLogin));
}

bool LoginScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!m_ui)return false;

	CCPoint pos = convertToNodeSpace(pTouch->getLocation());

	if(m_pTableView->isVisible())
	{
		return false;
	}

	//按钮
	CImageView* pBtnServer = (CImageView*)m_ui->findWidgetById("serverBtn");
	if(pBtnServer->boundingBox().containsPoint(pos) && m_logLay->isVisible())
	{
		showServerList();
		return true;
	}

	//登录按钮
	if (CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME)=="")
	{
		return false;
	}

	CCNode *login = m_ui->getChildByTag(1);
	CButton *btn = (CButton*)m_ui->getChildByTag(11);
	if (!login->boundingBox().containsPoint(pos))
	{
		showOrHideLogin(btn);
		return true;
	}

	return false;
}

//连接网关服务器成功后,服务器自动推送消息，返回网关服务器列表
bool LoginScene::ProcessMsg(int type, google::protobuf::Message *msg)
{
	m_iCurrentType = type;
	switch (type)
	{
	case ServerListMsg:
		{
			unschedule(schedule_selector(LoginScene::scheduleForConnect));

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

			//检查版本号，是否要更新
			bool isNeedUpdate = CUpdateLayer::checkUpdate(serverInfo->game_version().c_str());
			//创建更新
			if(isNeedUpdate)
			{				
				CUpdateLayer* pLayer = CUpdateLayer::create();
				pLayer->setVersion(m_serverInfo.game_version().c_str());
				this->addChild(pLayer, 999);
			}
			else
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
					this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginScene::callbackForSound))));
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
			this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(LoginScene::callbackForSound))));
			removeLogin();
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void LoginScene::onEnter()
{
	CScene::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,1,true);

	CNetClient::getShareInstance()->registerMsgHandler(this);
	FaceBookSDK::sharedInstance()->setCallbackForAuthor(this,callfuncO_selector(LoginScene::onFaceBookAuthor));
	FaceBookSDK::sharedInstance()->setCallbackForUserInfo(this,callfuncO_selector(LoginScene::onFaceBookUserInfo));

	CJniHelper::getInstance()->setJniListener(this);

	initLoadingEffect();
}

void LoginScene::addLoginField()
{

	//账户
	CursorTextField* textFieldAccount = nullptr;
	string userName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);

	if(userName != "")
	{
		textFieldAccount = CursorTextField::textFieldWithPlaceHolder(userName.c_str(), FONT_NAME, 18,CCSize(220, 40),ccWHITE);
		textFieldAccount->setPriority(1);
		textFieldAccount->setTextInput(userName);
		CCNode* tip = m_logLay->getChildByTag(20);
		tip->setVisible(false);
	}else
	{
		CCNode* tip = m_logLay->getChildByTag(20);
		tip->setAnchorPoint(ccp(0.5,0.5));
		textFieldAccount = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 18, CCSize(220, 40),ccWHITE);
		textFieldAccount->setPriority(1);
		textFieldAccount->setTextInput("");
	}

	CCNode* inputBgAccount = m_logLay->getChildByTag(2);
	textFieldAccount->setLimitNum(17);
	textFieldAccount->setAnchorPoint(ccp(0, 0.5f));
	textFieldAccount->setPosition( ccp(inputBgAccount->getPositionX()-90,inputBgAccount->getPositionY()));
	textFieldAccount->setTag(100);
	m_logLay->addChild(textFieldAccount);
	m_textFiledAccount = textFieldAccount;


	//密码
	CursorTextField* textFieldPass = nullptr;
	string psw = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);

	if(psw != "")
	{
		textFieldPass = CursorTextField::textFieldWithPlaceHolder(psw.c_str(), FONT_NAME, 18,CCSize(220, 40),ccWHITE);
		textFieldPass->setPriority(1);
		textFieldPass->setTextInput(psw);
	}
	else
	{
		textFieldPass = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 18, CCSize(220, 40),ccWHITE);//"請輸入你的密碼"
		textFieldPass->setPriority(1);
		textFieldPass->setTextInput("");
	}
	//帐号
	CCNode* inputBgPassword = m_logLay->getChildByTag(4);
	textFieldPass->setPosition(ccp(inputBgPassword->getPositionX()-90, inputBgPassword->getPositionY()));
	textFieldPass->setLimitNum(19);
	textFieldPass->setAnchorPoint(ccp(0, 0.5f));
	textFieldPass->setTag(101);
	m_logLay->addChild(textFieldPass);
	m_textFiledPassword = textFieldPass;



	//验证码
	m_accessCodeText = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 18,CCSize(220, 40),ccWHITE);
	CCNode* inputBg2 = (CCNode*)m_logLay->findWidgetById("rect_3");
	m_accessCodeText->setPosition(ccp(inputBg2->getPositionX()-90,inputBg2->getPositionY()));
	m_accessCodeText->setLimitNum(19);
	m_accessCodeText->setAnchorPoint(ccp(0, 0.5f));
	m_accessCodeText->setPriority(1);
	m_accessCodeText->setTextInput("");
	m_accessCodeText->setVisible(false);
	m_logLay->addChild(m_accessCodeText);
}


void LoginScene::callBackInGame()
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
			//战斗引导完了，弹框告知是否下载更新包
			if(CDownloadPackage::checkIsNeedPackage())
			{
				CPopTip *pop = CPopTip::create();
				LayerManager::instance()->pop();
				pop->setVisible(true);
				pop->addContentTip(GETLANGSTR(1156));
				pop->setTouchEnabled(true);
				pop->setTouchPriority(-100);
				pop->setButtonLisener(this, ccw_click_selector(LoginScene::onClickDownloadPackage));
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

void LoginScene::onFaceBookAuthor(CCObject *object)
{
	CCString *strResult=dynamic_cast<CCString *>(object);
	if(strResult!=NULL)
	{
		CCLog("onFaceBookAuthorChange:%s",strResult->getCString());
		string str = strResult->getCString();
		//	JNITool::sharedInstance()->showToastMsg(str);
		CLabel *label = CLabel::create(strResult->getCString(),DEFAULT_FONT,18);
		label->setColor(ccWHITE);
		label->setPosition(ccp(100,100));
		m_ui->addChild(label,100);
	}
}
void LoginScene::onFaceBookUserInfo(CCObject *object)
{
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	LinkedGameServer(gameServer);
	CCString *strResult=dynamic_cast<CCString *>(object);
	if(strResult!=NULL)
	{
		Json::Value jsValue;
		CCLog("onFaceBookUserInfoGet:%s",strResult->getCString());
		CLabel *label = CLabel::create(strResult->getCString(),DEFAULT_FONT,18);
		label->setPosition(VCENTER);
		label->setColor(ccBLACK);
		m_ui->addChild(label,100);
		if(Json::Reader().parse(strResult->getCString(),jsValue))
		{
			m_FaceBookAccount=jsValue["name"].asCString();
			m_FaceBookUserID =jsValue["id"].asCString();
			if (!CNetClient::getShareInstance()->isConnected())
			{
				CNetClient::getShareInstance()->connect();
				this->scheduleOnce(schedule_selector(LoginScene::doFacebookLogin),0.5);
			}
			else
			{
				DataCenter::sharedData()->getUser()->getUserData()->setIsFBLogin(true);
				GetTcpNet->sendLogin("","","",false,0,m_FaceBookUserID);
			}

			//FaceBookSDK::sharedInstance()->onOpenPickFriends();
		}
	}
}

void LoginScene::jniResult( int nReqCode, int nResult )
{
	if(nReqCode==SHOW_VIDEO)
	{
		if(nResult==0)
		{
			LGResume(this);
			//CCDirector::sharedDirector()->resume();
		}
	}
}


void LoginScene::frameEventCallBack( CCBone* pBone, const char* sEvent, int iTag1, int iTag2 )
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

void LoginScene::showVideo()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CJniHelper::getInstance()->showVideo(0);
	LGPause(this);
	//CCDirector::sharedDirector()->pause();
	//runAction(CCCallFunc::create(CCDirector::sharedDirector(), callfunc_selector(CCDirector::pause)));
#endif
}

void LoginScene::scheduleForConnect( float dt )
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

void authorLogin(C2DXResponseState state, C2DXPlatType platType, CCDictionary *error)
{
	if (state==C2DXResponseStateSuccess)
	{
		printf("success");
	}
}

void LoginScene::LinkedGameServer(const Server& server)
{
	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->setAddress(server.server_host().c_str(), server.server_port());
	if (!tcp->isConnected()&&!tcp->getIsCloseConn())
	{
		tcp->connect();
	}
}
//连接网关服务器，连接成功后，网关服务器会自动推送服务器列表的消息过来
void LoginScene::LinkedServerListNet()
{
#if DIRECT_CONNECT_GAMESERVER
	directConnectGameServer();
	return;
#endif
	CNetClient *tcp = CNetClient::getShareInstance();
	tcp->setAddress(GATE_SERVER.sHost.c_str(), GATE_SERVER.iPort);
	if (!tcp->isConnected())
	{
		tcp->connect();
	}
}

void LoginScene::onClickDownloadPackage( CCObject* pSender )
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

void LoginScene::downloadPackageEnd( bool bAnswer )
{
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoadScene));
}

void LoginScene::callbackForSound()
{
	PlayEffectSound(SFX_539);
}

void LoginScene::callBackForSoundUp()
{
	PlayEffectSound(SFX_541);
}

int LoginScene::getServerIndex( const std::string& sName )
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

void LoginScene::updateSelectServer()
{
	const Server& gameServer = m_serverInfo.server_list().Get(m_iSelectServerIndex);
	CLabel* pServerSelect = (CLabel*)m_ui->findWidgetById("server_select");
	pServerSelect->setString(gameServer.server_name().c_str());
	CLabel* pServerSelectStatus = (CLabel*)m_ui->findWidgetById("status_select");
	pServerSelectStatus->setString(GETLANGSTR(gameServer.server_status()+1183));
}

CCObject* LoginScene::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void LoginScene::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
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
				pBoard->setOnClickListener(this, ccw_click_selector(LoginScene::selectServer));
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
				pLabel->setString(GETLANGSTR(server.server_status()+1183));
			}
			break;
		default:
			break;
		}
	}
}

void LoginScene::selectServer( CCObject* pObj )
{
	PlayEffectSound(SFX_Button);
	CImageView* pImageView = (CImageView*)pObj;
	m_iSelectServerIndex = pImageView->getParent()->getTag();
	
	updateSelectServer();
	hideServerList();
}

void LoginScene::showServerList()
{
	m_pTableView->reloadData();
	m_pTableView->setVisible(true);
	//m_pTableView->runAction(CCMoveBy::create(0.0f, ccp(0, -1000)));
	m_logLay->setVisible(false);
	CButton* pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setVisible(false);

	m_textFiledAccount->setPositionY(m_textFiledAccount->getPositionY()-600);
	m_textFiledPassword->setPositionY(m_textFiledPassword->getPositionY()-600);
	m_accessCodeText->setPositionY(m_accessCodeText->getPositionY()-600);
}

void LoginScene::hideServerList()
{
	m_pTableView->setVisible(false);
	//m_pTableView->runAction(CCMoveBy::create(0.0f, ccp(0, 1000)));
	m_logLay->setVisible(true);
	CButton* pQuickLogin = (CButton *)m_ui->getChildByTag(1);
	pQuickLogin->setVisible(false);

	m_textFiledAccount->setPositionY(m_textFiledAccount->getPositionY()+600);
	m_textFiledPassword->setPositionY(m_textFiledPassword->getPositionY()+600);
	m_accessCodeText->setPositionY(m_accessCodeText->getPositionY()+600);
}

void LoginScene::initLoadingEffect()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/loading.plist");
	AnimationManager::sharedAction()->ParseAnimation("loading");

	//loading贴图
	EffectObject* pLoadEffect = EffectObject::create("loading",PLAYERTYPE::Repeat);
	pLoadEffect->setPosition(VCENTER);
	pLoadEffect->setEffAnchorPoint(0.5f,0.5f);
	pLoadEffect->setScale(0.6f);
	this->addChild(pLoadEffect, 999);
	m_pLoading = pLoadEffect;
	m_pLoading->setVisible(false);
}

void LoginScene::directConnectGameServer()
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

void LoginScene::showLabelRoll( const char* sInfo )
{
	if(checkDay())
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

	schedule(schedule_selector(LoginScene::updateForCheckDay), 1.0f);
}

void LoginScene::hideLableRoll()
{
	this->removeChildByTag(88888);
}

bool LoginScene::checkDay()
{
	struct   tm  * tm ;  
	time_t  timep;  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)   
	time(&timep);  
#else   
	struct  cc_timeval now;   
	CCTime::gettimeofdayCocos2d(&now, NULL);   
	timep = now.tv_sec;  
#endif   

	tm  = localtime(&timep);  
	int  year =  tm ->tm_year + 1900;  
	int  month =  tm ->tm_mon + 1;  
	int  day =  tm ->tm_mday;  
	int  hour= tm ->tm_hour;  

	CCString* sDay = CCString::createWithFormat("%d-%s%d-%s%d-%s%d", year, month>9?"":"0", month, day>9?"":"0", day, hour>9?"":"0", hour);
	if(sDay->compare("2016-05-07-11")>0)
	{
		return true;
	}

	return false;
}

void LoginScene::updateForCheckDay( float dt )
{
	if(checkDay())
	{
		hideLableRoll();
		unschedule(schedule_selector(LoginScene::updateForCheckDay));
	}
}


