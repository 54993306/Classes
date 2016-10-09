
#include "Battle/ComBatLogic.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/BattleScene/BattleScene.h"
#include "Battle/BufExp.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/BuffManage.h"
#include "Battle/CoordsManage.h"
#include "tools/ShowTexttip.h"
#include "Battle/ConstNum.h"
#include "Battle/WarAssist.h"
#include "Battle/MapEffect.h"
#include "Battle/CombatTask.h"
#include "Battle/EffectData.h"
#include "Battle/WarControl.h"
#include "Battle/WarBackLayer.h"
#include "Battle/CaptainSkill.h"
#include "Battle/CombatEffect.h"
#include "Battle/BattleResult.h"
#include "common/CommonFunction.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleLayer/BattleMapLayer.h"
#include "Battle/EffectObject.h"
#include "Battle/CombatGuideManage.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "Battle/BattleMessage.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleSkill.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
#include "Battle/Strategy/TotalStrategy.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BattleModel.h"
#include "Battle/Config/ConfigManage.h"
#include "Battle/Landform/TrapManage.h"
namespace BattleSpace
{
	CombatLogic::CombatLogic()
		:mTime(0),mAssist(nullptr),mTask(nullptr),mCombatEffect(nullptr),mFirstBatch(true)
		,mMapLayer(nullptr),mControlLayer(nullptr),mBattleScene(nullptr),mInterval(0)
		,mTotalStrategy(nullptr),m_Record(true),m_RecordNum(0)
		,mCritRole(nullptr),mRecvFinish(false),m_PlayerNum(0),mbufExp(nullptr)
		
	{}

	CombatLogic::~CombatLogic()
	{
		CC_SAFE_RELEASE(mAssist);
		mAssist = nullptr;
		CC_SAFE_RELEASE(mTask);
		mTask = nullptr;
		CC_SAFE_RELEASE(mbufExp);
		mbufExp = nullptr;
		CC_SAFE_RELEASE(mCombatEffect);
		mCombatEffect = nullptr;
		CC_SAFE_RELEASE(mTotalStrategy);
		mTotalStrategy = nullptr;
	}

	void CombatLogic::onExit()
	{
		CCNode::onExit();
		bNotification->removeAllObservers(this);
		this->unscheduleAllSelectors();
	}

	void CombatLogic::onEnter()
	{
		CCNode::onEnter();
		mBattleScene = (BattleScene*)this->getParent(); 
		mAssist->setScene(mBattleScene);
		mCombatEffect->setScene(mBattleScene);
		mControlLayer = mBattleScene->getWarUI();
		mControlLayer->setVisible(false);
		mMapLayer = mBattleScene->getBattleMapLayer();
		addEvent();
		initMapBackground();
	}

	bool CombatLogic::init()
	{
		mTask = CCArray::create();
		mTask->retain();
		mbufExp = BufExp::create();
		mbufExp->retain();
		mAssist = WarAssist::create();
		mAssist->retain();
		mCombatEffect = CombatEffect::create();
		mCombatEffect->retain();
		mTotalStrategy  = TotalStrategy::create();
		mTotalStrategy->retain();
		return true;
	}

	void CombatLogic::addEvent()
	{
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::StoryEndEvent),MsgStorOver,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::CritAtkEnd),B_CritEnd,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::BatterRecord),B_RecordContinuousSkill,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::rolePlyaSkill),B_RoleSkill,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::layerMove),B_EntranceBattle,nullptr);
	}

	void CombatLogic::initMapBackground()						//可以搬到maplayer
	{ 
		if (mMapLayer->getBackgroundManage())
		{
			mMapLayer->getBackgroundManage()->setMap(mMapLayer);
			mMapLayer->getBackgroundManage()->initWithStage(BattleManage->getStageIndex());
		}
		bNotification->postNotification(MsgUpBatchNumber);
	}

	void CombatLogic::showRound()
	{
		if (mTime >= 0.8f && mFirstBatch && !BattleGuide->IsGuide())
		{
			BattleManage->setLogicState(true);
			mFirstBatch = false;
			bNotification->postNotification(MsgMonsterTips);
		}
	}

	void CombatLogic::updateTask()
	{
		CCObject* object = nullptr;
		CCArray* tFinishs = CCArray::create();		//维护一个任务队列
		CCARRAY_FOREACH(mTask,object)
		{
			CombatTask* tTask = (CombatTask*) object;
			if(!tTask) continue;
			if(mTime > tTask->getInterval())
			{
				SEL_CallFuncO fun = tTask->getFun();
				(tTask->getPtarget()->*fun)(tTask->getParam());		//函数指针解引用传入参数
				tFinishs->addObject(object);
			}
		}
		CCARRAY_FOREACH(tFinishs,object)
		{
			mTask->removeObject(object);
		}
		tFinishs->removeAllObjects();					//可以执行其他对象中需要一直循环判断的方法
	}

	void CombatLogic::update(float delta)
	{
		mTime += delta;
		showRound();
		updateTask();
		mbufExp->ResetInterval(delta);
		BattleManage->setCostSpeed(0);
		runLogic(delta);
		mControlLayer->updateCostNumber();
		
	}

	void CombatLogic::runLogic(float delta)
	{
		if ( !BattleManage->getLogicState() || 
			BattleGuide->IsGuide())
			return;
		BattleManage->updateAlive();							//更新武将信息
		ExcuteAI(delta);
		mInterval += delta;
		if ( !mCritRole )
		{
			if (mInterval > 1.0f)						//每 1.0f 秒强行刷新一次策略
			{
				mInterval = 0;
				mTotalStrategy->updateStrategy();
				BattleTrapManage->updateActivateTrap();
				mControlLayer->updateCostSpeed(delta);
			}
			BattleManage->costUpdate(delta);
			mTotalStrategy->excuteStrategy(delta);
			BattleTrapManage->updateTrap(delta);
		}
	}
	//cost 计算(帧)或以秒为单位进行计算、位置实时更新    //该方法位置有问题，可进行重构
	void CombatLogic::CostCount(BaseRole* tRole,float dt)
	{
		if ( tRole->getOtherCamp() || tRole->getFatherID() || mCritRole || 
			 BattleModelManage->isPvEBattle() )			
			return;
		if (BattleConfig->inAddCostArea(tRole->getGridIndex()) || tRole->getCaptain())
		{
			float tSpeed = BattleManage->getCostSpeed() + (tRole->getAddCost()* dt);
			BattleManage->setCostSpeed(tSpeed);
		}
	}
	//每一帧都判断每个武将的信息                   
	void CombatLogic::ExcuteAI(float pTime)
	{
		const RolesMap* tMapRole = BattleManage->getRolesMap();
		RolesMap::const_iterator iter = tMapRole->begin();
		for (;iter != tMapRole->end(); iter++)
		{
			BaseRole* tRole = iter->second;
			if (mCritRole && tRole != mCritRole)
				continue;																	//一次只处理一个武将播放技能的情况，若是处理多个则以武将技能状态判断，而不用单一武将进行判断
			CostCount(tRole,pTime);															//计算我方所有武将cost值
			tRole->excuteLogic(pTime);
		}
	}

	void CombatLogic::BatterRecord( CCObject* ob )
	{
		if (m_Record)
			m_RecordNum++;
	}
	//在此处触发连接的大效果播放。并记录当前一共按了多少次技能,延长连击接上的判定时间
	void CombatLogic::critComboEffect()
	{
		m_Record = false;
		m_PlayerNum++;
		mCombatEffect->setPlayerNum(m_PlayerNum);
		if (m_PlayerNum >= m_RecordNum)
		{
			if (!BattleModelManage->isPvEBattle())
			{
				BattleManage->setLogicState(false);
				mCombatEffect->BatterSpine(m_RecordNum);
			}
			m_PlayerNum = 0; 
			m_RecordNum = 0;
			m_Record = true;
			mCombatEffect->setPlayerNum(0);
		}
	}

	void CombatLogic::CritAtkEnd(CCObject* ob)
	{
		BaseRole* tRole = dynamic_cast<BaseRole*>(ob);
		tRole->AliveCritEnd();
		if (tRole->getOtherCamp())
			return;
		tRole->AliveCritEnd();
		mControlLayer->ResetButtonState(tRole);
		bNotification->postNotification(MsgChangeLayerLight,CCBool::create(false));
		mCritRole = nullptr;
		critComboEffect();
	}

	void CombatLogic::rolePlyaSkill( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (BattleModelManage->battlePause())
		{
			mCombatEffect->PlayerSkill(tRole);
			if (!tRole->getEnemy())
			{
				BattleManage->setLogicState(false);
				mCritRole = tRole;
			}
			mMapLayer->DrawWarningEffect(tRole->mSkillArea);						//格子预警
		}else{
			mMapLayer->DrawWarningEffect(tRole->mSkillArea);						//格子预警
		}
	}
	void CombatLogic::beginStageFloorEffect()								//可以搬到maplayer
	{
		if (!BattleManage->getStageIndex())
		{
			CCSize size  = CCDirector::sharedDirector()->getWinSize();
			int grids[4] = {78,89,102,117};
			for (int i=0;i<4;i++)
			{
				EffectObject*ef  = EffectObject::create("121",sPlayType::eDelay); 
				ef->setMusic(525);
				ef->setShake(true);
				ef->setDelaytime(i*0.35f);
				ef->setLoopInterval(0.2f);
				ef->setLoopNum(2);
				ef->setPosition(BattleCoords->getPoint(grids[i]));
				ef->setZOrder(10);
				mMapLayer->addChild(ef);
			}
		}
	}

	void CombatLogic::layerMove( CCObject* ob )
	{
		if (BattleModelManage->isPvEBattle())
		{
			CCMoveTo* mt = CCMoveTo::create(0.25f,ccp( BattleCoords->CoordsMin() , mBattleScene->getMoveLayer()->getPositionY()));
			mBattleScene->getMoveLayer()->runAction(mt);
		}
	}

	void CombatLogic::beginStoryEnd()
	{
		beginStageFloorEffect();
		PlayEffectSound(SFX_413);
		if (BattleModelManage->layerMove())
		{
			CCDelayTime* delay = CCDelayTime::create(0.5f);
			CCDelayTime* delay2 = CCDelayTime::create(0.3f);
			CCMoveTo* mt = CCMoveTo::create(1.2f,ccp( BattleCoords->CoordsMin() , mBattleScene->getMoveLayer()->getPositionY()));
			CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(BattleScene::LayerMoveEnd),CCInteger::create((int)StoryType::eMoveEndStory));
			CCSequence* sqe = CCSequence::create(delay,mt,delay2,cfo,nullptr);
			mBattleScene->getMoveLayer()->runAction(sqe);
		}else{
			CCDelayTime* delay = CCDelayTime::create(0.5f);
			CCPlace* place = CCPlace::create(ccp( BattleCoords->CoordsMin()+280, mBattleScene->getMoveLayer()->getPositionY()));
			CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(BattleScene::LayerMoveEnd),CCInteger::create((int)StoryType::eMoveEndStory));
			CCSequence* sqe = CCSequence::create(delay,place,cfo,nullptr);
			mBattleScene->getMoveLayer()->runAction(sqe);
		}
	}

	void CombatLogic::moveStoryEnd(CCObject* ob)
	{
		mControlLayer->battleBegin();
		mBattleScene->AddEvent();																//场景移动动画结束，添加事件响应
		scheduleUpdate();																		//开启游戏主循环
		PlayBackgroundMusic(BGM_Battle,true);
		BattleGuide->setCurrBatchGuide(nullptr);
		if (BattleManage->getStageIndex() == 301)
			BattleGuide->setCurrBatchGuide("CostGuide");
	}

	void CombatLogic::StoryEndEvent(CCObject* ob)
	{
		switch ((StoryType)((CCInteger*)ob)->getValue())
		{
		case StoryType::eBeginStory:
			{
				beginStoryEnd();
			}break;
		case StoryType::eMoveEndStory:
			{
				if (BattleModelManage->isPvEBattle()){
					mCombatEffect->showVsAnimate( this );
				}
				else{
					moveStoryEnd(nullptr);	
				}
			}break;
		case StoryType::eOverStory:
			{
				bNotification->postNotification(MsgPlayWinEffect);
			}break;
		}
	}

	void CombatLogic::onPause()
	{
		this->pauseSchedulerAndActions();
	}

	void CombatLogic::onResume()
	{
		this->resumeSchedulerAndActions();
	}
}