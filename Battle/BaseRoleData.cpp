

#include <vector>
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
using namespace std;
namespace BattleSpace{

	BaseRoleData::BaseRoleData()
		:mServerID(0),mRoleModel(0),mRoleType(0),mRoleHp(0),mRoleAttack(0),mRoleDefense(0),mRoleCrit(0),mRoleHit(0),mRoleDodge(0)
		,mCostSpeed(0),mMaxCost(0),mInitGrid(0),mRoleRow(1),mRoleCol(1),mRoleZoom(0),mRoleDrop(0),mColdDown(0),mExpendCost(0)
		,mCritTime(0),mMoveSpeed(0),mCallRole(false),mCallID(0),mAlertRange(0),mCallType(0),mMonsterType(0)
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

	const RoleSkill* BaseRoleData::getNormalSkill() const
	{
		return mNormalSkill;
	}

	const RoleSkill* BaseRoleData::getSpecialSkill() const
	{
		return mSpecialSkill;
	}

	const RoleSkill* BaseRoleData::getActiveSkill() const
	{
		return mActiveSkill;
	}

	const RoleSkill* BaseRoleData::getCaptainSkill() const
	{
		return mCaptainSkill;
	}
}