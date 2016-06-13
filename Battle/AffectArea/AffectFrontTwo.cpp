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

#include "Battle/AffectArea/AffectFrontTwo.h"

namespace BattleSpace
{
	AffectType AffectFrontTwo::getAreaType()
	{
		return AffectType::eFrontTow;
	}

	void AffectFrontTwo::initArea(AreaCountInfo& pInfo)
	{
		vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		for (auto tGrid : tAliveCountGrids)								//����ÿ�ֹ������Ͷ����佫��Ч�����йص�			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
				!pInfo.getEnemy() && pInfo.getBackAttack())
			{
				for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
				{
					if (tRow-1<0)continue;
					pInfo.addGrid(i*C_GRID_ROW+tRow-1);
				}
				for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
				{
					if (tRow+1>=C_GRID_ROW)continue;
					pInfo.addGrid(i*C_GRID_ROW+tRow+1);
				}
			}else{
				for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
				{
					if (tRow-1<0)continue;
					pInfo.addGrid(i*C_GRID_ROW+tRow-1);
				}					
				for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
				{
					if (tRow+1>=C_GRID_ROW)continue;
					pInfo.addGrid(i*C_GRID_ROW+tRow+1);
				}
			}
		}
	}

}