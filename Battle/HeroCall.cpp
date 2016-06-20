/************************************************************* 
 *
 *
 *		Data : 2016.6.17
 *	
 *		Name : CallRole
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/HeroCall.h"

namespace BattleSpace
{

	HeroCall::HeroCall( BaseRoleData* pData,BaseRole* pRole ):HeroRole(pData)
	{

	}

	BattleSpace::sRoleType HeroCall::getRoleType()
	{
		return sRoleType::eHeroRole;
	}

	HeroCall* HeroCall::create( BaseRoleData* pData,BaseRole* pRole )
	{
		HeroCall* tRole = new HeroCall(pData,pRole);
		if (tRole)
		{
			tRole->autorelease();
			return tRole;
		}else{

		}
	}

}