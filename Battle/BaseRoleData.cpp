

#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
#include "Battle/HeroData.h"
#include "Battle/MonsterData.h"
namespace BattleSpace
{
	BaseRoleData::BaseRoleData()
	:mServerID(0),mRoleModel(0),mAttribute(sAttribute::eNull),mRoleHp(0)
	,mInitGrid(0),mRoleRow(1),mRoleCol(1),mRoleZoom(0),mRoleDodge(0),mRoleAttack(0)
	,mMoveSpeed(0),mCallID(0),mCallRoleType(sCallType::eCommon),mRoleHit(0),mRoleCrit(0)
	,mAttackSpeed(0),mDelayTime(0),mRoleRegain(0),mRoleAgility(0),mRoleDefense(0)
	,mBehavior(sBehavior::eNormal)
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

	BattleSpace::sDataType BaseRoleData::getDataType()
	{
		return sDataType::eBase;
	}

	BaseRoleData* BaseRoleData::create( sDataType pType,const google::protobuf::Message* pMessage )
	{
		switch (pType)
		{
		case BattleSpace::sDataType::eHero:
			{
				return HeroData::create(pMessage);
			}break;
		case BattleSpace::sDataType::eMonster:
			{
				return MonsterData::create(pMessage);
			}break;
		}
		CCLOG("[ *ERROR ] BaseRoleData::create( E_RoleNature pNature ) ");
		BaseRoleData* pData = new BaseRoleData();
		if (pData)
		{
			pData->autorelease();
			return pData;
		}else{
			delete pData;
			pData = nullptr;
			return nullptr;
		}
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

	bool BaseRoleData::isAttribute( sAttribute pType )
	{
		if (pType == getAttribute())
			return true;
		return false;
	}

	float BaseRoleData::judgeAttribute( sAttribute pType )			//»ð1>Ä¾3; Ä¾3>Ë®2; Ë®2>»ð1
	{
		if (getAttribute() == pType)
		{
			return 1.0f;
		}else if ( getAttribute() == sAttribute::eFire )
		{
			if (pType == sAttribute::eWood)
				return 1.3f;
			if (pType == sAttribute::eWater)
				return 0.7f;
		}else if ( getAttribute() == sAttribute::eWood )
		{
			if (pType == sAttribute::eWater)
				return 1.3f;
			if (pType == sAttribute::eFire)
				return 0.7f;
		}else if( getAttribute() == sAttribute::eWater )
		{
			if (pType == sAttribute::eFire)
				return 1.3f;
			if (pType == sAttribute::eWood)
				return 0.7f;
		}
		return 1.0f;
	}

	void BaseRoleData::copySelf( BaseRoleData* pData )
	{
		*this = *pData;
	}

}