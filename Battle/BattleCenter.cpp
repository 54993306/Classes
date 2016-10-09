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
#include "Battle/CoordsManage.h"
#include "Battle/Landform/TrapManage.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/SpineDataManage.h"
#include "Battle/Config/ConfigManage.h"
#include "Battle/Landform/AreaManage.h"
#include "Battle/BattleModel.h"

namespace BattleSpace
{
	BattleCenter* BattleCenter::mManageCenter = nullptr;

	BattleCenter::ManageSingletonDestory BattleCenter::mDestory;

	BattleCenter::BattleCenter():mMapManage(nullptr),mWarManage(nullptr),mTrapManage(nullptr),
	mGuideManage(nullptr),mSpineDataManage(nullptr),mConfigManage(nullptr),
	mAreaManage(nullptr),mBattleModel(nullptr)
	{}

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
		CC_SAFE_RELEASE(mTrapManage);
		mTrapManage = nullptr;
		CC_SAFE_RELEASE(mGuideManage);
		mGuideManage = nullptr;
		CC_SAFE_RELEASE(mSpineDataManage);
		mSpineDataManage = nullptr;
		CC_SAFE_RELEASE(mConfigManage);
		mConfigManage = nullptr;
		CC_SAFE_RELEASE(mBattleModel);
		mBattleModel = nullptr;
	}

	void BattleCenter::clearBattleData()
	{
		if (mGuideManage)
			mGuideManage->clearGuideData(true);
		if (mWarManage)
			mWarManage->BattleDataClear();
		if (mSpineDataManage)
			mSpineDataManage->ReleaseSpineData();
		if (mTrapManage)
			mTrapManage->clear();
		if (mMapManage)
			mMapManage->clearCoords();
		if (mConfigManage)
			mConfigManage->clearBattleConfig();
		if (mBattleModel)
			mBattleModel->clearModelData();
	}

	WarManager* BattleCenter::getWarManage()
	{
		if (mWarManage == nullptr)
		{
			mWarManage = WarManager::create();
			mWarManage->retain();
		}
		return mWarManage;
	}

	TrapManage* BattleCenter::getTrapManage()
	{
		if (mTrapManage == nullptr)
		{
			mTrapManage = TrapManage::create();
			mTrapManage->retain();
		}
		return mTrapManage;
	}
	//角色数据管理中心更合适
	CoordsManage* BattleCenter::getCoordsManage()
	{
		if (mMapManage == nullptr)
		{
			mMapManage = CoordsManage::create();
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

	SpineDataManage* BattleCenter::getSpineManage()
	{
		if (mSpineDataManage == nullptr)
		{
			mSpineDataManage = SpineDataManage::create();
			mSpineDataManage->retain();
		}
		return mSpineDataManage;
	}

	ConfigManage* BattleCenter::getConfigManage()
	{
		if (mConfigManage == nullptr)
		{
			mConfigManage = ConfigManage::create();
			mConfigManage->retain();
		}
		return mConfigManage;
	}

	AreaManage* BattleCenter::getAreaManage()
	{
		if (mAreaManage == nullptr)
		{
			mAreaManage = AreaManage::create();
			mAreaManage->retain();
		}
		return mAreaManage;
	}

	BattleModel* BattleCenter::getBattleModel()
	{
		if (mBattleModel == nullptr)
		{
			mBattleModel = BattleModel::create();
			mBattleModel->retain();
		}
		return mBattleModel;
	}

}