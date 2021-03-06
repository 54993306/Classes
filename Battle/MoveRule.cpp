﻿#include "Battle/MoveRule.h"
#include "Battle/ConstNum.h"
#include "Battle/BattleCenter.h"
#include "Battle/BaseRole.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleMessage.h"
namespace BattleSpace{
	MoveRule::MoveRule(){}

	MoveRule::~MoveRule(){bNotification->removeAllObservers(this);}

	bool MoveRule::init()
	{
		return true;
	}

	bool MoveRule::MonstMoveExcute(BaseRole* monster)
	{
		//int grid = monsterMove(monster);
		//if( grid!= INVALID_GRID && grid < C_CAPTAINGRID)	
		//{		
		//	if (monster->getMoveGrid() != grid)
		//		monster->setMoveGrid(grid);
		//	return true;
		//}
		return false;
	}

	int MoveRule::monsterMove(BaseRole* alive)
	{
		////怪物的移动处理是，哪个最先返回值不是INVALID_GRID就以哪个为标准，后面的不做处理了
		//if (alive->getMonsterSpecies() == sMonsterSpecies::eWorldBoss)
		//	return INVALID_GRID;
		//int index = alive->getGridIndex();
		//if (index/C_GRID_ROW >= C_GRID_COL-1)
		//	return INVALID_GRID;
		//int grid = getMonsterMoveGrid(alive);
		//return grid;
		return 0;
	}

	int MoveRule::getMonsterMoveGrid(BaseRole* alive)
	{
		//if (alive->getEnemy())
		//{
		//	int index = alive->getGridIndex();
		//	int row = index % C_GRID_ROW;
		//	int col = index / C_GRID_ROW;
		//	int Index = INVALID_GRID;
		//	if (!alive->getMove())
		//		return INVALID_GRID;
		//	for (int i = 1;i <= 1;i++)			//一个个区域逐个去进行判断，判断每一个区域的移动范围
		//	{
		//		col += 1;
		//		if(col >= C_GRID_COL-1||col < 0) 
		//			break;
		//		int TarIndex = col * C_GRID_ROW + row;
		//		int Cgrid = MoveJudge(alive,TarIndex);
		//		if (Cgrid == INVALID_GRID)break;
		//		Index = Cgrid;
		//	}
		//	return Index;
		//}
		return INVALID_GRID;
	}

	int MoveRule::MoveJudge(BaseRole* pRole,int grid)
	{
		if (pRole->getOtherCamp())						//怪物做一个击退的边界判断
		{
			vector<int> tGrdis;
			pRole->initContainGrid(grid,tGrdis);
			if (tGrdis.empty())
				return INVALID_GRID;
			return grid;
		}
		BaseRole* tRole = BattleManage->getAliveByGrid(grid);
		if( tRole == nullptr|| tRole == pRole ||tRole->getEnemy())
		{
			return CountMoveGrid(pRole,grid);	
		}else{
			return INVALID_GRID;
		}
	}

	int MoveRule::CountMoveGrid(BaseRole* alive,int grid)
	{
		if (alive->mStandGrids.size()>1)							//在判断多个格子情况
		{
			int us_grid = alive->getGridIndex();
			int row = us_grid % C_GRID_ROW;				
			int col = us_grid / C_GRID_ROW;				
			int t_Row = grid % C_GRID_ROW;
			int t_Col = grid / C_GRID_ROW;
			int c_row = t_Row - row;					//行变化量
			int c_col = t_Col - col;					//列变化量

			int c_Num = grid - alive->getGridIndex();		//标记格子变化量
			bool canMove = true;
			for (int j=1;j < alive->mStandGrids.size(); j++)
			{
				int us_point = alive->mStandGrids.at(j);
				int us_row = us_point % C_GRID_ROW;				
				int us_col = us_point / C_GRID_ROW;
				if (us_row+c_row >= C_GRID_ROW || 
					us_row+c_row < 0 ||
					us_col+c_col >= C_GRID_COL-1 ||
					us_col+c_col < 0)
				{
					canMove = false;						//判断行列变化是否超出边界
					break;
				}
				BaseRole* p_Alive = BattleManage->getAliveByGrid(us_point+c_Num);
				if (alive->getEnemy())
				{
					if (alive->getGridIndex()<C_BEGINGRID)
						return grid;
					if(p_Alive&&p_Alive->getHp()>0&&!p_Alive->getEnemy())//四格有一个为重复
					{
						canMove = false;				//4个格子中每个格子移动后的坐标有人都不能移动
						break;
					}		
				}else{
					if(p_Alive&&p_Alive->getHp()>0&&p_Alive != alive)//四格有一个为重复
					{
						canMove = false;				//4个格子中每个格子移动后的坐标有人都不能移动
						break;
					}
				}
			}
			if(!canMove)return INVALID_GRID;		//不能移动退出循环不继续判断
			return grid;
		}else{
			return grid;
		}
	}

	int MoveRule::FrontBack(BaseRole* tRole,int range,bool enemy)
	{
		int row = tRole->getGridIndex() % C_GRID_ROW;
		int col = tRole->getGridIndex() / C_GRID_ROW;
		int Index = INVALID_GRID;
		for (int i = 1;i <= abs(range);i++)			//一个个区域逐个去进行判断，判断每一个区域的移动范围
		{
			if (enemy)
			{
				if (range > 0)
				{
					col += 1;
				}else{
					col -= 1;
				}
			}else{
				if (range > 0)
				{
					col -= 1;
				}else{
					col += 1;
				}
			}
			if(col > C_GRID_COL-1||col < C_GRID_COL_MIN) break;
			int TarIndex = col * C_GRID_ROW + row;
			int tGrid = MoveJudge(tRole,TarIndex);
			if (tGrid == INVALID_GRID)break;
			Index = tGrid;
		}
		return Index;
	}
	//得到当前位置附近,并判断是否有人的格子
	int MoveRule::getCurrRandomGrid(int grid,bool hasAlive/*=false*/)
	{
		return 0;
	}
}