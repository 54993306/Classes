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

#include "Battle/AffectArea/AffectTArea.h"

namespace BattleSpace
{
	AffectType AffectTArea::getAreaType()
	{
		return AffectType::eTArea;
	}

	void AffectTArea::initArea(AreaCountInfo& pInfo)
	{
		vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		vector<int>& tStandVec = pInfo.getAlive()->mStandGrids;
		for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
				!pInfo.getEnemy() && pInfo.getBackAttack())
			{
				for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
					pInfo.addGrid(i*C_GRID_ROW+tRow);
				for (int j=tCol+2;j<tCol+2+pInfo.getAreaRange();j++)
				{
					for (int k=tRow-1;k<=tRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						pInfo.addGrid(j*C_GRID_ROW+k);
					}

				}
			}else{
				for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
					pInfo.addGrid(i*C_GRID_ROW+tRow);
				for (int j=tCol-2;j>tCol-2-pInfo.getAreaRange();j--)
				{
					for (int k=tRow-1;k<=tRow+1;k++)
					{
						if (k>=C_GRID_ROW||k<0)continue;
						pInfo.addGrid(j*C_GRID_ROW+k);
					}
				}
			}
		}
	}
}