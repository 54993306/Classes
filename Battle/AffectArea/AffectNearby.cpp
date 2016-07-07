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
#include "Battle/AffectArea/AffectNearby.h"
namespace BattleSpace
{
	sAffectType AffectNearby::getAreaType()
	{
		return sAffectType::eNearby;
	}

	void AffectNearby::initArea( AreaCountInfo& pInfo )
	{
		vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			int minRow = tRow - pInfo.getAreaRange();
			int maxRow = tRow + pInfo.getAreaRange();
			int minCol = tCol - pInfo.getAreaRange();
			int maxCol = tCol + pInfo.getAreaRange();
			minRow = max(0,minRow);
			maxRow = min(3,maxRow);
			minCol = max(16,minCol);
			maxCol = min(31,maxCol);
			for (int i=maxRow;i>=minRow;i--)
			{
				for (int j=maxCol;j>=minCol;j--)
					pInfo.addGrid(j*C_GRID_ROW+i);
			}
		}
	}

	void AffectNearby::moriGridRole( BaseRole* pRole,std::vector<int>& pValids )
	{
		if (pRole->getTouchState())
		{
			pValids.assign(pRole->mTouchGrids.begin(),pRole->mTouchGrids.end());
		}else{
			pValids.assign(pRole->mStandGrids.begin(),pRole->mStandGrids.end());
		}
	}


	sAffectType AffectNearbyUnself::getAreaType()
	{
		return sAffectType::eNearbyUnself;
	}

	void AffectNearbyUnself::initArea(AreaCountInfo& pInfo)
	{
		AffectNearby::initArea(pInfo);
		pInfo.excludeStandGrid();
	}
};