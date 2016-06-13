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

#include "Battle/AffectArea/AffectCross.h"

namespace BattleSpace
{
	AffectType AffectCross::getAreaType()
	{
		return AffectType::eCross;
	}

	void AffectCross::initArea(AreaCountInfo& pInfo)
	{
		vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			int tCenterRow = tRow;
			int center_col = 0;
			if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
				!pInfo.getEnemy() && pInfo.getBackAttack())
			{
				center_col = tCol+(pInfo.getAreaRange()+1);
			}else{
				center_col = tCol-(pInfo.getAreaRange()+1);
			}
			for (int i=center_col - pInfo.getAreaRange();i<=center_col + pInfo.getAreaRange();i++)
				pInfo.addGrid(i*C_GRID_ROW+tCenterRow);
			for (int j=tCenterRow - pInfo.getAreaRange();j<=tCenterRow + pInfo.getAreaRange();j++)
			{
				if (j>=C_GRID_ROW || j<0)
					continue;
				pInfo.addGrid(center_col*C_GRID_ROW+j);
			}
		}
	}

}
