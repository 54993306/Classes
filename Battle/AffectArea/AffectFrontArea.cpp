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
	sAffectType AffectFrontSingle::getAreaType()
	{
		return sAffectType::eFrontSingle;
	}

	void AffectFrontSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tRoles = pInfo.getAlive()->getCurrSkillTargets();
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
	sAffectType AffectFrontDisperse::getAreaType()
	{
		return sAffectType::eFrontDisperse;
	}

	void AffectFrontDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectFrontSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//前军n排
	sAffectType AffectFrontRow::getAreaType()
	{
		return sAffectType::eFrontRow;
	}

	void AffectFrontRow::initArea(AreaCountInfo& pInfo)
	{
		AffectFrontSingle::initArea(pInfo);
		pInfo.RowType(eFrontDirection);
	}
}