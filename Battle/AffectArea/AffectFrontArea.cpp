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

#include "Battle/AffectArea/AffectFrontArea.h"
namespace BattleSpace
{
	//前方单体
	AffectType AffectFrontSingle::getAreaType()
	{
		return AffectType::eFrontSingle;
	}

	void AffectFrontSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tRoles = pInfo.getAlive()->getSkillTargets();
		for (auto tRole : *tRoles)
		{
			if (!tRole->getCaptain())
			{
				pInfo.addGrid(tRole->getGridIndex());
				return ;
			}
		}
	}

	/***************************************************************************/

	//前方分散
	AffectType AffectFrontDisperse::getAreaType()
	{
		return AffectType::eFrontDisperse;
	}

	void AffectFrontDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectFrontSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//前军n排
	AffectType AffectFrontRow::getAreaType()
	{
		return AffectType::eFrontRow;
	}

	void AffectFrontRow::initArea(AreaCountInfo& pInfo)
	{
		AffectFrontSingle::initArea(pInfo);
		pInfo.RowType(eFrontDirection);
	}
}