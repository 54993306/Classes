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

#include "Battle/AffectArea/AffectFrontFourGrid.h"

namespace BattleSpace
{
	AffectType AffectFrontFourGrid::getAreaType()
	{
		return AffectType::eFrontFourGrid;
	}

	void AffectFrontFourGrid::initArea(AreaCountInfo& pInfo)
	{
		if (pInfo.getEnemy())
		{
			int col = pInfo.getDistance()+C_BEGINGRID/C_GRID_ROW;
			for (int i=col;i<col+pInfo.getAreaRange();i++)
			{
				for (int j=0;j<4;j++)
					pInfo.addGrid(i*C_GRID_ROW+j);
			}
		}else{
			int col = C_ENDGRID/C_GRID_ROW - pInfo.getDistance();
			for (int i=col;i>col-pInfo.getAreaRange();i--)
			{
				for (int j=0;j<4;j++)
					pInfo.addGrid(i*C_GRID_ROW+j);
			}
		}
	}

}