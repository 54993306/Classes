/************************************************************* 
 *
 *
 *		Data : 2016.8.11
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/MapManager.h"
#include "Battle/Landform/TerrainManager.h"
#include "Battle/CHeroSoundData.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/CHeroSoundData.h"

namespace BattleSpace
{
	BattleCenter* BattleCenter::mManageCenter = nullptr;

	BattleCenter::ManageSingletonDestory BattleCenter::mDestory;

	BattleCenter::BattleCenter():mMapManage(nullptr),mWarManage(nullptr),mTerrainManage(nullptr),
	mGuideManage(nullptr),mRoleConfigData(nullptr)
	{

	}

	BattleCenter::~BattleCenter()
	{
		rleaseData();
	}

	BattleCenter* BattleCenter::shareManageCenter()
	{
		if (mManageCenter == nullptr)
			mManageCenter = new BattleCenter();
		return mManageCenter;
	}

	void BattleCenter::rleaseData()
	{
		CC_SAFE_RELEASE(mWarManage);
		mWarManage = nullptr;
		CC_SAFE_RELEASE(mMapManage);
		mMapManage = nullptr;
		CC_SAFE_RELEASE(mRoleConfigData);
		mRoleConfigData = nullptr;
		CC_SAFE_RELEASE(mTerrainManage);
		mTerrainManage = nullptr;
		CC_SAFE_RELEASE(mGuideManage);
		mGuideManage = nullptr;
	}

	WarManager* BattleCenter::getWar()
	{
		if (mWarManage == nullptr)
		{
			mWarManage = WarManager::create();
			mWarManage->retain();
		}
		return mWarManage;
	}

	TerrainManager* BattleCenter::getTer()
	{
		if (mTerrainManage == nullptr)
		{
			mTerrainManage = TerrainManager::create();
			mTerrainManage->retain();
		}
		return mTerrainManage;
	}

	MapManager* BattleCenter::getMap()
	{
		if (mMapManage == nullptr)
		{
			mMapManage = MapManager::create();
			mMapManage->retain();
		}
		return mMapManage;
	}

	CombatGuideManage* BattleCenter::getCombatGuideMg()
	{
		if (mGuideManage == nullptr)
		{
			mGuideManage = CombatGuideManage::create();
			mGuideManage->retain();
		}
		return mGuideManage;
	}
	//每个武将的解析数据应该是跟武将模型绑定在一起的
	CRroleData* BattleCenter::getRoleConfig()
	{
		if (mRoleConfigData == nullptr)
		{
			mRoleConfigData = CRroleData::create();
			mRoleConfigData->retain();
		}
		return mRoleConfigData;
	}

}