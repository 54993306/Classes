#include "Battle/BattleScene/LogoScene.h"
#include "tools/commonDef.h"
#include "AppUI.h"
#include "Global.h"
#include "Battle/RoleObject/RoleObject.h"
#include "tools/StringUtil.h"
#include "Battle/EffectObject.h"
#include "common/CommonFunction.h"
#include "Battle/ArmatureEventDataMgr.h"
#include "Battle/WarAssist.h"
#include "tools/ShowTexttip.h"
#include "SimpleAudioEngine.h"
#include "Battle/BattleLayer/StoryLayer.h"
#include "../../../../plugin/plugins/jpush-cocos2d-x-plugin/Plugins/JPushService.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "Battle/ActionNameDefine.h"

using namespace CocosDenshion;
LogoScene::LogoScene():m_XmlLayer(nullptr),m_ModeIndex(0),m_PauseAction(""),m_Body(nullptr),m_InputScale(nullptr)
	,m_InputModeId(nullptr),m_InputAction(nullptr),m_ActionIndex(0),m_RunAction(true),m_armatureEventDataMgr(nullptr)
	,m_lastFrame(0),m_ActionKey(""),m_Armature(nullptr),m_ModeID(0),m_InputMap(nullptr)
{}
void LogoScene::onCreate()
{
	CWidgetWindow* LayoutRoot = CWidgetWindow::create();
	this->addChild(LayoutRoot, 2);

	CCLayerColor* pColorLayer = CCLayerColor::create(ccc4(255, 255, 255, 255));
	pColorLayer->setContentSize(WINSIZE);
	pColorLayer->setPosition(CCPointZero);
	this->addChild(pColorLayer, 1);

	m_XmlLayer = LoadComponent("LogoScene.xaml");
	m_XmlLayer->setPosition(VCENTER);
	LayoutRoot->addChild(m_XmlLayer);	

}
void LogoScene::onEnter()
{
	CScene::onEnter();
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JPushService::init();
#endif
	CCSize winSize = VirtualScreen::Instance()->VirtualSize();
	float dx = (DESIGN_WIDTH - winSize.width) / 2;
	m_Body = CCSprite::create();
	m_Body->retain();
	this->addChild(m_Body,10);

	CCSprite* logo1 = (CCSprite*)m_XmlLayer->getChildByTag(1);
	logo1->setPosition(ccp(winSize.width*0.3f, winSize.height * 0.62f));
	CCActionInterval* moveOut = CCMoveTo::create(1.2f, ccp(winSize.width/2 + dx, logo1->getPositionY()));
	CCActionInterval* moveBack = CCEaseElasticOut::create(moveOut);
	logo1->runAction(moveBack);

	CCSprite* logo2 = (CCSprite*)m_XmlLayer->getChildByTag(2);
	logo2->setPosition(ccp(winSize.width*0.7f, winSize.height * 0.36f));
	moveOut = CCMoveTo::create(2.0f, ccp(winSize.width/2+dx,logo2->getPositionY()));
	moveBack = CCEaseElasticOut::create(moveOut);
	logo2->runAction(moveBack);
	m_LogoSize = CCDirector::sharedDirector()->getWinSize();


	//spAtlas*atlas = spAtlas_createFromFile("Spine/spineboy.atlas", 0);
	//CCAssert(atlas, "Error reading atlas file.");
	//spSkeletonJson* atlas_json = spSkeletonJson_create(atlas);
	//atlas_json->scale = 1.0f;//骨骼动画大小
	//spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(atlas_json, "Spine/spineboy.json");
	//CCAssert(skeletonData, atlas_json->error ? atlas_json->error : "Error reading skeleton data file.");
	//spSkeletonJson_dispose(atlas_json);
	//SkeletonAnimation* skeletonNode = SkeletonAnimation::createWithFile("Spine/spineboy.json", "Spine/spineboy.atlas", 0.6f);
	//skeletonNode->setScale(0.5);
	//skeletonNode->retain();

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    testRests();
	m_XmlLayer->getChildByTag(3)->setVisible(false);
	//CountMethod();
	//this->scheduleOnce(schedule_selector(LogoScene::start), 0.1f);

	//SpineboyExample* TestSpineLayer = SpineboyExample::create();
	//TestSpineLayer->InitLayer((CLayout*)m_XmlLayer->getChildByTag(3));
	//m_XmlLayer->addChild(TestSpineLayer);

#else
	this->setVisible(false);
	this->scheduleOnce(schedule_selector(LogoScene::start), 0.1f);
#endif
}
void LogoScene::onExit()
{
	CScene::onExit();
	this->unschedule(schedule_selector(LogoScene::start));
}
void LogoScene::start(float dt)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoginScene));
#else
	CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoginScene));
#endif
}
void LogoScene::testRests()
{
	CCLabelTTF* ActionName = CCLabelTTF::create("Action Name",FONT_NAME,50);
	ActionName->setPosition(ccp(40,500));
	ActionName->setAnchorPoint(ccp(0,0));
	ActionName->setTag(MSGTTF);
	ActionName->setColor(ccc3(0,0,255));
	m_XmlLayer->addChild(ActionName);

	CCLabelTTF* ERRORtips = CCLabelTTF::create("ERROR TIPS",FONT_NAME,30);
	ERRORtips->setPosition(ccp(40,580));
	ERRORtips->setAnchorPoint(ccp(0,0));
	ERRORtips->setTag(ERRORTIPS);
	ERRORtips->setColor(ccc3(255,0,0));
	m_XmlLayer->addChild(ERRORtips);

	CCLabelTTF* ActionNum = CCLabelTTF::create("Test Action Num",FONT_NAME,20);
	ActionNum->setPosition(ccp(300,30));
	ActionNum->setAnchorPoint(ccp(0,0));
	ActionNum->setTag(ACTIONNUM);
	ActionNum->setColor(ccc3(0,150,150));
	m_XmlLayer->addChild(ActionNum);

	CButton* LoginBt = CButton::create();
	LoginBt->setEnabled(true);
	LoginBt->setZOrder(2);
	LoginBt->setTag(GotoLogin);
	LoginBt->setColor(ccc3(200,0,0));
	LoginBt->setNormalImage("loginScene/btn_shenqing02.png");
	LoginBt->setSelectedImage("loginScene/btn_shenqing01.png");
	LoginBt->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	LoginBt->setPosition(ccp(m_LogoSize.width/2+150,m_LogoSize.height/2-200));
	m_XmlLayer->addChild(LoginBt);

	//模型测试
	TestModel();
	//动作测试
	TestAction();
	//缩放测试
	CButton* btInput3 = CButton::create();
	btInput3->setScale(0.5f);
	btInput3->setEnabled(true);
	btInput3->setNormalImage("common/button_04_on.png");
	btInput3->setSelectedImage("common/button_04_off.png");
	btInput3->setColor(ccc3(0,200,0));
	btInput3->setTag(ScaleNum);
	btInput3->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	btInput3->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-200));
	m_XmlLayer->addChild(btInput3);
	CCLabelTTF* testScale = CCLabelTTF::create("TestScale",FONT_NAME,40);
	testScale->setColor(ccc3(0,255,0));
	btInput3->addChild(testScale);

	CCSprite* SpInput3 = CCSprite::create("common/button_37.png");
	SpInput3->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-150));
	m_InputScale =  CursorTextField::textFieldWithPlaceHolder("InPut Mode Scale", FONT_NAME, 15,CCSizeMake(150,50),ccWHITE); 
	m_InputScale->setPriority(-1);
	m_InputScale->setLimitNum(20);
	m_InputScale->setPosition(ccp(SpInput3->getContentSize().width/2,SpInput3->getContentSize().height/2));
	SpInput3->addChild(m_InputScale);	
	m_XmlLayer->addChild(SpInput3);
	
	CButton* btInput4 = CButton::create();
	btInput4->setScale(0.5f);
	btInput4->setEnabled(true);
	btInput4->setNormalImage("common/button_04_on.png");
	btInput4->setSelectedImage("common/button_04_off.png");
	btInput4->setColor(ccc3(0,200,0));
	btInput4->setTag(mapTest);
	btInput4->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	btInput4->setPosition(ccp(m_LogoSize.width-400,m_LogoSize.height-200));
	m_XmlLayer->addChild(btInput4);
	CCLabelTTF* testmap = CCLabelTTF::create("TestMap",FONT_NAME,40);
	testmap->setColor(ccc3(0,255,0));
	btInput4->addChild(testmap);

	CCSprite* SpInput4 = CCSprite::create("common/button_37.png");
	SpInput4->setPosition(ccp(m_LogoSize.width-400,m_LogoSize.height-150));
	m_InputMap =  CursorTextField::textFieldWithPlaceHolder("InPut Mode Scale", FONT_NAME, 15,CCSizeMake(150,50),ccWHITE); 
	m_InputMap->setPriority(-1);
	m_InputMap->setLimitNum(20);
	m_InputMap->setPosition(ccp(SpInput4->getContentSize().width/2,SpInput4->getContentSize().height/2));
	SpInput4->addChild(m_InputMap);	
	m_XmlLayer->addChild(SpInput4);

// 	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
// 	//Slider,值在 0 ~ 2 Pi 之间
// 	CCControlSlider * hueSlider = CCControlSlider::create("test/yinl_dutiao3.png", "test/yinl_dutiao1.png", "test/yinl_anniu3.png");
// 	hueSlider->setMinimumValue(0.0f);
// 	hueSlider->setMaximumValue(13);
// 	hueSlider->setPosition(CCPoint(winSize.width*0.5f, winSize.height*0.1f));
// 	hueSlider->addTargetWithActionForControlEvents(this, cccontrol_selector(LogoScene::hueChange), CCControlEventValueChanged);
// 	this->addChild(hueSlider, 999);

}
void LogoScene::testAnimation(int modeid)
{
	if(!modeid)return;
	if (m_Armature)
	{
		m_Armature->removeFromParentAndCleanup(true);				//如果不移除会一直累加
		m_Armature = nullptr;
	}
	char ExportJson_str[60] = {0};//"BoneAnimation/101.ExportJson"
	sprintf(ExportJson_str,"BoneAnimation/%d.ExportJson",modeid);
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ExportJson_str);
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(modeid));
	if (outPutERRORMsg("LogoScene::testAnimation animationData is NULL",animationData))
	{
		char ErrorInfo_[60] = {0};
		sprintf(ErrorInfo_,"Can't Find Animation in %d",modeid);
		CCLabelTTF* _ERRORTIPS = (CCLabelTTF*)m_XmlLayer->getChildByTag(ERRORTIPS);
		_ERRORTIPS->setString(ErrorInfo_);
		return;
	} 
	m_Armature = CCArmature::create(ToString(modeid));			//新建的项目名称 BoneAnimation
	if (outPutERRORMsg("LogoScene::testAnimation",m_Armature))return;
	CCLabelTTF* ActionName = (CCLabelTTF*)m_XmlLayer->getChildByTag(ACTIONNUM);
	char Numstr[60]={0};
	sprintf(Numstr,"Model %d Have %d Action",modeid,m_Armature->getAnimation()->getMovementCount());
	ActionName->setString(Numstr);
	//播放指定动作
	m_Armature->getAnimation()->play(Stand_Action);
	m_Armature->setPosition(180,50);
	CCMovementData *MovementData = m_Armature->getAnimation()->getAnimationData()->getMovement(Stand_Action);
	float speedScale = MovementData->scale;
	float frameNum = MovementData->duration /*+ MovementData->durationTo-1*/;
	float actionTime = frameNum * 1.0f/(60.0f*speedScale);	//帧数*帧速缩放比*默认帧速
	CCLOG("drationAll=%f,duration=%d,durationTo=%d,actionTime = %f,speedScale=%f",
		frameNum,MovementData->duration,MovementData->durationTo,actionTime,speedScale);
	m_Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(LogoScene::animationEvent));
	m_Body->addChild(m_Armature,2);

	m_ModeID = modeid;
	m_armatureEventData = m_armatureEventDataMgr->getArmatureEventData(m_ModeID);
}

void LogoScene::animationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	if (!m_Armature)
		return;
	std::string id = movementID;
	id = strRemoveSpace(id);
	if (movementType == LOOP_COMPLETE || COMPLETE==movementType)	//动作结束的标记
	{
		if (id.compare(Attack_Action) == 0						//普攻转站立
			||	id.compare(SpAttack_Action) == 0				//特攻转站立
			||	id.compare(Hit_Action) == 0						//受击转站立(连续受击问题)
			||	id.compare(Walk_Action) == 0					//走路转站立
			||	id.compare(Skill_Action) == 0)					//必杀技转站立
		{
			m_ActionKey = Stand_Action;
			m_Armature->getAnimation()->play(m_ActionKey.c_str(),0.01f);
			return;
		}
	}
}

void LogoScene::updateAction(float dt)
{
	if( !m_armatureEventData||!m_Armature		||
		m_ActionKey.compare(Stand_Action)==0	||
		m_ActionKey.compare(Hit_Action)==0		||
		m_ActionKey.compare(Dizzy_Action)==0)
	{
		return ;
	}
	int iCurrentFrameIndex = m_Armature->getAnimation()->getCurrentFrameIndex();
	if(m_lastFrame!=iCurrentFrameIndex)														//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
	{
		m_lastFrame = iCurrentFrameIndex;
		CCLOG("~~~~~~~~~~~~~~~%d",iCurrentFrameIndex);
		vector<ArmatureEvent*>& frameEvents = m_armatureEventData->getEventVector(m_ActionKey.c_str(), iCurrentFrameIndex);
		for(unsigned int i=0; i<frameEvents.size(); i++)
		{
			const ArmatureEvent& armatureEvent = *(frameEvents.at(i));
			string sEventName = armatureEvent.name;
			if(sEventName.size()>0)
			{
				sEventName = strRemoveSpace(sEventName);											//去空格
				if(sEventName.compare(EVE_ATKBEGIN)			==0	)
					CCLOG("AtkBegin index=%d",iCurrentFrameIndex);
				if(sEventName.compare(EVE_LOSTHP)			==0	)
					CCLOG("LostHp index=%d",iCurrentFrameIndex);
				if(sEventName.compare(EVE_ANIMAT)			==0 )
					CCLOG("AtkEnd index=%d",iCurrentFrameIndex);
				if(sEventName.compare(EVE_SHAKE)			==0	)
					CCLOG("Shark index=%d",iCurrentFrameIndex);
				//if(sEventName.compare(EVE_ANIMAT)			==0 )		PlayAnimat_Event(armatureEvent.extraInfo);			//播放动画
				if(sEventName.compare(EVE_SOUND)			==0 )
				{
					char str[60] = {0};
					sprintf(str,"SFX/Test/%d.mp3",armatureEvent.sound);											//使用宏将声音播出来  SFX_401
					PlayEffectSound(str);							//播放效果音效
					CCLOG("PlayerMusic index=%d",iCurrentFrameIndex);
				}
			}
		}
	}
}
//{
//		"EventName": "Sound",						//json格式
//		"Index": 6,
//		"soundNum":506
//} 
void LogoScene::onClick(CCObject* ob)
{
	CButton* btn = (CButton*)ob;
	int tag = btn->getTag();
	switch (tag)
	{		
	case GotoLogin:
		{
			CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoginScene));
		}break;
	case ModeID:
	case NetModel:
	case UpModel:
		{
			if (tag == NetModel)
			{
				m_ModeIndex++;
			}else if(tag == UpModel)
			{
				m_ModeIndex--;
			}else{
				testAnimation(atoi(m_InputModeId->getTextInput()->c_str()));
				return;
			}		
			if (m_ModeIndex<0)
				m_ModeIndex = m_VecModeId.size()-1;
			if (m_ModeIndex>m_VecModeId.size()-1)
				m_ModeIndex = 0;
			testAnimation(m_VecModeId.at(m_ModeIndex));
			return;
		}
	case TESTACTION:
	case UpAction:
	case NextAction:
	case RepeatCurrAction:
		{
			if (!m_Armature)return;
			if (TESTACTION == tag)
			{
				m_ActionIndex = atoi(m_InputAction->getTextInput()->c_str())-1;
			}else if(UpAction == tag)
			{
				m_ActionIndex--;
			}else if(NextAction == tag){
				m_ActionIndex++;
			}		
			if (m_ActionIndex > m_VecAction.size()-1)
				m_ActionIndex = 0;
			if (m_ActionIndex < 0)
				m_ActionIndex = m_VecAction.size()-1;
			m_ActionKey= m_VecAction.at(m_ActionIndex);
			CCMovementData *MovementData = m_Armature->getAnimation()->getAnimationData()->getMovement(m_ActionKey.c_str());
			if (MovementData)		//判断动作是否存在
			{
				m_Armature->getAnimation()->play(m_ActionKey.c_str(),0.01f);
			}else{
				m_Armature->getAnimation()->play(Stand_Action);
				char ErrorInfo_[60] = {0};
				sprintf(ErrorInfo_,"Lack Action < %s >",m_ActionKey.c_str());
				CCLabelTTF* _ERRORTIPS = (CCLabelTTF*)m_XmlLayer->getChildByTag(ERRORTIPS);
				_ERRORTIPS->setString(ErrorInfo_);
				m_ActionKey = Stand_Action;
			}
			CCLabelTTF* ActionName = (CCLabelTTF*)m_XmlLayer->getChildByTag(MSGTTF);
			ActionName->setString(m_ActionKey.c_str());
		}break;
	case ReLoadData:
		{
			ShaderDataMgr->loadShaderFile();
			m_armatureEventDataMgr->reloadData();
			m_armatureEventData = m_armatureEventDataMgr->getArmatureEventData(m_ModeID);
		}break;
	case ScaleNum:
		{
			if (!m_Armature)return;
			m_Armature->setScale(atof(m_InputScale->getTextInput()->c_str()));
		}break;
	case mapTest:
		{
			m_XmlLayer->removeChildByTag(mapImage);
			char str[60] = {0};
			sprintf(str,"mapImage/%d.xaml",atoi(m_InputMap->getTextInput()->c_str()));
			CLayout* mapLayer = LoadComponent(str);			
			if (mapLayer)
			{
				mapLayer->setScale(0.5f);
				mapLayer->setPosition(ccp(0,m_LogoSize.height/2));
				mapLayer->setAnchorPoint(ccp(0,0.5f));
				mapLayer->setTag(mapImage);
				m_XmlLayer->addChild(mapLayer,-1);
			}			
		}break;
	case TEST_Data:
		{

		}break;
	default:
		break;
	}
}

LogoScene::~LogoScene() 
{ 
	m_VecModeId.clear(); 
	if(m_Body) removeChild(m_Body);
	CC_SAFE_RELEASE(m_Body);
	m_Body = nullptr;
	CC_SAFE_RELEASE(m_armatureEventDataMgr);
	m_armatureEventDataMgr = nullptr;
}

void LogoScene::TestAction()
{
	m_armatureEventDataMgr = ArmatureEventDataMgr::create();
	m_armatureEventDataMgr->retain();
	this->schedule(schedule_selector(LogoScene::updateAction));
	CCLabelTTF* ActionNameStr = CCLabelTTF::create("",FONT_NAME,20);
	ActionNameStr->setString("1[standby]2[move]3[hit]4[dizzy]5[attack01]6[attack02]7[skill]8[win]9[die]10[start]");
	ActionNameStr->setColor(ccc3(100,100,0));
	ActionNameStr->setAnchorPoint(ccp(1,1));
	ActionNameStr->setPosition(ccp(m_LogoSize.width-50,m_LogoSize.height-50));
	m_XmlLayer->addChild(ActionNameStr);

	m_VecAction.push_back("standby");
	m_VecAction.push_back("move");
	m_VecAction.push_back("hit");
	m_VecAction.push_back("dizzy");
	m_VecAction.push_back("attack01");
	m_VecAction.push_back("attack02");
	m_VecAction.push_back("skill");
	m_VecAction.push_back("win");
	m_VecAction.push_back("die");
	m_VecAction.push_back("start");

	CButton* Next = CButton::create();
	Next->setEnabled(true);
	Next->setTag(NextAction);
	Next->setNormalImage("common/button_04_on.png");
	Next->setSelectedImage("common/button_04_off.png");
	Next->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	Next->setPosition(ccp(m_LogoSize.width-320,m_LogoSize.height-300));
	m_XmlLayer->addChild(Next);

	CButton* Up = CButton::create();
	Up->setEnabled(true);
	Up->setTag(UpAction);
	Up->setNormalImage("common/button_04_on.png");
	Up->setSelectedImage("common/button_04_off.png");
	Up->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	Up->setPosition(ccp(m_LogoSize.width-340-Up->getContentSize().width,m_LogoSize.height-300));
	m_XmlLayer->addChild(Up);

	CButton* curr = CButton::create();
	curr->setEnabled(true);
	curr->setTag(RepeatCurrAction);
	curr->setNormalImage("common/button_04_on.png");
	curr->setSelectedImage("common/button_04_off.png");
	curr->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	curr->setPosition(ccp(m_LogoSize.width-340-Up->getContentSize().width,m_LogoSize.height-300-curr->getContentSize().height));
	m_XmlLayer->addChild(curr);

	CButton* ReLoad = CButton::create();
	ReLoad->setScale(0.6f);
	ReLoad->setEnabled(true);
	ReLoad->setTag(ReLoadData);
	ReLoad->setNormalImage("common/button_04_on.png");
	ReLoad->setSelectedImage("common/button_04_off.png");
	ReLoad->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	ReLoad->setPosition(ccp(m_LogoSize.width-440-Up->getContentSize().width,m_LogoSize.height-360+curr->getContentSize().height));
	m_XmlLayer->addChild(ReLoad);
	CCLabelTTF*Reload = CCLabelTTF::create("ReLoadData",FONT_NAME,30);
	Reload->setColor(ccc3(0,255,0));
	Reload->setPosition(ccp(ReLoad->getContentSize().width/2,ReLoad->getContentSize().height/2));
	ReLoad->addChild(Reload);
	//动作测试
	CButton* btInput2 = CButton::create();
	btInput2->setScale(0.5f);
	btInput2->setEnabled(true);
	btInput2->setNormalImage("common/button_04_on.png");
	btInput2->setSelectedImage("common/button_04_off.png");
	btInput2->setColor(ccc3(0,200,0));
	btInput2->setTag(TESTACTION);
	btInput2->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	btInput2->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-350));
	m_XmlLayer->addChild(btInput2);
	CCLabelTTF* testAction = CCLabelTTF::create("TestAction",FONT_NAME,40);
	testAction->setColor(ccc3(0,255,0));
	btInput2->addChild(testAction);

	//for (int i=0;i<=14;i++)
	//{
	//	char str[30]={0};
	//	sprintf(str,"SFX/Test/%d.mp3",i+500);
	//	SimpleAudioEngine::sharedEngine()->preloadEffect(str);
	//}

	CCSprite* SpInput2 = CCSprite::create("common/button_37.png");
	SpInput2->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-300));
	m_InputAction =  CursorTextField::textFieldWithPlaceHolder("InPut Action Index", FONT_NAME, 15,CCSizeMake(150,50),ccWHITE); 
	m_InputAction->setPriority(-1);
	m_InputAction->setLimitNum(20);
	m_InputAction->setPosition(ccp(SpInput2->getContentSize().width/2,SpInput2->getContentSize().height/2));
	SpInput2->addChild(m_InputAction);

	m_XmlLayer->addChild(SpInput2);
}

void LogoScene::TestModel()
{
	int modeid[] = {
		111,121,131,146,151,156,161,306,311,316,321,331,336,346,351,506,516,521,
		526,536,541,546,551,556,556,701,706,708,709,711,713,714,716,718,719,721,
		723,724,731,733,734,746,751,756,761,816,821,826,831,1001,1011,1016,1021,
		1037,1038,1039,1051,1056,1061,1066,1071,1076,1081,8011,8016
	};
	for (int i=0;i<sizeof(modeid)/sizeof(modeid[0]) - 1;i++)
		m_VecModeId.push_back(modeid[i]);
	CButton* Next = CButton::create();
	Next->setEnabled(true);
	Next->setTag(NetModel);
	Next->setNormalImage("common/button_04_on.png");
	Next->setSelectedImage("common/button_04_off.png");
	Next->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	Next->setPosition(ccp(m_LogoSize.width-320,m_LogoSize.height-450));
	m_XmlLayer->addChild(Next);

	CButton* Up = CButton::create();
	Up->setEnabled(true);
	Up->setTag(UpModel);
	Up->setNormalImage("common/button_04_on.png");
	Up->setSelectedImage("common/button_04_off.png");
	Up->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	Up->setPosition(ccp(m_LogoSize.width-340-Up->getContentSize().width,m_LogoSize.height-450));
	m_XmlLayer->addChild(Up);

	CButton* btInput1 = CButton::create();
	btInput1->setScale(0.5f);
	btInput1->setEnabled(true);
	btInput1->setNormalImage("common/button_04_on.png");
	btInput1->setSelectedImage("common/button_04_off.png");
	btInput1->setColor(ccc3(0,200,0));
	btInput1->setTag(ModeID);
	btInput1->setOnClickListener(this,ccw_click_selector(LogoScene::onClick));
	btInput1->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-500));
	m_XmlLayer->addChild(btInput1);
	CCLabelTTF* testMove = CCLabelTTF::create("TestModel",FONT_NAME,40);
	testMove->setColor(ccc3(0,255,0));
	btInput1->addChild(testMove);

	CCSprite* SpInput1 = CCSprite::create("common/button_37.png");
	SpInput1->setPosition(ccp(m_LogoSize.width-200,m_LogoSize.height-450));
	m_InputModeId =  CursorTextField::textFieldWithPlaceHolder("InPut Mode ID Much Is Number", FONT_NAME, 15,CCSizeMake(150,50),ccWHITE); 
	m_InputModeId->setPriority(-1);
	m_InputModeId->setLimitNum(20);
	m_InputModeId->setPosition(ccp(SpInput1->getContentSize().width/2,SpInput1->getContentSize().height/2));
	SpInput1->addChild(m_InputModeId);

	m_XmlLayer->addChild(SpInput1);
}

void LogoScene::hueChange( CCObject *pObj, CCControlEvent controlEvent )
{
	CCControlSlider* pSlider = (CCControlSlider*)pObj;
	float fValue = pSlider->getValue();
	if(m_Armature)
		m_Armature->setShaderProgram(ShaderDataMgr->getShaderByType((ShaderType)(int(fValue))));
}
//快速降序排序
void Antitone_QuickSort(vector<int>&Vec,int left,int right)
{
	if (right>left)
	{
		int i=left;
		int j=right;
		int saveNum = Vec.at(i);
		while (i<j)								//循环判断,直到前面的数比标记的数大,后面的数比标记的数小
		{
			while (i<j&&saveNum>=Vec.at(j))			
				j--;
			if (i<j)
			{
				Vec.at(i)=Vec.at(j);			//将比标记为大的一个数放到前面
				i++;
			}
			while (i<j&&Vec.at(i)>saveNum)
				i++;
			if(i<j)
			{
				Vec.at(j) = Vec.at(i);			//将比标记为小的一个数放到后面
				j--;
			}
		}
		Vec.at(i) = saveNum;
		Antitone_QuickSort(Vec,left,i-1);
		Antitone_QuickSort(Vec,i+1,right);
	}
}


#include <string.h>
#include <random>				//随机数
#include <mutex>

USING_NS_CC;
using namespace spine;
using namespace std;


SpineboyExample::SpineboyExample()
	:m_skeletonNode(nullptr),m_index(0),m_Layer(nullptr),m_Node(nullptr)
	,m_LoadSpineNum(0),m_TotalSpineNum(0),m_Effect(nullptr)
{}

SpineboyExample::~SpineboyExample()
{
	m_VecName.clear();
	for (auto i:m_MapData)
	{
		spSkeletonData_dispose(i.second.first);
		spAtlas_dispose(i.second.second);
	}
	m_MapData.clear();
}

void SpineboyExample::InitLayer( CLayout* layer )
{
	if (!layer)
		return;
	m_Layer = layer;
	CButton* LoginBtn = (CButton*)m_Layer->getChildByTag(Login_Spine);
	LoginBtn->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* SizeScale = (CButton*)m_Layer->getChildByTag(SizeScale_Spine);
	SizeScale->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* SpeedScale = (CButton*)m_Layer->getChildByTag(SpeedScale_Spine);
	SpeedScale->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* ActionIndex = (CButton*)m_Layer->getChildByTag(ActionIndex_Spine);
	ActionIndex->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* ModelID = (CButton*)m_Layer->getChildByTag(ModelID_Spine);
	ModelID->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Load = (CButton*)m_Layer->getChildByTag(Load_Spine);
	Load->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Release = (CButton*)m_Layer->getChildByTag(Release_Spine);
	Release->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Next = (CButton*)m_Layer->getChildByTag(Next_Spine);
	Next->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Repeat = (CButton*)m_Layer->getChildByTag(Repeat_Spine);
	Repeat->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Previous = (CButton*)m_Layer->getChildByTag(Previous_Spine);
	Previous->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Create = (CButton*)m_Layer->getChildByTag(Create_Spine);
	Create->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* Remove = (CButton*)m_Layer->getChildByTag(Remove_Spine);
	Remove->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));
	CButton* ShowLoad = (CButton*)m_Layer->getChildByTag(ShowLoad_Spine);
	ShowLoad->setOnClickListener(this,ccw_click_selector(SpineboyExample::onClick));

	CursorTextField*c_SizeScale =  CursorTextField::textFieldWithPlaceHolder("InPut Size Scale", FONT_NAME, 20,CCSizeMake(150,25),ccWHITE); 
	c_SizeScale->setAnchorPoint(ccp(0,1));
	c_SizeScale->setPriority(-1);
	c_SizeScale->setLimitNum(20);
	c_SizeScale->setTag(get_SizeScale);
	c_SizeScale->setPosition(ccpAdd(m_Layer->getChildByTag(Input_SizeScale)->getPosition(),ccp(20,35)));
	m_Layer->addChild(c_SizeScale);	

	CursorTextField*c_SpeedScale =  CursorTextField::textFieldWithPlaceHolder("InPut Speed Scale", FONT_NAME, 20,CCSizeMake(150,25),ccWHITE); 
	c_SpeedScale->setAnchorPoint(ccp(0,1));
	c_SpeedScale->setPriority(-1);
	c_SpeedScale->setLimitNum(20);
	c_SpeedScale->setPosition(ccpAdd(m_Layer->getChildByTag(Input_SpeedScale)->getPosition(),ccp(20,35)));
	c_SpeedScale->setTag(get_SpeedScale);
	m_Layer->addChild(c_SpeedScale);

	CursorTextField*c_ActionIndex =  CursorTextField::textFieldWithPlaceHolder("InPut Action Index", FONT_NAME, 20,CCSizeMake(150,25),ccWHITE); 
	c_ActionIndex->setAnchorPoint(ccp(0,1));
	c_ActionIndex->setPriority(-1);
	c_ActionIndex->setLimitNum(20);
	c_ActionIndex->setPosition(ccpAdd(m_Layer->getChildByTag(Input_ActionIndex)->getPosition(),ccp(20,35)));
	c_ActionIndex->setTag(get_ActionIndex);
	m_Layer->addChild(c_ActionIndex);

	CursorTextField*c_ModelID =  CursorTextField::textFieldWithPlaceHolder("InPut Mode ID", FONT_NAME, 20,CCSizeMake(150,25),ccWHITE); 
	c_ModelID->setAnchorPoint(ccp(0,1));
	c_ModelID->setPriority(-1);
	c_ModelID->setLimitNum(20);
	c_ModelID->setPosition(ccpAdd(m_Layer->getChildByTag(Input_ModelID)->getPosition(),ccp(20,35)));
	c_ModelID->setTag(get_ModelID);
	m_Layer->addChild(c_ModelID);
}

bool SpineboyExample::init () {
	if (!CCLayerColor::initWithColor(ccc4(128, 128, 128, 255))) return false;
	m_Node = CCNode::create();
	this->addChild(m_Node);
	//ThreadTest();
	m_VecSpine.push_back(146);
	m_VecSpine.push_back(100);
	m_VecSpine.push_back(381);

	m_ActionName.push_back(Stand_Action);
	m_ActionName.push_back(Walk_Action);
	m_ActionName.push_back(Hit_Action);
	m_ActionName.push_back(Dizzy_Action);
	m_ActionName.push_back(Attack_Action);
	m_ActionName.push_back(SpAttack_Action);
	m_ActionName.push_back(Skill_Action);
	m_ActionName.push_back(Win_Action);
	m_ActionName.push_back(Die_Action);
	m_ActionName.push_back(Start_Action);
	return true;
}

void SpineboyExample::DefaultCreat(int modeID)
{
	if (m_skeletonNode)
	{
		m_skeletonNode->removeFromParentAndCleanup(true);
		m_skeletonNode = nullptr;
	}
	char json[60] = {0};
	char altlas[60] = {0};
	sprintf(json,"Spine/%d.json",modeID);
	sprintf(altlas,"Spine/%d.atlas",modeID);
	m_skeletonNode = SkeletonAnimation::createWithFile(json, altlas, 1);
	m_skeletonNode->startListener = [this] (int trackIndex) {//设置动作开始检测的监听器  传入this就可以使用this的成员了
		spTrackEntry* entry = spAnimationState_getCurrent(m_skeletonNode->state, trackIndex);//获得当前动作的状态信息  
		const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;  
		CCLog("%d start: %s", trackIndex, animationName);  
	};  
	//m_skeletonNode->endListener = [] (int trackIndex) {//同理，结束动作监听  
	//	CCLog("%d end", trackIndex);  
	//};  
	EndCount = 1;
	m_skeletonNode->endListener = std::bind(&SpineboyExample::ActionEnd,this,EndCount,std::placeholders::_1);

	m_skeletonNode->completeListener = [] (int trackIndex, int loopCount) {//同理，完成动作监听  
		CCLog("%d complete: %d", trackIndex, loopCount);  
	};  
	m_skeletonNode->eventListener = [] (int trackIndex, spEvent* event) {//同理，事件监听，在spine中编辑好event导出  
		CCLog("%d event: %s, %d, %f, %s", trackIndex, event->data->name, event->intValue, event->floatValue, event->stringValue);  
	};  
	m_VecName.clear();
	for (int i=0;i<m_skeletonNode->state->data->skeletonData->animationsCount;i++)
	{
		m_VecName.push_back(m_skeletonNode->state->data->skeletonData->animations[i]->name);
		CCLog("Action Name=%s",m_skeletonNode->skeleton->data->animations[i]->name);						//2个文件的读取方式都是一样的
	}
	CCLabelTTF* num = (CCLabelTTF*)m_Layer->getChildByTag(ActionCountLab_Spine);
	char str[20] = {0};
	sprintf(str,"%d",m_skeletonNode->state->data->skeletonData->animationsCount);
	num->setString(string(str).c_str());
	//当要连续播放不同的动画时，需要使用addAnimation方法来实现，它可以一条一条的播放不同的动画。
	int index = CCRANDOM_0_1()*(m_skeletonNode->state->data->skeletonData->animationsCount-1);
	m_skeletonNode->setAnimation(0,m_skeletonNode->state->data->skeletonData->animations[index]->name,false);
	//spTrackEntry* jumpEntry2 = m_skeletonNode->addAnimation(0, "skill", false, 3);//上个动作播放延迟3秒之后再播放动作

	////动作监听事件
	//m_skeletonNode->setEndListener(jumpEntry2,[](int trackIndex){                  
	//	CCLog(" EndListener !");
	//});

	m_skeletonNode->setPosition(ccp(500,80));
	m_skeletonNode->setTag(Spine_Animation);
	m_Node->addChild(m_skeletonNode);
}

void SpineboyExample::ActionEnd( int Count,int trackIndex )
{
	EndCount++;
	int index = CCRANDOM_0_1()*(m_skeletonNode->state->data->skeletonData->animationsCount-1);
	//m_skeletonNode->setAnimation(0,m_skeletonNode->state->data->skeletonData->animations[index]->name,false);

	CCLog("~~~~~~~test=%d",Count);
	CCLog("~~~~~~~test2=%d",EndCount);
	CCLog("action index = %d",trackIndex);
}

std::pair<spSkeletonData*,spAtlas*>* SpineboyExample::getData( std::string name )
{
	map<std::string,std::pair<spSkeletonData*,spAtlas*>>::iterator iter = m_MapData.find(name);
	if (iter != m_MapData.end())
		return &iter->second;
	//CCLog("[ *ERROR ] SpineboyExample::getData Null Name =%s",name.c_str());
	return nullptr;
}
//做异步加载测试试验
void SpineboyExample::LoadSpineData( int ID,bool isModel/*=true*/ )
{
	char c_json[60] = {0};
	char c_altlas[60] = {0};
	char c_effect[60]={0};
	if (isModel)
	{
		if (getData(ToString(ID)))
		{
			CCLog("[ *Tips ] SpineboyExample::LoadSpineData Load Spine Repeat =%d",ID);
			return;
		}
		sprintf(c_json,"Spine/%d.json",ID);
		sprintf(c_altlas,"Spine/%d.atlas",ID);
	}else{
		sprintf(c_effect,"%d_texiao",ID);
		if (getData(c_effect))
		{
			CCLog("[ *Tips ] SpineboyExample::LoadSpineData Load Spine Repeat =%d",ID);
			return;
		}
		sprintf(c_json,"Spine/%d_texiao.json",ID);
		sprintf(c_altlas,"Spine/%d_texiao.atlas",ID);
	}
	spAtlas*atlas = spAtlas_createFromFile(c_altlas, 0);
	if (!atlas)
	{
		if (isModel)
			CCAssert(atlas, "Error reading atlas file.");
		return;
	}
	spSkeletonJson* atlas_json = spSkeletonJson_create(atlas);
	atlas_json->scale = 1.0f;//骨骼动画大小
	spSkeletonData* skeletonData = spSkeletonJson_readSkeletonDataFile(atlas_json, c_json);
	if (isModel)
		CCAssert(skeletonData, atlas_json->error ? atlas_json->error : "Error reading skeleton data file.");
	spSkeletonJson_dispose(atlas_json);
	
	if (isModel)
	{
		AddSpineData(ToString(ID),skeletonData,atlas);
	}else{
		AddSpineData(c_effect,skeletonData,atlas);
	}
}

void SpineboyExample::AddSpineData( const char* name,spSkeletonData* skeletonData,spAtlas*atlas )
{
	if (getData(name))
	{
		CCLOG("[ *Tips ]SpineboyExample::AddSpineData Add Repead = %s",name);
		return;
	}
	m_MapData[name] = make_pair(skeletonData,atlas);
	CCLOG("SpineboyExample::AddSpineData  Load Succeed SpineName = %s",name);
}

bool SpineboyExample::SpineThread( vector<int>& Vec,bool LoadData /*= false*/ )
{
	if (LoadData)
	{
		for (auto i :Vec)
			LoadSpineData(i); 
		for (auto i :Vec)
			LoadSpineData(i,false); 
		return true;
	}else{											//加载纹理图片
		for (auto i : Vec)
		{
			SpineAsyncLoadImage(i);					//加载模型图片
			SpineAsyncLoadImage(i,false);			//加载特效图片
		}
		if (!m_TotalSpineNum)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			std::packaged_task<bool(vector<int>&,bool)>dTask(std::bind(&SpineboyExample::SpineThread,this,std::placeholders::_1,std::placeholders::_2));		//spine异步加载处理
			pFuture = dTask.get_future();
			std::thread cthread(std::move(dTask),m_VecSpine,true);
			cthread.detach();
#endif
		}
	}
	return false;
}

void SpineboyExample::SpineAsyncLoadImage( int ModelID,bool isModel /*= true*/ )
{
	char pngStr[60] = {0};
	if (isModel)
		sprintf(pngStr,"Spine/%d.png",ModelID);	
	else
		sprintf(pngStr,"Spine/%d_texiao.png",ModelID);
	//CCLOG("SpineboyExample::SpineAsyncLoadImage Load %s",pngStr);
	std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pngStr);
	if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
	{
		CCTexture2D* Texture = CCTextureCache::sharedTextureCache()->textureForKey(pngStr);
		if (!Texture)
		{
			m_TotalSpineNum++;
			CCTextureCache::sharedTextureCache()->addImageAsync(pngStr,this,callfuncO_selector(SpineboyExample::SpineThreadCallBack));
		}
	}else{
		CCLOG("[ *ERROR ]SpineboyExample::SpineAsyncLoadImage SpineFile Is NULL");
	}
	
	int MaxImageNum = 10;						//假设一个spine动画最多10张图片
	for(int i=2;i < MaxImageNum;i++)
	{
		if (isModel)
			sprintf(pngStr,"Spine/%d%d.png",ModelID,i);
		else
			sprintf(pngStr,"Spine/%d_texiao%d.png",ModelID,i);
		strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pngStr);
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))//判断文件是否存在
		{
			CCTexture2D* Texture = CCTextureCache::sharedTextureCache()->textureForKey(pngStr);
			if (Texture)
				continue;
			m_TotalSpineNum++;
			CCTextureCache::sharedTextureCache()->addImageAsync(pngStr,this,callfuncO_selector(SpineboyExample::SpineThreadCallBack));
			//CCLOG("SpineboyExample::SpineAsyncLoadImage Load %s",pngStr);
		}else{
			break;
		}
	}
}

void SpineboyExample::SpineThreadCallBack( CCObject* ob )
{
	m_LoadSpineNum++;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (m_LoadSpineNum < m_TotalSpineNum )
		return;
	std::packaged_task<bool(vector<int>&,bool)>dTask(std::bind(&SpineboyExample::SpineThread,this,std::placeholders::_1,std::placeholders::_2));		//spine异步加载处理
	pFuture = dTask.get_future();
	std::thread cthread(std::move(dTask),m_VecSpine,true);
	cthread.detach();
	if (pFuture.get())
	{
		if (!m_CreateID)
			return;
		CreateAnimation(m_CreateID);
	}
#endif
	
}

void SpineboyExample::CreateSkeletonByData( int id )
{
	if (id == m_CreateID)
	{
		CreateAnimation(id);
	}else{
		CCLabelTTF* error = (CCLabelTTF*)m_Layer->getChildByTag(ErrorLab_Spine);
		std::pair<spSkeletonData*,spAtlas*> * data = getData(ToString(id));
		if (data)
		{
			int x = CCRANDOM_0_1()*420+80;
			int y = CCRANDOM_0_1()*330+30;
			SkeletonAnimation* skAnimation = SkeletonAnimation::createWithData(data->first);
			int index = CCRANDOM_0_1()*(skAnimation->state->data->skeletonData->animationsCount-1);
			skAnimation->setAnimation(0,skAnimation->state->data->skeletonData->animations[index]->name,true);
			skAnimation->setPosition(ccp(x,y));
			m_Node->addChild(skAnimation);

			char str[60] = {0};
			sprintf(str," Create SkeletonAnimation Num=%d",m_Node->getChildrenCount());
			error->setString(string(str).c_str());
		}else{
			error->setString("CreateSkeletonByData DATA IS NULL");
		}
	}
}

void SpineboyExample::SpineRoleAndEffect( int id )
{
	if (id == m_CreateID)
		return;
	m_VecSpine.clear();
	m_VecSpine.push_back(id);
	SpineThread(m_VecSpine);
	m_CreateID = id;
}

void SpineboyExample::CreateAnimation( int id )
{
	//c++11 获取随机数的方法
	default_random_engine random(time(NULL));				//生成随机无符号数(默认随机种子是time)
	uniform_int_distribution<unsigned> x_range(80,420);		//生成80到420（包含）均匀分布的随机数 
	uniform_int_distribution<unsigned> y_range(30,330);		//生成30到330（包含）均匀分布的随机数 
	std::function<int(void)> RanDomX = std::bind(x_range,random);
	std::function<int(void)> RanDomY = std::bind(y_range,random);
	int x = RanDomX();
	int y = RanDomY();

	std::pair<spSkeletonData*,spAtlas*>* data = getData(ToString(id));
	if (data)
	{
		m_skeletonNode = SkeletonAnimation::createWithData(data->first);
		int index = CCRANDOM_0_1()*(m_skeletonNode->state->data->skeletonData->animationsCount-1);
		m_skeletonNode->setAnimation(0,Stand_Action,true);
		m_skeletonNode->setScale(1);
		m_skeletonNode->setPosition(ccp(x,y));
		m_Node->addChild(m_skeletonNode);
		m_skeletonNode->completeListener = std::bind(&SpineboyExample::SpineComplete,this,std::placeholders::_1,std::placeholders::_2);
		m_skeletonNode->eventListener = std::bind(&SpineboyExample::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
	}

	char efName[60] = {0};
	sprintf(efName,"%d_texiao",id);
	std::pair<spSkeletonData*,spAtlas*>* EFdata = getData(efName);
	if (EFdata)
	{
		m_Effect = SkeletonAnimation::createWithData(EFdata->first);
		m_Effect->setPosition(ccp(x,y));
		m_Effect->setScale(1);
		m_Node->addChild(m_Effect);
		for (int i=0;i<m_Effect->state->data->skeletonData->animationsCount;i++)
			CCLOG("[ Tips ] SpineboyExample::SpineThreadCallBack Effect Action Name=%s",m_Effect->state->data->skeletonData->animations[i]->name);
	}
	for (int i=0;i<m_skeletonNode->state->data->skeletonData->animationsCount;i++)
		m_VecName.push_back(m_skeletonNode->state->data->skeletonData->animations[i]->name);
	for (auto i:m_VecName)
	{
		bool ErrorActionName = true;
		for (auto j : m_ActionName)
		{
			if (strcmp(i.c_str(),j.c_str())==0)
			{
				ErrorActionName = false;
				break;
			}
		}
		if (ErrorActionName)
		{
			CCLabelTTF* error = (CCLabelTTF*)m_Layer->getChildByTag(ErrorLab_Spine);
			char str[60] = {0};
			sprintf(str,"ERROR: Action Name =%s",i.c_str());
			error->setString(str);
			CCLOG("[ *ERROR ] Logo::CreateAnimation Model=%d,ActionName=%s NameError",id,i.c_str());
		}
	}
}

void SpineboyExample::SpineActionEvent( int trackIndex,spEvent* Event )
{
	if (Event->intValue >= 200)			//音效从200号文件开始播放
	{
		char str[60] = {0};
		sprintf(str,"SFX/Test/%d.mp3",Event->intValue);	
		PlayEffectSound(str);				//播放效果音效
	}
}

void SpineboyExample::SpineComplete( int trackIndex,int loopCount )
{
	spTrackEntry*TrackEntry = m_skeletonNode->getCurrent(trackIndex);
	if (TrackEntry)
	{
		if (m_Effect)
			m_Effect->setToSetupPose();
		if (strcmp(TrackEntry->animation->name,Stand_Action)==0 ||
			strcmp(TrackEntry->animation->name,Walk_Action)==0  )
			return;
		m_skeletonNode->setAnimation(0,Stand_Action,true);
	}else{
		CCLOG("[ *ERROR ] RoleObject::SpineComplete spTrackEntry NULL");
	}
}

void SpineboyExample::onClick( CCObject* ob )
{
	CButton* btn = (CButton*)ob;
	CCLabelTTF* error = (CCLabelTTF*)m_Layer->getChildByTag(ErrorLab_Spine);
	if (!m_skeletonNode)error->setString("SkeletonNode IS NULL");
	switch (btn->getTag())
	{
	case Login_Spine:
		{
			CSceneManager::sharedSceneManager()->replaceScene(GETSCENE(LoginScene));
		}break;
	case SizeScale_Spine:
		{
			if (!m_skeletonNode)
				break;
			CursorTextField*c_SizeScale = (CursorTextField*)m_Layer->getChildByTag(get_SizeScale);
			m_skeletonNode->setScale(atof(c_SizeScale->getTextInput()->c_str()));
			if (m_Effect)
				m_Effect->setScale(atof(c_SizeScale->getTextInput()->c_str()));
		}break;
	case SpeedScale_Spine:
		{
			if (!m_skeletonNode)
				break;
			CursorTextField*c_SpeedScale = (CursorTextField*)m_Layer->getChildByTag(get_SpeedScale);
			m_skeletonNode->timeScale = (atoi(c_SpeedScale->getTextInput()->c_str()));
		}break;
	case ActionIndex_Spine:
	case Next_Spine:
	case Previous_Spine:
	case Repeat_Spine:
		{
			if (!m_skeletonNode)
				break;
			m_skeletonNode->clearTracks();

			if (btn->getTag() == ActionIndex_Spine)
			{
				CursorTextField*c_ActionIndex = (CursorTextField*)m_Layer->getChildByTag(get_ActionIndex);
				m_index = atoi(c_ActionIndex->getTextInput()->c_str());
			}else if (btn->getTag() == Next_Spine)
			{
				if (m_index < m_VecName.size()-1)
					m_index++;
				else
					m_index = 0;
			}else if (btn->getTag() == Previous_Spine)
			{
				if (m_index > 0)
					m_index--;
				else
					m_index = m_VecName.size()-1;
			}
			if (m_index >= 0 || m_index <= m_VecName.size()-1)
			{
				CCLabelTTF* name = (CCLabelTTF*)m_Layer->getChildByTag(ActionNameLab_Spine);
				name->setString(m_VecName.at (m_index).c_str());
				m_skeletonNode->setAnimation(0,m_VecName.at(m_index).c_str(),true);
				if (m_Effect)
				{
					for (int i = 0;i<m_Effect->skeleton->data->animationsCount;i++)
					{
						if (strcmp(m_Effect->skeleton->data->animations[i]->name,m_VecName.at(m_index).c_str())==0)
						{
							m_Effect->setVisible(true);
							m_Effect->setAnimation(0,m_VecName.at(m_index).c_str(),false);
							return;
						}
					}
					m_Effect->setVisible(false);
					m_Effect->setToSetupPose();
				}
			}else{
				char str[60] = {0};
				sprintf(str," Input Index > Model Action Size index = %d , size =%d",m_index,m_VecName.size());
				error->setString(string(str).c_str());
			}
		}break;
	case ModelID_Spine:
		{
			CursorTextField*c_ModelID = (CursorTextField*)m_Layer->getChildByTag(get_ModelID);
			//DefaultCreat(atoi(c_ModelID->getTextInput()->c_str()));
			SpineRoleAndEffect(atoi(c_ModelID->getTextInput()->c_str()));
		}break;
	case Load_Spine:
		{
			////同步方法
			//CursorTextField*c_ModelID = (CursorTextField*)m_Layer->getChildByTag(get_ModelID);
			//LoadSkeletonData(atoi(c_ModelID->getTextInput()->c_str()));

			//异步方法1
			//std::thread LoadThread(&SpineboyExample::testAsync,this);
			//LoadThread.detach();
			//异步方法2
			//std::future<bool> fut = std::async(&SpineboyExample::testAsync,this);
			//if (fut.get())
			//	CCLog(" Load Succeed !");
			//异步方法3

			//CCTexture2D* texture1 = CCTextureCache::sharedTextureCache()->addImage("Spine/146.png");	
			//CCTexture2D* texture2 = CCTextureCache::sharedTextureCache()->addImage("Spine/381.png");
			//CCTexture2D* texture3 = CCTextureCache::sharedTextureCache()->addImage("Spine/100.png");

			//std::packaged_task<bool()>dTask(std::bind(&SpineboyExample::testAsync,this));		//spine异步加载处理
			//std::future<bool> pFuture = dTask.get_future();
			//std::thread cthread(std::move(dTask));
			//cthread.detach();
			//if (pFuture.get())
			//	CCLog(" Load Succeed !");
			SpineThread(m_VecSpine);
		}break;
	case Release_Spine:
		{
			m_Node->removeAllChildrenWithCleanup(true);
			m_skeletonNode = nullptr;
			for (auto i:m_MapData)
			{
				spSkeletonData_dispose(i.second.first);
				spAtlas_dispose(i.second.second);
				CCLog("[ *Tips ] SpineboyExample::onClick Release Name=%s",i.first.c_str());
			}
			m_MapData.clear();
			m_LoadSpineNum = 0;
			m_TotalSpineNum = 0;
			m_CreateID = 0;
			CCTextureCache::sharedTextureCache()->removeUnusedTextures();				//清理掉所有的不使用的图片
			error->setString("Map Data Release Succeed");
		}break;
	case Create_Spine:
		{
			CursorTextField*c_ModelID = (CursorTextField*)m_Layer->getChildByTag(get_ModelID);
			CreateSkeletonByData(atoi(c_ModelID->getTextInput()->c_str()));
		}break;
	case Remove_Spine:
		{
			m_Node->removeAllChildrenWithCleanup(true);
			m_skeletonNode = nullptr;
		}break;
	case ShowLoad_Spine:
		{
			for (auto i : m_MapData)
				CCLog(" Map Has id=%s",i.first.c_str());
			char str[20] = {0};
			sprintf(str,"Map Data Size = %d",m_MapData.size());
			error->setString(string(str).c_str());
		}break;
	default:break;
	}
}

void SpineboyExample::ThreadTest()
{
	//this(可使用this的成员) 去调用 AddFun的指针 传入参数_1,_2
	std::function<int(int,int)> pce = bind(&SpineboyExample::AddFun,this,std::placeholders::_2,std::placeholders::_1,3);
	CCLog("AddFun Num = %d \n",pce(5,6));

	////预定义文件路径
	//std::vector<std::string> SVec;
	//SVec.push_back("Spine");
	//CCFileUtils::sharedFileUtils()->setSearchResolutionsOrder(SVec);
	//CCSprite* sp = CCSprite::create("100.png");	//图片的实际路径是 Spine/100.png
	//sp->setPosition(ccp(200,200));
	//this->addChild(sp);

	//c++11 获取随机数的方法
	default_random_engine e1;					//生成随机无符号数(默认随机种子)
	for (int i=0;i<3;i++)
		CCLog("%d\n",i);
	uniform_int_distribution<unsigned> u(0,10);//生成0到10（包含）均匀分布的随机数  
	default_random_engine e2;					//生成随机无符号数  
	for(int i=0;i<10;++i)  
		CCLog("%d\n",u(e2));					//u的范围范围便是e生成的随机数范围 
	default_random_engine e3(100);				//给定随机种子100
	default_random_engine e4;
	e4.seed(100);								//设置e4的随机种子为100

	//CCLog("this thread id = %d",std::this_thread::get_id());
	//std::thread LoadThread(&SpineboyExample::LoadSpine,this,1332,this);
	//LoadThread.join();			//等待子线程执行完后再执行主线程
	//LoadThread.detach();			//子线程和主线程分离处理,创建一个分支线程调用LoadSpine函数
	//std::mutex mutex;				//不可重入互斥锁 线程互斥对象  
	//mutex.lock();					//有lock必须有unlock否则会造成死锁的情况出现。
	//std::this_thread::sleep_for (chrono::seconds(10));	//让当前线程睡眠10S

	//如果多线程的函数有返回值的情况
	//std::packaged_task<int(int , SpineboyExample* ob)> pTask(std::bind(&SpineboyExample::LoadSpine,this,std::placeholders::_1,std::placeholders::_2));
	//std::future<int> pFuture = pTask.get_future();
	//std::thread pthread(std::move(pTask),1132,this);//move将左值转化为右值的引用相对于将threadTask转化为std::bind(&SpineboyExample::LoadSpine,this,1332,this)
	//pthread.detach();			
	//CCLOG("future = %d \n",pFuture.get());

	//std::future<int> fut = std::async(&SpineboyExample::LoadSpine,this,1332,this);
	//CCLOG("future = %d \n",fut.get());
}

int SpineboyExample::LoadSpine( int Num ,SpineboyExample* ob)
{
	//Sleep(10);  
	CCLog("LoadSpine   %d",Num);
	ob->showThread();
	return 6363;
}

void SpineboyExample::showThread()
{
	CCLog("showThread");
}

int SpineboyExample::AddFun( int a,int b,int c )
{
	int d = a + c;
	int e = d*b;
	CCLog("AddFun a=%d,b=%d,c=%d",a,b,c);
	return e;
}