
#include "Battle/RoleSkill.h"
#include "Battle/skEffectData.h"
#include <protos/common/skill_common.pb.h>
namespace BattleSpace
{
	RoleSkill::RoleSkill():mSkillID(0),mSkillType(sSkillType::eNullType),
	mUserRatio(0),mExpendCost(0),mCooldown(0),mSkillLevel(0),mMoveRole(false),
	mTargetType(0),mAffectRatio(0),mAffectType(0)
	{}

	RoleSkill::~RoleSkill()
	{
		clearEffectData();
	}

	RoleSkill* RoleSkill::create()
	{
		RoleSkill* tSkill = new RoleSkill();
		if (tSkill)
		{
			tSkill->autorelease();
			return tSkill;
		}else{
			delete tSkill;
			tSkill = nullptr;
			return nullptr;
		}
	}

	void RoleSkill::clearEffectData()
	{
		for (auto tVector : mEffectList)
		{
			for (auto tEffectData : tVector)
			{
				CC_SAFE_RELEASE(tEffectData);
			}
			tVector.clear();
		}
		mEffectList.clear();
	}

	void RoleSkill::readData(const protos::common::Skill* pSkillData)
	{
		this->setSkillID(pSkillData->skillid());
		this->setSkillType((sSkillType)pSkillData->type());
		this->setUserRatio(pSkillData->userate());
		this->setExpendCost(abs(pSkillData->cost()));
		this->setCooldown(pSkillData->colddown());
		this->setTargetType(pSkillData->target());
		this->setAffectType(pSkillData->pro_type());
		this->setAffectRatio(pSkillData->pro_rate());
		this->setSkillLevel(pSkillData->level());
		this->setMoveRole(pSkillData->move());
		this->initEffectData(pSkillData);
	}

	void RoleSkill::initEffectData( const protos::common::Skill* pSkillData )
	{
		std::vector<skEffectData*>	tFirstList;		//技能的效果列表1
		std::vector<skEffectData*>	tSecondList;	//技能的效果列表2
		enum E_EffectGroup
		{
			eFristGroup		=1,
			eSecondGroup	=2,
		};
		for (int i=0;i<pSkillData->effect_size();i++)
		{
			skEffectData* tEffectData = skEffectData::create();
			tEffectData->readData(&pSkillData->effect(i));
			tEffectData->retain();
			if (tEffectData->getGroup() == eFristGroup)
			{
				tFirstList.push_back(tEffectData);
			}else if(tEffectData->getGroup() == eSecondGroup){
				tSecondList.push_back(tEffectData);
			}else{
				CCLOG("[*ERROR] RoleSkill::readData");
			}
		}
		std::sort(tFirstList.begin(),tFirstList.end(),EffectSort);
		mEffectList.push_back(tFirstList);
		if (tSecondList.size())
		{
			std::sort(tSecondList.begin(),tSecondList.end(),EffectSort);
			mEffectList.push_back(tSecondList);
		}
	}

	int RoleSkill::getListSize() const
	{
		return mEffectList.size();
	}

	int RoleSkill::getEffectSize(unsigned int pIndex ) const
	{
		if (pIndex >= mEffectList.size())
			return 0;
		return mEffectList.at(pIndex).size();
	}

	skEffectData* RoleSkill::getIndexEffect(unsigned int pListIndex,unsigned int pEffectIndex ) const
	{
		if (pListIndex >= mEffectList.size())
			return nullptr;
		if (pEffectIndex >= mEffectList.at(pListIndex).size())
			return nullptr;
		return mEffectList.at(pListIndex).at(pEffectIndex);
	}

	skEffectData* RoleSkill::getFirstEffect() const
	{
		if ( !mEffectList.size())
			return nullptr;
		if ( !mEffectList.at(0).size())
			return nullptr;
		return mEffectList.at(0).at(0);
	}

	skEffectData* RoleSkill::getSecondEffect() const
	{
		if ( mEffectList.size() < 1)
			return nullptr;
		if ( !mEffectList.at(1).size())
			return nullptr;
		return mEffectList.at(1).at(0);
	}

	skEffectData* RoleSkill::getSummonEffect() const
	{
		if (getSkillType() == sSkillType::eCallAtk)
			if (mEffectList.size())
				if (mEffectList.at(0).size())
					return mEffectList.at(0).at(0);
		return nullptr;
	}

	const std::vector<std::vector<skEffectData*>>& RoleSkill::getEffectVector() const
	{
		return mEffectList;
	}
}
