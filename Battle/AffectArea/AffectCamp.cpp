/************************************************************* 
 *
 *
 *		Data : 2016.6.12
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/AffectArea/AffectCamp.h"

namespace BattleSpace
{
	sAffectType AffectEnemy::getAreaType()
	{
		return sAffectType::eEnemy;
	}

	void AffectEnemy::initArea(AreaCountInfo& pInfo)
	{
		for ( auto tAlive:*pInfo.getAlive()->getSkillTargets() )
			pInfo.addGrid(tAlive->getGridIndex());
		pInfo.excludeCaptain();
	}

	/***************************************************************************/

	sAffectType AffectOur::getAreaType()
	{
		return sAffectType::eOur;
	}

	/***************************************************************************/

	sAffectType AffectAllArea::getAreaType()
	{
		return sAffectType::eAllArea;
	}
}