
#include "Battle/MonsterRole.h"

MonsterRole::MonsterRole()
	:CritTime(0)
{}

MonsterRole* MonsterRole::Create( const protos::common::Monster &monster )
{
	MonsterRole* pData = new MonsterRole();
	if (pData && pData->initMonsterData(monster))
	{
		pData->autorelease();
		return pData;
	}else{
		delete pData;
		pData = NULL;
		return NULL;
	}
}

bool MonsterRole::initMonsterData( const protos::common::Monster &monster )
{



	return true;
}