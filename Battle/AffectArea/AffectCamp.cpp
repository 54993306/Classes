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
	AffectType AffectEnemy::getAreaType()
	{
		return AffectType::eEnemy;
	}

	void AffectEnemy::initArea(AreaCountInfo& pInfo)
	{
		for ( auto tAlive:*pInfo.getAlive()->getSkillTargets() )
			pInfo.addGrid(tAlive->getGridIndex());
		pInfo.excludeCaptain();
	}

	/***************************************************************************/

	AffectType AffectOur::getAreaType()
	{
		return AffectType::eOur;
	}

	/***************************************************************************/

	AffectType AffectAllArea::getAreaType()
	{
		return AffectType::eAllArea;
	}
}