
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
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/EffectObject.h"
#include "netcontrol/CPlayerControl.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/WarFailLayer.h"
#include "Battle/WarWinLayer.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "Battle/WorldBossEndLayer.h"
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
	:mTime(0),mAssist(nullptr),mTask(nullptr),mCombatEffect(nullptr)
	,m_CurrBatchNum(0),m_FiratBatch(true),mBattleScene(nullptr)
	,mMapLayer(nullptr),mRoleLayer(nullptr),mControlLayer(nullptr)
	,mManage(nullptr),mTotalStrategy(nullptr),mGuideManage(nullptr)
	,mCritRole(nullptr),m_bRecvFinish(false),m_PlayerNum(0),mbufExp(nullptr)
	,m_iGameTimeCount(0), m_bCountDown(false),m_Record(true),m_RecordNum(0)
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

		mBattleScene = nullptr;
		mControlLayer = nullptr;
		mMapLayer = nullptr;
		mRoleLayer = nullptr;
		mManage = nullptr;
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
		mRoleLayer = mBattleScene->getBattleRoleLayer();
		addEvent();
		initMapBackground();
		initWordBossTime();
	}

	void CombatLogic::initWordBossTime()
	{
		if (mManage->getWorldBoss())
		{
			m_iGameTimeCount = 180;			//世界boss打180s,3min
			m_bCountDown = true;
		}
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

		mGuideManage = DataCenter::sharedData()->getCombatGuideMg();
		mManage = DataCenter::sharedData()->getWar();
		m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
		return true;
	}

	void CombatLogic::addEvent()
	{
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::StoryEndEvent),MsgStorOver,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::ActObjectRemove),B_ActObjectRemove,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::CritAtkEnd),B_CritEnd,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::AliveDieDispose),MsgRoleDie,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::cReturnLayer),B_ReturnLayer,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::BatterRecord),B_RecordContinuousSkill,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::rolePlyaSkill),B_RoleSkill,nullptr);
		bNotification->addObserver(this,callfuncO_selector(CombatLogic::combatResult),MsgWordBossExit,nullptr);
		CNetClient::getShareInstance()->registerMsgHandler(ExitStage,this,CMsgHandler_selector(CombatLogic::OnBattleFinish));
		CNetClient::getShareInstance()->registerMsgHandler(BossFinishReqMsg,this,CMsgHandler_selector(CombatLogic::onWordBossFinish));
	}

	void CombatLogic::initMapBackground() 
	{ 
		if (mMapLayer->getBackgroundManage())
		{
			mMapLayer->getBackgroundManage()->setMap(mMapLayer);
			mMapLayer->getBackgroundManage()->initWithStage(mManage->getStageID());
		}
		mControlLayer->updateBatchNumber(1);
	}

	void CombatLogic::displayRoundTips()
	{
		CCLabelAtlas* labAt = CCLabelAtlas::create(ToString(m_CurrBatchNum+1),"label/wave_number.png", 81, 84, '0');
		ShowTexttip(labAt, ccc3(255, 255, 255), ROUNDNUM, CCPointZero, 0, 0, 0, 200);
		bNotification->postNotification(B_MonsterTips);
	}

	void CombatLogic::showRound()
	{
		if (mTime >= 0.8f && m_FiratBatch && !mGuideManage->IsGuide())
		{
			mManage->setLogicState(true);
			m_FiratBatch = false;
			displayRoundTips();
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
		mRoleLayer->changeLight(false);
		mCritRole = nullptr;
		critComboEffect();
	}

	void CombatLogic::displayGuide()
	{
		if (!mManage->getStageID())															//引导开启的情况
		{
			char path[60] = {0};
			sprintf(path,"%d_%d",0,m_CurrBatchNum+1);										//覆盖高亮区域的图片
			mGuideManage->setGuide(path);
		}
	}

	void CombatLogic::NextBatch( float dt )
	{
		m_CurrBatchNum++;
		mControlLayer->updateBatchNumber(m_CurrBatchNum+1);
		mRoleLayer->createBatchMonster(m_CurrBatchNum);
		if (m_CurrBatchNum < mManage->getBatch())
			displayRoundTips();
		displayGuide();
		if(mManage->getBatch() == m_CurrBatchNum && !mManage->getAliveByType(sMonsterSpecies::eBoss))
			mAssist->displayBossWarningBegin();
	}

	void CombatLogic::ActObjectRemove( CCObject* ob )
	{
		BaseRole* alive = (BaseRole*)ob;
		if (alive->getEnemy() && alive->getLastAlive())
		{
			if (mManage->getBatch() > m_CurrBatchNum)										//可能会出现的一个bug，两个人同时死亡的间隔太近，会多次调用这个方法。应该从逻辑处进行最后一个死亡武将的判定而不应该从这里进行处理
			{
				srandNum();																	//设置随机种子
				mManage->initMonsterByBatch(m_CurrBatchNum+1);								//初始化批次武将数据(必须马上初始化数据,但是绘制可以延迟,否则多个武将连续死亡会直接战斗胜利出现)
				this->scheduleOnce(schedule_selector(CombatLogic::NextBatch),1);				//打下一批次延时时间
			}else{
				mManage->setLogicState(false);
				bNotification->postNotification(MsgHideControlUp);
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
				bNotification->postNotification(MsgCreateStory,CCInteger::create((int)StoryType::eOverStory));
			}
		}
	}

	void CombatLogic::monsterDieDispose( BaseRole* pRole )
	{
		if (mManage->checkMonstOver())
		{
			pRole->setLastAlive(true);
			if (mManage->getBatch() > m_CurrBatchNum)				//判断最后一回合
				return;
			mManage->setLogicState(false);
			mControlLayer->RemoveEvent();															//最后一个怪物死亡不能再释放技能了
			if (mManage->getStageID())
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.3f);
			mManage->setbattleOver(true);
		}else{
			PlayEffectSound(SFX_423);
			mManage->changeCost(pRole->getInitCost());												//击杀敌方武将添加cost
			bNotification->postNotification(MsgMonsterDie,pRole->getRoleObject());
			mControlLayer->showFlyCostToBar(pRole->getRoleObject());									//粒子效果
		}
	}
	//控制关卡进度的地方处理( 抽出来到战斗结果判断对象中去进行。)
	void CombatLogic::AliveDieDispose( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy())
		{
			monsterDieDispose(tRole);
		}else{
			if ( tRole->getFatherID() )								//喽啰死亡不进行初始化处理
				return;
			if (tRole->getCaptain())
			{
				bNotification->postNotification(MsgRoleStand);
				mManage->setLogicState(false);
				combatResult(nullptr);												//战斗失败
			}else{
				bNotification->postNotification(MsgRoleBattleState,tRole);
			}
		}
	}

	void CombatLogic::battleWin()
	{
		PlayBackgroundMusic(SFX_Win,false);	
		BaseRole* alive = mManage->getAliveByGrid(C_CAPTAINGRID);	
		int hp = alive->getHp();
		if (hp>alive->getBaseData()->getRoleHp())
			hp = alive->getBaseData()->getRoleHp();
		m_finishData.res = true;
		m_finishData.roundNum = alive->getBaseData()->getRoleHp();							//这个验证并没有什么意义
		scheduleForRequestFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequestFinish)))));
	}

	void CombatLogic::battleFail()
	{
		m_finishData.res = false;
		m_finishData.roundNum = 0;
		scheduleForRequestFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequestFinish)))));
	}

	void CombatLogic::scheduleForRequestFinish()
	{
		if(m_bRecvFinish)
			return;
		CPlayerControl::getInstance().sendBattleFinish(1, m_finishData.res, m_finishData.roundNum);
	}
	//如果是世界boss调用
	void CombatLogic::scheduleForRequesBossFinish()
	{
		if(m_bRecvFinish)
			return;
		int hurt = mManage->getBossHurtCount();
		int checkNum = mManage->getVerifyNum();
		if ((checkNum + hurt) != 97231000)
		{
			CCLOG("[ *ERROR ] CombatLoginc::scheduleForRequesBossFinish");				//验证造成的伤害是否被修改内存
			return;
		}
		vector<int>* vec= mManage->getBossHurtVec();
		CPlayerControl::getInstance().sendWorldBossFinish(hurt, checkNum, *vec);
	}

	void CombatLogic::downloadPackageEnd( bool bAnswer )
	{
		if(!bAnswer)
			return;
		CScene* scene = GETSCENE(LoadBattleResource);
		((LoadBattleResource*)scene)->setRelease(true,skipSelectHero);
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	void CombatLogic::wordBossFinish()
	{
		mManage->setbattleOver(true);
		scheduleForRequesBossFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequesBossFinish)))));
	}

	void CombatLogic::combatResult(CCObject* ob)
	{
		if (mManage->getWorldBoss())
		{
			wordBossFinish();						//世界boss结束情况
			return;
		}
		if ( mManage->getStageID() )
		{
			if( ob )				
			{
				battleWin();
			}else{		
				battleFail();
			}
		}else{
			downloadPackageEnd(true);			//新手引导关卡
		}
	}

	void CombatLogic::beginStageFloorEffect()
	{
		if (mManage->getStageID())
			return;
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
		mControlLayer->setVisible(true);
		mBattleScene->AddEvent();																//场景移动动画结束，添加事件响应
		scheduleUpdate();																		//开启游戏主循环
		if(m_bCountDown)
			schedule(schedule_selector(CombatLogic::updateOneSecond), 1.0f);
		PlayBackgroundMusic(BGM_Battle,true);
		displayGuide(); 
		if (mManage->getStageID() == 301)
			mGuideManage->setGuide("CostGuide");
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
				mRoleLayer->heroWinAction();
				mTask->addObject(CombatTask::create(mAssist,callfuncO_selector(WarAssist::WinEffect),mTime+1));
			}break;
		}
	}
	//显示返回层
	void CombatLogic::cReturnLayer(CCObject* ob)
	{
		if(mManage->getbattleOver())
		{
			mBattleScene->removeChildByTag(backLayer_tag);
			return;
		}
		if (mBattleScene->getChildByTag(backLayer_tag))						//默认创建出来，放到warScene中去
		{
			if (mBattleScene->getChildByTag(backLayer_tag)->isVisible())
			{
				((WarBackLayer*)mBattleScene->getChildByTag(backLayer_tag))->hide();
				onResume();
			}else{
				((WarBackLayer*)mBattleScene->getChildByTag(backLayer_tag))->show();
				onPause();
			}
		}else{
			WarBackLayer* blayer = WarBackLayer::create();
			blayer->setTag(backLayer_tag);
			mBattleScene->addChild(blayer, 100);
			blayer->show();
			onPause();
		}
	}
	//世界boss结算
	void CombatLogic::onWordBossFinish( int type, google::protobuf::Message *msg )
	{
		if(type != BossFinishReqMsg)
			return;
		m_bRecvFinish = true;
		mBattleScene->removeChildByTag(backLayer_tag);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		WorldBossEndLayer * layer = WorldBossEndLayer::create();
		mBattleScene->addChild(layer);
		layer->processBattleFinish(type, msg);
	}
	//结算信息返回
	void CombatLogic::OnBattleFinish( int type, google::protobuf::Message *msg )
	{
		if(type != ExitStage)
			return;
		m_bRecvFinish = true;
		mBattleScene->removeChildByTag(backLayer_tag);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		if (res->win())
		{
			WarWinLayer *layer = WarWinLayer::create();
			mBattleScene->addChild(layer);
			layer->processBattleFinish(type, msg);
		}else{
			WarFailLayer *layer = WarFailLayer::create();
			mBattleScene->addChild(layer);
		}
	}

	void CombatLogic::onPause()
	{
		this->pauseSchedulerAndActions();
		if (mManage->getWorldBoss())			//停掉主循环,打boss时间继续计算
		{
			unschedule(schedule_selector(CombatLogic::updateOneSecond));
			schedule(schedule_selector(CombatLogic::updateOneSecond), 1.0f);
		}
	}

	void CombatLogic::onResume()
	{
		this->resumeSchedulerAndActions();
	}
	//世界boss倒计时
	void CombatLogic::updateOneSecond( float dt )
	{
		if(!m_bCountDown)
			return;
		m_iGameTimeCount--;
		if (m_iGameTimeCount%5 == 0)
			mManage->getBossHurtVec()->push_back(mManage->getBossHurtCount());			//每隔5秒存储一次世界boss伤害值
		if(m_iGameTimeCount<0)
		{
			unschedule(schedule_selector(CombatLogic::updateOneSecond));
			mManage->setLogicState(false);
			PlayBackgroundMusic(SFX_Win,false);
			combatResult(nullptr);															//结算
		}else{
			mControlLayer->updateTimeCountUI(m_iGameTimeCount);									//更新计时器
		}
	}

	void CombatLogic::rolePlyaSkill( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		mCombatEffect->PlayerSkill(tRole);
		if (!tRole->getEnemy())
		{
			mManage->setLogicState(false);
			mCritRole = tRole;
		}else{
			mMapLayer->DrawWarningEffect(tRole->mSkillArea);						//格子预警
		}
	}

}