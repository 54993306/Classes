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

#ifndef __CallRole_H_
#define __CallRole_H_
#include "Battle/HeroRole.h"
namespace BattleSpace
{
	class HeroCall : public HeroRole
	{
	public:
		HeroCall(BaseRoleData* pData,BaseRole* pRole);
		virtual ~HeroCall(){};
		virtual sRoleType getRoleType();
		static HeroCall* create(BaseRoleData* pData,BaseRole* pRole);

	};
};
#endif