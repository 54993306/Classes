
#include "Battle/ComBatLogic.h"
#include "Battle/BattleScene/LoadBattleResource.h"
#include "Battle/BattleScene/BattleScene.h"
#include "Battle/BufExp.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "model/BuffManage.h"
#include "model/MapManager.h"
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
#include "update/CDownloadPackage.h"
#include "jni/CJniHelper.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleSkill.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
#include "Battle/Strategy/TotalStrategy.h"
namespace BattleSpace
{
	CombatLogic::CombatLogic()
		:mTime(0),mAssist(nullptr),mTask(nullptr),mCombatEffect(nullptr),m_FiratBatch(true)
		,mMapLayer(nullptr),mControlLayer(nullptr),mBattleScene(nullptr)
		,mManage(nullptr),mTotalStrategy(nullptr),mGuideManage(nullptr)
		,mCritRole(nullptr),mRecvFinish(false),m_PlayerNum(0),mbufExp(nullptr)
		,m_Record(true),m_RecordNum(0)
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
		mManage = DataCenter::sharedData()->getWar();
		mGuideManage = DataCenter::sharedData()->getCombatGuideMg();
		m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
		return true;
	}

	void CombatLogic::addEvent()
	{
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::StoryEndEvent),MsgStorOver,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::CritAtkEnd),B_CritEnd,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::BatterRecord),B_RecordContinuousSkill,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::rolePlyaSkill),B_RoleSkill,nullptr);
	}

	void CombatLogic::initMapBackground()						//可以搬到maplayer
	{ 
		if (mMapLayer->getBackgroundManage())
		{
			mMapLayer->getBackgroundManage()->setMap(mMapLayer);
			mMapLayer->getBackgroundManage()->initWithStage(mManage->getStageID());
		}
		bNotification->postNotification(MsgUpBatchNumber);
	}

	void CombatLogic::showRound()
	{
		if (mTime >= 0.8f && m_FiratBatch && !mGuideManage->IsGuide())
		{
			mManage->setLogicState(true);
			m_FiratBatch = false;
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
		runLogic(delta);
		mControlLayer->updateCostNumber();
		mControlLayer->updateCostSpeed(delta);
		mManage->setCostSpeed(0);
	}

	void CombatLogic::runLogic(float delta)
	{
		if ( !mManage->getLogicState() || 
			mGuideManage->IsGuide())
			return;
		mManage->updateAlive();							//更新武将信息
		ExcuteAI(delta);
		if ( !mCritRole )
		{
			mManage->costUpdate(delta);
			mTotalStrategy->excuteStrategy(delta);		//我方释放必杀技是不做AI处理
		}
	}
	//cost 计算(帧)或以秒为单位进行计算、位置实时更新
	void CombatLogic::CostCount(BaseRole* tRole,float dt)
	{
		if ( tRole->getEnemy() || tRole->getFatherID() || mCritRole)			
			return;
		if (mManage->inAddCostArea(tRole->getGridIndex()))
		{
			float tSpeed = mManage->getCostSpeed() + (tRole->getAddCost()* dt);
			mManage->setCostSpeed(tSpeed);
		}
	}
	//每一帧都判断每个武将的信息                   
	void CombatLogic::ExcuteAI(float pTime)
	{
		const RolesMap* tMapRole = mManage->getRolesMap();
		RolesMap::const_iterator iter = tMapRole->begin();
		for (;iter != tMapRole->end(); iter++)
		{
			BaseRole* tRole = iter->second;
			if (mCritRole && tRole != mCritRole)continue;									//一次只处理一个武将播放技能的情况，若是处理多个则以武将技能状态判断，而不用单一武将进行判断
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
			mManage->setLogicState(false);
			mCombatEffect->BatterSpine(m_RecordNum);
			m_PlayerNum = 0; 
			m_RecordNum = 0;
			mCombatEffect->setPlayerNum(m_PlayerNum);
			m_Record = true;
		}
	}

	void CombatLogic::CritAtkEnd(CCObject* ob)
	{
		BaseRole* tRole = dynamic_cast<BaseRole*>(ob);
		tRole->AliveCritEnd();
		mControlLayer->ResetButtonState(mCritRole);
		bNotification->postNotification(MsgChangeLayerLight,CCBool::create(false));
		mCritRole = nullptr;
		critComboEffect();
	}

	void CombatLogic::rolePlyaSkill( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		mCombatEffect->PlayerSkill(tRole);
		if (!tRole->getEnemy())
		{
			mManage->setLogicState(false);
			mCritRole = tRole;
		}
		mMapLayer->DrawWarningEffect(tRole->mSkillArea);						//格子预警
	}
	void CombatLogic::beginStageFloorEffect()								//可以搬到maplayer
	{
		if (!mManage->getStageID())
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
				ef->setPosition(m_MapData->getPoint(grids[i]));
				ef->setZOrder(10);
				mMapLayer->addChild(ef);
			}
		}
	}

	void CombatLogic::beginStoryEnd()
	{
		beginStageFloorEffect();
		PlayEffectSound(SFX_413);	
		CCDelayTime* delay = CCDelayTime::create(0.5f);
		CCDelayTime* delay2 = CCDelayTime::create(0.3f);
		CCMoveTo* mt = CCMoveTo::create(1.2f,ccp( MAP_MINX(m_MapData) , mBattleScene->getMoveLayer()->getPositionY()));
		CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(BattleScene::LayerMoveEnd),CCInteger::create((int)StoryType::eMoveEndStory));
		CCSequence* sqe = CCSequence::create(delay,mt,delay2,cfo,nullptr);
		mBattleScene->getMoveLayer()->runAction(sqe);
	}

	void CombatLogic::moveStoryEnd()
	{
		mControlLayer->battleBegin();
		mBattleScene->AddEvent();																//场景移动动画结束，添加事件响应
		scheduleUpdate();																		//开启游戏主循环
		PlayBackgroundMusic(BGM_Battle,true);
		mGuideManage->setCurrBatchGuide(nullptr);
		if (mManage->getStageID() == 301)
			mGuideManage->setCurrBatchGuide("CostGuide");
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
				moveStoryEnd();		
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