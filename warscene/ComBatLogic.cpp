
#include "ComBatLogic.h"
#include "scene/loadWar.h"
#include "scene/WarScene.h"
#include "warscene/BufExp.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "model/BuffManage.h"
#include "model/MapManager.h"
#include "tools/ShowTexttip.h"
#include "warscene/MoveRule.h"
#include "warscene/ConstNum.h"
#include "model/terrainLayer.h"
#include "warscene/HurtCount.h"
#include "warscene/WarAssist.h"
#include "warscene/MapEffect.h"
#include "warscene/TerrainExp.h"
#include "warscene/CombatTask.h"
#include "warscene/EffectData.h"
#include "warscene/WarControl.h"
#include "model/TerrainManager.h"
#include "warscene/WarBackLayer.h"
#include "warscene/CaptainSkill.h"
#include "warscene/CombatEffect.h"
#include "warscene/BattleResult.h"
#include "common/CommonFunction.h"
#include "scene/layer/StoryLayer.h"
#include "scene/alive/AliveDefine.h"
#include "scene/layer/WarMapLayer.h"
#include "scene/layer/WarAliveLayer.h"
#include "scene/effect/EffectObject.h"
#include "netcontrol/CPlayerControl.h"
#include "warscene/CombatGuideManage.h"
#include "warscene/WarFailLayer.h"
#include "warscene/WarWinLayer.h"
#include "common/CGameSound.h"
#include "common/ShaderDataHelper.h"
#include "warscene/WorldBossEndLayer.h"
#include "Battle/BattleMessage.h"
#include "update/CDownloadPackage.h"
#include "jni/CJniHelper.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleSkill.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
namespace BattleSpace{
	CombatLogic::CombatLogic()
		:m_time(0),m_Assist(nullptr),m_task(nullptr),m_CombatEffect(nullptr),m_bufExp(nullptr),m_terExp(nullptr)
		,m_BatchNum(0),m_CurrBatchNum(0),m_FiratBatch(true),m_Scene(nullptr),m_UILayer(nullptr)
		,m_MapLayer(nullptr),m_AliveLayer(nullptr),m_TerrainLayer(nullptr),m_StoryLayer(nullptr)
		,m_GuideLayer(nullptr),m_Run(false),m_HurtCount(nullptr),m_Manage(nullptr),m_MaxCost(0)
		,m_CurrCost(0),m_speed(1),m_Alive(nullptr),m_bRecvFinish(false),m_fCurrentCostAdd(0),
		m_iGameTimeCount(0), m_bCountDown(false),m_Record(true),m_RecordNum(0),m_PlayerNum(0)
	{}

	CombatLogic::~CombatLogic()
	{
		CC_SAFE_RELEASE(m_Assist);
		m_Assist = nullptr;
		CC_SAFE_RELEASE(m_task);
		m_task = nullptr;
		CC_SAFE_RELEASE(m_terExp);
		m_terExp = nullptr;
		CC_SAFE_RELEASE(m_bufExp);
		m_bufExp = nullptr;
		CC_SAFE_RELEASE(m_CombatEffect);
		m_CombatEffect = nullptr;
		CC_SAFE_RELEASE(m_HurtCount);
		m_HurtCount = nullptr;

		m_Scene = nullptr;
		m_UILayer = nullptr;
		m_MapLayer = nullptr;
		m_AliveLayer = nullptr;
		m_TerrainLayer = nullptr;
		m_StoryLayer = nullptr;
		m_GuideLayer = nullptr;
		m_Manage = nullptr;
	}

	void CombatLogic::onExit()
	{
		CCNode::onExit();
		NOTIFICATION->removeAllObservers(this);
		this->unscheduleAllSelectors();
	}

	void CombatLogic::onEnter()
	{
		CCNode::onEnter();
		m_Scene = (WarScene*)this->getParent(); 
		m_Assist->setScene(m_Scene);
		m_CombatEffect->setScene(m_Scene);
		m_UILayer = m_Scene->getWarUI();
		m_UILayer->setVisible(false);
		m_MapLayer = m_Scene->getWarMapLayer();
		m_StoryLayer = m_Scene->getStoryLayer();
		m_AliveLayer = m_Scene->getWarAliveLayer();
		m_TerrainLayer = m_Scene->getTerrainLayer();
		m_GuideLayer = m_Scene->getCombatGuideLayer();
		m_BatchNum = m_Manage->getBatch();

		addEvent();
		initCost();
		initMapBackground();
		initWordBossTime();
	}

	void CombatLogic::initCost()
	{
		CCObject* obj = nullptr;
		CCArray* arr = m_Manage->getAlivesByCamp(false,true);
		CCARRAY_FOREACH(arr,obj)
		{
			BaseRole* alive = (BaseRole*)obj;
			m_MaxCost += alive->getCostmax();
			if (alive->getCaptain())
				m_CurrCost += alive->getInitCost();
		}
	}

	void CombatLogic::initWordBossTime()
	{
		if (m_Manage->getWorldBoss())
		{
			m_iGameTimeCount = 180;			//世界boss打180s,3min
			m_bCountDown = true;
		}
	}

	bool CombatLogic::init()
	{
		m_Assist = WarAssist::create();
		m_Assist->retain();
		m_task = CCArray::create();
		m_task->retain();
		m_bufExp = BufExp::create();
		m_bufExp->retain();
		m_terExp = TerrainExp::create();
		m_terExp->retain();
		m_CombatEffect = CombatEffect::create();
		m_CombatEffect->retain();
		m_HurtCount = HurtCount::create();
		m_HurtCount->retain();
		m_Manage = DataCenter::sharedData()->getWar();
		m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
		return true;
	}

	void CombatLogic::addEvent()
	{
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::StoryEndEvent),B_StoryOver,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::RoundStart),B_RoundStart,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::doLostHp),B_LostHpEvent,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::changeSpeed),B_ChangeSceneSpeed,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::ActObjectRemove),B_ActObjectRemove,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::CritAtkEnd),B_CritEnd,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::AliveDieDispose),B_AliveDie,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::cReturnLayer),B_ReturnLayer,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::changeCost),B_ChangeCostNumber,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::BatterRecord),B_RecordContinuousSkill,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::rolePlyaSkill),B_RoleSkill,nullptr);
		CNetClient::getShareInstance()->registerMsgHandler(ExitStage,this,CMsgHandler_selector(CombatLogic::OnBattleFinish));
		CNetClient::getShareInstance()->registerMsgHandler(BossFinishReqMsg,this,CMsgHandler_selector(CombatLogic::onWordBossFinish));
	}

	void CombatLogic::initMapBackground() 
	{ 
		if (m_MapLayer->getBackgroundManage())
		{
			m_MapLayer->getBackgroundManage()->setMap(m_MapLayer);
			m_MapLayer->getBackgroundManage()->initWithStage(m_Manage->getStageID());
		}
		m_UILayer->updateBatchNumber(1);
	}
	//测试用
	void CombatLogic::RoundStart(CCObject* ob)
	{
		m_Run = !m_Run;
		CCLabelTTF* lab = (CCLabelTTF*)m_UILayer->getChildByTag(10000);
		if (!lab)
		{
			lab = CCLabelTTF::create("STOP","Arial",50);
			lab->setColor(ccc3(255,0,0));
			lab->setPosition(ccp(180,100));
			m_UILayer->addChild(lab,1,10000);
		}
		if (m_Run)
		{
			lab->setString(" RUN ");
		}else{
			lab->setString(" STOP ");
		}
	}

	void CombatLogic::changeSpeed(CCObject* ob)
	{
		if (m_speed > 1)
		{
			m_speed = 1;
		}else{
			m_speed = 2;
		}
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(m_speed);
	}

	void CombatLogic::update(float delta)
	{
		m_time += delta;
		showRound();
		updateTask();
		m_bufExp->ResetInterval(delta);
		runLogic(delta);
		m_UILayer->updateCostNumber(m_CurrCost);
		m_UILayer->updateCostSpeed(m_fCurrentCostAdd);
		m_fCurrentCostAdd = 0;
	}

	void CombatLogic::displayRoundTips()
	{
		CCLabelAtlas* labAt = CCLabelAtlas::create(ToString(m_CurrBatchNum+1),"label/47_44.png",47,44,'0');
		ShowTexttip(labAt,RGB_GREEN,ROUNDNUM,CCPointZero,0,0,0,200);
		NOTIFICATION->postNotification(B_MonsterTips);
	}

	void CombatLogic::showRound()
	{
		if (m_time >= 0.8f && m_FiratBatch && !DataCenter::sharedData()->getCombatGuideMg()->IsGuide())
		{
			m_Run = true;	
			m_FiratBatch = false;
			displayRoundTips();
		}
	}

	void CombatLogic::updateTask()
	{
		CCObject* object = nullptr;
		CCArray* findRs = CCArray::create();		//维护一个任务队列
		CCARRAY_FOREACH(m_task,object)
		{
			CombatTask* task = (CombatTask*) object;
			if(!task) continue;
			if(m_time > task->getInterval())
			{
				SEL_CallFuncO fun = task->getFun();
				(task->getPtarget()->*fun)(task->getParam());		//函数指针解引用传入参数
				findRs->addObject(object);
			}
		}
		CCARRAY_FOREACH(findRs,object)
		{
			m_task->removeObject(object);
		}
		findRs->removeAllObjects();					//可以执行其他对象中需要一直循环判断的方法
	}

	void CombatLogic::runLogic(float delta)
	{
		if (!m_Run || DataCenter::sharedData()->getCombatGuideMg()->IsGuide())
			return;
		m_Manage->updateAlive();				//更新武将信息
		ExcuteAI(delta);
		costUpdate(delta);
	}

	void CombatLogic::changeCost( CCObject* ob ) 
	{ 
		m_CurrCost += ((CCInteger*)ob)->getValue(); 
		costUpdate(0);
	}

	void CombatLogic::costUpdate(float delta)
	{
		if (m_CurrCost < m_MaxCost && !m_Alive)	//放必杀技时cost不发生改变
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			m_CurrCost += delta * 100;				//系统一直增加cost
			m_fCurrentCostAdd += 1;
#else
			m_CurrCost += delta;
			m_fCurrentCostAdd += delta;
#endif	
		}
		if ( m_CurrCost <= 0 ) //武将移动回我方半屏
		{
			m_CurrCost = 0;
		}else if(m_CurrCost >= m_MaxCost){
			m_CurrCost = m_MaxCost;
		}
	}
	//cost 计算(帧)或以秒为单位进行计算、位置实时更新
	void CombatLogic::CostCount(BaseRole* tRole,float dt)
	{
		if ( tRole->getEnemy() || tRole->getFatherID() || m_Alive)			
			return;
		if (m_Manage->inAddCostArea(tRole->getGridIndex()))
		{
			m_CurrCost += tRole->getAddCost() * dt;					//怒气值每帧变化率
			m_fCurrentCostAdd += tRole->getAddCost();
		}
	}
	//每一帧都判断每个武将的信息                   
	void CombatLogic::ExcuteAI(float pTime)
	{
		const Members* map_Alive = m_Manage->getMembers();
		for (Members::const_iterator iter = map_Alive->begin();iter != map_Alive->end(); iter++)
		{
			BaseRole* tRole = iter->second;
			if (m_Alive && tRole != m_Alive)continue;										//一次只处理一个武将播放技能的情况，若是处理多个则以武将技能状态判断，而不用单一武将进行判断
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
		m_CombatEffect->setPlayerNum(m_PlayerNum);
		if (m_PlayerNum >= m_RecordNum)
		{
			m_Run = false;
			m_CombatEffect->BatterSpine(m_RecordNum);
			m_PlayerNum = 0; 
			m_RecordNum = 0;
			m_CombatEffect->setPlayerNum(m_PlayerNum);
			m_Record = true;
		}
	}

	void CombatLogic::CritAtkEnd(CCObject* ob)
	{
		BaseRole* tRole = dynamic_cast<BaseRole*>(ob);
		tRole->AliveCritEnd();
		m_AliveLayer->clearAlivesPauseMark();
		m_UILayer->ResetButtonState(m_Alive);
		m_AliveLayer->getLayerColor()->setVisible(false);
		LGResume(m_AliveLayer);
		m_Alive = nullptr;
		critComboEffect();
	}
	//攻击帧回调逻辑处理、武将执行一次伤害计算并播放效果
	void CombatLogic::doLostHp(CCObject* ob)
	{
		BaseRole* alive = (BaseRole*)ob;
		const skEffectData* effect = alive->getCurrEffect();
		if(!effect || alive->getSortieNum() >= effect->getBatter() )										//当掉血帧多于实际逻辑值，少于实际逻辑值情况处理
			return;	
		switch (alive->getSkillType())
		{
		case eNorAtk:
		case eSpeAtk:
		case eCriAtk:
			{
				alive->setSortieNum(alive->getSortieNum()+1);								//表示执行了一次攻击逻辑
				BattleResult* Result = m_HurtCount->AttackExcute(alive);					//实际进行伤害计算的地方，不应由动作来控制的，动作可以控制播放。
				if (alive->getCriAtk()&&!alive->getEffIndex())								//必杀技多释一个空效果
					m_CombatEffect->AttackNull(Result);
				if (!Result->m_HitTargets.empty())
					m_CombatEffect->BattleEffect(Result);
			}break;
		case eCallAtk:
			{
				alive->setSortieNum(alive->getSortieNum()+1);								//一次性可召唤多个武将
				BaseRole* pAlive = m_Manage->getCallAlive(alive,alive->getCurrSkill());	//得到被召唤的武将
				if (!pAlive)
				{
					CCLOG("[ *ERROR ] CombatLoginc::AtkLogic CallAlive NULL");
					return;
				}		
				m_AliveLayer->initActobject(pAlive,SceneTrap);
			}break;
		}
		if( alive->getSortieNum() >= effect->getBatter() )
		{
			m_HurtCount->BuffHandleLogic(alive);									//伤害计算完成才能添加新的BUFF
			alive->clearHitAlive();
		}
	}

	void CombatLogic::displayBatchWarning()
	{
		if (m_Manage->getAliveByType(E_ALIVETYPE::eBoss))
		{
			BaseRole* boss = m_Manage->getAliveByType(E_ALIVETYPE::eBoss);
			m_Assist->DisplayBossWarning(m_UILayer,boss->getModel());							//第一波就出现超大boss的情况	
			m_AliveLayer->removeMessage();														//释放掉触摸消息
		}else if(m_BatchNum == m_CurrBatchNum)
		{
			m_Assist->DisplayBossWarning(m_UILayer);
			m_AliveLayer->removeMessage();														//释放掉触摸消息
		}
	}

	void CombatLogic::displayGuide()
	{
		if (!m_Manage->getStageID())														//引导开启的情况
		{
			char path[60] = {0};
			sprintf(path,"%d_%d",0,m_CurrBatchNum+1);										//覆盖高亮区域的图片
			DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
		}
	}

	void CombatLogic::NextBatch( float dt )
	{
		m_CurrBatchNum++;
		m_UILayer->updateBatchNumber(m_CurrBatchNum+1);
		m_AliveLayer->createBatchMonster(m_CurrBatchNum);
		if (m_CurrBatchNum < m_BatchNum)
			displayRoundTips();
		displayGuide();
		displayBatchWarning();
	}

	void CombatLogic::ActObjectRemove( CCObject* ob )
	{
		BaseRole* alive = (BaseRole*)ob;
		if (alive->getEnemy() && alive->getLastAlive())
		{
			if (m_BatchNum > m_CurrBatchNum)										//可能会出现的一个bug，两个人同时死亡的间隔太近，会多次调用这个方法。应该从逻辑处进行最后一个死亡武将的判定而不应该从这里进行处理
			{
				srandNum();															//设置随机种子
				m_Manage->initMonsterByBatch(m_CurrBatchNum+1);							//初始化批次武将数据(必须马上初始化数据,但是绘制可以延迟,否则多个武将连续死亡会直接战斗胜利出现)
				this->scheduleOnce(schedule_selector(CombatLogic::NextBatch),1);	//打下一批次延时时间
			}else{
				m_Run = false;
				m_UILayer->hideUiUpPart();			//隐藏control UI的上部分
				CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
				m_StoryLayer->CreateStory(CCInteger::create((int)StoryType::eOverStory));
			}
		}
	}

	void CombatLogic::monsterDieDispose( BaseRole* alive )
	{
		if (m_Manage->checkMonstOver())
		{
			alive->setLastAlive(true);
			if (m_BatchNum > m_CurrBatchNum)				//判断最后一回合
				return;
			m_Run = false;
			m_UILayer->RemoveEvent();															//最后一个怪物死亡不能再释放技能了
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.3f);
			DataCenter::sharedData()->getWar()->setbattleOver(true);
		}else{
			PlayEffectSound(SFX_423);
			m_CurrCost += alive->getInitCost();													//击杀敌方武将添加cost
			m_UILayer->showFlyCostToBar(m_AliveLayer->convertToWorldSpace(alive->getActObject()->getPosition()));//粒子效果
		}
	}
	//控制关卡进度的地方处理
	void CombatLogic::AliveDieDispose( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		if (tRole->getEnemy())
		{
			monsterDieDispose(tRole);
		}else{
			if (tRole->getBaseData()->getCallRole() || tRole->getCaptain())								//喽啰死亡不进行初始化处理
				return;
			if (tRole->getCaptain())
			{
				m_Run = false;
				m_Assist->ActStandExcute(m_AliveLayer->getAlivesOb());
				combatResult(nullptr);															//战斗失败
			}else{
				m_UILayer->AliveBattleDispose(tRole);											//武将下阵	
			}
		}
	}

	void CombatLogic::battleWin()
	{
		PlayBackgroundMusic(SFX_Win,false);	
		BaseRole* alive = m_Manage->getAliveByGrid(C_CAPTAINGRID);	
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
		int hurt = m_Manage->getBossHurtCount();
		int checkNum = m_Manage->getVerifyNum();
		if ((checkNum + hurt) != 97231000)
		{
			CCLOG("[ *ERROR ] CombatLoginc::scheduleForRequesBossFinish");				//验证造成的伤害是否被修改内存
			return;
		}
		vector<int>* vec= m_Manage->getBossHurtVec();
		CPlayerControl::getInstance().sendWorldBossFinish(hurt, checkNum, *vec);
	}

	void CombatLogic::downloadPackageEnd( bool bAnswer )
	{
		if(!bAnswer)
			return;
		CScene* scene = GETSCENE(LoadWar);
		((LoadWar*)scene)->setRelease(true,skipSelectHero);
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	void CombatLogic::wordBossFinish()
	{
		DataCenter::sharedData()->getWar()->setbattleOver(true);
		scheduleForRequesBossFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequesBossFinish)))));
	}

	void CombatLogic::combatResult(CCObject* ob)
	{
		if (m_Manage->getWorldBoss())
		{
			wordBossFinish();						//世界boss结束情况
		}else{
			if ( m_Manage->getStageID() )
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
	}

	void CombatLogic::beginStageFloorEffect()
	{
		if (m_Manage->getStageID())
			return;
		CCSize size  = CCDirector::sharedDirector()->getWinSize();
		int grids[4] = {78,89,102,117};
		for (int i=0;i<4;i++)
		{
			EffectObject*ef  = EffectObject::create("121",PLAYERTYPE::Delay); 
			ef->setMusic(525);
			ef->setShake(true);
			ef->setDelaytime(i*0.35f);
			ef->setLoopInterval(0.2f);
			ef->setLoopNum(2);
			ef->setPosition(m_MapData->getPoint(grids[i]));
			ef->setZOrder(10);
			m_MapLayer->addChild(ef);
		}
	}

	void CombatLogic::beginStoryEnd()
	{
		beginStageFloorEffect();
		PlayEffectSound(SFX_413);	
		CCDelayTime* delay = CCDelayTime::create(0.5f);
		CCDelayTime* delay2 = CCDelayTime::create(0.3f);
		CCMoveTo* mt = CCMoveTo::create(1.2f,ccp( MAP_MINX(m_MapData) , m_Scene->getMoveLayer()->getPositionY()));
		CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(WarScene::LayerMoveEnd),CCInteger::create((int)StoryType::eMoveEndStory));
		CCSequence* sqe = CCSequence::create(delay,mt,delay2,cfo,nullptr);
		m_Scene->getMoveLayer()->runAction(sqe);
	}

	void CombatLogic::moveStoryEnd()
	{
		m_UILayer->setVisible(true);
		m_Scene->AddEvent();																	//场景移动动画结束，添加事件响应
		scheduleUpdate();																		//开启游戏主循环
		if(m_bCountDown)
			schedule(schedule_selector(CombatLogic::updateOneSecond), 1.0f);
		PlayBackgroundMusic(BGM_Battle,true);
		displayGuide(); 
		if (DataCenter::sharedData()->getWar()->getStageID() == 301)
			DataCenter::sharedData()->getCombatGuideMg()->setGuide("CostGuide");
		displayBatchWarning();																	//第一波出现超大boss的情况
	}

	void CombatLogic::StoryEndEvent(CCObject* ob)
	{
		switch ((StoryType)m_StoryLayer->getSType())
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
				m_AliveLayer->heroWinAction();
				m_task->addObject(CombatTask::create(m_Assist,callfuncO_selector(WarAssist::WinEffect),m_time+1));
			}break;
		default:break;
		}
	}
	//显示返回层
	void CombatLogic::cReturnLayer(CCObject* ob)
	{
		if(DataCenter::sharedData()->getWar()->getbattleOver())
		{
			m_Scene->removeChildByTag(backLayer_tag);
			return;
		}
		if (m_Scene->getChildByTag(backLayer_tag))
		{
			if (m_Scene->getChildByTag(backLayer_tag)->isVisible())
			{
				((WarBackLayer*)m_Scene->getChildByTag(backLayer_tag))->hide();
				onResume();
			}else{
				((WarBackLayer*)m_Scene->getChildByTag(backLayer_tag))->show();
				onPause();
			}
		}else{

			WarBackLayer* blayer = WarBackLayer::create();
			blayer->setTag(backLayer_tag);
			m_Scene->addChild(blayer, 100);
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
		m_Scene->removeChildByTag(backLayer_tag);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		WorldBossEndLayer * layer = WorldBossEndLayer::create();
		m_Scene->addChild(layer);
		layer->processBattleFinish(type, msg);
	}
	//结算信息返回
	void CombatLogic::OnBattleFinish( int type, google::protobuf::Message *msg )
	{
		if(type != ExitStage)
			return;
		m_bRecvFinish = true;
		m_Scene->removeChildByTag(backLayer_tag);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		if (res->win())
		{
			WarWinLayer *layer = WarWinLayer::create();
			m_Scene->addChild(layer);
			layer->processBattleFinish(type, msg);
		}else{
			WarFailLayer *layer = WarFailLayer::create();
			m_Scene->addChild(layer);
		}
	}

	void CombatLogic::onPause()
	{
		this->pauseSchedulerAndActions();
		if (m_Manage->getWorldBoss())			//停掉主循环,打boss时间继续计算
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
			m_Manage->getBossHurtVec()->push_back(m_Manage->getBossHurtCount());			//每隔5秒存储一次世界boss伤害值
		if(m_iGameTimeCount<0)
		{
			unschedule(schedule_selector(CombatLogic::updateOneSecond));
			m_Run = false;
			PlayBackgroundMusic(SFX_Win,false);
			combatResult(nullptr);															//结算
		}else{
			m_UILayer->updateTimeCountUI(m_iGameTimeCount);									//更新计时器
		}
	}

	void CombatLogic::rolePlyaSkill( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		m_CombatEffect->PlayerSkill(tRole);
		if (!tRole->getEnemy())
		{
			m_Run = false;
			m_Alive = tRole;
		}else{
			m_MapLayer->DrawWarningEffect(tRole->mSkillArea);						//格子预警
		}
	}

}