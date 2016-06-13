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
	AffectType AffectCenterSingle::getAreaType()
	{
		return AffectType::eCenterSingle;
	}

	void AffectCenterSingle::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tAlives = pInfo.getAlive()->getSkillTargets();
		if (!tAlives->size())
			return;
		pInfo.addGrid(tAlives->at(tAlives->size()/2)->getGridIndex());
	}

	/***************************************************************************/

	//中心分散
	AffectType AffectCenterDisperse::getAreaType()
	{
		return AffectType::eCenterDisperse;
	}

	void AffectCenterDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectCenterSingle::initArea(pInfo);
		pInfo.DisperseDispose();
	}

	/***************************************************************************/

	//中心n排
	AffectType AffectCenterRow::getAreaType()
	{
		return AffectType::eCenterRow;
	}

	void AffectCenterRow::initArea(AreaCountInfo& pInfo)
	{
		AffectCenterSingle::initArea(pInfo);
		pInfo.RowType(eCentenDirection);
	}

}