/************************************************************* 
 *
 *
 *		Data : 2016.6.17
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/HeroRole.h"
#include "Battle/HeroData.h"

namespace BattleSpace
{
	HeroRole::HeroRole(BaseRoleData* pData)
	:mHeroData((HeroData*)pData),mLogicData(nullptr)
	{
		mLogicData = HeroData::create();
		mLogicData->copySelf(pData);
		mLogicData->retain();
		mHeroData = (HeroData*)pData;
		setBaseData(pData);
		setLogicData(mLogicData);
	}

	HeroRole::~HeroRole()
	{
		CC_SAFE_RELEASE(mLogicData);
		mLogicData = nullptr;
	}

	HeroRole* HeroRole::create(BaseRoleData* pData)
	{
		HeroRole* tRole = new HeroRole(pData);
		if (tRole)
		{
			tRole->autorelease();
			return tRole;
		}else{
			delete tRole;
			tRole = nullptr;
			return nullptr;
		}
	}

	BattleSpace::sRoleType HeroRole::getRoleType()
	{
		return sRoleType::eHeroRole;
	}

	


}