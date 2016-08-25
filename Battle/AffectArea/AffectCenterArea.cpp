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

	//���ķ�ɢ
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

	//����n��
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