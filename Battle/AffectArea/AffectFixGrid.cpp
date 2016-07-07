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

#include "Battle/AffectArea/AffectFixGrid.h"

namespace BattleSpace
{
	//����̶�����
	sAffectType AffectAnyFixGrid::getAreaType()
	{
		return sAffectType::eAnyFixGrid;
	}

	void AffectAnyFixGrid::initArea(AreaCountInfo& pInfo)
	{
		int tLoopNumberMax  = 200;
		for (int tLoopNum = 0;tLoopNum < tLoopNumberMax;tLoopNum++)
		{
			int tIndex = CCRANDOM_0_1() *(C_CAPTAINGRID-C_BEGINGRID)+C_BEGINGRID;	//������ķ�Χ C_BEGINGRID ~ C_CAPTAINGRID
			if (pInfo.hasGrid(tIndex))
				continue;
			pInfo.addGrid(tIndex);
			if(pInfo.getVector().size() >= pInfo.getAreaRange()+pInfo.getAlive()->mStandGrids.size()) 
				return ;
		}
		CCLOG("[ *ERROR ]SkillRange::FixGrid");
	}

	/***************************************************************************/

	//����̶���������
	sAffectType AffectAnyFixGridDisperse::getAreaType()
	{
		return sAffectType::eAnyFixGridDisperse;
	}

	void AffectAnyFixGridDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectAnyFixGrid::initArea(pInfo);
		pInfo.DisperseDispose();
	}
}