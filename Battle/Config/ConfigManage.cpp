/************************************************************* 
 *
 *
 *		Data : 2016.8.17
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/Config/ConfigManage.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/ParseFileData.h"
#include "Battle/ConstNum.h"
#include "Battle/EffectData.h"
#include "Battle/BaseRole.h"
#include "Battle/RoleConfig.h"
#include "Battle/ArmatureEventDataMgr.h"

namespace BattleSpace
{
	ConfigManage::ConfigManage():mEffectData(nullptr),mBuffData(nullptr),mStoryData(nullptr),mModelEventData(nullptr)
		,mRoleConfig(nullptr)
	{}

	ConfigManage::~ConfigManage()
	{
		CC_SAFE_RELEASE(mEffectData);
		mEffectData = nullptr;
		CC_SAFE_RELEASE(mStoryData);
		mStoryData = nullptr;
		CC_SAFE_RELEASE(mBuffData);
		mBuffData = nullptr;
		CC_SAFE_RELEASE(mModelEventData);
		mModelEventData = nullptr;
		CC_SAFE_RELEASE(mRoleConfig);
		mRoleConfig = nullptr;
	}

	bool ConfigManage::init()
	{
		//������Ϸȫ������Ҫ�����ݰ���spineID�ļ�
		mEffectData = EffectData::create();
		mEffectData->retain();
		mStoryData = StoryData::create();
		mStoryData->retain();
		mBuffData = BuffConfig::create();
		mBuffData->retain();
		mModelEventData = ArmatureEventDataMgr::create();
		mModelEventData->retain();
		mRoleConfig = RoleConfig::create();
		mRoleConfig->retain();
		return true;
	}

	void ConfigManage::clearBattleConfig()
	{
		mMoveArea.clear();
		mCostArea.clear();
		mEnterArea.clear();
		mOtherEnter.clear();
		mObstacleArea.clear();
		mUndefinedArea.clear();
	}
	//Ӧ�ý����ڹؿ������������ļ����洢��һ��map����ȥ���������ڴ��У������ļ��Ĵ�С�Ƚ�С����ζ�ȡ�ļ��Ŀ���������
	void ConfigManage::initBattleData()
	{
		//���ݹؿ�����ս����������
		int tStageIndex = BattleManage->getStageIndex();
		ParseBattleGrid(tStageIndex,mMoveArea,mUndefinedArea,mEnterArea,mOtherEnter);
		ParseAddCostGrid(tStageIndex,mCostArea);
		mStoryData->initStoryData(tStageIndex);
		if (!mUndefinedArea.empty())
			mObstacleArea.assign(mUndefinedArea.begin(),mUndefinedArea.end());
		for (int tGrid = C_BEGINGRID;tGrid<C_CAPTAINGRID;tGrid++)
		{
			vector<int>::iterator iter = mMoveArea.begin();
			if (iter != mMoveArea.end())
				continue;
			mObstacleArea.push_back(*iter);
		}
	}

	const vector<int>& ConfigManage::getMoveVec() const
	{
		return mMoveArea;
	}

	const vector<int>& ConfigManage::getEnterVec() const
	{
		return mEnterArea;
	}

	const vector<int>& ConfigManage::getAddcostVec() const
	{
		return mCostArea;
	}

	const vector<int>& ConfigManage::getOtherEnter() const
	{
		return mOtherEnter;
	}

	const vector<int>& ConfigManage::getNndefinedArea() const
	{
		return mUndefinedArea;
	}

	const vector<int>& ConfigManage::getObstacleArea() const
	{
		return mObstacleArea;
	}
	//����������Ĺ�����Գ����һ�������Ķ��������д���
	bool ConfigManage::inAddCostArea( int pGrid )
	{
		if (pGrid == INVALID_GRID)
			return true;
		vector<int>::iterator tEnditer = mCostArea.end();
		if (std::find(mCostArea.begin(),tEnditer,pGrid) != tEnditer)
			return true;
		return false;
	}

	bool ConfigManage::inMoveArea( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnditer = mMoveArea.end();
		if (std::find(mMoveArea.begin(),tEnditer,pGrid) != tEnditer)
			return true;						//�㲻�ڿ��ƶ���Χ��
		return false;
	}
	//�ٻ�������ƶ�����Ĺ�ϵ���⣬�����ٻ��Ĺ�ȥ�����ǲ����ٸ���λ�ã����ǳ��������ٻ�ȥ
	bool ConfigManage::inEnterArea( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnterIter = mEnterArea.end();
		if (std::find(mEnterArea.begin(),tEnterIter,pGrid) != tEnterIter)
			return true;
		return false;
	}

	bool ConfigManage::inOtherEnter( int pGrid )
	{
		if (inUnDefineArea(pGrid))
			return false;
		vector<int>::iterator tEnterIter = mOtherEnter.end();
		if (std::find(mOtherEnter.begin(),tEnterIter,pGrid) != tEnterIter)
			return true;
		return false;
	}

	bool ConfigManage::inUnDefineArea( int pGrid )
	{
		vector<int>::iterator tUnDefine = mUndefinedArea.end();
		if (std::find(mUndefinedArea.begin(),tUnDefine,pGrid) != tUnDefine)
			return true;						//����δ����������
		return false;
	}

	bool ConfigManage::inObstacleArea( int pGrid )
	{
		vector<int>::iterator tObstacle = mObstacleArea.end();
		if (std::find(mObstacleArea.begin(),tObstacle,pGrid) != tObstacle)
			return true;
		return false;
	}

	const EffectData* ConfigManage::getEffData() const
	{
		return mEffectData;
	}

	const BuffConfig* ConfigManage::getBuffData() const
	{
		return mBuffData;
	}

	const StoryData* ConfigManage::getStoryData() const
	{
		return mStoryData;
	}

	const ArmatureEventDataMgr* ConfigManage::getArmatureDataMgr() const
	{
		return mModelEventData;
	}

	const EffectInfo* ConfigManage::getEffectInfo( int pInfoID ) const
	{
		return mEffectData->getEffectInfo(pInfoID);
	}

	void ConfigManage::initRoleSkillInfo( int pEffectID,BaseRole* pRole )
	{
		const EffectInfo* tInfo = mEffectData->getEffectInfo(pEffectID);
		pRole->SkillActionAndEffect(tInfo->getActionID(),tInfo->getusEft());
	}

	void ConfigManage::initObstacle( vector<int>& pVector )
	{
		if (mObstacleArea.empty())
			return;
		pVector.assign(mObstacleArea.begin(),mObstacleArea.end());
	}

	void ConfigManage::initRoleConfigData( BaseRole* pRole )
	{
		pRole->setConfigData(mRoleConfig->getConfigData(pRole->getModel()));
	}

	const RoleConfigData* ConfigManage::getConfigData( int pModel ) const
	{
		return mRoleConfig->getConfigData(pModel);
	}

}