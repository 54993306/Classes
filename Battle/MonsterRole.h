/************************************************************* 
*
*
*		Data : 2016.6.7
*	
*		Name : MonsterRole
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __MonsterRole_H_
#define __MonsterRole_H_
#include "Battle/BaseRole.h"
namespace BattleSpace
{
	class MonsterData;
	class MonsterRole : public BaseRole
	{
	public:
		MonsterRole(BaseRoleData* pData);
		virtual ~MonsterRole();
		static MonsterRole* create(BaseRoleData* pData);
		virtual sRoleType getRoleType();
	protected:
		MonsterData* mMonsterData;
		MonsterData* mLogicData;
	};
};


#endif