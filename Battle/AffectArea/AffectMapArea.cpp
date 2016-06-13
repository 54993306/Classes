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

#include "Battle/AffectArea/AffectMapArea.h"

namespace BattleSpace
{
	//地图中心两行攻击范围
	AffectType AffectMapCenterTwoLine::getAreaType()
	{
		return AffectType::eMapCenterTwoLine;
	}

	void AffectMapCenterTwoLine::initArea(AreaCountInfo& pInfo)
	{
		if (pInfo.getEnemy())
		{
			int col = pInfo.getDistance()+C_BEGINGRID/C_GRID_ROW;
			for (int i=col;i<col+pInfo.getAreaRange();i++)
			{
				pInfo.addGrid(i*C_GRID_ROW+1);
				pInfo.addGrid(i*C_GRID_ROW+2);
			}
		}else{
			int col = C_ENDGRID/C_GRID_ROW - pInfo.getDistance();
			for (int i=col;i>col-pInfo.getAreaRange();i--)
			{
				pInfo.addGrid(i*C_GRID_ROW+1);
				pInfo.addGrid(i*C_GRID_ROW+2);
			}
		}
	}

	/***************************************************************************/

	//地图中任意两行攻击范围
	AffectType AffectMapAnyDoubleLine::getAreaType()
	{
		return AffectType::eMapAnyDoubleLine;
	}

	void AffectMapAnyDoubleLine::initArea(AreaCountInfo& pInfo)
	{
		map<int,pair<int , int>> LineMap;
		LineMap[1] = make_pair(0,1);
		LineMap[2] = make_pair(1,2);
		LineMap[3] = make_pair(2,3);
		LineMap[4] = make_pair(0,3);
		LineMap[5] = make_pair(0,2);
		LineMap[6] = make_pair(1,3);
		pair<int , int> cpair = LineMap.find(pInfo.getDistance())->second;
		int tCol =pInfo.getGrid()/ C_GRID_ROW;
		if (!pInfo.getEnemy()&&!pInfo.getBackAttack())
		{
			for (int i=tCol-1;i>=tCol-pInfo.getAreaRange();i--)
			{
				pInfo.addGrid(i*C_GRID_ROW+cpair.first);
				pInfo.addGrid(i*C_GRID_ROW+cpair.second);
			}		
		}else{
			int col = C_ENDGRID/C_GRID_ROW;
			for (int i=tCol+1;i<=tCol+pInfo.getAreaRange();i++)
			{
				pInfo.addGrid(i*C_GRID_ROW+cpair.first);
				pInfo.addGrid(i*C_GRID_ROW+cpair.second);
			}
		}
	}
}