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

#include "Battle/AffectArea/AffectVertical.h"
#include "Battle/BaseRole.h"
namespace BattleSpace
{
	AffectType AffectVertical::getAreaType()
	{
		return AffectType::eVertical;
	}

	void AffectVertical::initArea( AreaCountInfo& pInfo )
	{
		vector<int> tAliveCountGrids;
		initValidGrids(pInfo.getAlive(),tAliveCountGrids);
		for (auto tGrid : tAliveCountGrids)								//不是每种攻击类型都跟武将有效区域有关的			
		{
			int tCol = pInfo.getColByInfo(tGrid);
			int tRow = pInfo.getRowByGrid(tGrid);
			if (pInfo.getEnemy() && !pInfo.getBackAttack() || 
				!pInfo.getEnemy() && pInfo.getBackAttack())
			{
				for (int i=tRow-1; i<=tRow+1; i++)
				{
					if (i>=C_GRID_ROW || i<0)
						continue;
					for (int j=tCol+1; j<=tCol+pInfo.getAreaRange(); j++)
						pInfo.addGrid(j*C_GRID_ROW+i);						
				}
			}else{
				for (int i=tRow-1; i<=tRow+1; i++)
				{
					if (i>=C_GRID_ROW || i<0)
						continue;
					for (int j=tCol-1; j>=tCol-pInfo.getAreaRange(); j--)
						pInfo.addGrid(j*C_GRID_ROW+i);
				}
			}
		}
	}

	void AffectVertical::moriGridRole( BaseRole* pRole,std::vector<int>& pValids )
	{
		if (pRole->getTouchState())
		{
			pValids.assign(pRole->mTouchGrids.begin(),pRole->mTouchGrids.end());
		}else{
			pValids.assign(pRole->mStandGrids.begin(),pRole->mStandGrids.end());
		}
	}

}