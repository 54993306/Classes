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
#include "Battle/AffectArea/AffectFront.h"

namespace BattleSpace
{
	AffectType AffectFront::getAreaType()
	{
		return AffectType::eFront;
	}

	void AffectFront::initArea(AreaCountInfo& pInfo)
	{
		std::vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		for (auto tGrid : tAliveCountGrids)								//����ÿ�ֹ������Ͷ����佫��Ч�����йص�			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
				!pInfo.getEnemy() && pInfo.getBackAttack())
			{
				for(int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
					pInfo.addGrid(i*C_GRID_ROW+tRow);
			}else{
				for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
					pInfo.addGrid(i*C_GRID_ROW+tRow);
			}
		}
	}

	/***************************************************************************/

	//ǰ����Χ���ᴩ����
	AffectType AffectPuncture::getAreaType()
	{
		return AffectType::ePuncture;
	}

	void AffectPuncture::initArea( AreaCountInfo& pInfo )
	{
		AffectFront::initArea(pInfo);
		pInfo.excludeInvalid();
		vector<BaseRole*>* VecAlive = pInfo.getAlive()->getSkillTargets();
		for (auto tRole: *VecAlive)
		{
			for (int tIndex=0;tIndex < pInfo.getVector().size();tIndex++)
			{
				if (tRole->getGridIndex() == pInfo.getVector().at(tIndex) && !tRole->getCloaking())
				{
					pInfo.removeByIndex(++tIndex);
					return;
				}
			}
		}
	}
}