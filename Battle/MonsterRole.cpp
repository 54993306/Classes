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
#include "Battle/MonsterRole.h"
#include "Battle/MonsterData.h"

namespace BattleSpace
{
	MonsterRole::MonsterRole(BaseRoleData* pData)
	:mMonsterData((MonsterData*)pData)
	{
		mLogicData = MonsterData::create();
		mLogicData->copySelf(pData);
		mLogicData->retain();
		mMonsterData = (MonsterData*)pData;
		setBaseData(pData);
		setLogicData(mLogicData);
	}

	MonsterRole::~MonsterRole()
	{
		CC_SAFE_RELEASE(mLogicData);
		mLogicData = nullptr;
	}

	MonsterRole* MonsterRole::create(BaseRoleData* pData)
	{
		MonsterRole* tRole = new MonsterRole(pData);
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

	BattleSpace::sRoleType MonsterRole::getRoleType()
	{
		return  sRoleType::eMonsterRole;
	}

}