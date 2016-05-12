
#include "GMessage.h"
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
#include "warscene/WarRelive.h"
#include "warscene/WorldBossEndLayer.h"

#include "update/CDownloadPackage.h"
#include "jni/CJniHelper.h"

CombatLogic::CombatLogic()
	:m_time(0),m_Assist(nullptr),m_task(nullptr),m_CombatEffect(nullptr),m_bufExp(nullptr),m_terExp(nullptr),m_SkillRange(nullptr)
	,m_BatchNum(0),m_send(1),m_CurrBatchNum(0),m_FiratBatch(true),m_Scene(nullptr),m_UILayer(nullptr)
	,m_MapLayer(nullptr),m_AliveLayer(nullptr),m_TerrainLayer(nullptr),m_StoryLayer(nullptr)
	,m_GuideLayer(nullptr),m_Run(false),m_HurtCount(nullptr),m_MoveRule(nullptr),m_Manage(nullptr),m_MaxCost(0),m_CostChange(true)
	,m_CurrCost(0),m_speed(1),m_FrameTime(0),m_Alive(nullptr),m_bFinish(false),m_bGameOver(false),m_fCurrentCostAdd(0),
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
		startCountDown(180);
		
	CCObject* obj = nullptr;
	CCArray* arr = m_Manage->getHeros(true);
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		m_MaxCost += alive->getCostmax();
		if (alive->getCaptain())
			m_CurrCost += alive->getInitCost();
	}
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	//m_CurrCost = m_MaxCost;
#endif
	return true;
}

void CombatLogic::ChangeCost( CCObject* ob ) { setCurrCost(((CCFloat*)ob)->getValue()); }

void CombatLogic::setCurrCost(float var)
{
	m_CurrCost += var;
	if (m_CurrCost <=0 )
	{
		CCLOG("[ *Tips ] CombatLoginc::setCurrCost <= 0");
		m_CurrCost = 0;
	}
	if (m_CurrCost >= m_MaxCost)
		m_CurrCost = m_MaxCost;
}
float CombatLogic::getCurrCost(){return m_CurrCost;}

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

	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::StoryEndEvent),WAR_STORY_OVER,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::RoundStart),WAR_ROUND_START,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::AtkLogic),ALIVEATTACK,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::ChangeSpeed),CHANGEBATTLESPEED,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::ActRemove),ACTREMOVE,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::CritAtkEnd),SKILLEND,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::AliveDieDispose),ALIVEDIE,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::CBackLayer),SHOW_PLAN_PANEL,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::ChangeCost),CHANGECOST,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(CombatLogic::BatterRecord),RECORDBATTERNUM,nullptr);
	CNetClient::getShareInstance()->registerMsgHandler(HeroInfoMsg,this,CMsgHandler_selector(CombatLogic::ContinueStage));
	CNetClient::getShareInstance()->registerMsgHandler(ExitStage,this,CMsgHandler_selector(CombatLogic::OnBattleFinish));
	CNetClient::getShareInstance()->registerMsgHandler(BossFinishReqMsg,this,CMsgHandler_selector(CombatLogic::OnBattleFinish));
	

	if (m_MapLayer->getBackgroundManage())
	{
		m_MapLayer->getBackgroundManage()->setMap(m_MapLayer);
		m_MapLayer->getBackgroundManage()->initWithStage(m_Manage->getStageID());
	}
	m_BatchNum = m_Manage->getBatch();
	CLayout* m_ControLayer = m_UILayer->getLaout();
	CLayout* m_pNormal = (CLayout*)m_ControLayer->findWidgetById("layer_up_normal");
	CCNode* node = m_pNormal->getChildByTag(CL_Batch);
	node->setVisible(false);
	char path[60] = {0};
	sprintf(path,"%d/%d",m_send,m_BatchNum+1);
	CCLabelAtlas* BatchNum = CCLabelAtlas::create(path,"label/number_03.png",27.0f,35,47);
	BatchNum->setScale(0.6f);
	BatchNum->setAnchorPoint(ccp(0.5f,0.43f));
	BatchNum->setPosition(node->getPosition());
	BatchNum->setTag(CL_Batch);
	m_pNormal->addChild(BatchNum);
	node->removeFromParentAndCleanup(true);
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

void CombatLogic::ChangeSpeed(CCObject* ob)
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
	RunLogic(delta);
	m_UILayer->updateUiLayerCostNumber(m_CurrCost);
	m_UILayer->showCostAddOrReduceEffect(m_fCurrentCostAdd);
	m_fCurrentCostAdd = 0;
}

void CombatLogic::showRound()
{
	if (m_time >= 0.8f && m_FiratBatch && !DataCenter::sharedData()->getCombatGuideMg()->IsGuide())
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		m_Run = true;
#else
		m_Run = true;
#endif		
		m_FiratBatch = false;
		CCLabelAtlas* labAt = CCLabelAtlas::create(ToString(m_CurrBatchNum+1),"label/47_44.png",47,44,'0');
		ShowTexttip(labAt,RGB_GREEN,ROUNDNUM,CCPointZero,0,0,0,200);
		NOTIFICATION->postNotification(TIPSEFFECT,CCBool::create(false));
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

void CombatLogic::RunLogic(float delta)
{
	if (m_Run)
	{
		m_Manage->updateAlive();				//更新武将信息
		if (!DataCenter::sharedData()->getCombatGuideMg()->IsGuide())
		{
			if (m_CurrCost < m_MaxCost && m_CostChange && !m_Alive)	//放必杀技时cost不发生改变
			{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
				m_CurrCost += delta * 100;				//系统一直增加cost
				m_fCurrentCostAdd += 1;
#else
				m_CurrCost += delta;
				m_fCurrentCostAdd += delta;
#endif	
			}
			ExcuteAI(delta);
		}
	}
}
//每一帧都判断每个武将的信息                    BUFF应该是每一帧都自动处理的
void CombatLogic::ExcuteAI(float delta)
{
	Members* map_Alive = m_Manage->getMembers();
	for (Members::iterator iter = map_Alive->begin();iter != map_Alive->end(); iter++)
	{
		WarAlive* alive = iter->second;
		CostCount(alive,delta);															//计算我方所有武将cost值
		if (!alive->getActObject())continue;
		if (m_Alive && alive != m_Alive)continue;										//技能状态,非释放技能武将不做处理
		if (StateDispose(alive,delta))
			continue;										
		if (!AttackJudge(alive))
			continue;
		if (alive->getEnemy())
		{
			MonsterExcuteAI(alive,delta);
		}else{
			HeroExcuteAI(alive);			
		}
	}
}

void CombatLogic::HeroExcuteAI( WarAlive* alive )
{
	int ActionCode = alive->getActObject()->getCurrActionCode();
	CCDictionary* AttackInfo = m_SkillRange->PlaySkillInfo(alive);		//生命周期只有一帧
	if (!AttackInfo)return;
	CCArray* Alives = (CCArray*)AttackInfo->objectForKey(Hit_Alive);		//受击目标	
	if (Alives->count())
	{
		if (ActionCode == Walk_Index)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
			alive->setAIState(false);
			return;
		}else{
			AliveExcuteAI(alive,AttackInfo);
		}
	}else{
		if (m_Manage->getSkill(alive)->skillType == CallAtk)
		{
			AliveExcuteAI(alive,AttackInfo);
		}else if (alive->getCriAtk())
		{
			AliveExcuteAI(alive,AttackInfo);
		}else{				
			if (alive->getSpeAtk())
				alive->setSpeAtk(false);
			if (IsAutoMoveType(alive) || ActionCode == Walk_Index)//自动移动类,或已经开始执行AI状态
				return;
			if (!alive->getCaptain()&&alive->getGridIndex()!=alive->getMoveObj()->getgrid())//武将当前是否在固定位置,如果不在则移动回固定位置(执行AI完毕状态)
			{			
				alive->setAIState(false);
				alive->setMoveGrid(alive->getMoveObj()->getgrid());							//武将有一个回到初始位置的方法
				alive->getActObject()->setMoveState(Walk_Index);
			}else{
				int grid = m_SkillRange->GuardMoveArea(alive);
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
}

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
	alive->setMoveGrid(alive->getGridIndex()-C_GRID_ROW);
	alive->getActObject()->setMoveState(Walk_Index);
	return true;
}

//骸兽逃跑处理
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
			NOTIFICATION->postNotification(ACTREMOVE,alive);		//超出预定范围执行死亡操作
		}		
		return true;
	}
	return false;
}

void CombatLogic::MonsterExcuteAI( WarAlive* alive,float dt )
{
	if (monsterFlee(alive))											//骸兽逃跑处理,将AI执行的都放在这个地方后面好整理
		return;
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (alive->getGridIndex()<C_BEGINGRID)
	{
		if (!alive->getActObject()->getMoveState() 
			&& m_MoveRule->MonstMoveExcute(alive))
			alive->getActObject()->setMoveState(Walk_Index);
		return ;
	}
	CCDictionary* AttackInfo = m_SkillRange->PlaySkillInfo(alive);				//生命周期只有一帧
	if (!AttackInfo)
	{
		if (!alive->getActObject()->getMoveState() && m_MoveRule->MonstMoveExcute(alive) )
			alive->getActObject()->setMoveState(Walk_Index);
		return ;
	}
	CCArray* Alives = (CCArray*)AttackInfo->objectForKey(Hit_Alive);		//受击目标
	if (Alives->count())
	{
		if (ActionCode == Walk_Index)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
			return ;
		}
		AliveExcuteAI(alive,AttackInfo);
	}else{
		if (m_Manage->getSkill(alive)->skillType == CallAtk)
		{
			alive->getActObject()->setMoveState(0);
			alive->getActObject()->TurnStateTo(Stand_Index);
			AliveExcuteAI(alive,AttackInfo); 
		}else{
			if (alive->getSpeAtk())
				alive->setSpeAtk(false);
			if (alive->getCriAtk())
				alive->setCriAtk(false);			
			if (!alive->getActObject()->getMoveState() && m_MoveRule->MonstMoveExcute(alive) )
				alive->getActObject()->setMoveState(Walk_Index);
		}
	}
}

void CombatLogic::AliveExcuteAI(WarAlive* alive,CCDictionary*pDic)
{
	CCArray* Alives = (CCArray*)pDic->objectForKey(Hit_Alive);
	ActObject* pActObject = alive->getActObject();
	if (!alive->getCaptain()&&
		alive->getCriAtk()&&
		!alive->getCritEffect()&&
		alive->getCallType() != AutoSkill)
	{
		alive->setCritEffect(true);
		if (alive->getEnemy())
		{
			alive->getActObject()->TurnStateTo(Stand_Index);		
			CCArray* Grids  = (CCArray*)pDic->objectForKey(Atk_Area);
			m_MapLayer->DrawWarningEffect(Grids);						//格子预警
			m_CombatEffect->PlayerSkill(alive);
			return;
		}
		m_AliveLayer->removeEvent();
		m_Run = false;
		m_Alive = alive;
		pActObject->setUserObject(CCBool::create(true));
		pActObject->setZOrder(1000);
		CCObject* obj = nullptr;
		CCArray* actArr = CCArray::create();
		CCARRAY_FOREACH(Alives,obj)
		{
			WarAlive* pAlive = (WarAlive*)obj;
			pAlive->getActObject()->setUserObject(CCBool::create(true));
			pAlive->getActObject()->setZOrder(1000);
			pAlive->getActObject()->pauseSchedulerAndActions();
			actArr->addObject(pAlive->getActObject());
		}
		m_Assist->ActStandExcute(actArr);										//受击目标站立处理
		m_AliveLayer->getLayerColor()->setVisible(true);
		LGPause(m_AliveLayer);													//暂停刷新位置和zorder的值
		m_CombatEffect->PlayerSkill(alive);
		return;
	}
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(Alives,obj)
	{
		WarAlive* pAlive = (WarAlive*)obj;
		if (pAlive->getHp()<=0)continue;
		alive->AtkAlive.push_back(pAlive);
	}
	CEffect* effect = m_Manage->getEffect(alive);						//开始播放攻击音效攻击特效的时机可以由策划配置
	if (!effect)
	{
		CCLOG("[ ERROR ] CombatLoginc::AliveExcuteAI CEffect NULL AliveID=%d",alive->getAliveID());
		alive->ResetAttackState();
		return;
	}
	EffectInfo* info = m_Manage->getEffData()->getEffectInfo(effect->effectId);
	if (!info)
	{
		CCLOG("[ ERROR ] CombatLoginc::AliveExcuteAI EffectInfo NULL %d",effect->effectId);
		info = m_Manage->getEffData()->getEffectInfo(10000021);
	}
	pActObject->setAtkEffect(info->getusEft());
	if (!alive->getCaptain()&& alive->getNegate())									//反向攻击处理
	{
		if (pActObject->getEnemy())
		{
			pActObject->setDirection(Ditection_Left);
		}else{
			pActObject->setDirection(Ditection_Right);
		}
	}
	pActObject->TurnStateTo(info->getActionID());
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
		if (alive->role->alert)
		{
			if (alive->getAIState())
			{
				alive->setMoveGrid(alive->getMoveObj()->getgrid());
				alive->getActObject()->setMoveState(Walk_Index);
				alive->getActObject()->setUpdateState(true);
				alive->setAIState(false);
			}else{
				CCLOG("111");
				alive->getActObject()->setMoveState(Walk_Index);				//判断当前位置是否有目标而后再做处理，技能没有击杀目标
				alive->getActObject()->setUpdateState(true);
			}
		}else{
			alive->getActObject()->setMoveState(Walk_Index);	
			alive->getActObject()->setUpdateState(true);
		}
	}
}
void CombatLogic::CritAtkEnd(CCObject* ob)
{
	WarAlive* alive = dynamic_cast<WarAlive*>(ob);;
	if (!alive)
	{
		CCLOG("[ ERROR ] CombatLoginc::CritAtkEnd");
		return ;
	}
	AliveCritEnd(alive);
	alive->ResetAttackState();
	if (alive->getEnemy())
		return;
	CCArray* arr = m_AliveLayer->getAlivesOb();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		ActObject* act = (ActObject*)obj;
		act->setUserObject(nullptr);
		act->setFront(false);
		act->getHp()->setUserObject(nullptr);
	}
	m_UILayer->AddEvent();
	m_AliveLayer->addEvent();
	m_AliveLayer->getLayerColor()->setVisible(false);
	LGResume(m_AliveLayer);
	m_Alive = nullptr;

	m_Record = false;
	m_PlayerNum++;
	m_CombatEffect->setPlayerNum(m_PlayerNum);
	if (m_PlayerNum >= m_RecordNum)
	{
		m_Run = false;
		m_CombatEffect->BatterSpine(m_RecordNum);
		m_PlayerNum = 0; 
		m_RecordNum = 0;
		m_CombatEffect->setPlayerNum(0);
		m_Record = true;
	}
	//在此处触发连接的大效果播放。并记录当前一共按了多少次技能,延长连击接上的判定时间
}

//攻击帧回调逻辑处理、武将执行一次伤害计算并播放效果
void CombatLogic::AtkLogic(CCObject* ob)
{
	WarAlive* alive = (WarAlive*)ob;
	CEffect* effect = DataCenter::sharedData()->getWar()->getEffect(alive);
	if (!effect)return;
	if( alive->getSortieNum() >= effect->batter )
		return;	
	switch (m_Manage->getSkill(alive)->skillType)
	{
	case NorAtk:
	case SpeAtk:
	case CriAtk:
		{
			BattleResult* Result = m_HurtCount->AttackExcute(alive);
			if (alive->getCriAtk()&&!alive->getEffIndex())								//必杀技多释一个空效果
				m_CombatEffect->AttackNull(Result);
			if (!Result->m_HitTargets.empty())
				m_CombatEffect->BattleEffect(Result);
		}
		break;
	case CallAtk:
		{
			alive->setSortieNum(alive->getSortieNum()+1);								//一次性可召唤多个武将
			WarAlive* pAlive = m_Manage->getCallAlive(alive,m_Manage->getSkill(alive));	//得到被召唤的武将
			if (!pAlive)
			{
				CCLOG("[ *ERROR ] CombatLoginc::AtkLogic CallAlive NULL");
				return;
			}		
			m_AliveLayer->createAlive(pAlive,SceneTrap);
		}
		break;
	case CapAtk:
		break;
	default:
		break;
	}
	if( alive->getSortieNum() >= effect->batter )
	{
		for (auto i:alive->HittingAlive)
		{
			if (i->getHp()>0)
			{
				m_HurtCount->BuffHandleLogic(alive,i);									//伤害计算完成才能添加新的BUFF
			}else{
				//Mr.yan 判空
				if(i->getActObject() != nullptr)
					i->getActObject()->AliveDie();
			}
		}
		alive->HittingAlive.clear();
	}
}

//武将进行逻辑处理前状态结算
bool CombatLogic::StateDispose( WarAlive* alive,float dt )
{
	if (alive->getCallType() == NotAttack || alive->getDieState())//石头类武将不做攻击判断处理
		return true;
	if (!alive->getBattle())
	{
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
		return true;
	}
	if ((alive->role->CallType == AutoSkill || alive->role->MstType == MST_SKILL)&&!alive->getCriAtk())						//进入战场就释放技能(陨石类)
	{
		if (alive->getAliveStat()==COMMONSTATE)
		{
			if (alive->role->MstType == MST_SKILL)
				alive->role->MstType = MST_COMMON;
			alive->setCriAtk( true );
			return false;
		}
	}
	if (alive->getStatDelay()>0)
		alive->setStatDelay(alive->getStatDelay()-dt);									//敌方武将释放必杀技攻击延迟时间
	if (alive->getAtkDelay()>0)
	{
		alive->setAtkDelay(alive->getAtkDelay()-dt);									//敌方武将释放必杀技攻击延迟时间
		return true;
	}
	alive->getActObject()->updateFrameEvent(dt);										//刷新武将帧事件
	if (alive->getHp()<=0)
		return true;
	alive->getBuffManage()->upDateBuff(dt);												//刷新武将buff信息
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (ActionCode == Stand_Index || 
		ActionCode == Hit_Index || 
		ActionCode == Walk_Index)//怪物的技能释放是由攻击次数和时间确定的
		alive->setAtktime(dt);
	if (!alive->getCriAtk()&&alive->getEnemy())
		alive->setCritTime(dt);
	if (ActionCode == Dizzy_Index||
		ActionCode == Skill_Index||
		ActionCode == SpAttack_Index||
		ActionCode == Attack_Index||
		ActionCode == victory_Index||
		ActionCode == Die_Index)	
	{
		if (alive->getMstType() == MST_HIDE)
		{
			alive->setcloaking(false);
			((CCArmature*)alive->getActObject()->getArmature())->setOpacity(255);
		}
	}
	return false;
}

//武将状态判断,武将是否可攻击判断,提高效率关键方法之一
bool CombatLogic::AttackJudge( WarAlive* alive )
{
	int ActionCode = alive->getActObject()->getCurrActionCode();
	if (ActionCode == Walk_Index)
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
	if (ActionCode == Dizzy_Index||
		ActionCode == Skill_Index||
		ActionCode == SpAttack_Index||
		ActionCode == Attack_Index||
		ActionCode == victory_Index||
		ActionCode == Die_Index)
	{
		return false;
	}
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
void CombatLogic::NextBatch( float dt )
{
	m_CurrBatchNum++;
	CCArray* arr =m_Manage->getMonsts(true);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if(alive->getAliveID() >= C_BatchMonst+m_CurrBatchNum*100)
			m_AliveLayer->createAlive(alive,SceneTrap);
	}
	char path[60] = {0};
	sprintf(path,"%d/%d",m_CurrBatchNum+1,m_BatchNum+1);
	CLayout* m_pNormal = (CLayout*)m_UILayer->findWidgetById("layer_up_normal");
	CCLabelAtlas* BatchNum = (CCLabelAtlas*)m_pNormal->getChildByTag(CL_Batch);
	BatchNum->setString(path);
	if (m_CurrBatchNum < m_BatchNum/*&&DataCenter::sharedData()->getWar()->getStageID()*/)
	{
		CCLabelAtlas* labAt = CCLabelAtlas::create(ToString(m_CurrBatchNum+1),"label/47_44.png",47,44,'0');
		ShowTexttip(labAt,RGB_GREEN,ROUNDNUM,CCPointZero,0,0,0,200);
		NOTIFICATION->postNotification(TIPSEFFECT,CCBool::create(false));
	}
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (DataCenter::sharedData()->getWar()->getStageID() == 0)							//引导开启的情况
	{
		char path[60] = {0};
		sprintf(path,"%d_%d",0,m_CurrBatchNum+1);										//覆盖高亮区域的图片
		DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
	}
#else
	if (DataCenter::sharedData()->getWar()->getStageID() == 0)							//引导开启的情况
	{
		char path[60] = {0};
		sprintf(path,"%d_%d",0,m_CurrBatchNum+1);										//覆盖高亮区域的图片
		DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
	}
#endif
	//m_MapLayer->getBackgroundManage()->initWithStageBatch(m_Manage->getStageID(),m_send);	//一场战斗一个地图		
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

void CombatLogic::ActRemove( CCObject* ob )
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getEnemy())
	{
		if (!m_Manage->checkMonstOver())
			return;
		if (m_BatchNum > m_CurrBatchNum)	
		{
			srandNum();															//设置随机种子
			m_Manage->initBatchData(m_CurrBatchNum+1);							//初始化批次武将数据(必须马上初始化数据,但是绘制可以延迟,否则多个武将连续死亡会直接战斗胜利出现)
			this->scheduleOnce(schedule_selector(CombatLogic::NextBatch),1);	//打下一批次延时时间
		}else{
			if (m_CurrBatchNum == 1000)
				return;															//防止多次调用出现
			m_Run = false;
			m_CurrBatchNum = 1000;
			CCArray* arr = m_AliveLayer->getAlivesOb(AliveType_Monster);
			CCObject* obj = nullptr;
			CCARRAY_FOREACH(arr,obj);
			{
				ActObject* act = (ActObject*)obj;
				if (act->getCurrActionCode() != Die_Index)
					act->AliveDie();
			}
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
			if (DataCenter::sharedData()->getWar()->getAliveByGrid(C_CAPTAINGRID))
			{
				m_UILayer->hideUpUiPart();			//隐藏control UI的上部分
				m_StoryLayer->CreateStory(CCInteger::create((int)StoryType::OverStory));			//我方胜利(播放最后一个剧情)
			}
		}
	}else{
		if (!alive->getCaptain())
			return;
		m_CurrBatchNum = 1000;
		m_Run = false;
		m_Assist->ActStandExcute(m_AliveLayer->getAlivesOb());
		CombatResult(CCInteger::create(fail));		//战斗失败
	}
}

void CombatLogic::AliveDieDispose( CCObject* ob )
{
	WarAlive* alive = (WarAlive*)ob;
	if (alive->getEnemy())
	{
		CCArray* arr = m_AliveLayer->getAlivesOb(AliveType_Monster);
		if (arr->count()==1)
		{
			if (m_BatchNum > m_CurrBatchNum)				//判断最后一回合
				return;
			m_UILayer->RemoveEvent();															//最后一个怪物死亡不能再释放技能了
			CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.3f);
			m_Run = false;
			DataCenter::sharedData()->getWar()->setbattleOver(true);
		}else{
			PlayEffectSound(SFX_423);
			m_CurrCost += alive->getInitCost();													//击杀敌方武将添加cost
			m_UILayer->showFlyCostToBar(m_AliveLayer->convertToWorldSpace(alive->getActObject()->getPosition()));//粒子效果
		}
	}else{
		if (alive->role->isCall || alive->getCaptain())											//喽啰死亡不进行初始化处理
			return;
		m_UILayer->AliveBattleDispose(alive);//武将下阵	
	}
}

void CombatLogic::CombatResult(CCObject* ob)
{
	WarManager* Manage = DataCenter::sharedData()->getWar();
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);

	if (m_Manage->getWorldBoss())
	{
		m_bGameOver = true;
		scheduleForRequesBossFinish();
		this->runAction(
			CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCDelayTime::create(5.0f), 
			CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequesBossFinish)))));
		return;
	}

	if( ((CCInteger*)ob)->getValue() == win )				
	{
		PlayBackgroundMusic(SFX_Win,false);	
		if (Manage->getStageID())
		{
			WarAlive* alive = Manage->getAliveByGrid(C_CAPTAINGRID);	
			int hp = alive->getHp();
			if (hp>alive->role->hp)
				hp = alive->role->hp;
			CPlayerControl::getInstance().sendBattleFinish(1, true, hp, 0);
			m_gameFinishData.reason = 1;
			m_gameFinishData.res = true;
			m_gameFinishData.roundNum = alive->role->hp;
			m_gameFinishData.aliveNum = 0;
			m_bGameOver = true;
			this->runAction(
				CCRepeatForever::create(CCSequence::createWithTwoActions(
				CCDelayTime::create(5.0f), 
				CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequestFinish)))));
			return;
		}
	}else{
		if (Manage->getStageID())
		{			
			CPlayerControl::getInstance().sendBattleFinish(1,false,0,0);
			m_gameFinishData.reason = 1;
			m_gameFinishData.res = false;
			m_gameFinishData.roundNum = 0;
			m_gameFinishData.aliveNum = 0;
			m_bGameOver = true;
			this->runAction(
				CCRepeatForever::create(CCSequence::createWithTwoActions(
				CCDelayTime::create(5.0f), 
				CCCallFunc::create(this, callfunc_selector(CombatLogic::scheduleForRequestFinish)))));
			return;
		}
	}

//#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
//	//战斗引导完了，弹框告知是否下载更新包
//	if(CDownloadPackage::checkIsNeedPackage())
//	{
//		CPopTip *pop = CPopTip::create();
//		LayerManager::instance()->pop();
//		pop->setVisible(true);
//		pop->addContentTip(GETLANGSTR(1156));
//		pop->setTouchEnabled(true);
//		pop->setTouchPriority(-100);
//		pop->setButtonLisener(this, ccw_click_selector(CombatLogic::onClickDownloadPackage));
//		CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 899);
//		return;
//	}
//#endif
	//直接继续游戏
	downloadPackageEnd(true);

}

void CombatLogic::StoryEndEvent(CCObject* ob)
{
	switch ((StoryType)m_StoryLayer->getSType())
	{
	case StoryType::beginStory:
		{
			if (!m_Manage->getStageID())
			{
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
			PlayEffectSound(SFX_413);	
			CCDelayTime* delay = CCDelayTime::create(0.5f);
			CCDelayTime* delay2 = CCDelayTime::create(0.3f);
			CCMoveTo* mt = CCMoveTo::create(1.2f,ccp( MAP_MINX(m_MapData) , m_Scene->getMoveLayer()->getPositionY()));
			CCCallFuncO* cfo = CCCallFuncO::create(this,callfuncO_selector(WarScene::LayerMoveEnd),CCInteger::create((int)StoryType::MoveEndStory));
			CCSequence* sqe = CCSequence::create(delay,mt,delay2,cfo,nullptr);
			m_Scene->getMoveLayer()->runAction(sqe);
		}break;
	case StoryType::MoveEndStory:
		{
			m_UILayer->setVisible(true);
			m_Scene->AddEvent();					//场景移动动画结束，添加事件响应
			scheduleUpdate();										//开启游戏主循环
			if(m_bCountDown)
				schedule(schedule_selector(CombatLogic::updateOneSecond), 1.0f);
			PlayBackgroundMusic(BGM_Battle,true);
#if CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM
			if (DataCenter::sharedData()->getWar()->getStageID() == 0)						//引导开启的情况
			{
				char path[60] = {0};
				sprintf(path,"%d_%d",0,1);													//覆盖高亮区域的图片
				DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
				//DataCenter::sharedData()->getCombatGuideMg()->setGuide("CostGuide");
			}else if (DataCenter::sharedData()->getWar()->getReliveNeedGoldNum() && 
					  DataCenter::sharedData()->getWar()->getStageID() == 301		)
			{
				DataCenter::sharedData()->getCombatGuideMg()->setGuide("CostGuide");
			}
			//DataCenter::sharedData()->getCombatGuideMg()->setGuide("CostGuide");
#else
			if (DataCenter::sharedData()->getWar()->getStageID() == 0)						//引导开启的情况
			{
				char path[60] = {0};
				sprintf(path,"%d_%d",0,1);													//覆盖高亮区域的图片
				DataCenter::sharedData()->getCombatGuideMg()->setGuide(path);
			}else if (DataCenter::sharedData()->getWar()->getReliveNeedGoldNum() && 
				DataCenter::sharedData()->getWar()->getStageID() == 301
				)
			{
				DataCenter::sharedData()->getCombatGuideMg()->setGuide("CostGuide");
			}
#endif
			if (m_Manage->getAliveByType(AliveType::Boss))
			{
				WarAlive* boss = m_Manage->getAliveByType(AliveType::Boss);
				m_Assist->DisplayBossWarning(m_UILayer,boss->getModel());							//第一波就出现超大boss的情况
			}				
		}break;
	case StoryType::OverStory:
		{
			if (m_CurrBatchNum >= m_BatchNum && m_Manage->checkMonstOver())
			{
				CCArray* arr = m_AliveLayer->getAlivesOb(AliveType_Hero);
				CCObject* obj = nullptr;
				CCARRAY_FOREACH(arr,obj)
				{
					ActObject* act = (ActObject*)obj;
					act->TurnStateTo(victory_Index);
				}
				m_task->addObject(CombatTask::create(m_Assist,callfuncO_selector(WarAssist::WinEffect),m_time+1));
				return;
			}
		}break;
	default:break;
	}
}
//cost 计算(帧)或以秒为单位进行计算、位置实时更新
void CombatLogic::CostCount(WarAlive* alive,float dt)
{
	if ( alive->getEnemy() || DataCenter::sharedData()->getCombatGuideMg()->IsGuide() || alive->getFatherID()) //引导状态关闭
		return;
	if (m_Alive)				//连击状态不计算cost变化
		return;
	vector<int>* grids = m_Manage->getAddcostVec();
	if (alive->getMoveObj() && 
		std::find(grids->begin(),grids->end(),alive->getMoveObj()->getgrid()) == grids->end())
	{
		//m_CurrCost -= alive->getAddCost() * dt;				//只会不加cost不会减少cost
		//m_fCurrentCostAdd -= alive->getAddCost();
	}else{
		m_CurrCost += alive->getAddCost() * dt;					//怒气值每帧变化率
		m_fCurrentCostAdd += alive->getAddCost();
	}
	if ( m_CurrCost <= 0 ) //武将移动回我方半屏
	{
		m_CurrCost = 0;//ResetAlive();							//暂时
	}else if(m_CurrCost >= m_MaxCost){
		m_CurrCost = m_MaxCost;
	}
}

//显示返回层
void CombatLogic::CBackLayer(CCObject* ob)
{
	if(dynamic_cast<WarRelive*>(CCDirector::sharedDirector()->getRunningScene()->getChildByTag(WAR_RELIVE_TAG)))
		return;
	////如果是在打世界BOSS，屏蔽返回键
	//if(m_Manage->getWorldBoss())
	//{
	//	return;
	//}

	if(m_bGameOver)
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
		onPause();
		blayer->show();
	}
}

void CombatLogic::ContinueStage( int type, google::protobuf::Message *msg )
{
	int i =0;
}

void CombatLogic::OnBattleFinish( int type, google::protobuf::Message *msg )
{
	//非世界BOSS模式
	if(type == ExitStage)
	{
		m_bFinish = true;
		
		m_Scene->removeChildByTag(backLayer_tag);

		this->stopAllActions();

		BattleFinishRep *res = (BattleFinishRep*)msg;

		CCSize size = CCDirector::sharedDirector()->getWinSize();	

		if (res->win())
		{
			WarWinLayer *layer = WarWinLayer::create();
			m_Scene->addChild(layer);
			layer->processBattleFinish(type, msg);
		}
		else
		{
			WarFailLayer *layer = WarFailLayer::create();
			m_Scene->addChild(layer);
		}
	}
	else if(type == BossFinishReqMsg)
	{
		m_bFinish = true;

		m_Scene->removeChildByTag(backLayer_tag);

		this->stopAllActions();

		BattleFinishRep *res = (BattleFinishRep*)msg;

		WorldBossEndLayer * layer = WorldBossEndLayer::create();
		m_Scene->addChild(layer);
		layer->processBattleFinish(type, msg);
	}
}

void CombatLogic::scheduleForRequestFinish()
{
	if(!m_bFinish)
	{
		CPlayerControl::getInstance().sendBattleFinish(m_gameFinishData.reason, m_gameFinishData.res, m_gameFinishData.roundNum, m_gameFinishData.aliveNum);
	}
}


void CombatLogic::scheduleForRequesBossFinish()
{
	if(!m_bFinish)
	{
		//如果是世界boss调用
		int hurt = m_Manage->getBossHurtCount();
		int checkNum = m_Manage->getVerifyNum();
		if ((checkNum + hurt) != 97231000)
		{
			CCLOG("[ *ERROR ] CombatLoginc::CombatResult");				//验证造成的伤害是否被修改内存
			return;
		}
		vector<int>* vec= m_Manage->getBossHurtVec();
		CPlayerControl::getInstance().sendWorldBossFinish(hurt, checkNum, *vec);
	}
}


void CombatLogic::onPause()
{
	this->pauseSchedulerAndActions();

	//世界BOSS不停止计时
	if (m_Manage->getWorldBoss())
	{
		unschedule(schedule_selector(CombatLogic::updateOneSecond));
		schedule(schedule_selector(CombatLogic::updateOneSecond), 1.0f);
	}
}

void CombatLogic::onResume()
{
	this->resumeSchedulerAndActions();
}

void CombatLogic::reliveSuccess()
{
	CombatResult(CCInteger::create(fail));
}

void CombatLogic::startCountDown( int iTime )
{
	m_iGameTimeCount = iTime;
	m_bCountDown = true;
}

void CombatLogic::onClickDownloadPackage( CCObject* pSender )
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

void CombatLogic::downloadPackageEnd( bool bAnswer )
{
	if(bAnswer)
	{
		CScene* scene = GETSCENE(LoadWar);
		((LoadWar*)scene)->setRelease(true,skipSelectHero);
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}
}

void CombatLogic::updateOneSecond( float dt )
{
	if(m_bCountDown)
	{
		m_iGameTimeCount--;
		if (m_iGameTimeCount%5 == 0)
			m_Manage->getBossHurtVec()->push_back(m_Manage->getBossHurtCount());			//每隔5秒存储一次世界boss伤害值
		if(m_iGameTimeCount<0)
		{
			unschedule(schedule_selector(CombatLogic::updateOneSecond));
			//结算
			m_Run = false;
			PlayBackgroundMusic(SFX_Win,false);
			CombatResult(CCInteger::create(win));
		}else{
			//更新计时器
			m_UILayer->updateTimeCountUI(m_iGameTimeCount);
		}
	}
}

