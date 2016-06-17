#include "Battle/MoveRule.h"
#include "Battle/ConstNum.h"
#include "model/DataCenter.h"
#include "Battle/BaseRole.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/BattleMessage.h"
namespace BattleSpace{
	MoveRule::MoveRule(){}

	MoveRule::~MoveRule(){NOTIFICATION->removeAllObservers(this);}

	bool MoveRule::init()
	{
		return true;
	}

	bool MoveRule::MonstMoveExcute(BaseRole* monster)
	{
		if (!monster->getMove()||monster->getAliveStat() == INVINCIBLE)
			return false;
		int grid = monsterMove(monster);
		if( grid!= INVALID_GRID && grid < C_CAPTAINGRID)	
		{		
			if (monster->getMoveGrid() != grid)
				monster->setMoveGrid(grid);
			return true;
		}
		return false;
	}

	int MoveRule::monsterMove(BaseRole* alive)
	{
		//怪物的移动处理是，哪个最先返回值不是INVALID_GRID就以哪个为标准，后面的不做处理了
		if (alive->getAliveType() == E_ALIVETYPE::eWorldBoss)
			return INVALID_GRID;
		int index = alive->getGridIndex();
		if (index/C_GRID_ROW >= C_GRID_COL-1)
			return INVALID_GRID;
		int grid = getMonsterMoveGrid(alive);
		return grid;
	}

	int MoveRule::getMonsterMoveGrid(BaseRole* alive)
	{
		if (alive->getEnemy())
		{
			int index = alive->getGridIndex();
			int row = index % C_GRID_ROW;
			int col = index / C_GRID_ROW;
			int Index = INVALID_GRID;
			if (!alive->getMove())
				return INVALID_GRID;
			for (int i = 1;i <= 1;i++)			//一个个区域逐个去进行判断，判断每一个区域的移动范围
			{
				col += 1;
				if(col >= C_GRID_COL-1||col < 0) 
					break;
				int TarIndex = col * C_GRID_ROW + row;
				int Cgrid = MoveJudge(alive,TarIndex);
				if (Cgrid == INVALID_GRID)break;
				Index = Cgrid;
			}
			return Index;
		}
		return INVALID_GRID;
	}

	int MoveRule::MoveJudge(BaseRole* alive,int grid)
	{
		if (alive->getEnemy())
			return grid;
		BaseRole* targetAlive = DataCenter::sharedData()->getWar()->getAliveByGrid(grid);
		if( targetAlive == nullptr|| targetAlive == alive ||targetAlive->getEnemy())
		{
			return CountMoveGrid(alive,grid);	
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
				BaseRole* p_Alive = DataCenter::sharedData()->getWar()->getAliveByGrid(us_point+c_Num);
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

	int MoveRule::FrontBack(BaseRole* alive,int range,bool enemy)
	{
		int index = alive->getGridIndex();
		int row = index % C_GRID_ROW;
		int col = index / C_GRID_ROW;
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
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		return Index;
	}
	//得到当前位置附近,并判断是否有人的格子
	int MoveRule::getCurrRandomGrid(int grid,bool hasAlive/*=false*/)
	{
		int row = grid % C_GRID_ROW;
		int col = grid / C_GRID_ROW;

		CCArray* arr = CCArray::create();
		if ((row+1)<C_GRID_ROW)
			arr->addObject(CCInteger::create(col * C_GRID_ROW + row+1));
		if ((row-1)>=0)
			arr->addObject(CCInteger::create(col * C_GRID_ROW + row-1));
		if ((col+1)<C_GRID_COL)
		{
			arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row));
			if ((row+1)<C_GRID_ROW)
				arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row+1));
			if ((row-1)>=0)
				arr->addObject(CCInteger::create((col+1) * C_GRID_ROW + row-1));
		}	
		if ((col-1)>=0)
		{
			arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row));
			if ((row+1)<C_GRID_ROW)
				arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row+1));
			if ((row-1)>=0)
				arr->addObject(CCInteger::create((col-1) * C_GRID_ROW + row-1));
		}
		int pGrid = grid;
		do{
			pGrid = ((CCInteger*)arr->randomObject())->getValue();
			if (pGrid>=C_CAPTAINGRID)
				continue;
			if (!hasAlive)
			{
				return pGrid;
			}else{
				if (!DataCenter::sharedData()->getWar()->getAliveByGrid(pGrid))			//得到当前格子为无人格子
				{
					CCArray* arr = DataCenter::sharedData()->getWar()->getAlivesByCamp(false,true);
					CCObject* obj = nullptr;
					bool pMoveGrid = false;
					CCARRAY_FOREACH(arr,obj)
					{
						BaseRole* alive = (BaseRole*)obj;
						if (alive->getMoveGrid() == pGrid)								//且无人将要过去
							pMoveGrid = true;
					}
					if (pMoveGrid)continue;
					break;
				}	
			}	
		} while (true);
		return pGrid;
	}
}