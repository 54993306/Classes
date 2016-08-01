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
#include "Battle/BattleModel.h"
#include "Battle/RoleSkill.h"
#include "Battle/RoleMacro.h"
#include "Battle/BaseRoleData.h"
#include "Battle/MonsterData.h"
#include "Battle/HeroData.h"
#include "Battle/skEffectData.h"

namespace BattleSpace
{
	WarManager::WarManager()
	:m_efdata(nullptr),m_armatureEventDataMgr(nullptr),mMaxCost(0),mCurrBatch(0)
	,mStageID(-1),mBatch(0),m_iLastStageId(0),m_StageType(0),mLogicState(false)
	,m_bNormal(true),m_ChapterCount(0),m_ChapterIndex(0), m_BossModel(false),mCostSpeed(0)
	,m_VerifyNum(0),m_BossHurtCount(0),mBossHurtPe(0),m_iWorldBossRank(0),mCurrCost(0)
	,mBattleOver(false),mFirstStage(true),mStoryData(nullptr),mBuffData(nullptr)
	,mCaptainSkill(nullptr)
	{
		parseSpineModelFile(m_SpineID);
	}

	void WarManager::BattleDataClear()
	{
		for (auto tRole : mBattleRole)
		{
			CC_SAFE_RELEASE(tRole.second);
		}
		mBattleRole.clear();
		mBatch = 0;
		mCurrBatch = 0;
		//mStageID = -1;
		mBattleHeros.clear();
		mBattleMonsters.clear();
		mFirstStage = true;
		mBattleOver = true;
		mBossHurtPe = 0;
		mCurrCost = 0;
		mCostSpeed = 0;
		mMaxCost = 0;
		mLogicState = false;
		ReleaseSpineData();
		m_VecBossHurt.clear();
		mBattleRoles.clear();
		mMoveArea.clear();
		mCostArea.clear();
		mEnterArea.clear();
		mOtherEnter.clear();
		mUndefinedArea.clear();
		BattleData->releaseRoleData();
		CC_SAFE_RELEASE(m_efdata);
		m_efdata = nullptr;
		CC_SAFE_RELEASE(mBuffData);
		mBuffData = nullptr;
		CC_SAFE_RELEASE(mStoryData);
		mStoryData = nullptr;
		CC_SAFE_RELEASE(m_armatureEventDataMgr);
		m_armatureEventDataMgr = nullptr;
		CC_SAFE_RELEASE(mCaptainSkill);
		mCaptainSkill = nullptr;
	}

	void WarManager::ReleaseSpineData()
	{
		for (auto tPair:mSpineData)
		{
			spSkeletonData_dispose(tPair.second.first);
			spAtlas_dispose(tPair.second.second);
			CCLOG("LoadSpineData::ReleaseData Release %s",tPair.first.c_str());
		}
		mSpineData.clear();
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
		RolesMap::iterator iter = mBattleRole.find(aliveID);
		if( iter != mBattleRole.end() ) return iter->second;
		return nullptr;
	}

	SpData* WarManager::getSpineData( std::string Name )
	{
		MapSkeletonData::iterator iter = mSpineData.find(Name);
		if (iter != mSpineData.end())
			return &iter->second;
		return nullptr;
	}

	void WarManager::executeCaptainSkill( bool mOther /*= false*/ )
	{
		mCaptainSkill->ExecuteSkill(mOther);
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
		mCaptainSkill = CaptainSkill::create();
		mCaptainSkill->retain();
		initHeroData();
		initMonsterByBatch(0);												//开始只创建第0批次怪物
		executeCaptainSkill();
	}
	//添加我方武将
	void WarManager::initHeroData()
	{
		const vector<HeroData*>tVector = BattleData->getHeroVector();
		for (int tIndex = 0 ;tIndex<tVector.size(); tIndex++)
		{
			BaseRole* tRole = BaseRole::create();							//创建数据对象
			tRole->setBaseData(tVector.at(tIndex));
			tRole->setAliveID(tIndex+1);										//战场上武将的唯一id
			tRole->setEnemy(false);
			tRole->initAliveData();
			mMaxCost += tRole->getCostMax();
			if (tRole->getCaptain())
				mCurrCost = tRole->getInitCost();
			addBattleRole(tRole);
		}
		
		if (BattleData->getBattleModel()->isPvEBattle())
		{
			const vector<HeroData*>tVector = BattleData->getPvPHeros();
			for (int tIndex = 0 ;tIndex<tVector.size(); tIndex++)
			{
				BaseRole* tRole = BaseRole::create();							//创建数据对象
				tRole->setBaseData(tVector.at(tIndex));
				tRole->setAliveID(tIndex+1+C_PVEStage);							//战场上武将的唯一id
				tRole->setEnemy(false);
				tRole->initAliveData();
				addBattleRole(tRole);
			}
			executeCaptainSkill(true);
		}
	}
	//初始化关卡批次数据到战斗武将列表
	void WarManager::initMonsterByBatch( int batch )
	{
		const vector<MonsterData*>tVector = BattleData->getMonsterVector();
		for (int tIndex=0 ;tIndex < tVector.size(); tIndex++)
		{
			if (tVector.at(tIndex)->getBatchNumber()!=batch)
				continue;
			BaseRole* tRole = BaseRole::create();								//创建数据对象
			if (tVector.at(tIndex)->getBossMonster())							//大怪物提示
				if (m_BossModel)
					tRole->setMonsterSpecies(sMonsterSpecies::eWorldBoss);				//世界boss
				else
					tRole->setMonsterSpecies(sMonsterSpecies::eBoss);					//一般类型大boss
			tRole->setBaseData(tVector.at(tIndex));
			tRole->setAliveID(C_BatchMonst+tIndex+batch*100);
			tRole->setEnemy(true);
			tRole->setBehavior(tRole->getBaseData()->getBehavior());
			if (tRole->getBaseData()->getBehavior() == sBehavior::eHide)
				tRole->setCloaking(true);
			tRole->initAliveData();
			tRole->setMove(tVector.at(tIndex)->getMoveState());
			addBattleRole(tRole);
		}
	}

	CCArray* WarManager::getHeros( bool pIsAlive/*= false*/ )
	{
		return getRoles(false,pIsAlive);
	}

	CCArray* WarManager::getMonsters( bool pIsAlive/*= false*/ )
	{
		return getRoles(true,pIsAlive);
	}

	CCArray* WarManager::getRoles( bool pEnemy,bool isAlive/*=false*/)
	{
		CCArray* tRoles = CCArray::create();
		if(mBattleRole.empty()) tRoles;
		for (auto tPair : mBattleRole)
		{
			BaseRole* tRole = tPair.second;
			if ((pEnemy && !tRole->getEnemy()) || (!pEnemy && tRole->getOtherCamp()))
				continue;
			if (isAlive)
			{
				if (tRole->getAliveState())
					tRoles->addObject(tRole);
			}else{
				tRoles->addObject(tRole);
			}
		}
		sortRole(tRoles);
		return tRoles;
	}
	//冒泡排序
	void WarManager::sortRole(CCArray* pRoles)
	{
		if (!pRoles->count())
			return;
		for (int i=0;i < pRoles->count()-1;i++)			//冒泡：判断相邻两个大小关系互换位置
		{
			for (int j =0;j<pRoles->count()-i-1;j++)
			{
				int front = ((BaseRole*)pRoles->objectAtIndex(j))->getGridIndex();
				int back  = ((BaseRole*)pRoles->objectAtIndex(j+1))->getGridIndex();
				if ( front > back)						//从小到大排序,把大的换到后面
					pRoles->exchangeObjectAtIndex(j,j+1);
			}
		}
	}

	void WarManager::initHeros( vector<BaseRole*>& pHeros,bool pAlive /*= false*/ )
	{
		initRoles(pHeros,pAlive,false);
	}

	void WarManager::initMonsters( vector<BaseRole*>& pMonsters,bool pAlive /*= false*/ )
	{
		initRoles(pMonsters,pAlive,true);
	}

	void WarManager::initOtherCamp( vector<BaseRole*>& pOthers,bool pAlive /*= false*/ )
	{
		for (auto tPair : mBattleRole)
		{
			BaseRole* tRole = tPair.second;
			if ( !tRole->getOtherCamp() || tRole->getFatherID())
				continue;
			if (pAlive)
			{
				if ( !tRole->getAliveState())
					continue;
				pOthers.push_back(tRole);
			}else{
				pOthers.push_back(tRole);
			}
		}
		VectorUnique(pOthers);
	}

	//此方法频繁使用会否影响系统效率？
	void WarManager::initRoles( vector<BaseRole*>& pRoles,bool pAlive ,bool pEnemy )
	{
		for (auto tPair : mBattleRole)
		{
			BaseRole* tRole = tPair.second;
			if ( tRole->getEnemy() != pEnemy || tRole->getFatherID())
				continue;
			if (pAlive)
			{
				if ( !tRole->getAliveState())
					continue;
				pRoles.push_back(tRole);
			}else{
				pRoles.push_back(tRole);
			}
		}
		VectorUnique(pRoles);
	}

	vector<BaseRole*>* WarManager::inBattleMonsters(bool pSort /*=false*/)
	{
		if (pSort)
			VectorUnique(mBattleMonsters);
		return &mBattleMonsters;
	}

	vector<BaseRole*>* WarManager::inBattleHeros(bool pSort /*=false*/)
	{
		if (pSort)
			VectorUnique(mBattleHeros);
		return &mBattleHeros;
	}

	vector<BaseRole*>* WarManager::inBattleRoles( bool pSort /*= false*/ )
	{
		if (pSort)
			VectorUnique(mBattleRoles);
		return &mBattleRoles;
	}

	BaseRole* WarManager::getAliveByGrid(int pGrid)
	{
		if (pGrid == INVALID_GRID)
			return nullptr;
		for (auto tPair : mBattleRole)
		{
			BaseRole* tRole = tPair.second;
			if ( !tRole->getAliveState())
				continue;
			if (tRole->inStandGrid(pGrid))
				return tRole;
		}
		return nullptr;
	}

	BaseRole* WarManager::getAliveByType( sMonsterSpecies type,bool Monster/* = true*/ )
	{
		for (auto tPair : mBattleRole)
		{
			BaseRole* tRole = tPair.second;
			if (!tRole->getAliveState() || tRole->getEnemy() != Monster)
				continue;
			if (tRole->getMonsterSpecies() == type)
				return tRole;
		}
		return nullptr;
	}

	bool WarManager::checkBatchOver()
	{
		for (auto tPair : mBattleRole)
		{
			if (tPair.second->getOtherCamp() && tPair.second->getAliveState())
				return false;
		}
		return true;
	}

	ArmatureEventDataMgr* WarManager::getArmatureDataMgr() 
	{ 
		return m_armatureEventDataMgr; 
	}
	const RolesMap* WarManager::getRolesMap() 
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
	//每帧刷新一次武将信息,武将上下阵，死亡时，进入战斗区域时，有效角色数组需要刷新
	void WarManager::updateAlive()
	{
		mBattleHeros.clear();
		mBattleMonsters.clear();
		mBattleRoles.clear();
		RolesMap::iterator iter = mBattleRole.begin();
		for(; iter != mBattleRole.end();++iter)
		{
			BaseRole* tAlive = iter->second;
			if (!tAlive->getBattle()		||
				!tAlive->getRoleObject())
				continue;
			if (tAlive->getOtherCamp())
			{
				if (tAlive->getGridIndex()<C_MOSTERATKGRID)
					continue;
				mBattleMonsters.push_back(tAlive);
			}else{
				mBattleHeros.push_back(tAlive);
			}	
			mBattleRoles.push_back(tAlive);
		}
	}

	void WarManager::clearOldData()
	{
		srandNum();
		BattleDataClear();
		DataCenter::sharedData()->getMap()->clearMap();
	}

	void WarManager::initCommonData()
	{
		initData();
		ReleaseSpineData();													//应该有更好的管理方法
		ParseBattleGrid(mStageID,mMoveArea,mUndefinedArea,mEnterArea,mOtherEnter);		//解析的数据使用单一的对象进行管理(重构方向)
		ParseAddCostGrid(mStageID,mCostArea);
		mBattleOver = false;
		mStoryData->initStoryData(mStageID);

		CScene* scene = GETSCENE(LoadBattleResource);						//接收数据完成，跳转Load界面
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
				CCArray* arr = getHeros(true);
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

	void WarManager::initRoleSkillInfo( int pEffectID,BaseRole* pRole )
	{
		EffectInfo* tInfo = m_efdata->getEffectInfo(pEffectID);
		if (tInfo)
		{
			pRole->SkillActionAndEffect(tInfo->getActionID(),tInfo->getusEft());
		}else{
			pRole->SkillActionAndEffect(1,0);
			CCLOG("[ *ERROR ] WarManager::initRoleSkillInfo %d",pEffectID);
		}
	}

	bool WarManager::isSpine( int pModeID )
	{
		vector<int>::iterator tSpine = m_SpineID.end();
		if (std::find(m_SpineID.begin(),tSpine,pModeID) != tSpine)
			return true;
		return false;
	}

	bool WarManager::inAddCostArea( int pGrid )
	{
		if (pGrid == INVALID_GRID)
			return true;
		vector<int>::iterator tEnditer = mCostArea.end();
		if (std::find(mCostArea.begin(),tEnditer,pGrid) != tEnditer)
			return true;
		return false;
	}

	bool WarManager::inMoveArea( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnditer = mMoveArea.end();
		if (std::find(mMoveArea.begin(),tEnditer,pGrid) != tEnditer)
			return true;						//点不在可移动范围内
		return false;
	}
	//召唤区域和移动区域的关系问题，可以召唤的过去，但是不能再更换位置，或是出来后不能再回去
	bool WarManager::inEnterArea( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnterIter = mEnterArea.end();
		if (std::find(mEnterArea.begin(),tEnterIter,pGrid) != tEnterIter)
			return true;
		return false;
	}

	bool WarManager::inOtherEnter( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnterIter = mOtherEnter.end();
		if (std::find(mOtherEnter.begin(),tEnterIter,pGrid) != tEnterIter)
			return true;
		return false;
	}

	bool WarManager::inUnDefineArea( int pGrid )
	{
		vector<int>::iterator tUnDefine = mUndefinedArea.end();
		if (std::find(mUndefinedArea.begin(),tUnDefine,pGrid) != tUnDefine)
			return true;						//点在未定义区域内
		return false;
	}

	void WarManager::costUpdate( float delta )
	{
		mCostSpeed += delta;
		if ( mCurrCost < mMaxCost )	//放必杀技时cost不发生改变
		{
#if BATTLE_TEST
			mCurrCost += mCostSpeed * 100;				//系统一直增加cost
#else
			mCurrCost += mCostSpeed; 
#endif	
		}
		if ( mCurrCost <= 0 ) //武将移动回我方半屏
		{
			mCurrCost = 0;
		}else if(mCurrCost >= mMaxCost)
		{
			mCurrCost = mMaxCost;
		}
	}

	void WarManager::changeCost( float pCost )
	{
		mCurrCost += pCost; 
		costUpdate(0);
	}

	bool WarManager::lastBatch()
	{
		if (getCurrBatch() < getBatch())
			return false;
		return true;
	}

	void WarManager::saveWordBossHurt()
	{
		m_VecBossHurt.push_back(m_BossHurtCount);
	}

}