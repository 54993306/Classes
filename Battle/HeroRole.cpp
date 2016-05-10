

#include "Battle/HeroRole.h"

HeroRole::HeroRole()
	:Captain(false)
{}

HeroRole* HeroRole::Create( const protos::common::Hero &hero )
{
	HeroRole* pData = new HeroRole();
	if (pData && pData->initHeroData(hero))
	{
		pData->autorelease();
		return pData;
	}else{
		delete pData;
		pData = NULL;
		return NULL;
	}
}

bool HeroRole::initHeroData( const protos::common::Hero &hero )
{

	return true;
}