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
#include "Battle/BaseRole.h"
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
/**************************************/
#include "Battle/BattleDataCenter.h"
#include "Battle/RoleSkill.h"
#include "Battle/RoleMacro.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
#include "Battle/HeroData.h"
#include "Battle/skEffectData.h"
namespace BattleSpace{

};
namespace BattleSpace{
	WarManager::WarManager()
		:m_SceneTarpID(0),m_efdata(nullptr),m_terData(nullptr),m_armatureEventDataMgr(nullptr)
		,m_StageID(-1),m_Batch(-1),m_Logic(nullptr),m_iLastStageId(0),m_StageType(0),m_BuffData(nullptr)
		,m_bNormal(true),m_ChapterCount(0),m_ChapterIndex(0), m_BossModel(false)
		,m_VerifyNum(0),m_BossHurtCount(0),m_BossHurtPe(0),m_iWorldBossRank(0),m_StoryData(nullptr)
		,m_BattleOver(false),m_FirstStage(true)
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
		m_FirstStage = true;
		BattleData->releaseRoleData();
		m_BattleOver = true;
		m_BossHurtPe = 0;
		ReleaseSpineData();
		m_members.clear();
		m_SceneTarpID = 0;
		m_StageID = -1;
		m_Batch = -1;
		m_Logic = nullptr;
		m_VecBossHurt.clear();
		m_Heros.clear();
		m_Monsters.clear();
		m_AliveRoles.clear();
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
	void WarManager::setWorldBoss(bool model)
	{
		if (model)
		{
			m_BossHurtCount = 0;
			m_VerifyNum = 97231000;
		}else
			m_VerifyNum = 0;
		m_BossModel = model;
	}

	void WarManager::setVerifyNum(int num){ m_VerifyNum -= num; }

	void WarManager::setBossHurtCount(int hurt){ m_BossHurtCount += hurt;}

	void WarManager::addAlive(BaseRole* alive)
	{
		if(!alive) return;
		BaseRole* tpAlive = getAlive(alive->getAliveID());
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

	BaseRole* WarManager::getAlive(unsigned int aliveID)
	{
		Members::iterator iter = m_members.find(aliveID);
		if( iter != m_members.end() ) return iter->second;
		return nullptr;
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
		m_efdata = EffectData::create();									//创建这些对象都需要读取文件，最好是整个战斗过程中只创建一次
		m_efdata->retain();
		m_terData = terData::create();
		m_terData->retain();
		m_BuffData = BuffConfig::create();
		m_BuffData->retain();
		m_StoryData = StoryData::create();
		m_StoryData->retain();
		m_armatureEventDataMgr = ArmatureEventDataMgr::create();
		m_armatureEventDataMgr->retain();
		initHeroData();
		initMonsterByBatch(0);												//开始只创建第0批次怪物
		CaptainSkill* captain = CaptainSkill::create();
		captain->ExecuteCaptainSkill();
	}
	//添加我方武将
	void WarManager::initHeroData()
	{
		const vector<HeroData*>tVector = BattleData->getHeroVector();
		for (int tIndex = 1 ;tIndex<=tVector.size(); tIndex++)
		{
			BaseRole* alive = BaseRole::create();							//创建数据对象
			alive->setBaseData(tVector.at(tIndex-1));
			alive->setAliveID(tIndex);										//战场上武将的唯一id
			alive->setEnemy(false);
			alive->initAliveData();
			addAlive(alive);
		}
	}
	//初始化关卡批次数据到战斗武将列表
	void WarManager::initMonsterByBatch( int batch )
	{
		const vector<MonsterData*>tVector = BattleData->getMonsterVector();
		for (int tIndex=0 ;tIndex < tVector.size(); tIndex++)
		{
			if (tVector.at(tIndex)->getCallRole() ||
				tVector.at(tIndex)->getBatchNumber()!=batch)
				continue;
			BaseRole* alive = BaseRole::create();								//创建数据对象
			if (tVector.at(tIndex)->getBossMonster())							//大怪物提示
				if (m_BossModel)
					alive->setAliveType(E_ALIVETYPE::eWorldBoss);				//世界boss
				else
					alive->setAliveType(E_ALIVETYPE::eBoss);					//一般类型大boss
			alive->setBaseData(tVector.at(tIndex));
			alive->setAliveID(C_BatchMonst+tIndex+batch*100);
			alive->setEnemy(true);
			alive->setMstType(alive->getBaseData()->getMonsterType());
			if (alive->getBaseData()->getMonsterType() == MST_HIDE)
				alive->setCloaking(true);
			alive->initAliveData();
			alive->setMove(tVector.at(tIndex)->getMoveState());
			addAlive(alive);
		}
	}
	//冒泡排序
	void WarManager::sortArrayByGridIndex(CCArray* arr)
	{
		for (int i=0;i < arr->count()-1;i++)			//冒泡：判断相邻两个大小关系互换位置
		{
			for (int j =0;j<arr->count()-1-i;j++)
			{
				int front = ((BaseRole*)arr->objectAtIndex(j))->getGridIndex();
				int back  = ((BaseRole*)arr->objectAtIndex(j+1))->getGridIndex();
				if ( front > back)						//从小到大排序,把大的换到后面
					arr->exchangeObjectAtIndex(j,j+1);
			}
		}
	}

	CCArray* WarManager::getAlivesByCamp( bool enemy/*=true*/,bool isAlive/*=false*/,bool sort/*=true*/ )
	{
		if(m_members.empty()) return nullptr;
		CCArray* arr = CCArray::create();
		for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
		{
			if (iter->second->getEnemy() != enemy)
				continue;
			if (isAlive)
			{
				if (iter->second->getHp()>0)
					arr->addObject(iter->second);
			}else{
				arr->addObject(iter->second);
			}
		}
		if (sort)
			sortArrayByGridIndex(arr);
		return arr;
	}

	BaseRole* WarManager::getAliveByGrid(int grid)
	{
		if (grid == INVALID_GRID)
			return nullptr;
		for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
		{
			//先判断是否为boss如果为boss判断boss所站的区域是否包含该格子
			BaseRole* alive = iter->second;
			if (alive->getHp() <= 0)
				continue;
			if (grid >=  C_CAPTAINGRID)
			{
				if (alive->getCaptain())
					return alive;
				else
					continue;
			}
			for (int i=0; i < alive->mStandGrids.size();i++)
			{
				if (alive->mStandGrids.at(i) == grid)
					return alive;
			}
		}
		return nullptr;
	}

	BaseRole* WarManager::getAliveByType( E_ALIVETYPE type,bool Monster/* = true*/ )
	{
		for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
		{
			BaseRole* alive = iter->second;
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
			if(iter->second->getEnemy() && !iter->second->getDieState())
				return false;
		}
		return true;
	}

	ArmatureEventDataMgr* WarManager::getArmatureDataMgr() { 
		return m_armatureEventDataMgr; }
	const Members* WarManager::getMembers() { 
		return &m_members; }
	EffectData* WarManager::getEffData() { 
		return m_efdata; }
	BuffConfig* WarManager::getBuffData(){
		return m_BuffData;}
	StoryData* WarManager::getStoryData(){
		return m_StoryData;}

	BaseRole* WarManager::getAbsentCallAlive( BaseRole* fatherAlive )
	{
		Members::iterator iter = m_members.begin();
		for(; iter != m_members.end();++iter)			//判断是否有已创建但未上阵的召唤武将
		{
			BaseRole* alive = iter->second;
			if (alive->getHp()<=0 || alive->getEnemy() != fatherAlive->getEnemy())
				continue;
			if (alive->getBaseData()->getCallRole() && 
				!alive->getBattle() && 
				alive->getFatherID()==fatherAlive->getAliveID())
				return alive;
		}
		return nullptr;
	}

	BaseRole* WarManager::getNewCallAlive(BaseRole* Father,int CallId)
	{
		const vector<MonsterData*>tVector = BattleData->getCallRoleVector();
		for (MonsterData* tBaseData:tVector)
		{
			if (tBaseData->getCallID() != CallId)
				continue;
			BaseRole* child = BaseRole::create();
			child->setBaseData(tBaseData);
			child->setCallType(tBaseData->getCallType());
			child->setEnemy(Father->getEnemy());
			if (Father->getEnemy())
			{
				child->setAliveID(m_members.size()+C_CallMonst);
				if (child->getBaseData()->getMonsterType() == MST_HIDE)
					child->setCloaking(true);
				child->setDelaytime(tBaseData->getDelayTime());
				if (tBaseData->getInitGrid())
				{
					CallAliveByFixRange(Father,child);
				}else{
					int ran = CCRANDOM_0_1()*(Father->mStandGrids.size()-1);
					int grid = MoveRule::create()->getCurrRandomGrid(Father->mStandGrids.at(ran));	//得到当前武将格子的附近范围格子
					child->setGridIndex(grid);
				}
				child->setMstType(child->getBaseData()->getMonsterType());
				child->setMove(tBaseData->getMoveState());
			}else{
				child->setAliveID(m_members.size()+C_CallHero);
				child->setGridIndex(INVALID_GRID);
			}
			child->initAliveByFather(Father);
			addAlive(child);
			child->setFatherID(Father->getAliveID());
			return child;
		}
		CCLOG("[ *ERROR ]WarManager::getNewCallAlive  CallId =%d ",CallId);
		return nullptr;
	}

	BaseRole* WarManager::getCallAlive(BaseRole* Father,const RoleSkill* skill)
	{
		BaseRole* alive = getAbsentCallAlive(Father);					
		if (alive)
			return alive;
		if (Father->captainCallNumberJudge())
			return nullptr;
		const skEffectData* effect = skill->getSummonEffect();
		if (!effect)
		{
			CCLOG("[ *ERROR ] WarManager::getCallAlive Skill Effect NULL");
			return nullptr;
		}
		return getNewCallAlive(Father,effect->getTargetType());
	}
	//每帧刷新一次武将信息
	void WarManager::updateAlive()
	{
		m_Heros.clear();
		m_Monsters.clear();
		m_AliveRoles.clear();
		for(Members::iterator iter = m_members.begin(); iter != m_members.end();++iter)
		{
			BaseRole* tAlive = iter->second;
			if (tAlive->getHp() <= 0||!tAlive->getBattle()||!tAlive->getActObject())
				continue;
			if (tAlive->getEnemy())
			{
				if (tAlive->getGridIndex()<C_MOSTERATKGRID)
					continue;
				m_Monsters.push_back(tAlive);
			}else{
				m_Heros.push_back(tAlive);
			}	
			m_AliveRoles.push_back(tAlive);
		}
	}

	void WarManager::clearOldData()
	{
		srandNum();
		DataCenter::sharedData()->getWar()->BattleDataClear();
		DataCenter::sharedData()->getMap()->clearMap();
	}

	void WarManager::initCommonData()
	{
		initData();
		ReleaseSpineData();													//应该有更好的管理方法
		ParseMoveGrid(m_StageID,m_CantMoveGrid);
		ParseAddCostGrid(m_StageID,m_AddCostGrid);
		m_BattleOver = false;
		m_StoryData->initStoryData(m_StageID);

		CScene* scene = GETSCENE(LoadWar);
		LayerManager::instance()->closeAll();
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	vector<BaseRole*>* WarManager::getVecMonsters(bool pSort /*=false*/)
	{
		if (pSort)
			VectorRemoveRepeat(m_Monsters);
		return &m_Monsters;
	}

	vector<BaseRole*>* WarManager::getVecHeros(bool pSort /*=false*/)
	{
		if (pSort)
			VectorRemoveRepeat(m_Heros);
		return &m_Heros;
	}

	vector<BaseRole*>* WarManager::getAliveRoles( bool pSort /*= false*/ )
	{
		if (pSort)
			VectorRemoveRepeat(m_AliveRoles);
		return &m_AliveRoles;
	}

	void WarManager::initRoleSkillInfo( int pEffectID,BaseRole* pRole )
	{
		EffectInfo* tInfo = m_efdata->getEffectInfo(pEffectID);
		if (tInfo)
		{
			pRole->SkillActionAndEffect(tInfo->getActionID(),tInfo->getusEft());
		}else{
			CCLOG("[ *ERROR ] WarManager::initRoleSkillInfo");
			pRole->SkillActionAndEffect(Stand_Index,0);
		}
	}

	bool WarManager::inAddCostArea( int pGrid )
	{
		if (std::find(m_AddCostGrid.begin(),m_AddCostGrid.end(),pGrid) != m_AddCostGrid.end())
		{
			return true;
		}else{
			return false;
		}
	}
}