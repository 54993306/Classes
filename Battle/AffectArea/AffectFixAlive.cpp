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

#include "Battle/AffectArea/AffectFixAlive.h"

namespace BattleSpace
{
	//随机固定武将
	AffectType AffectAnyAlive::getAreaType()
	{
		return AffectType::eAnyFixAlive;
	}

	void AffectAnyAlive::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tAlives = pInfo.getAlive()->getSkillTargets();
		int tRange = min(tAlives->size(),pInfo.getAreaRange());		// num < val 表示武将不足	
		int tLoopNumberMax = 200;
		for (int tLoopNum = 0;tLoopNum < tLoopNumberMax;tLoopNum++)
		{
			int tIndex = CCRANDOM_0_1() *(tAlives->size() - 1);						//随机数的范围 num > j >= 0 
			BaseRole* tAlive = tAlives->at(tIndex);
			if (tAlive->getCaptain())												//随机得到几个有人的位置,不包含主帅
				if (tAlives->size() == 1)											//只有敌方主帅一人的情况
					return;
				else
					continue;
			if (pInfo.hasGrid(tAlive->getGridIndex()))
				continue;
			pInfo.addGrid(tAlive->getGridIndex());
			if(pInfo.getVector().size() >= tRange+pInfo.getAlive()->mStandGrids.size())
				return;
		}
		CCLOG("[ *ERROR ]  SkillRange::FixAlive ");
	}


	/***************************************************************************/

	//随机固定武将区域
	AffectType AffectAnyAliveDisperse::getAreaType()
	{
		return AffectType::eAnyFixAliveDisperse;
	}

	void AffectAnyAliveDisperse::initArea(AreaCountInfo& pInfo)
	{
		AffectAnyAlive::initArea(pInfo);
		pInfo.DisperseDispose();
	}
}