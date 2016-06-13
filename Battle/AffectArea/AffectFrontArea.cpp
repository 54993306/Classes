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

	//ǰ����ɢ
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

	//ǰ��n��
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