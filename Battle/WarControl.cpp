
#include "Battle/WarControl.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "tools/CCShake.h"
#include "Battle/BattleScene/BattleScene.h"
#include "Battle/BattleCenter.h"
#include "Battle/BaseRole.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "tools/ShowTexttip.h"
#include "Battle/ConstNum.h"
#include "cctk/scenemanager.h"
#include "Battle/WarAssist.h"
#include "Battle/SkillTips.h"
#include "Battle/CombatTask.h" 
#include "common/ProgressLabel.h"
#include "Battle/CaptainSkill.h"
#include "common/CommonFunction.h"	
#include "Battle/ComBatLogic.h"
#include "Battle/AnimationManager.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "Battle/EffectObject.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/CombatGuideData.h"
#include "common/CGameSound.h"
#include "Battle/RoleConfig.h"
#include "common/CCRollLabelAction.h"
#include "tools/UICloneMgr.h"
#include "Battle/BattleTools.h"
#include "Battle/BattleMessage.h"
#include "Battle/BattleModel.h"
#include "common/CSpecialProgress.h"

#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BattleLayer/BattleTips.h"
#include "model/DataCenter.h"

namespace BattleSpace
{
	WarControl::WarControl()
		:m_ControLayer(nullptr),m_boxNum(0),m_goldNum(0),m_iAimCost(0),m_pBossBar1(nullptr)
		,m_pCostChange(nullptr),m_LayerColor(nullptr),m_ArmatureTips(nullptr)
		,m_fdetal(0),m_costTime(0),m_ContinuousNum(0),m_pCostNum(nullptr),m_pBossBar2(nullptr)
		,m_goldIconPos(CCPointZero), m_boxIconPos(CCPointZero),m_pAllDamage(nullptr)
		,m_batchNodeEffect(nullptr),mAutoState(false),m_iGameTimeCount(0),m_bCountDown(false)
	{}

	WarControl::~WarControl(){}

	void WarControl::onEnter()
	{
		CCNode::onEnter();
		bNotification->addObserver(this,callfuncO_selector(WarControl::ChangeBoxGoldNum),B_DropItem,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::upContinuousNumber),B_ContinuousNumber,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::AliveBattleDispose),MsgRoleBattleState,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::showMonsterTips),MsgMonsterTips,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::CaptainHit),B_CaptainHurt,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::SkillMask),B_SKILL_MASK,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::postButtonState),MsgGetButtonState,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::RemoveEvent),MsgControlRemove,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::showFlyCostToBar),MsgMonsterDie,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::hideUiUpPart),MsgHideControlUp,nullptr);
		bNotification->addObserver(this,callfuncO_selector(WarControl::updateBatchNumber),MsgUpBatchNumber,nullptr);
	}

	void WarControl::onExit()
	{
		CCNode::onExit();
		RemoveEvent();
		this->unscheduleUpdate();
		bNotification->removeAllObservers(this);
	}
	CLayout* WarControl::getLaout() { return m_ControLayer; }
	void WarControl::AddEvent(CCObject* ob /*=nullptr*/) { this->setTouchEnabled(true); }
	void WarControl::RemoveEvent(CCObject* ob /*=nullptr*/) { this->setTouchEnabled(false); }

	CWidgetTouchModel WarControl::onTouchBegin( CCObject* ob,CCTouch* pTouch )
	{
		UserData *user = DataCenter::sharedData()->getUser()->getUserData();
		switch (((CCNode*)ob)->getTag())
		{
		case CL_AddSpeedBtn:
			{
#if BATTLE_TEST
				return eWidgetTouchTransient;
#else
				if (!user->getVip())
				{
					BattleTips* tips = BattleTips::create();
					tips->showConfirmOnly();
					string str = GETLANGSTR(2016);
					tips->addContentTip(str.c_str());
					this->addChild(tips);
					return eWidgetTouchNone;
				}	
#endif
			}break;
		case CL_AutoPlay:
			{
#if BATTLE_TEST
				return eWidgetTouchTransient;
#else
				if (user->getLevel() <= 14 )
				{
					BattleTips* tips = BattleTips::create();
					tips->showConfirmOnly();
					string str = GETLANGSTR(2017);
					tips->addContentTip(str.c_str());
					this->addChild(tips);
					return eWidgetTouchNone;
				}
#endif
			}break;
		}
		return eWidgetTouchTransient;
	}

	void WarControl::OnClick(CCObject* ob)
	{
		CCLabelTTF* tTestTips = (CCLabelTTF*)m_ControLayer->getChildByTag(TEST_Tips);
		switch (((CCNode*)ob)->getTag())
		{
		case CL_Menu:
			{
				bNotification->postNotification(MsgReturnLayer);
			}break;
		case CL_StarBtn:
			{
				mManage->setLogicState( !mManage->getLogicState() );
				if (mManage->getLogicState())
				{
					tTestTips->setString("RunLogic");
				}else{
					tTestTips->setString("StopLogic");
				}
			}break;
		case CL_AddSpeedBtn:
			{
				changeSpeed();
			}break;
		case GU_openGuide:
			{
				char path[60] = {0};
				sprintf(path,"%d_%d",0,7);														//覆盖高亮区域的图片
				ManageCenter->getCombatGuideMg()->setCurrBatchGuide(path);
			}break;
		case TEST_MoveState:
			{
				bNotification->postNotification(B_ChangeMoveState);
			}break;
		case TEST_Role:
			{
#if BATTLE_TEST
				const RolesMap* tRoleMap = mManage->getRolesMap();
				for (auto tPair : *tRoleMap)
				{
					if (tPair.second->getEnemy())
					{
						//tPair.second->setAtk(500000);
						tPair.second->setMaxHp(500000);
						tPair.second->setHp(500000);	
					}else{
						//tPair.second->setAtk(500000);
						tPair.second->setMaxHp(500000);
						tPair.second->setHp(500000);	
					}
				}
#endif;
			}break;
		case CL_AutoPlay:
			{
				mAutoState = !mAutoState;
				if (mAutoState)
				{
					tTestTips->setString("AutoState");
				}else{
					tTestTips->setString("");
				}
				bNotification->postNotification(MsgAutoBattle);
			}break;
		}
	}

	void WarControl::iniTestUi()
	{
		CButton* tStarButton = (CButton*)m_ControLayer->getChildByTag(CL_StarBtn);
		tStarButton->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));

		CButton* tGuideTest = CButton::create("public/btn_tihuanwujiang_01.png","public/btn_tihuanwujiang_02.png");
		tGuideTest->setPosition(CCPoint(200, 100));
		tGuideTest->setTag(GU_openGuide);
		tGuideTest->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
		m_ControLayer->addChild(tGuideTest);

		CButton* MoveTest = CButton::create("public/btn_tihuanwujiang_01.png","public/btn_tihuanwujiang_02.png");
		MoveTest->setPosition(ccpAdd(tStarButton->getPosition(),ccp(300,60)));
		MoveTest->setTag(TEST_MoveState);
		MoveTest->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
		m_ControLayer->addChild(MoveTest);

		CButton* tRoleTest = CButton::create("public/btn_tihuanwujiang_01.png","public/btn_tihuanwujiang_02.png");
		tRoleTest->setPosition(ccpAdd(tStarButton->getPosition(),ccp(300,0)));
		tRoleTest->setTag(TEST_Role);
		tRoleTest->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
		m_ControLayer->addChild(tRoleTest);

		CCLabelTTF* TestTips = CCLabelTTF::create("","Arial",50);
		TestTips->setTag(TEST_Tips);
		TestTips->setPosition(ccpAdd(tStarButton->getPosition(),ccp(400,60)));
		TestTips->setColor(ccc3(0,255,0));
		m_ControLayer->addChild(TestTips);

#if BATTLE_TEST
		tStarButton->setVisible(true);
		tGuideTest->setVisible(true);
		MoveTest->setVisible(true);
		TestTips->setVisible(true);
		tRoleTest->setVisible(true);
#else
		tGuideTest->setVisible(false);
		tStarButton->setVisible(false);
		MoveTest->setVisible(false);
		TestTips->setVisible(false);
		tRoleTest->setVisible(false);
		//m_ControLayer->getChildByTag(CL_AutoPlay)->setVisible(false);
#endif
	}
	//创建一些其他的对象用于管理相应的效果，把主控制节点传过去，再根据ID进行相应的获取就可以了
	bool WarControl::init()
	{
		if (!BaseLayer::init())
			return false;
		m_ControLayer = LoadComponent("WarControlPanel.xaml");	//默认设置锚点为(0.5,0.5)所以将坐标设置在VCENTER是有效的
		if (outPutERRORMsg("WarControl::init",m_ControLayer))
			return false;
		m_ControLayer->setPosition(VCENTER);
		this->setIsShowBlack(false);
		this->setTouchPriority(WarControlPriority);
		this->setTouchEnabled(false);
		this->setOnTouchEndedAfterLongClickListener(this, ccw_afterlongclick_selector(WarControl::onlongClickEnd));		//只对开启了长按监听的控件有效
		mManage = BattleManage;
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
			CCPoint p = BattleCoords->getPoint(i);
			EffectObject* ef = EffectObject::create("10030",sPlayType::eRepeat);
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
		CButton* bt_return = (CButton*)m_ControLayer->getChildByTag(CL_Menu);								//返回按钮
		bt_return->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));		
		CCheckBox* tAutoPlay = CCheckBox::create();
		tAutoPlay->setNormalImage("warScene/Auto.png");
		tAutoPlay->setCheckedImage("warScene/Auto_on.png");
		tAutoPlay->setPosition(ccpAdd(bt_return->getPosition(),ccp(-200,0)));
		tAutoPlay->setTag(CL_AutoPlay);
		tAutoPlay->setVisible(false);
		tAutoPlay->setOnTouchBeganListener(this,ccw_touchbegan_selector(WarControl::onTouchBegin));
		tAutoPlay->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));
		m_ControLayer->addChild(tAutoPlay);
		if (BattleModelManage->isPvEBattle())
		{
			m_ControLayer->findWidgetById("layer_up_boss")->setVisible(false);
			m_ControLayer->findWidgetById("layer_up_normal")->setVisible(false);
			m_ControLayer->findWidgetById("layer_up_pvp")->setVisible(true);
			m_ControLayer->findWidgetById("layer_time")->setVisible(true);
			initPvEAbove();
			return ;
		}

		BaseRole* boss = mManage->getAliveByType(sMonsterSpecies::eWorldBoss);
		if (boss)
		{
			
			m_ControLayer->findWidgetById("layer_up_boss")->setVisible(true);
			m_ControLayer->findWidgetById("layer_up_normal")->setVisible(false);
			m_ControLayer->findWidgetById("layer_up_pvp")->setVisible(false);
			m_ControLayer->findWidgetById("layer_time")->setVisible(true);
			updateTimeCountUI(180);																			//时间
			initWorldBossAbove(boss);
		}else{
			tAutoPlay->setVisible(true);
			m_ControLayer->findWidgetById("layer_up_boss")->setVisible(false);
			m_ControLayer->findWidgetById("layer_up_normal")->setVisible(true);
			m_ControLayer->findWidgetById("layer_up_pvp")->setVisible(false);
			initNormalAbove();
			return;
		}
	}

	void WarControl::initPvEAbove()
	{
		CCheckBox* bt_speed = (CCheckBox*)m_ControLayer->getChildByTag(CL_AddSpeedBtn);						//加速按钮
		bt_speed->setOnTouchBeganListener(this,ccw_touchbegan_selector(WarControl::onTouchBegin));
		bt_speed->setOnClickListener(this,ccw_click_selector(WarControl::OnClick));

		CLayout* pLayoutPvp = (CLayout*)m_ControLayer->findWidgetById("layer_up_pvp");

		m_iGameTimeCount = 90;			//世界boss打180s,3min
		m_bCountDown = true;

		//vs数据
		VsAnimateData &vsData = DataCenter::sharedData()->getVsAnimateData();

		//名字
		CLabel *pName1 = (CLabel *)pLayoutPvp->findWidgetById("pvp_name1");
		pName1->setString(vsData.sEnemyRoleName.c_str());
		CLabel *pName2 = (CLabel *)pLayoutPvp->findWidgetById("pvp_name2");
		pName2->setString(vsData.sSelfRoleName.c_str());

		//更新头像
		updateHeadIcon();

	}

	void WarControl::initNormalAbove()
	{
		//时间layer
		CLayout *pTimeUILayer = (CLayout *)m_ControLayer->findWidgetById("layer_time");
		int iCountDownTime = mManage->getStageTimeCountDown();
		bool isShowCountDown = iCountDownTime > 0;
		
		//倒计时
		if ( isShowCountDown )
		{
			m_iGameTimeCount = iCountDownTime;			//服务器传回时间
			m_bCountDown = true;
			pTimeUILayer->setVisible(true);		//显示时间layer
			pTimeUILayer->setPosition( pTimeUILayer->getPosition() + ccp(410, -37) );		//位置调整
		}
		else
		{
			pTimeUILayer->setVisible(false);
		}

		CCheckBox* bt_speed = (CCheckBox*)m_ControLayer->getChildByTag(CL_AddSpeedBtn);						//加速按钮
		bt_speed->setOnTouchBeganListener(this,ccw_touchbegan_selector(WarControl::onTouchBegin));
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
	void WarControl::initWorldBossAbove(BaseRole* boss)
	{			
		m_iGameTimeCount = 180;			//世界boss打180s,3min
		m_bCountDown = true;

		CLabel* pDamageText = (CLabel*)m_ControLayer->findWidgetById("attack_index");					//伤害
		m_pAllDamage = CCLabelAtlas::create("0", "worldBoss/no_07.png", 17, 28, 48);
		m_pAllDamage->setAnchorPoint(ccp(1.0f, 0.5f));
		m_pAllDamage->setPosition(pDamageText->getPosition() + ccp(-36, 15));
		pDamageText->getParent()->addChild(m_pAllDamage, pDamageText->getZOrder());
		bNotification->addObserver(this,callfuncO_selector(WarControl::updateWorldBossBloodBar),B_WorldBoss_HurtUpdate,nullptr);

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
		pBuffRank->setString(ToString(mManage->getWorldBossRank()));
	}
	//update word boss damage
	void WarControl::updateWorldBossDamage()
	{
		BaseRole* boss = mManage->getAliveByType(sMonsterSpecies::eWorldBoss);
		int num = mManage->getBossHurtCount();
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
		CCArray* arr = mManage->getHeros(true);
		CCObject* obj = nullptr;
		int costNum = 0;
		int costMax = 0;
		CCARRAY_FOREACH(arr,obj)
		{
			BaseRole* alive = (BaseRole*)obj;
			costMax += alive->getCostMax();
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
		EffectObject* Eff = EffectObject::create("9051",sPlayType::eRepeat);						//火焰特效
		Eff->setPosition(ccp(pCircle->getContentSize().width/2,pCircle->getContentSize().height/2));
		pCircle->addChild(Eff);
	}
	//初始化武将按钮信息
	void WarControl::initAliveButton(CCNode* Layout,BaseRole* pRole)
	{
		CButton* btn = (CButton*)Layout->getChildByTag(CL_Btn);
		btn->setLoingClickTime(0.5f);
		btn->setVisible(true);
		btn->setUserObject(pRole);														//按钮和武将绑定
		btn->setOnClickListener(this,ccw_click_selector(WarControl::AliveButtonClick));
		btn->setOnTouchBeganListener(this,ccw_touchbegan_selector(WarControl::AliveButtonBeginClick));
		btn->setOnLongClickListener(this,ccw_longclick_selector(WarControl::AliveButtonLongClick));
		btn->getSelectedImage()->setScale(0.95f);
		if (BattleModelManage->isPvEBattle() && pRole->getCaptain())
		{
			btn->setEnabled(false);
			btn->setVisible(false);
		}

		EffectObject* CallAliveEffect = EffectObject::create("10028",sPlayType::eRepeat);			//call role effect
		CallAliveEffect->setTag(CL_BtnCallEff);
		CallAliveEffect->setPosition(ccpAdd(btn->getPosition(),ccp(0,50)));
		Layout->addChild(CallAliveEffect);

		EffectObject* AliveSkillEffect = EffectObject::create("10019",sPlayType::eRepeat);			//can use role skill effect
		AliveSkillEffect->setTag(CL_BtnSkillEff);
		AliveSkillEffect->setEffAnchorPoint(0.5f,0.2f);
		AliveSkillEffect->setPosition(btn->getPosition());
		Layout->addChild(AliveSkillEffect, -1);

		const skEffectData* effect = pRole->getBaseData()->getActiveSkill()->getSummonEffect();		//这个方法是应该放在武将身上的
		if (!effect)
			return;
		initButtonBackImage(btn,effect->getCallNumber());											//reason call number init back image
		int CallNumber = effect->getCallNumber()>5?5:effect->getCallNumber();						//guide chapter call number infinite
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
			sp->initWithFile(ptr);
		}else{
			sp->setVisible(true);
		}

		CProgressBar* CdBar = (CProgressBar*)Layout->getChildByTag(CL_HeroPro);	//Bar都是增量变化的CD时间也做成增量的变化方式,满的情况bar消失。点击按钮后重现
		CdBar->setProgressImage(ptr);
		CdBar->setValue(100);
		CdBar->setDirection(eProgressBarDirectionBottomToTop);
		CdBar->setOnProgressEndedListener(this,ccw_progressended_selector(WarControl::AliveBarFullCallBack));
	}

	void WarControl::initCaptinButtonBar( CCNode* Layout,BaseRole* pRole )
	{
		const RoleSkill* tSkill = pRole->getBaseData()->getActiveSkill();
		const skEffectData* tEffect = tSkill->getSummonEffect();
		BaseRoleData* tBaseData = BattleData->getChildRoleData(tEffect->getCallRoleID());
		if ( !tBaseData)
		{
			CCLOG("[ *ERROR ]WarControl::initCaptinButton  CallId =%d ",tEffect->getCallRoleID());
			return;
		}
		initAliveButtonBar(Layout,tBaseData->getRoleModel());
	}

	//根据按钮上的bar来控制按钮的状态。
	void WarControl::AliveBarFullCallBack(CCObject* ob)
	{
		CProgressBar* CdBar = (CProgressBar*)ob;
		CButton* btn = (CButton*)CdBar->getParent()->getChildByTag(CL_Btn);
		btn->setEnabled(true);
		bNotification->postNotification(MsgButtonStateUpdate);
	}
	//初始化我方武将UI列表
	void WarControl::initAliveButtons()
	{
		CCArray* arr = mManage->getHeros();
		CCObject* obj = nullptr;
		int tIndex = 0;
		CCARRAY_FOREACH(arr,obj)
		{
			BaseRole* tHero = (BaseRole*)obj;
			CLayout* MoveLaout = nullptr;	
			if (tHero->getCaptain())
			{
				tHero->setUiLayout(CL_BtnLayout1);
				CLayout*BtnLay = (CLayout*)m_ControLayer->getChildByTag(CL_BtnLayout1);
				BtnLay->setVisible(true);
				CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
				const RoleSkill* skill = tHero->getBaseData()->getActiveSkill();
				NeedCost->setString(ToString(skill->getExpendCost()));									//初始化值为技能消耗cost
				MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
				MoveLaout->setUserObject(MoveLaout);
				initCaptinButtonBar(MoveLaout,tHero);
			}else{
				tHero->setUiLayout(CL_BtnLayout2+tIndex);
				CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(CL_BtnLayout2+tIndex);
				BtnLay->setVisible(true);
				CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
				NeedCost->setString(ToString(tHero->getBaseData()->getExpendCost()));						//初始化值为上阵消耗cost
				MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
				MoveLaout->setPosition(ccpAdd(MoveLaout->getPosition(),ccp(0,-30)));
				MoveLaout->setUserObject(nullptr);
				initAliveButtonBar(MoveLaout,tHero->getModel());
				tIndex++;
			}
			initAliveButton(MoveLaout,tHero);
		}
	}
	//cost变化数字,每一帧的时间都是有误差的,极端的情况容易出现变化的数字
	void WarControl::updateCostSpeed(float dt)
	{
		int tSpeed = mManage->getCostSpeed() * 60;
		m_pCostChange->setString(CCString::createWithFormat("%d", abs(tSpeed))->getCString());
		if(m_pCostChange->getColor().g == 0)
		{
			m_pCostChange->stopAllActions();
			m_pCostChange->setColor(ccc3(255, 255, 255));

			m_pCostBar->setOpacity(255);
			m_pCostBar->stopAllActions();
		}
	}
	//必须实时更新，当cost足够时，会刷新显示特效	
	void WarControl::updateCostNumber()
	{
		m_iAimCost = mManage->getCurrCost();
		updateAliveButtonEffect();														//update button effect state									

		if (m_pCostBar->getPercentage()==1)
			m_ControLayer->getChildByTag(CL_MAXcostTips)->setVisible(true);
		else
			m_ControLayer->getChildByTag(CL_MAXcostTips)->setVisible(false);	
		m_pCostNum->setString(CCString::createWithFormat("%d / %d", m_iAimCost, m_pCostBar->getMaxValue())->getCString());

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
	bool WarControl::guideStateButtonEffect( CCNode* layout,bool pClick )
	{
		if (!ManageCenter->getCombatGuideMg()->IsGuide())					//引导判断
			return false;
		CombatGuideStep* step = ManageCenter->getCombatGuideMg()->getCurrStep();
		if (step->getType() == UI_Type || step->getType() == CallAalive_Type)
		{
			if (layout->getParent()->getTag() != (step->getUiType()-1)+CL_BtnLayout1)
				return true;
			else if (step->getType() == UI_Type && pClick)
				ManageCenter->getCombatGuideMg()->NextStep();
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
			BaseRole* tRole = (BaseRole*)btn->getUserObject();
			if (!tRole)continue;
			MoveLaout->getChildByTag(CL_BtnSkillEff)->setVisible(false);
			MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(false);	
			if (guideStateButtonEffect(MoveLaout,false))									//in guide state effect dispose 
				continue;
			if (tRole->getBattle())
			{
				const RoleSkill* skill = tRole->getBaseData()->getActiveSkill();
				if (m_iAimCost >= skill->getExpendCost() && btn->isEnabled())
				{
					if (tRole->canSummonAlive())
					{
						MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(true);				
					}else if(skill->getSkillID() && skill->getSkillType() != eCallAtk){
						MoveLaout->getChildByTag(CL_BtnSkillEff)->setVisible(true);
					}	
				}		
			}else{			
				if(m_iAimCost >= tRole->getBaseData()->getExpendCost() && btn->isEnabled())
					MoveLaout->getChildByTag(CL_BtnCallEff)->setVisible(true);		
			}
		}
	}
	//call role log in battlefield
	void WarControl::CallRoleEntranceBattle(BaseRole*pRole)
	{
		BaseRole* tRole = mManage->getRole(pRole->getFatherID());
		CCNode* MoveLaout = getMoveLayout(tRole->getUiLayout()-CL_BtnLayout1);
		CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
		initButtonBackImage(btn,tRole->getCallAliveNum());

		CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
		btn->setEnabled(false);
		mManage->changeCost( - tRole->getBaseData()->getActiveSkill()->getExpendCost());
		float tCdTime = tRole->getBaseData()->getActiveSkill()->getCooldown();
		float tDirectorTime = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
		if (tRole->canSummonAlive())														//is call full
			CdBar->startProgressFromTo(0,100,tCdTime/tDirectorTime);
	}
	//role log in battlefield or leave
	void WarControl::AliveBattlefield( BaseRole* pRole )
	{
		CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(pRole->getUiLayout());
		CCNode* MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
		CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
		CLabel* NeedCost = (CLabel*)BtnLay->getChildByTag(CL_NeedCost);
		const RoleSkill* skill = pRole->getBaseData()->getActiveSkill();
		float tDirectorTime = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
		if ((pRole->getBattle()&&MoveLaout->getUserObject()) || 
			(!pRole->getBattle() && !MoveLaout->getUserObject()) )
			return;
		if (pRole->getBattle())														
		{
			MoveLaout->setUserObject(MoveLaout);
			MoveLaout->runAction(CCMoveBy::create(0.2f,ccp(0,30)));								//武将上阵
			CdBar->startProgressFromTo(0,100,skill->getCooldown()/tDirectorTime);				//技能CD
			NeedCost->setString(ToString(skill->getExpendCost()));								//初始化值为上阵消耗cost
			mManage->changeCost(-pRole->getBaseData()->getExpendCost());
		}else{	
			MoveLaout->setUserObject(nullptr);
			MoveLaout->runAction(CCMoveBy::create(0.2f,ccp(0,-30)));								//武将死亡
			float tCdTime = pRole->getBaseData()->getColdDown();
			CdBar->startProgressFromTo(0,100,tCdTime/tDirectorTime);								//复活时间
			NeedCost->setString(ToString(pRole->getBaseData()->getExpendCost()));				//上阵消耗cost
		}
		CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
		btn->setEnabled(false);
	}
	//role log in or leave message dispose
	void WarControl::AliveBattleDispose(CCObject* ob)
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getOtherCamp())
			return;
		if (tRole->getFatherID())
		{
			BaseRole* tFather = mManage->getRole(tRole->getFatherID());
			if (!tFather || tFather->getCallType() == sCallType::eBoxHaveRole)
				return;
			CallRoleEntranceBattle(tRole);
		}else{
			AliveBattlefield(tRole);
		}
	}

	void WarControl::postButtonState( CCObject* ob )
	{
		if ( !ob )return ;
		BaseRole* tRole = (BaseRole*)ob;
		CLayout* BtnLay = (CLayout*)m_ControLayer->getChildByTag(tRole->getUiLayout());
		CCNode* MoveLaout = (CLayout*)BtnLay->getChildByTag(CL_HeroNode);
		CButton* btn = (CButton*)MoveLaout->getChildByTag(CL_Btn);
		if (tRole->getBattle())
		{
			const RoleSkill* skill = tRole->getBaseData()->getActiveSkill();
			if (m_iAimCost >= skill->getExpendCost() && btn->isEnabled())
			{
				AliveButtonClick(btn);
				this->scheduleOnce(schedule_selector(WarControl::upAutoSkillState),6.0f);		//技能释放间隔最少6秒
			}
		}else{
			if(m_iAimCost >= tRole->getBaseData()->getExpendCost() && btn->isEnabled())
			{
				bNotification->postNotification(MsgSendButtonState,CCBool::create(true));
			}else{
				bNotification->postNotification(MsgSendButtonState,CCBool::create(false));
			}
		}
	}
	void WarControl::upAutoSkillState( float dt )
	{
		bNotification->postNotification(MsgUpAutoSkillState);
	}
	//judge button touch transmit direction
	CWidgetTouchModel WarControl::AliveButtonBeginClick(CCObject* ob,CCTouch* pTouch)
	{
		CButton* btn = (CButton*)ob;	
		BaseRole* tRole = dynamic_cast<BaseRole*>(btn->getUserObject());			//根据点击的按钮来判断点了哪个人
		int cost = mManage->getCurrCost();
		if (tRole->getBattle())
		{
			RoleSkill* skill = tRole->getBaseData()->getActiveSkill();		//我方目前只有主动技为召唤技
			if (skill->getSkillType() == eCallAtk&&cost >= skill->getExpendCost())
			{
				BaseRole* atRole = tRole->getCallRole(skill);
				if (!atRole)
					return eWidgetTouchTransient;
				mManage->executeCaptainSkill();
				bNotification->postNotification(B_EntranceBattle,atRole);			//召唤成功才能扣减Cost值
				return eWidgetTouchNone;
			}	
			return eWidgetTouchTransient;											//武将为上阵状态则触摸不传递到下层,否则将触摸传递到下层处理
		}else{
			if (!tRole->getAliveState())
			{
				tRole->initAliveData();
				mManage->executeCaptainSkill();
			}
			if (cost < tRole->getBaseData()->getExpendCost())
				return eWidgetTouchTransient;
			bNotification->postNotification(B_EntranceBattle,tRole);			//召唤成功才能扣减Cost值
			showMonsterTips(nullptr);
			return eWidgetTouchNone;	
		}
	}
	//role skill button dispose
	void WarControl::AliveButtonClick( CCObject* ob )
	{
		CButton* btn = (CButton*)ob;	
		BaseRole* tRole = dynamic_cast<BaseRole*>(btn->getUserObject());			//根据点击的按钮来判断点了哪个人
		const RoleSkill* skill = tRole->getBaseData()->getActiveSkill();
		if (  skill->getSkillType() == eCallAtk
			||mManage->getCurrCost()<skill->getExpendCost() 
			||!tRole->getBattle() 
			||!tRole->getRoleObject()
			||tRole->getLogicState() == sLogicState::eFree
			||tRole->getLogicState() == sLogicState::eInvincible)
			return;
		CCNode* MoveLaout = getMoveLayout(tRole->getUiLayout()-CL_BtnLayout1);
		btn->setEnabled(false);
		if (guideStateButtonEffect(MoveLaout,true))
			return;
		tRole->getRoleObject()->setMoveState(sStateCode::eNullState);
		tRole->getRoleObject()->TurnStateTo(sStateCode::eStandState);
		tRole->ResetAttackState();																	//点击了必杀技按钮，但是武将并没有进入必杀技状态的情况,强制切换至必杀技状态
		tRole->setCriAtk(true);
		CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
		CdBar->setValue(0);
		bNotification->postNotification(B_RecordContinuousSkill);
		mManage->changeCost( -skill->getExpendCost());
		if (tRole->getCaptain())
			ResetButtonState(tRole);
		else
			tRole->getRoleObject()->setUpdateState(false);
	}

	void WarControl::ResetButtonState( CCObject* ob )
	{
		if (!ob)return;
		BaseRole* tRole = (BaseRole*)ob;
		CCNode* MoveLaout = getMoveLayout(tRole->getUiLayout() - CL_BtnLayout1);
		CProgressBar* CdBar = (CProgressBar*)MoveLaout->getChildByTag(CL_HeroPro);
		float tDirectorTime = CCDirector::sharedDirector()->getScheduler()->getTimeScale();
		float tCdTime = tRole->getBaseData()->getActiveSkill()->getCooldown();
		CdBar->startProgressFromTo(0,100,tCdTime/tDirectorTime);										//技能CD
	}

	bool WarControl::AliveButtonLongClick(CCObject* pSender, CCTouch* pTouch)
	{
		CButton* tog = (CButton*)pSender;
		BaseRole* alive = (BaseRole*)tog->getUserObject();
		SkillTips *tips = SkillTips::create(); 
		tips->setSkillInfo(pSender,alive);//根据武将当前状态判断武将释放的技能从而给出提示信息( CC制不需要技能提示 )
		LayerManager::instance()->push(tips);
		bNotification->postNotification(B_DrawDynamicSkillArea,alive);
		return true;
	}
	//长按取消操作,可以使用添加精灵或是做一个类似于连击节点的方法实现，而不用添加层
	void WarControl::onlongClickEnd(CCObject* pSender, CCTouch* pTouch, float fDuration)
	{
		bNotification->postNotification(B_RemoveSkillTipsLayer);
		bNotification->postNotification(B_CancelCostArea,nullptr);
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

	void WarControl::upContinuousNumber(CCObject* ob)
	{
		CLayout* Layout = (CLayout*)m_ControLayer->getChildByTag(CL_ContinuousLayou);
		if (ob)
		{
			if (!Layout->isVisible())
				return;
			this->unschedule(schedule_selector(WarControl::upContinuousNodeState));
			this->scheduleOnce(schedule_selector(WarControl::upContinuousNodeState),3.0f);
		}else{
			m_ContinuousNum++;
			if (m_ContinuousNum <= 1)
				return;
			this->unschedule(schedule_selector(WarControl::upContinuousNodeState));
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
			this->scheduleOnce(schedule_selector(WarControl::upContinuousNodeState),1.5f);
		}
	}

	void WarControl::upContinuousNodeState(float dt)
	{
		m_ControLayer->getChildByTag(CL_ContinuousLayou)->setVisible(false);
		m_ContinuousNum = 0;
	}
	//绘制怪物出现行数效果提示
	void WarControl::showMonsterTips(CCObject* ob)
	{
		vector<int>Vec;
		for (auto tRole: mManage->inBattleMonsters())
		{
			if (tRole->getGridIndex() < 52)continue;			//不再预警标记显示范围内
			Vec.push_back(tRole->getGridIndex() % C_GRID_ROW);
		}
		VectorUnique(Vec);
		for (auto i:Vec)
		{
			EffectObject* ef = (EffectObject*)m_ControLayer->getChildByTag(CL_TipsEffect1 + i);
			ef->setVisible(true);
			ef->stopAllActions();
			CCDelayTime* dt = CCDelayTime::create(1.5f);
			ef->runAction(CCSequence::create(dt,CCHide::create(),NULL));
		}
	}
	//必杀技闪屏效果
	void WarControl::SkillMask( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy())
			return;
		if (tRole->getBaseData()->getProperty() == sRoleNature::eFire)
		{
			m_LayerColor->setColor(ccc3(206,17,0));
		}else if (tRole->getBaseData()->getProperty() == sRoleNature::eWater)
		{
			m_LayerColor->setColor(ccc3(30,69,218));
		}else{
			m_LayerColor->setColor(ccc3(200,167,60));
		}
		m_LayerColor->stopAllActions();
		m_LayerColor->setVisible(true);
		m_LayerColor->runAction(CCSequence::create(CCFadeOut::create(0.07f),CCHide::create(),NULL));
	}
	//显示击杀怪物增加cost效果
	void WarControl::showFlyCostToBar( CCObject* ob )
	{
		CCNode* tRoleObject = (CCNode*)ob;
		CCPoint pStartPos = tRoleObject->getParent()->convertToWorldSpace(tRoleObject->getPosition());
		float fPosAdd = m_pCostBar->getContentSize().width*(m_pCostBar->getPercentage()-0.5f);
		if(m_pCostBar->getPercentage()>=0.95f)
			fPosAdd = 0;
		CCPoint pEndPos = m_pCostBar->getPosition() + ccp(fPosAdd, 0);
		ccBezierConfig bezier;
		pStartPos = m_ControLayer->convertToNodeSpace(pStartPos);
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
		CCParticleSystemQuad* pLz  = CCParticleSystemQuad::create("lz/blue_bomb.plist");
		pLz->setAutoRemoveOnFinish(true);
		pLz->setPosition(pEndPos);
		pLz->resetSystem();
		m_ControLayer->addChild(pLz);			//绑定粒子
		PlayEffectSound(SFX_423);
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
			CCSize size = CCDirector::sharedDirector()->getWinSize();
			m_ArmatureTips->setPosition(ccp(size.width/2,size.height/2));
			m_ArmatureTips->setVisible(false);
			m_ControLayer->addChild(m_ArmatureTips);
		}else{
			CCLOG("[ *ERROR ] WarAssist::PlayBeginAnimation animationData NULL");
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MsgCreateStory,CCInteger::create(1));
		}
	}

	void WarControl::CaptainHit( CCObject* ob )
	{
		BaseRole* alive = (BaseRole*)ob;
		if (alive->getHp()>0)
		{
			m_ArmatureTips->stopAllActions();
			m_ArmatureTips->setVisible(true);
			m_ArmatureTips->setOpacity(255);
			m_ArmatureTips->runAction(CCSequence::create(CCDelayTime::create(0.5f),CCFadeOut::create(0.8f),nullptr));
		}else{
			PlayEffectSound("SFX/511.ogg");
			m_ArmatureTips->setOpacity(255);
			m_ArmatureTips->setVisible(true);
			m_ArmatureTips->getAnimation()->play("die",0.01f);
		}
	}

	void WarControl::hideUiUpPart(CCObject* ob)
	{
		m_ControLayer->findWidgetById("layer_up_boss")->setVisible(false);
		m_ControLayer->findWidgetById("layer_up_normal")->setVisible(false);
		m_ControLayer->getChildByTag(CL_Menu)->setVisible(false);
		m_ControLayer->getChildByTag(CL_AddSpeedBtn)->setVisible(false);
		m_ControLayer->getChildByTag(CL_AutoPlay)->setVisible(false);
	}

	void WarControl::updateBatchNumber( CCObject* ob )
	{
		CLayout* m_pNormal = (CLayout*)m_ControLayer->findWidgetById("layer_up_normal");
		CCNode* node = m_pNormal->getChildByTag(CL_Batch);
		node->setVisible(false);
		char path[60] = {0};
		sprintf(path,"%d/%d",mManage->getCurrBatch()+1,mManage->getBatch()+1);											
		CCLabelAtlas* BatchNum = CCLabelAtlas::create(path,"label/wave_number2.png", 22, 24, 47);
		BatchNum->setAnchorPoint(ccp(0.5f,0.43f));
		BatchNum->setPosition(node->getPosition());
		BatchNum->setTag(CL_Batch);
		m_pNormal->addChild(BatchNum);
		node->removeFromParentAndCleanup(true);
	}

	void WarControl::changeSpeed()
	{
		if (CCDirector::sharedDirector()->getScheduler()->getTimeScale() > 1)
		{
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
		}else{
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(2);
		}
	}

	void WarControl::battleBegin()
	{
		this->setVisible(true);
		if(m_bCountDown)
			schedule(schedule_selector(WarControl::updateOneSecond), 1.0f);
	}

	void WarControl::updateOneSecond( float dt )
	{
		if(!m_bCountDown)
			return;
		m_iGameTimeCount--;
		if (m_iGameTimeCount%5 == 0)
			mManage->saveWordBossHurt();
		if( m_iGameTimeCount < 0 )
		{
			unschedule(schedule_selector(WarControl::updateOneSecond));
			mManage->setLogicState(false);
			PlayBackgroundMusic(SFX_Win,false);
			NOTIFICATION->postNotification(MsgBattleOver);
		}else{
			updateTimeCountUI(m_iGameTimeCount);							//更新计时器
		}
	}

	void WarControl::imageLoadSuccessCallBack( string sTag, vector<char>* pBuffer )
	{
		CCImage* img = new CCImage;
		img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
		CCTexture2D* texture = new CCTexture2D();
		texture->initWithImage(img);
		//保存facebook 头像
		string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
		string buff(pBuffer->begin(), pBuffer->end());
		CCLOG("path: %s", path.c_str());
		FILE *fp = fopen(path.c_str(), "wb+");
		fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
		fclose(fp);

		updateHeadIcon();
	}

	void WarControl::updateHeadIcon()
	{
		CLayout* pLayoutPvp = (CLayout*)m_ControLayer->findWidgetById("layer_up_pvp");

		//vs数据
		VsAnimateData &vsData = DataCenter::sharedData()->getVsAnimateData();

		//头像
		const char *sHead[2] = {"pvp_head1", "pvp_head2"};
		int iThumb[2] = {vsData.iEnemyHead, vsData.iSelfHead};
		string iFbId[2] = {vsData.sEnemyFacebookId, vsData.sSelfFacebookId};

		for(int i=0; i<2; i++)
		{
			CCSprite* pHead = (CCSprite*)pLayoutPvp->findWidgetById(sHead[i]);
			CCString* pHeadStr = CCString::createWithFormat("headImg/%d.png", iThumb[i]);
			if ( iThumb[i]>0 )
			{
				std::string strFullPathHead = CCFileUtils::sharedFileUtils()->fullPathForFilename(pHeadStr->getCString());
				if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPathHead))
				{
					pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(pHeadStr->getCString()));
				} 
			}
			else
			{
				//加载facebook头像
				string fbName = iFbId[i]+".jpg";
				string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
				bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
				if(isFileExist)
				{
					pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(fullName.c_str()));
				}
				else
				{
					CCString *imgUrl = CCString::createWithFormat("http://graph.facebook.com/%s/picture?width=92&height=92", iFbId[i].c_str());
					HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(), iFbId[i].c_str());
				}
			}
		}
	}

};