/************************************************************* 
*
*
*		Data : 2016.6.7
*	
*		Name : HeroRole
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __HeroRole_H_
#define __HeroRole_H_
#include "Battle/BaseRole.h"
namespace BattleSpace
{
	class HeroData;
	class HeroRole : public BaseRole
	{
	public:
		HeroRole(BaseRoleData* pData);
		virtual ~HeroRole();
		static HeroRole* create(BaseRoleData* pData);
		virtual sRoleType getRoleType();
	protected:
		HeroData* mHeroData;
		HeroData* mLogicData;
	};
};
#endif