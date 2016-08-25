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

#include "Battle/AffectArea/AffectLowesHp.h"
#include "Battle/BattleTools.h"

namespace BattleSpace
{
	//血量最少目标
	sAffectType AffectLowesHp::getAreaType()
	{
		return sAffectType::eLowestHp;
	}

	void AffectLowesHp::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*> tAlives = pInfo.getAlive()->getCurrSkillTargets();
		SortRoleHp(tAlives);
		if (pInfo.getAreaRange() >= tAlives.size())
		{
			for(auto tRole:tAlives)
				pInfo.addGrid(tRole->getGridIndex());
		}else{
			for (auto tRole:tAlives)
			{
				pInfo.addGrid(tRole->getGridIndex());
				if (pInfo.getVector().size() >= pInfo.getAreaRange())
					break;
			}
		}
	}

	/***************************************************************************/

	//血量最少目标除主帅外
	sAffectType AffectLowesHpUnCaptain::getAreaType()
	{
		return sAffectType::eLowestHpUnCaptain;
	}

	void AffectLowesHpUnCaptain::initArea(AreaCountInfo& pInfo)
	{
		AffectLowesHp::initArea(pInfo);
		pInfo.excludeCaptain();
	}
}