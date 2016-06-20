#include "WarManager.h"
#include "AppUI.h"
#include "Global.h"
#include "DataCenter.h"
#include "common/CommonFunction.h"
#include "model/BuffManage.h"
#include "Battle/HurtCount.h"
#include "Battle/SkillRange.h"
#include "Battle/ConstNum.h"
#include "tools/commonDef.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include <functional>					//sort 的降序排序使用
#include "Battle/BaseRole.h"
#include "Battle/EffectData.h"
#include "tollgate/Chapter.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/CaptainSkill.h"
#include "TerrainManager.h"
#include "Battle/MoveRule.h"
#include "model/MapManager.h"
#include "Battle/ComBatLogic.h"
#include "Battle/ArmatureEventDataMgr.h"
#include "Battle/ParseFileData.h"
#include "Battle/BattleTools.h"
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
		:m_SceneTarpID(0),m_efdata(nullptr),m_armatureEventDataMgr(nullptr)
		,m_StageID(-1),m_Batch(-1),m_Logic(nullptr),m_iLastStageId(0),m_StageType(0)
		,m_bNormal(true),m_ChapterCount(0),m_ChapterIndex(0), m_BossModel(false)
		,m_VerifyNum(0),m_BossHurtCount(0),m_BossHurtPe(0),m_iWorldBossRank(0)
		,mBattleOver(false),mFirstStage(true),mStoryData(nullptr),mBuffData(nullptr)
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

	void WarManager::BattleDataClear()
	{
		for (auto tRole : mBattleRole)
		{
			CC_SAFE_RELEASE(tRole.second);
		}
		mBattleRole.clear();
		m_Batch = -1;
		m_StageID = -1;
		m_Heros.clear();
		m_Logic = nullptr;
		m_Monsters.clear();
		mFirstStage = true;
		mBattleOver = true;
		m_BossHurtPe = 0;
		ReleaseSpineData();
		m_SceneTarpID = 0;
		m_VecBossHurt.clear();
		m_AliveRoles.clear();
		m_CantMoveGrid.clear();
		m_AddCostGrid.clear();
		BattleData->releaseRoleData();
		CC_SAFE_RELEASE(m_efdata);
		m_efdata = nullptr;
		CC_SAFE_RELEASE(mBuffData);
		mBuffData = nullptr;
		CC_SAFE_RELEASE(mStoryData);
		mStoryData = nullptr;
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

	void WarManager::addBattleRole(BaseRole* pRole)
	{
		if (pRole->getAliveID() >= C_CallHero)
		{
			pRole->setAliveID(pRole->getAliveID()+mBattleRole.size());
		}else{
			BaseRole* tRole = getAlive( pRole->getAliveID() );
			if( tRole )
			{
				CCLOG("[ **ERROR ]WarManager::addAlive has exists AliveModel:%d",pRole->getModel());
				return;
			}
		}
		pRole->retain();
		mBattleRole[pRole->getAliveID()] = pRole;
	}

	BaseRole* WarManager::getAlive(unsigned int aliveID)
	{
		Members::iterator iter = mBattleRole.find(aliveID);
		if( iter != mBattleRole.end() ) return iter->second;
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
		mBuffData = BuffConfig::create();
		mBuffData->retain();
		mStoryData = StoryData::create();
		mStoryData->retain();
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
		const vector<BaseRoleData*>tVector = BattleData->getHeroVector();
		for (int tIndex=0 ;tIndex<tVector.size(); tIndex++)
		{
			BaseRole* alive = BaseRole::create(tVector.at(tIndex));			//创建数据对象
			alive->setAliveID(tIndex+1);										//战场上武将的唯一id
			alive->setEnemy(false);
			alive->initAliveData();
			addBattleRole(alive);
		}
	}
	//初始化关卡批次数据到战斗武将列表
	void WarManager::initMonsterByBatch( int pBatch )
	{
		const vector<BaseRoleData*>tVector = BattleData->getMonsterVector();
		for (int tIndex=0 ;tIndex < tVector.size(); tIndex++)
		{
			if (((MonsterData*)tVector.at(tIndex))->currBatchRole(pBatch))
			{
				BaseRole* alive = BaseRole::create(tVector.at(tIndex));				//创建数据对象
				if (tVector.at(tIndex)->getBossMonster())							//大怪物提示
					if (m_BossModel)
						alive->setAliveType(sMonsterSpecies::eWorldBoss);				//世界boss
					else
						alive->setAliveType(sMonsterSpecies::eBoss);						//一般类型大boss
				alive->setAliveID(C_BatchMonst+tIndex+pBatch*100);
				alive->setEnemy(true);
				alive->setMstType(alive->getBaseData()->getBehavior());
				if (alive->getBaseData()->getBehavior() == sBehavior::eHide)
					alive->setCloaking(true);
				alive->initAliveData();
				alive->setMove(tVector.at(tIndex)->getMoveState());
				addBattleRole(alive);
			}
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
		if(mBattleRole.empty()) return nullptr;
		CCArray* arr = CCArray::create();
		for(Members::iterator iter = mBattleRole.begin(); iter != mBattleRole.end();++iter)
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
		for(Members::iterator iter = mBattleRole.begin(); iter != mBattleRole.end();++iter)
		{
			BaseRole* alive = iter->second;
			if (alive->getHp() <= 0)
				continue;
			for (int i=0; i < alive->mStandGrids.size();i++)
			{
				if (alive->mStandGrids.at(i) == grid)
					return alive;
			}
			if (grid >=  C_CAPTAINGRID && alive->getCaptain())
				return alive;
		}
		return nullptr;
	}

	BaseRole* WarManager::getAliveByType( sMonsterSpecies type,bool Monster/* = true*/ )
	{
		for(Members::iterator iter = mBattleRole.begin(); iter != mBattleRole.end();++iter)
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
		for(Members::iterator iter = mBattleRole.begin(); iter != mBattleRole.end();++iter)
		{
			if(iter->second->getEnemy() && !iter->second->getDieState())
				return false;
		}
		return true;
	}

	ArmatureEventDataMgr* WarManager::getArmatureDataMgr() 
	{ 
		return m_armatureEventDataMgr; 
	}
	const Members* WarManager::getMembers() 
	{ 
		return &mBattleRole; 
	}
	EffectData* WarManager::getEffData() 
	{ 
		return m_efdata; 
	}
	BuffConfig* WarManager::getBuffData()
	{
		return mBuffData;
	}
	StoryData* WarManager::getStoryData()
	{
		return mStoryData;
	}
	//每帧刷新一次武将信息
	void WarManager::updateAlive()
	{
		m_Heros.clear();
		m_Monsters.clear();
		m_AliveRoles.clear();
		for(Members::iterator iter = mBattleRole.begin(); iter != mBattleRole.end();++iter)
		{
			BaseRole* tAlive = iter->second;
			if (tAlive->getHp() <= 0||!tAlive->getBattle()||!tAlive->getRoleObject())
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
		mBattleOver = false;
		mStoryData->initStoryData(m_StageID);

		CScene* scene = GETSCENE(LoadBattleResource);
		LayerManager::instance()->closeAll();
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	int WarManager::getCurrRandomGrid( int grid,bool hasAlive /*= false*/ )
	{
		int row = grid % C_GRID_ROW;
		int col = grid / C_GRID_ROW;

		CCArray* arr = CCArray::create();
		if ((row+1)<C_GRID_ROW)
			arr->addObject(CCInteger::create(col * C_GRID_ROW + row+1));
		if ((row-1)>=0)
			arr->addObject(CCInteger::create(col * C_GRID_ROW + row-1));
		if ((col+1)<C_GRID_COL)
		{
			arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row));
			if ((row+1)<C_GRID_ROW)
				arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row+1));
			if ((row-1)>=0)
				arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row-1));
		}	
		if ((col-1)>=0)
		{
			arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row));
			if ((row+1)<C_GRID_ROW)
				arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row+1));
			if ((row-1)>=0)
				arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row-1));
		}
		int pGrid = grid;
		do{
			pGrid = ((CCInteger*)arr->randomObject())->getValue();
			if (pGrid >= C_CAPTAINGRID)
				continue;
			if ( hasAlive && !getAliveByGrid(pGrid))
			{
				CCArray* arr = getAlivesByCamp(false,true);
				CCObject* obj = nullptr;
				bool pMoveGrid = false;
				CCARRAY_FOREACH(arr,obj)
				{
					BaseRole* alive = (BaseRole*)obj;
					if (alive->getMoveGrid() == pGrid)								//且无人将要过去
						pMoveGrid = true;
				}
				if (pMoveGrid)
					continue;
			}
			return pGrid;
		} while (true);
		return pGrid;
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
			pRole->SkillActionAndEffect(1,0);
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

	bool WarManager::inMoveArea( int pGrid )
	{
		if (std::find(m_CantMoveGrid.begin(),m_CantMoveGrid.end(),pGrid) != m_CantMoveGrid.end())
			return true;						//点不在可移动范围内
		return false;
	}

}