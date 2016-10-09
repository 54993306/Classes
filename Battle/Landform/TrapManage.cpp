
#include "Battle/Landform/TrapManage.h"
#include "Battle/WarManager.h"
#include "Battle/BattleCenter.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
#include "Battle/TrapData.h"				//这样的耦合是最合适的
#include "Battle/Landform/BattleTrap.h"

namespace BattleSpace
{
	TrapManage::TrapManage(){}

	TrapManage::~TrapManage()
	{
		clear();
		CC_SAFE_RELEASE(mDefault);
		mDefault = nullptr;
		CC_SAFE_RELEASE(mTrapData);
		mTrapData = nullptr;
	}

	bool TrapManage::init()
	{
		mTrapData = TrapData::createTrap(nullptr);
		mTrapData->retain();
		mDefault = BattleTrap::createTrap(mTrapData);
		mDefault->retain();
		return true;
	}

	void TrapManage::clear()
	{
		for (auto tTrap : mTrapMap)
		{
			CC_SAFE_RELEASE(tTrap.second);
		}
		mTrapMap.clear();
	}

	void TrapManage::updateTrap(float dt)
	{
		for (auto tTrap : mTrapMap)
			tTrap.second->updateState(dt);
	}
	//把地形添加到容器中
	void TrapManage::AddTrap(BattleTrap* pTrap)
	{
		const BattleTrap* tTrap = getTrap(pTrap->getTrapID());
		if (tTrap->getTrapID())
		{
			CCLOG("[ *ERROR ] TrapManage::AddTrap has exists TerrainID:%d",pTrap->getTrapID());
			return;
		}
		pTrap->retain();
		mTrapMap[pTrap->getTrapID()] = pTrap;
	}
	//根据id得到地形
	const BattleTrap* TrapManage::getTrap(int pTrapID) const
	{
		map<int,BattleTrap*>::const_iterator iter = mTrapMap.find(pTrapID);
		if (iter != mTrapMap.end())
			return iter->second;
		CCLOG("[ *ERROR ] TrapManage::getTerrain %d",pTrapID);
		return mDefault;
	}
	//得到当前位置的地形
	const BattleTrap* TrapManage::getTrapByGrid(int pGrid) const
	{
		for (auto tPair : mTrapMap)
		{
			if (tPair.second->getGridIndex() == pGrid)
				return tPair.second;
		}
		CCLOG("[ *ERROR ] TrapManage::getTerrainByGrid %d",pGrid);
		return mDefault;
	}
	//添加服务器传过来的地形
	void TrapManage::initTrapData(const vector<TrapData*>& pTraps)
	{
		for (auto tTrap : pTraps)
		{
			BattleTrap* tBattleTrap = BattleTrap::createTrap(tTrap);
			AddTrap(tBattleTrap);
		}
	}

	void TrapManage::updateActivateTrap()
	{
		for (auto tTrap : mTrapMap)
		{
			tTrap.second->setTriggerRole(nullptr);
			tTrap.second->setActivate(false);
		}
		for (auto tTrap : mTrapMap)
		{
			for (auto tRole : BattleManage->inBattleRoles())
			{
				if (tRole->getCaptain())
					continue;
				if (tRole->inStandGrid(tTrap.second->getGridIndex()))
				{
					tTrap.second->setTriggerRole(tRole);
					tTrap.second->setActivate(true);
				}
			}
		}
	}

	const map<int,BattleTrap*>& TrapManage::getBattleTraps() const
	{
		return mTrapMap;
	}

	void TrapManage::initObstacle( BaseRole* pRole,vector<int>& pVector )
	{
		for (auto tPair : mTrapMap)
		{
			if (tPair.second->throughJudge(pRole))
				continue;
			pVector.push_back(tPair.second->getGridIndex());
		}
	}
}