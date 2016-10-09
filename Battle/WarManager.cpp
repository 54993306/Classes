#include "Battle/WarManager.h"
#include "Battle/BattleCenter.h"
#include "common/CommonFunction.h"
#include "Battle/BuffManage.h"
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
#include "Battle/Landform/TrapManage.h"
#include "Battle/MoveRule.h"
#include "Battle/CoordsManage.h"
#include "Battle/ComBatLogic.h"
#include "Battle/ArmatureEventDataMgr.h"
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
#include "Battle/Config/ConfigManage.h"

namespace BattleSpace
{
	WarManager::WarManager()
		:mMaxCost(0),mCurrBatch(0),mCaptainSkill(nullptr),mBattleOver(false),mFirstStage(true)
		,mStageID(-1),mBatch(0),m_iLastStageId(0),m_StageType(0),mLogicState(false),m_iStageTimeCountDown(0)
		,m_bNormal(true),m_ChapterCount(0),m_ChapterIndex(0), m_BossModel(false),mCostSpeed(0)
		,m_VerifyNum(0),m_BossHurtCount(0),mBossHurtPe(0),m_iWorldBossRank(0),mCurrCost(0)
	{}

	void WarManager::BattleDataClear()
	{
		for (auto tRole : mBattleRole)
		{
			CC_SAFE_RELEASE(tRole.second);
		}
		setWorldBoss(false);
		setWorldBossRank(0);
		mBattleRole.clear();
		mBatch = 0;
		mCurrBatch = 0;
		mBattleHeros.clear();
		mBattleMonsters.clear();
		mFirstStage = true;
		mBattleOver = true;
		mBossHurtPe = 0;
		mCurrCost = 0;
		mCostSpeed = 0;
		mMaxCost = 0;
		mLogicState = false;
		m_VecBossHurt.clear();
		mBattleRoles.clear();
		BattleData->releaseRoleData();				//关于配置文件的部分也应该单独管理,做一个独立的管理器
		CC_SAFE_RELEASE(mCaptainSkill);
		mCaptainSkill = nullptr;
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
			BaseRole* tRole = getRole( pRole->getAliveID() );
			if( tRole )
			{
				CCLOG("[ **ERROR ]WarManager::addAlive has exists AliveModel:%d",pRole->getModel());
				return;
			}
		}
		pRole->retain();
		BattleConfig->initRoleConfigData(pRole);
		mBattleRole[pRole->getAliveID()] = pRole;
	}

	BaseRole* WarManager::getRole(unsigned int pRoleID)
	{
		RolesMap::iterator iter = mBattleRole.find(pRoleID);
		if( iter != mBattleRole.end() ) return iter->second;
		return nullptr;
	}

	void WarManager::executeCaptainSkill( bool mOther /*= false*/ )
	{
		mCaptainSkill->ExecuteSkill(mOther);
	}

	void WarManager::initData()
	{
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
			if (BattleModelManage->isPvEBattle())
			{
				if (!tRole->getCaptain())
					mCurrCost += tRole->getBaseData()->getExpendCost();
			}else{
				if (tRole->getCaptain())
					mCurrCost = tRole->getInitCost();
			}
			addBattleRole(tRole);
		}

		if (BattleModelManage->isPvEBattle())
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

	vector<BaseRole*>& WarManager::inBattleMonsters(bool pSort /*=false*/)
	{
		if (pSort)
			VectorUnique(mBattleMonsters);
		return mBattleMonsters;
	}

	vector<BaseRole*>& WarManager::inBattleHeros(bool pSort /*=false*/)
	{
		if (pSort)
			VectorUnique(mBattleHeros);
		return mBattleHeros;
	}

	vector<BaseRole*>& WarManager::inBattleRoles( bool pSort /*= false*/ )
	{
		if (pSort)
			VectorUnique(mBattleRoles);
		return mBattleRoles;
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

	const RolesMap* WarManager::getRolesMap() const
	{ 
		return &mBattleRole; 
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
			if (tAlive->getCallType() == sCallType::eBoxHaveRole)
			{
				mBattleMonsters.push_back(tAlive);
				mBattleHeros.push_back(tAlive);
				mBattleRoles.push_back(tAlive);
				continue;
			}
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
		BattleCoords->clearCoords();
	}

	void WarManager::initCommonData()
	{
		initData();
		mBattleOver = false;
		CScene* scene = GETSCENE(LoadBattleResource);						//接收数据完成，跳转Load界面
		LayerManager::instance()->closeAll();
		CSceneManager::sharedSceneManager()->replaceScene(scene);
	}

	void WarManager::costUpdate( float delta )
	{
		if (BattleModelManage->isPvEBattle())
		{
			mCostSpeed += delta * 5;
		}else{
			mCostSpeed += delta;
		}
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