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
	//ǰ������
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

	//ǰ����ɢ
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

	//ǰ��n��
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