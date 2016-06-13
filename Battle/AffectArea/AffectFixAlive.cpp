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
	//����̶��佫
	AffectType AffectAnyAlive::getAreaType()
	{
		return AffectType::eAnyFixAlive;
	}

	void AffectAnyAlive::initArea(AreaCountInfo& pInfo)
	{
		vector<BaseRole*>* tAlives = pInfo.getAlive()->getSkillTargets();
		int tRange = min(tAlives->size(),pInfo.getAreaRange());		// num < val ��ʾ�佫����	
		int tLoopNumberMax = 200;
		for (int tLoopNum = 0;tLoopNum < tLoopNumberMax;tLoopNum++)
		{
			int tIndex = CCRANDOM_0_1() *(tAlives->size() - 1);						//������ķ�Χ num > j >= 0 
			BaseRole* tAlive = tAlives->at(tIndex);
			if (tAlive->getCaptain())												//����õ��������˵�λ��,��������˧
				if (tAlives->size() == 1)											//ֻ�ез���˧һ�˵����
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

	//����̶��佫����
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