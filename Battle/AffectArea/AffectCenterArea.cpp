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
	//���ĵ���
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

	//���ķ�ɢ
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

	//����n��
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