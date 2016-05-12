#include "WarManager.h"
#include "AppUI.h"
#include "Global.h"
#include "DataCenter.h"
#include "common/CommonFunction.h"
#include "model/BuffManage.h"
#include "warscene/HurtCount.h"
#include "warscene/SkillRange.h"
#include "warscene/ConstNum.h"
#include "tools/commonDef.h"
#include "scene/layer/WarAliveLayer.h"
#include <functional>					//sort 的降序排序使用
#include "DataDefine.h"
#include "warscene/EffectData.h"
#include "tollgate/Chapter.h"
#include "scene/alive/AliveDefine.h"
#include "warscene/CaptainSkill.h"
#include "TerrainManager.h"
#include "warscene/MoveRule.h"
#include "model/MapManager.h"
#include "warscene/ComBatLogic.h"
#include "warscene/ArmatureEventDataMgr.h"
#include "warscene/ParseFileData.h"
#include "warscene/BattleTools.h"

WarManager::WarManager()
	:m_SceneTarpID(0),m_efdata(nullptr),m_terData(nullptr),m_armatureEventDataMgr(nullptr)
	,m_StageID(-1),m_Batch(-1),m_Logic(nullptr),m_iLastStageId(0),m_StageType(0),m_BuffData(nullptr)
	,m_bNormal(true),m_ChapterCount(0),m_ChapterIndex(0),m_iReliveNeedGoldNum(0), m_BossModel(0)
	,m_VerifyNum(0),m_BossHurtCount(0),m_BossHurtPe(0),m_iWorldBossRank(0),m_StoryData(nullptr)
	,m_BattleOver(false)
{
	int id[100] = {146,1464,1565,3014,3064,3514,10664,319,576,1096,1101,391,1102};
	for (auto i:id)
	{
		bool add = true;
		for (auto j:m_SpineID)
			if (i == j)
				add = false;
		if (add&&i)
			m_SpineID.push_back(i);
	}
}

bool WarManager::isSpine( int id )
{
	for (auto i:m_SpineID)
		if (i == id)
			return true;
	return false;
}

WarManager::~WarManager(){}

void WarManager::BattleDataClear()
{
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		CC_SAFE_RELEASE(iter->second);
		iter->second = nullptr;
	}
	m_BattleOver = true;
	m_BossHurtPe = 0;
	ReleaseSpineData();
	m_CallRole.clear();
	m_members.clear();
	m_SceneTarpID = 0;
	m_StageID = -1;
	m_Batch = -1;
	m_Logic = nullptr;
	m_VecBossHurt.clear();
	CSceneTrap nullSceneTarp;
	setSceneTrap(nullSceneTarp);
	m_Heros.clear();
	m_Monsters.clear();
	m_CantMoveGrid.clear();
	m_AddCostGrid.clear();
	CC_SAFE_RELEASE(m_efdata);
	m_efdata = nullptr;
	CC_SAFE_RELEASE(m_terData);
	m_terData = nullptr;
	CC_SAFE_RELEASE(m_BuffData);
	m_BuffData = nullptr;
	CC_SAFE_RELEASE(m_StoryData);
	m_StoryData = nullptr;
	CC_SAFE_RELEASE(m_armatureEventDataMgr);
	m_armatureEventDataMgr = nullptr;
}
void WarManager::ReleaseSpineData()
{
	for (auto i:m_MapSpineData)
	{
		spSkeletonData_dispose(i.second.first);
		spAtlas_dispose(i.second.second);
		CCLOG("LoadSpineData::ReleaseData Release %s",i.first.c_str());
	}
	m_MapSpineData.clear();
}
//初始化验证伤害值(服务器约定值),多个地方记录造成伤害防止被修改内存
void WarManager::setWorldBoss(int model)
{
	if (model)
	{
		m_BossHurtCount = 0;
		m_VerifyNum = 97231000;
	}else
		m_VerifyNum = 0;
	m_BossModel = model;
}
int WarManager::getWorldBoss(){return m_BossModel;}

int WarManager::getVerifyNum(){ return m_VerifyNum; }
void WarManager::setVerifyNum(int num){ m_VerifyNum -= num; }

int WarManager::getBossHurtCount(){return m_BossHurtCount;}
void WarManager::setBossHurtCount(int hurt){ m_BossHurtCount += hurt;}

void WarManager::addAlive(WarAlive* alive)
{
	if(!alive) return;
	WarAlive* tpAlive = getAlive(alive->getAliveID());
	if( tpAlive == alive ) return;
	if( tpAlive )
	{
		CCLOG("[ **ERROR ]WarManager::addAlive has exists AliveModel:%d",alive->getModel());
		return;
	}
	alive->retain();
	alive->getBuffManage()->setAlive(alive);//将自己传入buf管理器
	m_members[alive->getAliveID()] = alive;
}

WarAlive* WarManager::getAlive(unsigned int aliveID)
{
	Members::iterator iter = m_members.find(aliveID);
	if( iter != m_members.end() ) return iter->second;
	return nullptr;
}

CSceneTrap* WarManager::getSceneTarp() { return &m_SceneTarp;}
void WarManager::setSceneTrap(CSceneTrap& tarp)
{
	m_SceneTarpID = m_SceneTarp.id;			//记录前一个场景效果的id
	m_SceneTarp = tarp; 
}

SpData* WarManager::getSpineData( std::string Name )
{
	MapSkeletonData::iterator iter = m_MapSpineData.find(Name);
	if (iter != m_MapSpineData.end())
		return &iter->second;
	return nullptr;
}

void WarManager::initData()
{
	m_efdata = EffectData::create();			//创建这些对象都需要读取文件，最好是整个战斗过程中只创建一次
	m_efdata->retain();
	m_terData = terData::create();
	m_terData->retain();
	m_BuffData = BuffData::create();
	m_BuffData->retain();
	m_StoryData = StoryData::create();
	m_StoryData->retain();
	m_armatureEventDataMgr = ArmatureEventDataMgr::create();
	m_armatureEventDataMgr->retain();
	//添加我方武将
	for (int i = 1 ; i <= m_battleInit.heroList.size() ; i++)
	{
		WarAlive* alive = WarAlive::create();						//创建数据对象
		alive->role = &m_battleInit.heroList.at(i-1);
		alive->setAliveID(i);			//战场上武将的唯一id
		alive->setEnemy(false);
		initAlive(alive);
	}
	//添加敌方武将
	initBatchData(0);												//开始只创建第0批次怪物
	CaptainSkill* captain = CaptainSkill::create();
	captain->ExecuteCaptainSkill();
}

//初始化战斗数据
void WarManager::initBattleData(const BattleDataInit& bat)
{
	m_BattleOver = false;
	ReleaseSpineData();												//应该有更好的管理方法
	ParseMoveGrid(m_StageID,m_CantMoveGrid);
	ParseAddCostGrid(m_StageID,m_AddCostGrid);
	m_battleInit = bat;
	DataCenter::sharedData()->getWar()->initData();
	DataCenter::sharedData()->getTer()->initData(m_battleInit.terrList);//所有波次的地形效果一次推送,自行管理自己的数据
	m_StoryData->initStoryData(m_StageID);
}
//初始化关卡批次数据到战斗武将列表
void WarManager::initBatchData( int batch )
{
	for (int i = 0 ; i < m_battleInit.monsterList.size() ; i++)
	{
		if (!batch&&m_battleInit.monsterList.at(i).isCall)
		{
			m_CallRole.push_back(&m_battleInit.monsterList.at(i));
			continue;
		}
		if (m_battleInit.monsterList.at(i).batch!=batch)					//开始只创建第0批次怪物
			continue;
		WarAlive* alive = WarAlive::create();								//创建数据对象
		if (m_battleInit.monsterList.at(i).isBoss)							//大怪物提示
			if (m_BossModel)
				alive->setAliveType(AliveType::WorldBoss);					//世界boss
			else
				alive->setAliveType(AliveType::Boss);						//一般类型大boss
		alive->role = &m_battleInit.monsterList.at(i);
		alive->setAliveID(C_BatchMonst+i+batch*100);
		alive->setEnemy(true);
		alive->setMstType(alive->role->MstType);
		if (alive->role->MstType == MST_HIDE)
			alive->setcloaking(true);
		alive->setDelaytime(-m_battleInit.monsterList.at(i).delay);
		alive->moves.push_back(m_battleInit.monsterList.at(i).move1);
		alive->moves.push_back(m_battleInit.monsterList.at(i).move2);
		alive->moves.push_back(m_battleInit.monsterList.at(i).move3);
		alive->moves.push_back(m_battleInit.monsterList.at(i).move4);
		alive->moves.push_back(m_battleInit.monsterList.at(i).move5);
		initAlive(alive); 
	}
}

void WarManager::initAlive(WarAlive* alive)
{
	alive->grids.clear();
	if (alive->getEnemy())
	{
		alive->setGridIndex(alive->role->grid);
	}else{
		if (alive->role->battle)									//队长进来则为上阵状态
		{
			alive->setCaptain(true);
			alive->setGridIndex(C_CAPTAINSTAND);
			CEffect* effect = getSummonEffect(&alive->role->skill3);
			if (effect)
				alive->setcallAliveNum(effect->pro_Type);
		}else{
			alive->setGridIndex(INVALID_GRID);
		}
	}
	for (int j = 0 ; j<alive->role->buffList.size();j++ )
		alive->getBuffManage()->AddBuff(alive->role->buffList.at(j));
	alive->setExecuteCap(false);
	alive->setDieState(false);
	alive->setModel(alive->role->thumb);
	alive->setCallType(alive->role->CallType);
	alive->setAtkInterval(alive->role->atkInterval);
	alive->setMoveSpeed(alive->role->MoveSpeed);
	if (alive->getAliveType() == AliveType::WorldBoss)
	{
		alive->setMaxHp(alive->role->maxhp);
	}else{
		alive->setMaxHp(alive->role->hp);
	}
	alive->setHp(alive->role->hp);									//第一次进来是满血状态
	alive->setCostmax(alive->role->maxCost);
	alive->setInitCost(alive->role->initCost);			
	alive->setAddCost(alive->role->addCost);
	alive->setAtk(alive->role->atk);
	alive->setCrit(alive->role->crit);
	alive->setDef(alive->role->def);
	alive->setHit(alive->role->hit);
	alive->setName(alive->role->name.c_str());
	alive->setRenew(alive->role->renew);
	alive->setDoge(alive->role->dodge);
	alive->setZoom(alive->role->zoom*0.01f);
	alive->setMove(true); 
	addAlive(alive);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (alive->getEnemy())
	{
		//alive->setMaxHp(1000000);
		//alive->setHp(7500000);		//第一次进来是满血状态
		//alive->setAtk(1000);
		//alive->setGridIndex(122);
		//alive->setMaxHp(5);
		//alive->setHp(5);		//第一次进来是满血状态
	}else{		
		//alive->setAtk(500000);
		//alive->setMaxHp(500000);
		//alive->setHp(500000);		//第一次进来是满血状态
	}
#endif
}

CSkill* WarManager::getSkill( WarAlive* alive )
{
	CSkill* skill = nullptr;
	if (alive->getCriAtk())
	{
		if (alive->role->skill3.skillId)
		{
			return &alive->role->skill3;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill CritSKILL IS NULL");
			//alive->setCriAtk(false);
			return &alive->role->skill1;
		}
	}else if (alive->getSpeAtk())
	{
		if (alive->role->skill2.skillId)
		{
			return &alive->role->skill2;
		}else{
			CCLOG("[ *ERROR ] WarManager::getSkill SpecialSKILL IS NULL");
			return &alive->role->skill1;
		}
	}else{
		skill = &alive->role->skill1;
	}
	return skill;
}
//得到武将当前状态技能效果
CEffect* WarManager::getEffect(WarAlive* alive)
{
	CSkill* skill = getSkill(alive);
	int efid = alive->getEffIndex();		//效果index
	if (alive->getEffectGroup()<=skill->EffectList.size())
		if (skill->EffectList.at(alive->getEffectGroup()-1).size()>efid)
			return &skill->EffectList.at(alive->getEffectGroup()-1).at(efid);
		else
			CCLOG("[ *ERROR ] EffIndex > EffectListSize  AliveID=%d mode=%d",alive->getAliveID(),alive->role->thumb);
	else{
		CCLOG("[ *ERROR ]Skill Effect Is NULL  AliveID=%d mode=%d EffectGroup=%d",alive->getAliveID(),alive->role->thumb,alive->getEffectGroup());
		alive->setEffectGroup(1);
	}	
	return nullptr;
	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
}

bool WarManager::NextEffect(WarAlive* alive)
{
	int effId = alive->getEffIndex() + 1;
	int ranNum = CCRANDOM_0_1()*100;								//0到100的数
	CSkill* skill = getSkill(alive);
	if (alive->getEffectGroup()>skill->EffectList.size())
		return false;
	if (skill->skillId&&effId < skill->EffectList.at(alive->getEffectGroup()-1).size())			//判断是否为最后的一个特效
	{
		if (skill->EffectList.at(alive->getEffectGroup()-1).at(effId).userRate > ranNum)
		{
			alive->setEffIndex(effId);
			return true;
		}
	}
	return false;
}

CCArray* WarManager::getAlives(bool isAlive /*=false*/)
{
	if(m_members.empty()) return nullptr;
	CCArray* arr = CCArray::create();
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		if (isAlive)
		{
			if (iter->second->getHp() > 0)
				arr->addObject(iter->second);
		}else{ 
			arr->addObject(iter->second); 
		}
	}
	return arr;
}

CCArray* WarManager::getHeros(bool isAlive,bool sort/*= true*/)
{
	if(m_members.empty()) return nullptr;
	CCArray* arr = CCArray::create();
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		if (!iter->second->getEnemy())
		{
			if (isAlive)
			{
				if (iter->second->getHp()>0)
					arr->addObject(iter->second);
			}else{
				arr->addObject(iter->second);
			}
		}
	}
	if (!arr->count()||!sort)
		return arr;
	for (int i=0;i < arr->count()-1;i++)			//冒泡：判断相邻两个大小关系互换位置
	{
		for (int j =0;j<arr->count()-1-i;j++)
		{
			int front = ((WarAlive*)arr->objectAtIndex(j))->getGridIndex();
			int back  = ((WarAlive*)arr->objectAtIndex(j+1))->getGridIndex();
			if ( front > back)						//从小到大排序,把大的换到后面
				arr->exchangeObjectAtIndex(j,j+1);
		}
	}
	return arr;
}

CCArray* WarManager::getMonsts(bool isAlive,bool sort /*=true*/)
{
	if(m_members.empty()) return nullptr;
	CCArray* arr = CCArray::create();
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		if (iter->second->getEnemy())
		{
			if (isAlive)
			{
				if (iter->second->getHp()>0)
					arr->addObject(iter->second);
			}else{
				arr->addObject(iter->second);
			}
		}
	}
	if (!arr->count()||!sort)
		return arr;
	for (int i=0;i < arr->count()-1;i++)			//冒泡：判断相邻两个大小关系互换位置
	{
		for (int j =0;j<arr->count()-1-i;j++)
		{
			int front = ((WarAlive*)arr->objectAtIndex(j))->getGridIndex();
			int back  = ((WarAlive*)arr->objectAtIndex(j+1))->getGridIndex();
			if ( front < back)						//从大到小排序,把小的换到后面
				arr->exchangeObjectAtIndex(j,j+1);
		}
	}
	return arr;
}

WarAlive* WarManager::getAliveByGrid(int grid)
{
	if (grid == INVALID_GRID)
		return nullptr;
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		//先判断是否为boss如果为boss判断boss所站的区域是否包含该格子
		WarAlive* alive = iter->second;
		if (alive->getHp() <= 0)
			continue;
		if (grid >=  C_CAPTAINGRID)
		{
			if (alive->getCaptain())
				return alive;
			else
				continue;
		}
		for (int i=0; i < alive->grids.size();i++)
		{
			if (alive->grids.at(i) == grid)
				return alive;
		}
	}
	return nullptr;
}

WarAlive* WarManager::getAliveByType( AliveType type,bool Monster/* = true*/ )
{
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		WarAlive* alive = iter->second;
		if (alive->getHp() <= 0 || alive->getEnemy() != Monster)
			continue;
		if (alive->getAliveType() == type)
			return alive;
	}
	return nullptr;
}

bool WarManager::checkMonstOver()
{
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		if(iter->second->getEnemy() && iter->second->getHp()>0&&
			iter->second->getBattle() && iter->second->getActObject()&&!iter->second->getDieState())
			return false;
	}
	return true;
}

BattleDataInit* WarManager::getBattleData() { 
	return &m_battleInit; }
vector<CMonster*>* WarManager::getCallMonst() { 
	return & m_CallRole; }
ArmatureEventDataMgr* WarManager::getArmatureDataMgr() { 
	return m_armatureEventDataMgr; }
void WarManager::setTollgate(CStage& tag) { 
	tollgateInfo = tag; }
CStage* WarManager::getTollgete() { 
	return& tollgateInfo; }
Members* WarManager::getMembers() { 
	return &m_members; }
EffectData* WarManager::getEffData() { 
	return m_efdata; }
BuffData* WarManager::getBuffData(){
	return m_BuffData;}
StoryData* WarManager::getStoryData(){
	return m_StoryData;}

//先判断是否已经存在我方的召唤武将且未上阵的
//判断召唤数量是否达到上限
//获取我方将要召唤武将信息


WarAlive* WarManager::getAbsentCallAlive( WarAlive* fatherAlive )
{
	Members::iterator iter = m_members.begin();
	for(; iter != m_members.end();++iter)			//判断是否有已创建但未上阵的召唤武将
	{
		WarAlive* alive = iter->second;
		if (alive->getHp()<=0 || alive->getEnemy() != fatherAlive->getEnemy())
			continue;
		if (alive->role->isCall && 
			!alive->getBattle() && 
			alive->getFatherID()==fatherAlive->getAliveID())
			return alive;
	}
	return nullptr;
}

bool WarManager::captainCallNumberJudge( WarAlive* alive )
{
	if (alive->getCaptain() )
	{
		if (alive->getcallAliveNum()<1)
		{
			return true;
		}else{
			alive->setcallAliveNum(alive->getcallAliveNum() - 1);
		}
	}
	return false;
}

WarAlive* WarManager::getCallAlive(WarAlive* Father,CSkill* skill)
{
	WarAlive* alive = getAbsentCallAlive(Father);					
	if (alive)
		return alive;
	if (captainCallNumberJudge(Father))
		return nullptr;
	CEffect* effect = getSummonEffect(skill);
	if (!effect)
	{
		CCLOG("[ *ERROR ] WarManager::getCallAlive Skill Effect NULL");
		return nullptr;
	}
	for (auto i:m_CallRole)
	{
		if (i->CallID != effect->pTarget)
			continue;
		WarAlive* child = WarAlive::create();
		child->role = i;
		child->setCallType(i->CallType);
		child->setEnemy(Father->getEnemy());
		if (child->getEnemy())
		{
			child->setAliveID(m_members.size()+C_CallMonst);
			if (child->role->MstType == MST_HIDE)
				child->setcloaking(true);
			child->setDelaytime(i->delay);
			if (i->grid)
			{
				CallAliveByFixRange(Father,child);
			}else{
				int ran = CCRANDOM_0_1()*(Father->grids.size()-1);
				int grid = MoveRule::create()->getCurrRandomGrid(Father->grids.at(ran));	//得到当前武将格子的附近范围格子
				child->setGridIndex(grid);
			}
			child->setMstType(child->role->MstType);
			child->moves.push_back(i->move1);
			child->moves.push_back(i->move2);
			child->moves.push_back(i->move3);
			child->moves.push_back(i->move4);
			child->moves.push_back(i->move5);
		}else{
			child->setAliveID(m_members.size()+C_CallHero);
			child->setGridIndex(INVALID_GRID);
		}
		initCallAlive(child,Father);
		child->setFatherID(Father->getAliveID());
		return child;
	}
	CCLOG("[ *ERROR ]WarManager::getCallAlive  CallID != skill->pTarget =%d ",effect->pTarget);
	return nullptr;
}
//pAlive为释放技能的武将,召唤出来的武将继承释放技能对象的百分比属性
void WarManager::initCallAlive(WarAlive* alive,WarAlive*pAlive)
{
	alive->setModel(alive->role->thumb);
	alive->setZoom(alive->role->zoom*0.01f);
	alive->setInitCost(alive->role->initCost);									//召唤消耗的cost(根技能消耗的相等)
	if (pAlive->getAliveType() == AliveType::WorldBoss)
	{
		alive->setAtkInterval(alive->role->atkInterval);
		alive->setMoveSpeed(alive->role->MoveSpeed);
		alive->setMaxHp(alive->role->hp);
		alive->setHp(alive->role->hp);					//第一次进来是满血状态
		alive->setAddCost(alive->role->addCost);
		alive->setAtk(alive->role->atk);
		alive->setCrit(alive->role->crit);
		alive->setDef(alive->role->def);
		alive->setHit(alive->role->hit);
		alive->setRenew(alive->role->renew);
		alive->setDoge(alive->role->dodge);				//数值型是召唤它武将的百分比
	}else{
		alive->setAtkInterval((alive->role->atkInterval*0.01f)*pAlive->role->atkInterval);
		alive->setMoveSpeed((alive->role->MoveSpeed*0.01f)*pAlive->role->MoveSpeed);
		alive->setMaxHp((alive->role->hp*0.01f)*pAlive->role->hp);
		alive->setHp((alive->role->hp*0.01f)*pAlive->role->hp);							//第一次进来是满血状态		
		alive->setAddCost((alive->role->addCost*0.01f)*pAlive->role->addCost);
		alive->setAtk((alive->role->atk*0.01f)*pAlive->role->atk);
		alive->setCrit((alive->role->crit*0.01f)*pAlive->role->crit);
		alive->setDef((alive->role->def*0.01f)*pAlive->role->def);
		alive->setHit((alive->role->hit*0.01f)*pAlive->role->hit);
		alive->setRenew((alive->role->renew*0.01f)*pAlive->role->renew);
		alive->setDoge((alive->role->dodge*0.01f)*pAlive->role->dodge);				//数值型是召唤它武将的百分比
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (alive->getEnemy())
		{
			//alive->setMaxHp(5);
			//alive->setHp(5);		//第一次进来是满血状态
		}else{	
			//alive->setMaxHp(500000);
			//alive->setHp(500000);		//第一次进来是满血状态
			//alive->setAtk(10);
		}
#endif
	}
	addAlive(alive);
}
//每帧刷新一次武将信息
void WarManager::updateAlive()
{
	m_Monsters.clear();
	m_Heros.clear();
	for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
	{
		WarAlive* alive = iter->second;
		if (alive->getHp() <= 0||!alive->getBattle()||!alive->getActObject())
			continue;
		if (alive->getEnemy())
		{
			if (alive->getGridIndex()<C_MOSTERATKGRID)
				continue;
			m_Monsters.push_back(alive);
		}else{
			m_Heros.push_back(alive);
		}															
	}
}