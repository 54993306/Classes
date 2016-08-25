/************************************************************* 
 *
 *
 *		Data : 2016.8.9
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/Landform//AreaManage.h"
#include "Battle/Config/ConfigManage.h"
#include "Battle/Landform/TrapManage.h"
#include "Battle/Coords/Coords.h"
#include "Battle/CoordsManage.h"
#include "Battle/Landform/AStar.h"
#include "Battle/BattleCenter.h"
#include "Battle/ParseFileData.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRole.h"

namespace BattleSpace
{
	AreaManage::AreaManage():mAStar(nullptr){}

	AreaManage::~AreaManage()
	{
		CC_SAFE_RELEASE(mAStar);
		mAStar = nullptr;
	}

	bool AreaManage::init()
	{
		mAStar = AStar::create();
		mAStar->setRowMax(C_GRID_ROW);
		mAStar->setColMan(C_GRID_COL);
		mAStar->retain();
		return true;
	}

	//角色从当前位置移动到目标点是否可以执行通过
	bool AreaManage::roleThrough( BaseRole* pRole,int pGrid )
	{
		vector<int>tMoveGrids;
		mAStar->initMovePath(tMoveGrids,tMoveGrids,pRole->getGridIndex(),pGrid);
		for (auto tGrid : pRole->mObstacle)
		{
			for (auto t2Grid : tMoveGrids)
			{
				if (tGrid == t2Grid)
					return false;
			}
		}
		return true;
	}

	void AreaManage::initRoleObstacle( BaseRole* pRole )
	{
		pRole->mObstacle.clear();
		BattleConfig->initObstacle(pRole->mObstacle);
		BattleTrapManage->initObstacle(pRole,pRole->mObstacle);
	}

	void AreaManage::initRoleMovePath( BaseRole* pRole,int pGrid )
	{
		if (!pRole->getBattle() && !pRole->getEnemy())
			return;
		initRoleObstacle(pRole);
		mAStar->initMovePath(pRole->mObstacle,pRole->mMoveGrids,pRole->getGridIndex(),pGrid);
	}
}