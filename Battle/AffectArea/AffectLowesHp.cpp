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
	AffectType AffectLowesHp::getAreaType()
	{
		return AffectType::eLowestHp;
	}

	void AffectLowesHp::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tAlives = pInfo.getAlive()->getSkillTargets();
		VectorSortAliveHp(*tAlives);
		if (pInfo.getAreaRange() >= tAlives->size())
		{
			for(auto i:*tAlives)
				pInfo.addGrid(i->getGridIndex());
		}else{
			for (auto i:*tAlives)
			{
				pInfo.addGrid(i->getGridIndex());
				if (pInfo.getVector().size() >= pInfo.getAreaRange())
					break;
			}
		}
	}

	/***************************************************************************/

	//血量最少目标除主帅外
	AffectType AffectLowesHpUnCaptain::getAreaType()
	{
		return AffectType::eLowestHpUnCaptain;
	}

	void AffectLowesHpUnCaptain::initArea(AreaCountInfo& pInfo)
	{
		AffectLowesHp::initArea(pInfo);
		pInfo.excludeCaptain();
	}
}