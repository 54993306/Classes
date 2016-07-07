

#include <vector>
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
using namespace std;
namespace BattleSpace{

	BaseRoleData::BaseRoleData()
		:mServerID(0),mRoleModel(0),mProperty(0),mRoleHp(0),mRoleAttack(0),mRoleDefense(0),mRoleCrit(0),mRoleHit(0),mRoleDodge(0)
		,mCostSpeed(0),mMaxCost(0),mInitGrid(0),mRoleRow(1),mRoleCol(1),mRoleZoom(0),mRoleDrop(0),mColdDown(0),mExpendCost(0)
		,mCritTime(0),mMoveSpeed(0),mCallRole(false),mAlertRange(0),mCallType(sCallType::eCommon),mBehavior(sBehavior::eNormal),mRoleNature(sDataType::eNULL)
		,mMaxHp(0),mAttackSpeed(0),mCaptain(false),mDelayTime(0),mAlertType(0),mRoleRegain(0),mRoleAgility(0),mInitCost(0)
	{
		mNormalSkill = RoleSkill::create();
		mNormalSkill->retain();
		mSpecialSkill = RoleSkill::create();
		mSpecialSkill->retain();
		mActiveSkill = RoleSkill::create();
		mActiveSkill->retain();
		mCaptainSkill = RoleSkill::create();
		mCaptainSkill->retain();
	}

	BaseRoleData::~BaseRoleData()
	{
		CC_SAFE_RELEASE(mNormalSkill);
		mNormalSkill = nullptr;
		CC_SAFE_RELEASE(mSpecialSkill);
		mSpecialSkill = nullptr;
		CC_SAFE_RELEASE(mActiveSkill);
		mActiveSkill = nullptr;
		CC_SAFE_RELEASE(mCaptainSkill);
		mCaptainSkill = nullptr;
	}

	bool BaseRoleData::hasSpecialSkill() const
	{
		if (mSpecialSkill->getSkillID())
			return true;
		return false;
	}

	bool BaseRoleData::hasActiveSkill() const
	{
		if (mActiveSkill->getSkillID())
			return true;
		return false;
	}

	int BaseRoleData::getActiveSkillType() const
	{
		if (mActiveSkill->getSkillID())
			return mActiveSkill->getSkillType();
		return 0;
	}

	bool BaseRoleData::hasCaptainSkill() const
	{
		if (mCaptainSkill->getSkillID())
			return true;
		return false;
	}

	RoleSkill* BaseRoleData::getNormalSkill() const
	{
		return mNormalSkill;
	}

	RoleSkill* BaseRoleData::getSpecialSkill() const
	{
		return mSpecialSkill;
	}

	RoleSkill* BaseRoleData::getActiveSkill() const
	{
		return mActiveSkill;
	}

	RoleSkill* BaseRoleData::getCaptainSkill() const
	{
		return mCaptainSkill;
	}

	bool BaseRoleData::isAttribute( sProperty pType )
	{
		if (pType == getProperty())
			return true;
		return false;
	}

	float BaseRoleData::judgeAttribute( sProperty pType )
	{
		if (getProperty() == pType)
		{
			return 1.0f;
		}else if ( getProperty() == sProperty::eFire )
		{
			if (pType == sProperty::eWood)
				return 1.3f;
			if (pType == sProperty::eWater)
				return 0.7f;
		}else if ( getProperty() == sProperty::eWood )
		{
			if (pType == sProperty::eWater)
				return 1.3f;
			if (pType == sProperty::eFire)
				return 0.7f;
		}else if( getProperty() == sProperty::eWater )
		{
			if (pType == sProperty::eFire)
				return 1.3f;
			if (pType == sProperty::eWood)
				return 0.7f;
		}
		return 1.0f;
	}
}