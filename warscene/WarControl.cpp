
#include "WarControl.h"
#include "Global.h"
#include "GMessage.h"
#include "scene/CPopTip.h"
#include "scene/loadWar.h"
#include "tools/CCShake.h"
#include "scene/WarScene.h"
#include "model/DataCenter.h"
#include "model/DataDefine.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "tools/ShowTexttip.h"
#include "warscene/ConstNum.h"
#include "cctk/scenemanager.h"
#include "warscene/WarAssist.h"
#include "warscene/SkillTips.h"
#include "warscene/CombatTask.h" 
#include "common/ProgressLabel.h"
#include "warscene/CaptainSkill.h"
#include "common/CommonFunction.h"	
#include "warscene/ComBatLogic.h"
#include "scene/AnimationManager.h"
#include "scene/layer/PauseLayer.h"
#include "scene/alive/AliveDefine.h"
#include "scene/layer/WarAliveLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/effect/EffectObject.h"
#include "warscene/GameEditorLayer.h"
#include "warscene/CombatGuideManage.h"
#include "warscene/CombatGuideData.h"
#include "common/CGameSound.h"
#include "warscene/CHeroSoundData.h"
#include "common/CCRollLabelAction.h"
#include "tools/UICloneMgr.h"
#include "warscene/BattleTools.h"
#include "Battle/BattleMessage.h"
#include "common/CSpecialProgress.h"

WarControl::WarControl()
	:m_ControLayer(nullptr),m_boxNum(0),m_goldNum(0),m_iAimCost(0),m_pBossBar1(nullptr)
	,m_interval(0),m_openEye(false),m_WindResume(false),m_pCostChange(nullptr),m_LayerColor(nullptr)
	,m_fdetal(0),m_costTime(0),m_ContinuousNum(0),m_pCostNum(nullptr),m_pBossBar2(nullptr)
	,m_goldIconPos(CCPointZero), m_boxIconPos(CCPointZero),m_pAllDamage(nullptr),m_ArmatureTips(nullptr)
	,m_batchNodeEffect(nullptr)
{}

WarControl::~WarControl()
{
	REMOVE_TARGET(m_ControLayer);
	CC_SAFE_RELEASE(m_ControLayer);
	m_ControLayer = nullptr;
}

void WarControl::onEnter()
{
	CCNode::onEnter();
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::ChangeBoxGoldNum),Drop_Item_NumDispose,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::upContinuousNum),BAR_ATK_HANDLE,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::AliveBattleDispose),ALIVEBATTLET,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::initTipsEffect),TIPSEFFECT,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::CaptainHit),CAPTAINHIT,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::SkillMask),B_SKILL_MASK,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::upButtonState),UPSKILLBUTTON,nullptr);
}

void WarControl::onExit()
{
	CCNode::onExit();
	RemoveEvent();
	this->unscheduleUpdate();
	NOTIFICATION->removeAllObservers(this);
}
CLayout* WarControl::getLaout() { return m_ControLayer; }
void WarControl::AddEvent() { this->setTouchEnabled(true); }
void WarControl::RemoveEvent() { this->setTouchEnabled(false); }

void WarControl::OnClick(CCObject* ob)
{
	CCNode* btn = (CCNode*)ob;
	int tag = btn->getTag();
	switch (tag)
	{
	case CL_Menu:
		{
			NOTIFICATION->postNotification(SHOW_PLAN_PANEL);
		}break;
	case CL_StarBtn:
		{
			NOTIFICATION->postNotification(WAR_ROUND_START);
			m_openEye = false;
			m_WindResume = false;
		}break;
	case CL_AddSpeedBtn:
		{
			NOTIFICATION->postNotification(CHANGEBATTLESPEED);
		}break;
	case GU_openGuide:
		{
			char path[60] = {0};
			sprintf(path,"%d_%d",0,1);														//覆盖高亮区域的图片
			DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
		}break;
	case TEST_MoveState:
		{
			NOTIFICATION->postNotification(B_ChangeMoveState);
		}break;
	case TEST_BattleData:																	//测试重置当前关卡
		{
			BattleDataInit* data = DataCenter::sharedData()->getWar()->getBattleData();
			int stageId = DataCenter::sharedData()->getWar()->getStageID();
			CPlayerControl::getInstance().sendEnterStage(stageId,data->heroList.at(0).id,data->heroList.at(1).id,data->heroList.at(2).id,data->heroList.at(3).id,data->heroList.at(4).id);
		}break;
	default:break;
	}
}

void WarControl::iniTestUi()
{
	CButton* bt_starWar = (CButton*)m_ControLayer->getChildByTag(CL_StarBtn);
	bt_starWar->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	bt_starWar->setVisible(true);
#else
	bt_starWar->setVisible(false);
#endif

	CButton* open = CButton::create("public/btn_tihuanwujiang_01.png","public/btn_tihuanwujiang_02.png");
	open->setPosition(CCPoint(200, 100));
	open->setTag(GU_openGuide);
	open->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
	m_ControLayer->addChild(open);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	open->setVisible(false);
#else
	open->setVisible(false);
#endif

	CButton* MoveTest = CButton::create("public/btn_tihuanwujiang_01.png","public/btn_tihuanwujiang_02.png");
	MoveTest->setPosition(ccpAdd(bt_starWar->getPosition(),ccp(200,60)));
	MoveTest->setTag(TEST_MoveState);
	MoveTest->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
	m_ControLayer->addChild(MoveTest);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	MoveTest->setVisible(true);
#else
	MoveTest->setVisible(false);
#endif
}

bool WarControl::init()
{
	if (!BaseLayer::init())
		return false;
	m_ControLayer = LoadComponent("WarControlPanel.xaml");	//默认设置锚点为(0.5,0.5)所以将坐标设置在VCENTER是有效的
	if (outPutERRORMsg("WarControl::init",m_ControLayer))
		return false;
	m_ControLayer->setPosition(VCENTER);
	m_ControLayer->retain();
	this->setIsShowBlack(false);
	this->setTouchPriority(WarControlPriority);
	this->setTouchEnabled(false);
	this->setOnTouchEndedAfterLongClickListener(this, ccw_afterlongclick_selector(WarControl::onlongClickEnd));		//只对开启了长按监听的控件有效
	m_Manage = DataCenter::sharedData()->getWar();
	this->addChild(m_ControLayer);

	initUIAbove();									//初始化界面上部分

	initAliveButtons();								//初始化武将按钮

	iniCaptainHurtTips();							//主帅受击提示

	initUIEffect();									//添加在界面上的特效

	initCostBar();									//初始化costBar

	iniTestUi();									//初始化测试按钮

	return true;
}

CCNode* WarControl::getMoveLayout( int index )
{
	CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(CL_BtnLayout1+index);
	return (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
}
//添加在界面上的特效
void WarControl::initUIEffect()
{
	for (int i=0;i<4;i++)
	{
		CCPoint p = DataCenter::sharedData()->getMap()->getCurrWarMap()->getPoint(i);
		EffectObject* ef = EffectObject::create("10030",EffectType::Repeat);
		ef->setTag(CL_TipsEffect1+i);
		ef->setPosition(ccp(100,p.y));
		ef->setVisible(false);
		m_ControLayer->addChild(ef);
	}
	m_LayerColor = CCLayerColor::create();															//技能触发的全屏颜色
	m_LayerColor->setContentSize(CCDirector::sharedDirector()->getWinSize()*2);
	m_LayerColor->setAnchorPoint(ccp(0,0));
	m_LayerColor->setZOrder(1);
	m_ControLayer->addChild(m_LayerColor);
	m_batchNodeEffect = CCSpriteBatchNode::create("common/icon_11.png");							//金币掉落
	m_ControLayer->addChild(m_batchNodeEffect);
}
//初始化界面上部分
void WarControl::initUIAbove()
{
	WarAlive* boss = m_Manage->getAliveByType(AliveType::WorldBoss);
	if (boss)
	{
		m_ControLayer->findWidgetById("layer_up_boss")->setVisible(true);
		m_ControLayer->findWidgetById("layer_up_normal")->setVisible(false);
		updateTimeCountUI(180);																			//时间
		initWorldBossAbove(boss);
	}else{
		m_ControLayer->findWidgetById("layer_up_boss")->setVisible(false);
		m_ControLayer->findWidgetById("layer_up_normal")->setVisible(true);
		initNormalAbove();
		return;
	}
}

void WarControl::initNormalAbove()
{
	CButton* bt_return = (CButton*)m_ControLayer->getChildByTag(CL_Menu);								//返回按钮
	bt_return->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));		

	CCheckBox* bt_speed = (CCheckBox*)m_ControLayer->getChildByTag(CL_AddSpeedBtn);						//加速按钮
	bt_speed->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));

	CLayout* pLayoutNormal = (CLayout*)m_ControLayer->findWidgetById("layer_up_normal");
	CProgressLabel* GoldNum = CProgressLabel::create();
	CLabel* goldNum_ = (CLabel*)pLayoutNormal->getChildByTag(CL_GoldNum);
	goldNum_->setString("0");
	GoldNum->setLabel(goldNum_);
	GoldNum->setTag(CL_GoldNumPro);
	m_ControLayer->addChild(GoldNum);

	CProgressLabel* BoxNum = CProgressLabel::create();
	CLabel* boxNum_ = (CLabel*)pLayoutNormal->getChildByTag(CL_BoxNum);
	boxNum_->setString("0");
	BoxNum->setLabel(boxNum_);
	BoxNum->setTag(CL_BoxNumPro);
	m_ControLayer->addChild(BoxNum);

	CCSprite* pGold = ((CCSprite*)pLayoutNormal->findWidgetById("gold"));								//初始化金币图标和盒子图标的世界坐标位置
	m_goldIconPos = pGold->getPosition()+ccp(pGold->getContentSize().width*0.5f, pGold->getContentSize().height*0.5f);
	m_goldIconPos = pGold->getParent()->convertToWorldSpace(m_goldIconPos);
	CCSprite* pBox = ((CCSprite*)pLayoutNormal->findWidgetById("box"));
	m_boxIconPos = pBox->getPosition();
	m_boxIconPos = pBox->getParent()->convertToWorldSpace(m_boxIconPos);
}
//初始化世界bossUI
void WarControl::initWorldBossAbove(WarAlive* boss)
{											
	CLabel* pDamageText = (CLabel*)m_ControLayer->findWidgetById("attack_index");					//伤害
	m_pAllDamage = CCLabelAtlas::create("0", "worldBoss/no_07.png", 17, 28, 48);
	m_pAllDamage->setAnchorPoint(ccp(1.0f, 0.5f));
	m_pAllDamage->setPosition(pDamageText->getPosition() + ccp(-36, 15));
	pDamageText->getParent()->addChild(m_pAllDamage, pDamageText->getZOrder());
	NOTIFICATION->addObserver(this,callfuncO_selector(WarControl::updateWorldBossBloodBar),B_WorldBoss_HurtUpdate,nullptr);

	CCheckBox* bt_speed = (CCheckBox*)m_ControLayer->getChildByTag(CL_AddSpeedBtn);					//屏蔽加速和暂停
	bt_speed->setEnabled(false);
	bt_speed->setVisible(false);

	CImageView* pHead = (CImageView*)m_ControLayer->findWidgetById("boss_head");					//BOSS头像
	CCString* pStr = CCString::createWithFormat("headImg/%d.png", boss->getModel());
	std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
	if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
		pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));

	m_pBossBar1 = (CProgressBar*)m_ControLayer->findWidgetById("bar1");
	m_pBossBar2 = (CProgressBar*)m_ControLayer->findWidgetById("bar2");
	updateWorldBossBloodBar(CCInteger::create(boss->getHp()));

	CLabel* pBuffRank = (CLabel*)m_ControLayer->findWidgetById("boss_fire_level");					//BUFF层次
	pBuffRank->setString(ToString(DataCenter::sharedData()->getWar()->getWorldBossRank()));
}
//update word boss damage
void WarControl::updateWorldBossDamage()
{
	WarAlive* boss = m_Manage->getAliveByType(AliveType::WorldBoss);
	int num = DataCenter::sharedData()->getWar()->getBossHurtCount();
	m_pAllDamage->runAction(CCRollLabelAction::create(0.3f, atoi(m_pAllDamage->getString()), num, m_pAllDamage));
	m_pAllDamage->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.1f, 1.5f), CCScaleTo::create(0.05f, 1.0f)));
}
//世界boss两个bar叠加在一起构成世界boss血量条
void WarControl::updateWorldBossBloodBar( CCObject* ob )
{
	updateWorldBossDamage();
	
	int iValue = ((CCInteger*)ob)->getValue();					
	iValue = iValue<0?0:iValue;									//保证大于等于0
	int iOneBarMax = 10000;										//一管血10000	
	int iRemain = iValue%iOneBarMax;							//剩余多少
	int iBarCount = iValue/iOneBarMax;							//多少整管	
	int iLowBarIndex = iBarCount%4;								//底管血条颜色	
	int iUpBarIndex = (iLowBarIndex+1)%4;						//上层管血条颜色

	if(iBarCount==0)
	{
		m_pBossBar1->setVisible(false);
	}else{
		m_pBossBar1->setProgressImage(CCString::createWithFormat("warScene/boss_bar_%d.png", iLowBarIndex)->getCString());
		m_pBossBar1->setMaxValue(iOneBarMax);
		m_pBossBar1->setValue(iOneBarMax);
	}

	m_pBossBar2->setMaxValue(iOneBarMax);
	m_pBossBar2->setProgressImage(CCString::createWithFormat("warScene/boss_bar_%d.png", iUpBarIndex)->getCString());
	if(m_pBossBar2->getValue()<iRemain)
	{
		float fTime = 0.015f*100*(iOneBarMax-iRemain)/iOneBarMax;
		m_pBossBar2->setValue(iOneBarMax);
		m_pBossBar2->startProgress(iRemain, fTime);
	}else{
		float fTime = 0.015f*100*(m_pBossBar2->getValue()-iRemain)/iOneBarMax;
		m_pBossBar2->startProgress(iRemain, fTime);
	}
}
//初始化costBar相关效果
void WarControl::initCostBar()
{
	CCArray* arr = m_Manage->getHeros(true);
	CCObject* obj = nullptr;
	int costNum = 0;
	int costMax = 0;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		costMax += alive->getCostmax();
		costNum += alive->getInitCost();
	}
	CProgressBar* pBar = (CProgressBar*)m_ControLayer->getChildByTag(CL_CostBar);//cost进度条
	pBar->setMaxValue(costMax);
	pBar->setValue(0);
	m_pCostBar = CSpecialProgress::CreateWithProgressBar(pBar);
	m_pCostBar->addParticleEffect("lz/aaaa.plist");
	CCAnimation * pHeadAnimation = AnimationManager::sharedAction()->getAnimation("9055");
	pHeadAnimation->setDelayPerUnit(0.1f);
	CCAnimate* pHeadAnimate = CCAnimate::create(pHeadAnimation);
	CCSprite* pProcessHead  = CCSprite::create();
	pProcessHead->runAction(CCRepeatForever::create(pHeadAnimate));
	m_pCostBar->addHead(pProcessHead);
	pBar->getParent()->addChild(m_pCostBar);
	pBar->removeFromParentAndCleanup(true);

	//进度条数字
	m_pCostNum = (CLabel*)m_ControLayer->findWidgetById("cost_text");
	m_pCostNum->setString(CCString::createWithFormat("%d/%d",  costNum, costMax)->getCString());

	m_pCostChange = (CLabel*)m_ControLayer->findWidgetById("cost_change");
	m_pCostChange->setAnchorPoint(ccp(0.5f, 0.5f));
	m_pCostChange->setPosition(m_pCostChange->getPosition()+ccp(0, m_pCostChange->getContentSize().height/2));
	m_pCostChange->setColor(ccc3(255, 255, 255));

	//蓝色球
	CCSprite* MAXcostTips = (CCSprite*)m_ControLayer->getChildByTag(CL_MAXcostTips);
	CCFadeTo* fato = CCFadeTo::create(1.0f,125);
	CCFadeIn* fain = CCFadeIn::create(0.5f);
	MAXcostTips->runAction(CCRepeatForever::create(CCSequence::create(fato,fain,nullptr)));

	//旋转3球
	CCSprite* pCircle = (CCSprite*)m_ControLayer->getChildByTag(CL_CostEffect);
	pCircle->setOpacity(0);
	for(unsigned int i=0; i<3; i++)
	{
		CCSprite* pCircle1 = (CCSprite*)m_ControLayer->findWidgetById(CCString::createWithFormat("circle_%d", i)->getCString());
		pCircle1->removeFromParentAndCleanup(true);
	}
	EffectObject* Eff = EffectObject::create("9051",EffectType::Repeat);						//火焰特效
	Eff->setPosition(ccp(pCircle->getContentSize().width/2,pCircle->getContentSize().height/2));
	pCircle->addChild(Eff);
}
//初始化武将按钮信息
void WarControl::initAliveButton(CCNode* Layout,WarAlive* alive)
{
	CButton* btn = (CButton*)Layout->getChildByTag(CL_Btn);
	btn->setLoingClickTime(0.5f);
	btn->setVisible(true);
	btn->setUserObject(alive);														//按钮和武将绑定
	btn->setOnClickListener(this,ccw_click_selector(WarControl::AliveButtonClick));
	btn->setOnTouchBeganListener(this,ccw_touchbegan_selector(WarControl::AliveButtonBeginClick));
	btn->setOnLongClickListener(this,ccw_longclick_selector(WarControl::AliveButtonLongClick));
	btn->getSelectedImage()->setScale(0.95f);

	EffectObject* CallAliveEffect = EffectObject::create("10028",EffectType::Repeat);			//call role effect
	CallAliveEffect->setTag(CL_BtnCallEff);
	CallAliveEffect->setPosition(ccpAdd(btn->getPosition(),ccp(0,50)));
	Layout->addChild(CallAliveEffect);

	EffectObject* AliveSkillEffect = EffectObject::create("10019",EffectType::Repeat);			//can use role skill effect
	AliveSkillEffect->setTag(CL_BtnSkillEff);
	AliveSkillEffect->setEffAnchorPoint(0.5f,0.2f);
	AliveSkillEffect->setPosition(btn->getPosition());
	Layout->addChild(AliveSkillEffect, -1);

	CEffect* effect = getSummonEffect(&alive->role->skill3);									//这个方法是应该放在武将身上的
	if (!effect)
		return;
	initButtonBackImage(btn,effect->pro_Type);													//reason call number init back image
	int CallNumber = effect->pro_Type>5?5:effect->pro_Type;										//guide chapter call number infinite
	btn->setPosition(ccpAdd(btn->getPosition(),ccp(3.5f*CallNumber,0)));
}
//根据召唤数量初始化按钮背景图
void WarControl::initButtonBackImage( CButton* button,int index )
{
	if (index > 5)
		index = 5; 
	if (index < 1)
		return;
	char path[60] = {0};
	sprintf(path,"warScene/NewUI/kuang%d.png",index);
	button->setSelectedImage(path);
	button->setNormalImage(path);
	button->setDisabledImage(path);
	if (index < 5)																				//guide chapter call number is infinite
		button->setPosition(ccpAdd(button->getPosition(),ccp(-3.7f,0)));
}
//初始化武将头像和进度条
void WarControl::initAliveButtonBar( CCNode* Layout , int model )
{
	char ptr[60] = {0};
	sprintf(ptr,"headIcon/NewIcon/%d.png",model);
	CCSprite* sp = (CCSprite*)Layout->getChildByTag(CL_HeroIcon);								//role head image
	sp->setPosition(sp->getPosition()+ccp(5,0));
	if (!sp->initWithFile(ptr))
	{
		CCLOG("[ *ERROR ] WarControl::initAliveBar Lost Img %s",ptr);
		sprintf(ptr,"headIcon/NewIcon/146.png"); 
	}else{
		sp->setVisible(true);
	}
	
	CProgressBar* CdBar = (CProgressBar*)Layout->getChildByTag(CL_HeroPro);	//Bar都是增量变化的CD时间也做成增量的变化方式,满的情况bar消失。点击按钮后重现
	CdBar->setProgressImage(ptr);
	CdBar->setValue(100);
	CdBar->setDirection(eProgressBarDirectionBottomToTop);
	CdBar->setOnProgressEndedListener(this,ccw_progressended_selector(WarControl::AliveBarFullCallBack));
}
//根据按钮上的bar来控制按钮的状态。
void WarControl::AliveBarFullCallBack(CCObject* ob)
{
	CProgressBar* CdBar = (CProgressBar*)ob;
	CButton* btn = (CButton*)CdBar->getParent()->getChildByTag(CL_Btn);
	btn->setEnabled(true);
}
//初始化我方武将UI列表
void WarControl::initAliveButtons()
{
	CCArray* arr = m_Manage->getHeros(true);
	CCObject* obj = nullptr;
	int index = 0;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		CLayout* MoveLaout = nullptr;	
		if (alive->getCaptain())
		{
			alive->setUiLayout(CL_BtnLayout1);
			CLayout*BtnLay = (CLayout*)m_ControLayer->getChildByTag(CL_BtnLayout1);
			BtnLay->setVisible(true);
			CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
			CSkill skill = alive->role->skill3;
			NeedCost->setString(ToString(skill.cost));									//初始化值为技能消耗cost
			MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);	
			if (skill.skillType == CallAtk||!skill.skillId)
				MoveLaout->setPosition(ccpAdd(MoveLaout->getPosition(),ccp(0,-30)));
		}else{
			alive->setUiLayout(CL_BtnLayout2+index);
			CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(CL_BtnLayout2+index);
			BtnLay->setVisible(true);
			CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
			NeedCost->setString(ToString(alive->role->useCost));						//初始化值为上阵消耗cost
			MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
			MoveLaout->setPosition(ccpAdd(MoveLaout->getPosition(),ccp(0,-30)));
			index++;
		}
		initAliveButton(MoveLaout,alive);
		initAliveButtonBar(MoveLaout,alive->getModel());
	}
}
//必须实时更新，当cost足够时，会刷新显示特效	
void WarControl::updateUiLayerCostNumber(int cost)
{
	m_iAimCost = cost;
	updateAliveButtonEffect();														//update button effect state									

	if (m_pCostBar->getPercentage()==1)
		m_ControLayer->getChildByTag(CL_MAXcostTips)->setVisible(true);
	else
		m_ControLayer->getChildByTag(CL_MAXcostTips)->setVisible(false);	
	m_pCostNum->setString(CCString::createWithFormat("%d / %d", cost, m_pCostBar->getMaxValue())->getCString());

	int iNowCost = m_pCostBar->getValue();
	if(m_iAimCost == iNowCost)
	{
		return;
	}else{
		int iCurrentCost = m_iAimCost>iNowCost?++iNowCost:--iNowCost;				//让cost进度条缓慢变化过去
		m_pCostBar->setValue(iCurrentCost);
	}
}
//guide state button effect dispose
bool WarControl::guideStateButtonEffect( CCNode* layout )
{
	if (!DataCenter::sharedData()->getCombatGuideMg()->IsGuide())					//引导判断
		return false;
	CombatGuideStep* step = DataCenter::sharedData()->getCombatGuideMg()->getCurrStep();
	if (step->getType() == UI_Type || step->getType() == CallAalive_Type)
	{
		if (layout->getParent()->getTag() != (step->getUiType()-1)+CL_BtnLayout1)
			return true;
		else if (step->getType() == UI_Type)
			DataCenter::sharedData()->getCombatGuideMg()->NextStep();
	}
	return false;
}
//update role button effect
void WarControl::updateAliveButtonEffect()
{
	for (int i=0; i<5 ; i++)
	{
		CCNode* MoveLaout = getMoveLayout(i);
		CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
		WarAlive* alive = (WarAlive*)btn->getUserObject();
		if (!alive)continue;
		MoveLaout->getChildByTag(CL_BtnSkillEff)->setVisible(false);
		MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(false);	
		if (guideStateButtonEffect(MoveLaout))									//in guide state effect dispose 
			continue;
		if (alive->getBattle()&&alive->getHp()>0)
		{
			CSkill skill = alive->role->skill3;
			if (m_iAimCost >= skill.cost && btn->isEnabled())
			{
				if (alive->canSummonAlive())
				{
					MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(true);				
				}else if(skill.skillId && skill.skillType != CallAtk){
					MoveLaout->getChildByTag(CL_BtnSkillEff)->setVisible(true);
				}	
			}		
		}else{			
			if(m_iAimCost >= alive->role->useCost && btn->isEnabled())
				MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(true);		
		}
	}
}
//call role log in battlefield
void WarControl::CallAliveBattle(WarAlive*alive)
{
	WarAlive* pAlive = m_Manage->getAlive(alive->getFatherID());
	CCNode* MoveLaout = getMoveLayout(pAlive->getUiLayout()-CL_BtnLayout1);
	CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
	initButtonBackImage(btn,pAlive->getcallAliveNum());

	CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
	btn->setEnabled(false);
	int cost = pAlive->role->skill3.cost;
	NOTIFICATION->postNotification(CHANGECOST,CCFloat::create(-cost));			//this in a call alive real log in battlefield
	if (pAlive->canSummonAlive())												//is call full
		CdBar->startProgressFromTo(0,100,pAlive->role->skill3.coldDown);
}
//role log in battlefield or leave
void WarControl::AliveBattlefield( WarAlive* alive )
{
	CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(alive->getUiLayout());
	CCNode* MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
	CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
	CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
	CSkill skill = alive->role->skill3;
	if (alive->getBattle())														
	{
		MoveLaout->runAction(CCMoveBy::create(0.2f,ccp(0,-30)));				//武将死亡
		CdBar->startProgressFromTo(0,100,alive->role->coldDown);				//复活时间
		NeedCost->setString(ToString(alive->role->useCost));					//上阵消耗cost
	}else{																		
		MoveLaout->runAction(CCMoveBy::create(0.2f,ccp(0,30)));					//武将上阵
		CdBar->startProgressFromTo(0,100,skill.coldDown);						//技能CD
		NeedCost->setString(ToString(skill.cost));								//初始化值为上阵消耗cost
		NOTIFICATION->postNotification(CHANGECOST,CCFloat::create(-alive->role->useCost));
	}
	CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
	btn->setEnabled(false);
}
//role log in or leave message dispose
void WarControl::AliveBattleDispose(CCObject* ob)
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->role->isCall)
	{
		CallAliveBattle(alive);
	}else{
		AliveBattlefield(alive);
	}
}
//judge button touch transmit direction
CWidgetTouchModel WarControl::AliveButtonBeginClick(CCObject* ob,CCTouch* pTouch)
{
	CButton* btn = (CButton*)ob;	
	WarAlive* alive = dynamic_cast<WarAlive*>(btn->getUserObject());			//根据点击的按钮来判断点了哪个人
	int cost = m_Manage->getLogicObj()->getCurrCost();
	if (alive->getBattle()&&alive->getHp()>0)
	{
		CSkill skill = alive->role->skill3;
		if (skill.skillType == CallAtk&&cost >= skill.cost)
		{
			WarAlive* pAlive = m_Manage->getCallAlive(alive,&skill);
			if (!pAlive)
				return eWidgetTouchTransient;
			CaptainSkill::create()->ExecuteCaptainSkill();
			NOTIFICATION->postNotification(ALIVEBATTLETOUCH,pAlive);			//召唤成功才能扣减Cost值
			return eWidgetTouchNone;
		}	
		return eWidgetTouchTransient;											//武将为上阵状态则触摸不传递到下层,否则将触摸传递到下层处理
	}else{
		if (alive->getHp()<=0)
		{
			DataCenter::sharedData()->getWar()->initAlive(alive);				//使用元宝重置时将我方主帅进行重新初始化再次添加到战斗中
			CaptainSkill::create()->ExecuteCaptainSkill();
		}
		if (cost < alive->role->useCost)
			return eWidgetTouchTransient;
		NOTIFICATION->postNotification(ALIVEBATTLETOUCH,alive);
		initTipsEffect(CCBool::create(1));
		return eWidgetTouchNone;	
	}
}
//判断武将状态决定是否要处理这个消息。和消息是否要往下传递,可以在touchEnd处理技能触发。
void WarControl::AliveButtonClick( CCObject* ob )
{
	CButton* btn = (CButton*)ob;	
	WarAlive* alive = dynamic_cast<WarAlive*>(btn->getUserObject());			//根据点击的按钮来判断点了哪个人
	CSkill skill = alive->role->skill3;
	if (  skill.skillType == CallAtk
		||m_Manage->getLogicObj()->getCurrCost()<skill.cost 
		||!alive->getBattle() )
		return;
	CCNode* MoveLaout = getMoveLayout(alive->getUiLayout()-CL_BtnLayout1);
	if (guideStateButtonEffect(MoveLaout))
		return;
	alive->getActObject()->TurnStateTo(Stand_Index);
	alive->ResetAttackState();																	//点击了必杀技按钮，但是武将并没有进入必杀技状态的情况,强制切换至必杀技状态
	alive->setCriAtk(true);
	NOTIFICATION->postNotification(RECORDBATTERNUM);
	CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
	CdBar->setValue(0);
	btn->setEnabled(false);
	NOTIFICATION->postNotification(CHANGECOST,CCFloat::create(-skill.cost));
	if (alive->getCaptain())
		upButtonState(ob);
	else
		alive->getActObject()->setUpdateState(false);
}

void WarControl::upButtonState( CCObject* ob )
{
	for (int i=0; i<5 ; i++)
	{
		CCNode* MoveLaout = getMoveLayout(i);
		CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
		CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
		WarAlive* alive = (WarAlive*)btn->getUserObject();
		if (ob != alive)
			continue;
		CSkill skill = alive->role->skill3;
		CEffect* effect = getSummonEffect(&skill);
		if (effect&&effect->pro_Type)
		{
			++alive->role->skill3.pro_rate;
			initButtonBackImage(btn,effect->pro_Type-alive->role->skill3.pro_rate);
			if (alive->role->skill3.pro_rate >= effect->pro_Type)
				return;
		}
		CdBar->startProgressFromTo(0,100,skill.coldDown);										//技能CD
	}
}
bool WarControl::AliveButtonLongClick(CCObject* pSender, CCTouch* pTouch)
{
	CToggleView* tog = (CToggleView*)pSender;
	WarAlive* alive = (WarAlive*)tog->getUserObject();
	if (!alive)return false;
	SkillTips *tips = SkillTips::create(); 
	tips->setSkillInfo(pSender,alive);//根据武将当前状态判断武将释放的技能从而给出提示信息( CC制不需要技能提示 )
	LayerManager::instance()->push(tips);
	NOTIFICATION->postNotification(Draw_BtmSkill_Area,alive);
	return true;
}
//长按取消操作
void WarControl::onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration)
{
	NOTIFICATION->postNotification(Remove_SkillTips);
	NOTIFICATION->postNotification(WAR_CANCEL_DRAW_DRAG,nullptr);
}

CCNode* WarControl::getboxGold(bool box,bool label)
{
	if (box)
	{
		if (label)return m_ControLayer->getChildByTag(CL_BoxNum);
		return m_ControLayer->getChildByTag(CL_BoxNumPro);
	}else{
		if(label)return m_ControLayer->getChildByTag(CL_BoxNum);
		return m_ControLayer->getChildByTag(CL_BoxNumPro);
	}
	return nullptr;
}

void WarControl::ChangeBoxGoldNum(CCObject* ob)
{
	CCInteger* num = (CCInteger*)ob;
	if (num->getValue() < 10)
	{
		m_boxNum += 1;
		CProgressLabel* boxNum =(CProgressLabel*)m_ControLayer->getChildByTag(CL_BoxNumPro);
		boxNum->changeValueTo(m_boxNum,0.1f);
	}else{
		m_goldNum += num->getValue();
		CProgressLabel* goldNum =(CProgressLabel*)m_ControLayer->getChildByTag(CL_GoldNumPro);
		goldNum->changeValueTo(m_goldNum,0.5f);
	}
}

void WarControl::upContinuousNum(CCObject* ob)
{
	CLayout* Layout = (CLayout*)m_ControLayer->getChildByTag(CL_ContinuousLayou);
	if (ob)
	{
		if (!Layout->isVisible())
			return;
		this->unschedule(schedule_selector(WarControl::upContinuousState));
		this->scheduleOnce(schedule_selector(WarControl::upContinuousState),3.0f);
	}else{
		m_ContinuousNum++;
		if (m_ContinuousNum <= 1)
			return;
		this->unschedule(schedule_selector(WarControl::upContinuousState));
		Layout->runAction(CCShake::create(0.4f,3.0f));
		Layout->setVisible(true);
		CLabelAtlas* Lab_Num = (CLabelAtlas*)Layout->getChildByTag(CL_ContinuousNum);
		Lab_Num->stopAllActions();
		Lab_Num->setScale(1);
		Lab_Num->setString(ToString(m_ContinuousNum));
		CCScaleTo* sc1 = CCScaleTo::create(0.15f,1.8f);
		CCScaleTo* sc2 = CCScaleTo::create(0.05f,1.0f);
		CCSequence* sqes = CCSequence::create(sc1,sc2,nullptr);
		Lab_Num->runAction(sqes);
		this->scheduleOnce(schedule_selector(WarControl::upContinuousState),1.5f);
	}
}

void WarControl::upContinuousState(float dt)
{
	m_ControLayer->getChildByTag(CL_ContinuousLayou)->setVisible(false);
	m_ContinuousNum = 0;
}
//绘制怪物出现行数效果提示
void WarControl::initTipsEffect(CCObject* ob)
{
	CCBool* touch = (CCBool*)ob;
	CCArray* ArrRow = CCArray::create();
	CCArray* arr = DataCenter::sharedData()->getWar()->getMonsts(true);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (alive->getGridIndex() < 52)continue;			//不再预警标记显示范围内
		if (touch->getValue())								//召唤武将时显示提示
		{
			if (alive->getGridIndex() < 64 || alive->getGridIndex() > 91)
				continue;									//地图内非可视范围显示提示
		}else{
			if (alive->getGridIndex() > 75 )continue;		//新一波怪物的提醒
		}
		CCInteger* row = CCInteger::create(alive->getGridIndex() % C_GRID_ROW);
		if (!ArrRow->containsObject(row))
			ArrRow->addObject(row);
	}
	CCObject* pobj = nullptr;
	CCARRAY_FOREACH(ArrRow,pobj)
	{
		CCInteger* row = (CCInteger*)pobj;
		EffectObject* ef = (EffectObject*)m_ControLayer->getChildByTag(CL_TipsEffect1 + row->getValue());
		ef->setVisible(true);
		ef->stopAllActions();
		CCDelayTime* dt = CCDelayTime::create(1.0f);
		ef->runAction(CCSequence::create(dt,CCHide::create(),NULL));
	}
}
//必杀技闪屏效果
void WarControl::SkillMask( CCObject* ob )
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getEnemy())
		return;
	if (alive->role->roletype == FireType)
	{
		m_LayerColor->setColor(ccc3(206,17,0));
	}else if (alive->role->roletype == WaterType)
	{
		m_LayerColor->setColor(ccc3(30,69,218));
	}else{
		m_LayerColor->setColor(ccc3(200,167,60));
	}
	m_LayerColor->stopAllActions();
	m_LayerColor->setVisible(true);
	m_LayerColor->runAction(CCSequence::create(CCFadeOut::create(0.07f),CCHide::create(),NULL));
}
//cost变化数字
void WarControl::showCostAddOrReduceEffect( int iCostChange )
{
	m_pCostChange->setString(CCString::createWithFormat("%d", abs(iCostChange))->getCString());
	if(iCostChange > 0)
	{
		if(m_pCostChange->getColor().g == 255)
		{
			m_pCostChange->runAction(CCRepeatForever::create(
				CCSequence::createWithTwoActions(
				CCScaleTo::create(0.25f, 1.3f),
				CCScaleTo::create(0.25f, 1.0f)
				)));

			m_pCostChange->setColor(ccc3(255, 0, 0));
			m_pCostBar->runAction(CCRepeatForever::create(
				CCSequence::createWithTwoActions(
				CCFadeTo::create(0.2f, 80),
				CCFadeTo::create(0.2f, 240)
				)));
			m_pCostBar->setColor(ccc3(230, 50, 50));
		}
	}else{
		if(m_pCostChange->getColor().g == 0)
		{
			m_pCostChange->stopAllActions();
			m_pCostChange->setColor(ccc3(255, 255, 255));

			m_pCostBar->setOpacity(255);
			m_pCostBar->stopAllActions();
		}
	}
}
//显示击杀怪物增加cost效果
void WarControl::showFlyCostToBar( CCPoint pStartPos )
{
	pStartPos = m_ControLayer->convertToNodeSpace(pStartPos);
	
	float fPosAdd = m_pCostBar->getContentSize().width*(m_pCostBar->getPercentage()-0.5f);
	if(m_pCostBar->getPercentage()>=0.95f)
	{
		fPosAdd = 0;
	}
	CCPoint pEndPos = m_pCostBar->getPosition() + ccp(fPosAdd, 0);
	ccBezierConfig bezier;
	bezier.controlPoint_1 = ccp(pStartPos.x+(CCRANDOM_0_1()-0.5f)*180, pStartPos.y+(CCRANDOM_0_1()-0.5f)*180);
	bezier.controlPoint_2 = ccp(pEndPos.x+(CCRANDOM_0_1()-0.5f)*180, pEndPos.y+(CCRANDOM_0_1()-0.5f)*180);
	bezier.endPosition	= pEndPos;
	CCFiniteTimeAction *beizerMove = CCBezierTo::create(ccpDistance(pStartPos, pEndPos)/500, bezier);
	CCCallFuncN *callNext = CCCallFuncN::create(this, callfuncN_selector(WarControl::showFlyCostToBarCallBack));

	//绑定粒子
	CCParticleSystemQuad* pLz  = CCParticleSystemQuad::create("lz/eff_zz_blue.plist");
	pLz->setPosition(pStartPos);
	m_ControLayer->addChild(pLz);
	pLz->runAction(CCSequence::create(beizerMove,callNext, CCRemoveSelf::create(), nullptr));
	PlayEffectSound(SFX_422);
}

void WarControl::showFlyCostToBarCallBack( CCNode* pSender )
{
	CCPoint pEndPos = pSender->getPosition();
	//绑定粒子
	CCParticleSystemQuad* pLz  = CCParticleSystemQuad::create("lz/blue_bomb.plist");
	pLz->setAutoRemoveOnFinish(true);
	pLz->setPosition(pEndPos);
	pLz->resetSystem();
	m_ControLayer->addChild(pLz);
	PlayEffectSound(SFX_423);
}

void WarControl::flyCostToHeroBtn( CCNode* pNode )
{
	CCPoint pEndPos = pNode->getPosition();

	float fPosAdd = m_pCostBar->getContentSize().width*(m_pCostBar->getPercentage()-0.5f);
	if(m_pCostBar->getPercentage()>=0.95f)
	{
		fPosAdd = 0;
	}
	CCPoint pStartPos = m_pCostBar->getPosition() + ccp(fPosAdd, 0);
	ccBezierConfig bezier;
	bezier.controlPoint_1 = ccp(pStartPos.x+CCRANDOM_0_1()*100, pStartPos.y+CCRANDOM_0_1()*100);
	bezier.controlPoint_2 = ccp(pEndPos.x-CCRANDOM_0_1()*100, pEndPos.y+CCRANDOM_0_1()*100);
	bezier.endPosition	= pEndPos;
	CCFiniteTimeAction *beizerMove = CCBezierTo::create(ccpDistance(pStartPos, pEndPos)/1000, bezier);
	CCCallFuncN *callNext = CCCallFuncN::create(this, callfuncN_selector(WarControl::showFlyCostToBarCallBack));

	//绑定粒子
	CCParticleSystemQuad* pLz  = CCParticleSystemQuad::create("lz/eff_zz_blue.plist");
	pLz->setPosition(pStartPos);
	m_ControLayer->addChild(pLz);
	pLz->runAction(CCSequence::create(beizerMove,callNext, CCRemoveSelf::create(), nullptr));
}

void WarControl::updateTimeCountUI( int iCount )
{
	int iMinute = iCount/60;
	int iSec = iCount%60;
	CCString* pStr = CCString::createWithFormat("%s%d:%s%d", iMinute>9?"":"0", iMinute, iSec>9?"":"0", iSec);
	CLabel* pLabel = (CLabel*)m_ControLayer->findWidgetById("timeCount");
	pLabel->setString(pStr->getCString());
}

void WarControl::iniCaptainHurtTips()
{
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData("attack");
	if (animationData)
	{
		m_ArmatureTips = CCArmature::create("attack");
		m_ArmatureTips->getAnimation()->play("attack",0.01f);
		m_ArmatureTips->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(WarControl::frameEvent));
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		m_ArmatureTips->setPosition(ccp(size.width/2,size.height/2));
		m_ArmatureTips->setVisible(false);
		m_ControLayer->addChild(m_ArmatureTips);
	}else{
		CCLOG("[ *ERROR ] WarAssist::PlayBeginAnimation animationData NULL");
		CCNotificationCenter::sharedNotificationCenter()->postNotification(LAYERMOVEEND,CCInteger::create(1));
	}
}

void WarControl::CaptainHit( CCObject* ob )
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getHp()>0)
	{
		m_ArmatureTips->stopAllActions();
		m_ArmatureTips->setVisible(true);
		m_ArmatureTips->setOpacity(255);
		m_ArmatureTips->runAction(CCSequence::create(CCDelayTime::create(0.5f),CCFadeOut::create(0.8f),nullptr));
	}else{
		m_ArmatureTips->setOpacity(255);
		m_ArmatureTips->setVisible(true);
		m_ArmatureTips->getAnimation()->play("die",0.01f);
	}
}

void WarControl::frameEvent( CCBone *pBone, const char *str, int a, int b )
{
	if (m_ArmatureTips->isVisible())
	{
		if(strcmp(str, "435"))
		{
			//PlayEffectSound("SFX/435.ogg");
		}
		else if(strcmp(str, "511"))
		{
			//PlayEffectSound("SFX/511.ogg");
		}
	}
}

void WarControl::hideUpUiPart()
{
	m_ControLayer->findWidgetById("layer_up_boss")->setVisible(false);
	m_ControLayer->findWidgetById("layer_up_normal")->setVisible(false);
	m_ControLayer->getChildByTag(CL_Menu)->setVisible(false);
	m_ControLayer->getChildByTag(CL_AddSpeedBtn)->setVisible(false);
}