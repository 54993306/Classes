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

#include "Battle/AffectArea/AffectCenterArea.h"
namespace BattleSpace
{
	//中心单体
	sAffectType AffectCenterSingle::getAreaType()
	{
		return sAffectType::eCenterSingle;
	}

	void AffectCenterSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*> tAlives = pInfo.getAlive()->getCurrSkillTargets();
		if (tAlives.empty())
			return;
		pInfo.addGrid(tAlives.at(tAlives.size()/2)->getGridIndex());
	}

	/***************************************************************************/

	//中心分散
	sAffectType AffectCenterDisperse::getAreaType()
	{
		return sAffectType::eCenterDisperse;
	}

	void AffectCenterDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectCenterSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//中心n排
	sAffectType AffectCenterRow::getAreaType()
	{
		return sAffectType::eCenterRow;
	}

	void AffectCenterRow::initArea(AreaCountInfo& pInfo)
	{
		AffectCenterSingle::initArea(pInfo);
		pInfo.RowType(eCentenDirection);
	}

}