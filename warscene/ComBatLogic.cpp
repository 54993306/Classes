
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
#include "warscene/SkillRange.h"
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

CombatLogic::CombatLogic()
	:m_time(0),m_Assist(nullptr),m_task(nullptr),m_CombatEffect(nullptr),m_bufExp(nullptr),m_terExp(nullptr),m_SkillRange(nullptr)
	,m_BatchNum(0),m_CurrBatchNum(0),m_FiratBatch(true),m_Scene(nullptr),m_UILayer(nullptr)
	,m_MapLayer(nullptr),m_AliveLayer(nullptr),m_TerrainLayer(nullptr),m_StoryLayer(nullptr)
	,m_GuideLayer(nullptr),m_Run(false),m_HurtCount(nullptr),m_MoveRule(nullptr),m_Manage(nullptr),m_MaxCost(0)
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
	CC_SAFE_RELEASE(m_SkillRange);
	m_SkillRange = nullptr;
	CC_SAFE_RELEASE(m_HurtCount);
	m_HurtCount = nullptr;
	CC_SAFE_RELEASE(m_MoveRule);
	m_MoveRule = nullptr;

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

bool CombatLogic::init()
{
	m_time = 0;
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
	m_SkillRange = SkillRange::create();
	m_SkillRange->retain();
	m_MoveRule = MoveRule::create();
	m_MoveRule->retain();
	m_HurtCount = HurtCount::create();
	m_HurtCount->retain();

	m_Manage = DataCenter::sharedData()->getWar();
	m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
	if (m_Manage->getWorldBoss())
	{
		m_iGameTimeCount = 180;			//世界boss打180s,3min
		m_bCountDown = true;
	}
		
	CCObject* obj = nullptr;
	CCArray* arr = m_Manage->getHeros(true);
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		m_MaxCost += alive->getCostmax();
		if (alive->getCaptain())
			m_CurrCost += alive->getInitCost();
	}
	return true;
}

void CombatLogic::initMember() 
{ 
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
	CNetClient::getShareInstance()->registerMsgHandler(ExitStage,this,CMsgHandler_selector(CombatLogic::OnBattleFinish));
	CNetClient::getShareInstance()->registerMsgHandler(BossFinishReqMsg,this,CMsgHandler_selector(CombatLogic::onWordBossFinish));
	

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
	TaskArray();
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

void CombatLogic::TaskArray()
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
	m_CurrCost += ((CCFloat*)ob)->getValue(); 
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
void CombatLogic::CostCount(WarAlive* alive,float dt)
{
	if ( alive->getEnemy() ||	//敌方武将
		alive->getFatherID()||	//召唤类武将
		m_Alive	)				//必杀技状态
		return;
	vector<int>* grids = m_Manage->getAddcostVec();
	if (!alive->getMoveObject()||
		std::find(grids->begin(),grids->end(),alive->getMoveObject()->getgrid()) != grids->end())
	{
		m_CurrCost += alive->getAddCost() * dt;					//怒气值每帧变化率
		m_fCurrentCostAdd += alive->getAddCost();
	}
}
//每一帧都判断每个武将的信息                   
void CombatLogic::ExcuteAI(float delta)
{
	Members* map_Alive = m_Manage->getMembers();
	for (Members::iterator iter = map_Alive->begin();iter != map_Alive->end(); iter++)
	{
		WarAlive* alive = iter->second;
		CostCount(alive,delta);															//计算我方所有武将cost值
		if (!alive->getActObject())continue;
		if (m_Alive && alive != m_Alive)continue;										//一次只处理一个武将播放技能的情况，若是处理多个则以武将技能状态判断，而不用单一武将进行判断
		alive->getActObject()->updateFrameEvent(delta);									//刷新武将帧事件
		if (StateDispose(alive,delta))
			continue;										
		if (!AttackJudge(alive))
			continue;
		//CCLOG("test alive id=%d",alive->getAliveID());
		if (alive->getEnemy())
		{
			MonsterExcuteAI(alive,delta);
		}else{
			HeroExcuteAI(alive);			
		}
	}
	//int i=0;
}
//@@
void CombatLogic::HeroExcuteAI( WarAlive* alive )
{
	int ActionCode = alive->getActObject()->getCurrActionCode();
	CCDictionary* AttackInfo = m_SkillRange->PlaySkillInfo(alive);		//生命周期只有一帧
	CCArray* Alives = (CCArray*)AttackInfo->objectForKey(Hit_Alive);		//受击目标	
	if (Alives->count()||alive->getCriAtk())
	{
		if (ActionCode == Walk_Index)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
			alive->setAIState(false);
		}
		AliveExcuteAI(alive,AttackInfo);
	}else{		
		//@@这些逻辑，应该都封装在武将的内部，单个武将去单独处理自身的情况
		if (IsAutoMoveType(alive) || ActionCode == Walk_Index)//自动移动类,或已经开始执行AI状态
			return;
		if (!alive->getCaptain()&&alive->getGridIndex()!=alive->getMoveObject()->getgrid())//武将当前是否在固定位置,如果不在则移动回固定位置(执行AI完毕状态)
		{			
			alive->setAIState(false);
			alive->setMoveGrid(alive->getMoveObject()->getgrid());							//@@武将有一个回到初始位置的方法
			alive->getActObject()->setMoveState(Walk_Index);
		}else{
			int grid = m_SkillRange->GuardMoveArea(alive);								//@@有警戒区域的武将处理
			alive->setAIState(true);
			if(grid)
			{
				alive->setMoveGrid(grid);
				alive->getActObject()->setMoveState(Walk_Index);
			}else{
				alive->setAIState(false);
			}	
		}
	}
}
//@@自动移动的我方武将无受击目标情况处理
bool CombatLogic::IsAutoMoveType( WarAlive*alive )
{
	if (alive->getCallType() != AutoMoveType)
		return false;
	if (alive->getActObject()->getMoveState()||!alive->getMove())
		return true;
	if (alive->getGridIndex() <= C_BEGINGRID-C_GRID_ROW*3)			
	{
		alive->getActObject()->AliveDie();							//超出预定范围执行死亡操作
		return true;
	}			
	alive->setMoveGrid(alive->getGridIndex()-C_GRID_ROW);			//这里的移动范围是写死了的
	alive->getActObject()->setMoveState(Walk_Index);
	return true;
}
//@@骸兽逃跑处理
bool CombatLogic::monsterFlee( WarAlive* alive )
{
	if (DataCenter::sharedData()->getWar()->getStageID())				//当前是为新手引导关卡,可以抽象出一个方法用于判断是否为新手关卡
		return false;
	if (alive->getModel() == 1056  
		&& ((alive->getHp()*1.0f/alive->getMaxHp()) < 0.60f))					//武将需要一个得到当前血量百分比的方法
	{
		if (!alive->getActObject()->getMoveState())
		{
			alive->setMoveGrid(4);
			alive->getActObject()->setMoveState(Walk_Index);
			alive->setMoveSpeed(0.5f);
			alive->setDef(1000000);
		}
		if (alive->getGridIndex()<= C_BEGINGRID-C_GRID_ROW*4)
		{		
			alive->getActObject()->AliveDie();
			alive->getActObject()->setVisible(false);
			NOTIFICATION->postNotification(B_ActObjectRemove,alive);		//超出预定范围执行死亡操作
		}		
		return true;
	}
	return false;
}
//@@
void CombatLogic::MonsterExcuteAI( WarAlive* alive,float dt )
{
	if (monsterFlee(alive))											//骸兽逃跑处理,将AI执行的都放在这个地方后面好整理
		return;
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (alive->getGridIndex()<C_BEGINGRID)							//@@每个怪物都有的方法
	{
		if (!alive->getActObject()->getMoveState() 
			&& m_MoveRule->MonstMoveExcute(alive))
			alive->getActObject()->setMoveState(Walk_Index);
		return ;
	}
	CCDictionary* AttackInfo = m_SkillRange->PlaySkillInfo(alive);			//生命周期只有一帧
	CCArray* Alives = (CCArray*)AttackInfo->objectForKey(Hit_Alive);		//受击目标
	if (Alives->count())
	{
		if (ActionCode == Walk_Index)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
		}
		AliveExcuteAI(alive,AttackInfo);
	}else{
		if (m_Manage->getSkill(alive)->skillType == CallAtk)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
			AliveExcuteAI(alive,AttackInfo); 
		}else{
			if (alive->getCriAtk())
				alive->setCriAtk(false);			
			if (!alive->getActObject()->getMoveState() && m_MoveRule->MonstMoveExcute(alive) )
				alive->getActObject()->setMoveState(Walk_Index);
		}
	}
}

bool CombatLogic::critJudge( WarAlive* alive )
{
	if (!alive->getCaptain()&&
		alive->getCriAtk()&&
		!alive->getCritEffect()&&
		alive->getCallType() != AutoSkill)
	{
		alive->setCritEffect(true);
		return true;
	}
	return false;
}

void CombatLogic::monsterCritEffect( WarAlive* alive ,CCArray* arr)
{
	alive->getActObject()->TurnStateTo(Stand_Index);		
	m_MapLayer->DrawWarningEffect(arr);						//格子预警
	m_CombatEffect->PlayerSkill(alive);
}

void CombatLogic::heroCritEffect( WarAlive* alive ,CCArray* arr)
{
	ActObject* pActObject = alive->getActObject();
	m_Run = false;
	m_Alive = alive;
	pActObject->setUserObject(CCBool::create(true));
	pActObject->setZOrder(0);
	CCObject* obj = nullptr;
	CCArray* actArr = CCArray::create();
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* pAlive = (WarAlive*)obj;
		pAlive->getActObject()->setUserObject(CCBool::create(true));
		pAlive->getActObject()->setZOrder(0);
		pAlive->getActObject()->pauseSchedulerAndActions();
		actArr->addObject(pAlive->getActObject());
	}
	m_Assist->ActStandExcute(actArr);										//受击目标站立处理
	m_AliveLayer->getLayerColor()->setVisible(true);
	LGPause(m_AliveLayer);													//暂停刷新位置和zorder的值
	m_CombatEffect->PlayerSkill(alive);
}

void CombatLogic::excuteCritEffect( WarAlive* alive ,CCDictionary*pDic)
{
	if (alive->getEnemy())
	{
		monsterCritEffect(alive,(CCArray*)pDic->objectForKey(Atk_Area));
	}else{
		heroCritEffect(alive,(CCArray*)pDic->objectForKey(Hit_Alive));
	}
}

void CombatLogic::attackEffect( WarAlive*alive )
{
	ActObject* pActObject = alive->getActObject();
	CEffect* effect = m_Manage->getEffect(alive);						//开始播放攻击音效攻击特效的时机可以由策划配置
	EffectInfo* info = m_Manage->getEffData()->getEffectInfo(effect->effectId);
	if (!info)
	{
		CCLOG("[ ERROR ] CombatLoginc::AliveExcuteAI EffectInfo NULL %d",effect->effectId);
		info = m_Manage->getEffData()->getEffectInfo(10000021);
	}
	pActObject->setAtkEffect(info->getusEft());
	pActObject->TurnStateTo(info->getActionID());
}

void CombatLogic::attackDirection( WarAlive*alive )
{
	ActObject* pActObject = alive->getActObject();
	if (!alive->getCaptain()&& alive->getNegate())									//反向攻击处理
	{
		if (alive->getEnemy())
		{
			pActObject->setDirection(Ditection_Left);
		}else{
			pActObject->setDirection(Ditection_Right);
		}
	}
}
//@@
void CombatLogic::AliveExcuteAI(WarAlive* alive,CCDictionary*pDic)
{
	CCArray* Alives = (CCArray*)pDic->objectForKey(Hit_Alive);
	if (critJudge(alive))
	{
		excuteCritEffect(alive,pDic);
		return;
	}
	attackDirection(alive);
	attackEffect(alive);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(Alives,obj)
	{
		WarAlive* pAlive = (WarAlive*)obj;
		if (pAlive->getHp()<=0)continue;
		alive->AtkAlive.push_back(pAlive);
	}
}

void CombatLogic::BatterRecord( CCObject* ob )
{
	if (m_Record)
		m_RecordNum++;
}

void CombatLogic::AliveCritEnd( WarAlive* alive )
{
	if (alive->getHp()>0&&alive->getBattle()&&!alive->getCaptain())				//我方武将释放技能时会扣自己血将自己击杀
	{
		alive->getActObject()->setMoveState(Walk_Index);	
		alive->getActObject()->setUpdateState(true);
		if (alive->role->alert)
			alive->setAIState(true);
	}
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
	WarAlive* alive = dynamic_cast<WarAlive*>(ob);
	AliveCritEnd(alive);
	m_AliveLayer->clearAlivesPauseMark();
	m_UILayer->ResetButtonState(m_Alive);
	m_AliveLayer->getLayerColor()->setVisible(false);
	LGResume(m_AliveLayer);
	m_Alive = nullptr;
	critComboEffect();
}

void CombatLogic::attackEnd( WarAlive* alive)
{
	for (auto i:alive->HittingAlive)
	{
		if (i->getHp()>0)
		{
			m_HurtCount->BuffHandleLogic(alive,i);									//伤害计算完成才能添加新的BUFF
		}else{
			if(i->getActObject() != nullptr)
				i->getActObject()->AliveDie();
		}
	}
	alive->HittingAlive.clear();
}
//攻击帧回调逻辑处理、武将执行一次伤害计算并播放效果
void CombatLogic::doLostHp(CCObject* ob)
{
	WarAlive* alive = (WarAlive*)ob;
	CEffect* effect = DataCenter::sharedData()->getWar()->getEffect(alive);
	if(!effect || alive->getSortieNum() >= effect->batter )										//当掉血帧多于实际逻辑值，少于实际逻辑值情况处理
		return;	
	switch (m_Manage->getSkill(alive)->skillType)
	{
	case NorAtk:
	case SpeAtk:
	case CriAtk:
		{
			alive->setSortieNum(alive->getSortieNum()+1);								//表示执行了一次攻击逻辑
			BattleResult* Result = m_HurtCount->AttackExcute(alive);					//实际进行伤害计算的地方，不应由动作来控制的，动作可以控制播放。
			if (alive->getCriAtk()&&!alive->getEffIndex())								//必杀技多释一个空效果
				m_CombatEffect->AttackNull(Result);
			if (!Result->m_HitTargets.empty())
				m_CombatEffect->BattleEffect(Result);
		}break;
	case CallAtk:
		{
			alive->setSortieNum(alive->getSortieNum()+1);								//一次性可召唤多个武将
			WarAlive* pAlive = m_Manage->getCallAlive(alive,m_Manage->getSkill(alive));	//得到被召唤的武将
			if (!pAlive)
			{
				CCLOG("[ *ERROR ] CombatLoginc::AtkLogic CallAlive NULL");
				return;
			}		
			m_AliveLayer->initActobject(pAlive,SceneTrap);
		}break;
	case CapAtk:break;
	default:break;
	}
	if( alive->getSortieNum() >= effect->batter )
		attackEnd(alive);													//武将攻击结束处理
}
//延迟出场武将
bool CombatLogic::delayEntrance( WarAlive* alive,float dt )
{
	if (alive->getBattle())
		return false;
	if (((CMonster*)alive->role)->delay)
	{
		if (alive->getDelaytime()<=0)
		{
			alive->setGridIndex(alive->role->grid);
			m_AliveLayer->AliveObEffect(alive->getActObject());
			m_AliveLayer->AddActToGrid(alive->getActObject(),alive->getGridIndex());
		}else{
			alive->setDelaytime(dt);
		}
	}
	if (alive->getAtkDelay()>0)
		alive->setAtkDelay(alive->getAtkDelay()-dt);									//敌方武将释放必杀技攻击延迟时间
	return true;
}
//
bool CombatLogic::autoSkillAlive( WarAlive* alive )
{
	if ((alive->role->CallType == AutoSkill || alive->role->MstType == MST_SKILL)&&!alive->getCriAtk())						//进入战场就释放技能(陨石类)
	{
		if (alive->getAliveStat()==COMMONSTATE)
		{
			if (alive->role->MstType == MST_SKILL)
				alive->role->MstType = MST_COMMON;
			alive->setCriAtk( true );
			return true;
		}
	}
	return false;
}

void CombatLogic::attackTime( WarAlive* alive,float dt )
{
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (ActionCode == Stand_Index || 
		ActionCode == Hit_Index || 
		ActionCode == Walk_Index)//怪物的技能释放是由攻击次数和时间确定的
		alive->setAtktime(dt);
	if (!alive->getCriAtk()&&alive->getEnemy())
		alive->setCritTime(dt);
	alive->getBuffManage()->upDateBuff(dt);									// BUFF应该是每一帧都自动处理的
}
//武将进行逻辑处理前状态结算
bool CombatLogic::StateDispose( WarAlive* alive,float dt )
{
	if (alive->getCallType() == NotAttack ||								//石头类武将不做攻击判断处理
		alive->getDieState() || 
		alive->getHp()<=0)
		return true;
	if (delayEntrance(alive,dt))
		return true;
	if (autoSkillAlive(alive))
		return false;
	attackTime(alive,dt);
	return false;
}

bool CombatLogic::walkState( WarAlive* alive )
{
	if (alive->getEnemy()||alive->getCallType()==AutoMoveType)		//我方自动移动类武将
	{
		return true;
	}else{
		if (alive->getAIState())
		{
			return true;
		}else{
			return false;
		}
	}
}

bool CombatLogic::aliveAttackState( WarAlive* alive )
{
	if (alive->getSpeAtk())
	{
		return true;
	}else if (alive->getNorAtk())
	{
		return true;
	}else if (alive->getCriAtk()){
		return true;
	}else{
		return false;
	}
}
//武将状态判断,武将是否可攻击判断,提高效率关键方法之一
bool CombatLogic::AttackJudge( WarAlive* alive )
{
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (ActionCode == Walk_Index)
		return walkState(alive);

	if (ActionCode == Dizzy_Index||
		ActionCode == Skill_Index||
		ActionCode == SpAttack_Index||
		ActionCode == Attack_Index||
		ActionCode == victory_Index||
		ActionCode == Die_Index)
	{
		if (alive->getCloaking())
		{
			alive->setCloaking(false);
			((CCArmature*)alive->getActObject()->getArmature())->setOpacity(255);
		}
		return false;
	}
	return aliveAttackState(alive);
}

void CombatLogic::displayBatchWarning()
{
	if (m_Manage->getAliveByType(AliveType::Boss))
	{
		WarAlive* boss = m_Manage->getAliveByType(AliveType::Boss);
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
	if (DataCenter::sharedData()->getWar()->getStageID() != 0)							//引导开启的情况
		return;
	char path[60] = {0};
	sprintf(path,"%d_%d",0,m_CurrBatchNum+1);										//覆盖高亮区域的图片
	DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
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

void CombatLogic::monsterRemove( WarAlive* alive )
{
	if (!alive->getLastAlive())
		return;
	if (m_BatchNum > m_CurrBatchNum)										//可能会出现的一个bug，两个人同时死亡的间隔太近，会多次调用这个方法。应该从逻辑处进行最后一个死亡武将的判定而不应该从这里进行处理
	{
		srandNum();															//设置随机种子
		m_Manage->initBatchData(m_CurrBatchNum+1);							//初始化批次武将数据(必须马上初始化数据,但是绘制可以延迟,否则多个武将连续死亡会直接战斗胜利出现)
		this->scheduleOnce(schedule_selector(CombatLogic::NextBatch),1);	//打下一批次延时时间
	}else{
		m_Run = false;
		m_UILayer->hideUiUpPart();			//隐藏control UI的上部分
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
		m_StoryLayer->CreateStory(CCInteger::create((int)StoryType::OverStory));
	}
}

void CombatLogic::ActObjectRemove( CCObject* ob )
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getEnemy())
	{
		monsterRemove(alive);
	}else{
		if (!alive->getCaptain())
			return;
		m_CurrBatchNum = 1000;
		m_Run = false;
		m_Assist->ActStandExcute(m_AliveLayer->getAlivesOb());
		combatResult(nullptr);		//战斗失败
	}
}

void CombatLogic::monsterDieDispose( WarAlive* alive )
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
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getEnemy())
	{
		monsterDieDispose(alive);
	}else{
		if (alive->role->isCall || alive->getCaptain())											//喽啰死亡不进行初始化处理
			return;
		m_UILayer->AliveBattleDispose(alive);//武将下阵	
	}
}

void CombatLogic::battleWin()
{
	PlayBackgroundMusic(SFX_Win,false);	
	WarAlive* alive = m_Manage->getAliveByGrid(C_CAPTAINGRID);	
	int hp = alive->getHp();
	if (hp>alive->role->hp)
		hp = alive->role->hp;
	m_finishData.res = true;
	m_finishData.roundNum = alive->role->hp;							//这个验证并没有什么意义
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
		EffectObject*ef  = EffectObject::create("121",EffectType::Delay); 
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
	CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(WarScene::LayerMoveEnd),CCInteger::create((int)StoryType::MoveEndStory));
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
	case StoryType::beginStory:
		{
			beginStoryEnd();
		}break;
	case StoryType::MoveEndStory:
		{
			moveStoryEnd();		
		}break;
	case StoryType::OverStory:
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
	if (!m_Manage->getWorldBoss())			//停掉主循环,打boss时间继续计算
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