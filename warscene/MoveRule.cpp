#include "MoveRule.h"
#include "ConstNum.h"
#include "model/DataCenter.h"
#include "model/DataDefine.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "scene/alive/AliveDefine.h"
#include "Battle/BattleMessage.h"
MoveRule::MoveRule():m_testState(false){}

MoveRule::~MoveRule(){NOTIFICATION->removeAllObservers(this);}

bool MoveRule::init()
{
	NOTIFICATION->addObserver(this,callfuncO_selector(MoveRule::changeTestState),B_ChangeMoveState,nullptr);
	return true;
}

void MoveRule::changeTestState( CCObject* ob )
{
	m_testState = !m_testState;
}

bool MoveRule::MonstMoveExcute(WarAlive* monster)
{
	if (!monster->getMove()||monster->getAliveStat() == INVINCIBLE)
		return false;
	int grid = MonstMoveAreaDispose(monster);
	if( grid!= INVALID_GRID && grid < C_CAPTAINGRID)	
	{		
		if (monster->getMoveGrid() != grid)
			monster->setMoveGrid(grid);
		return true;
	}
	return false;
}

int MoveRule::MonstMoveAreaDispose(WarAlive* alive)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (m_testState)
		return INVALID_GRID;
#endif
	//怪物的移动处理是，哪个最先返回值不是INVALID_GRID就以哪个为标准，后面的不做处理了
	if (alive->getAliveType() == AliveType::WorldBoss)
		return INVALID_GRID;
	int index = alive->getGridIndex();
	if (index/C_GRID_ROW >= C_GRID_COL-1)
		return INVALID_GRID;
	int grid = One_FourTypeDispose(alive,0);
	return grid;
	if(grid != INVALID_GRID)
		return grid;
	grid = Two_FiveTypeDispose(alive,0);
	if(grid != INVALID_GRID)
		return grid;
	grid = ThreeTypeDispose(alive,0);
	if(grid != INVALID_GRID)
		return grid;
	grid = One_FourTypeDispose(alive,0,false);
	if(grid!= INVALID_GRID)
		return grid;
	grid = Two_FiveTypeDispose(alive,0,false);
	if(grid!= INVALID_GRID)
		return grid;	 
	return INVALID_GRID;
}

int MoveRule::One_FourTypeDispose(WarAlive* alive,int grid,bool typeOne)
{
	if (alive->getEnemy())
	{
		int index = alive->getGridIndex();
		int row = index % C_GRID_ROW;
		int col = index / C_GRID_ROW;
		int Index = INVALID_GRID;
		if (!alive->moves.at(TypeOne))
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
	/********************************************************************************/
	bool Enemy = alive->getEnemy();
	int range = alive->moves.at(TypeFour);
	if(typeOne)range = alive->moves.at(TypeOne);
	if (!range)return INVALID_GRID;
	int targetRow = grid % C_GRID_ROW;
	int targetCol = grid / C_GRID_ROW;
	if (Enemy)
	{
		int index = alive->getGridIndex();
		int row = index % C_GRID_ROW;
		int col = index / C_GRID_ROW;
		int Index = INVALID_GRID;
		for (int i = 1;i <= range;i++)			//一个个区域逐个去进行判断，判断每一个区域的移动范围
		{
			if (typeOne)
			{
				col += 1;
			}else{
				col -= 1;
			}
			if(col >= C_GRID_COL||col < 0) break;
			int TarIndex = col * C_GRID_ROW + row;
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		return Index;
	}else{
		int index = 0/*DataCenter::sharedData()->getWar()->getPoint(alive->getAliveID())*/;
		if (!index)return INVALID_GRID;
		int row = index % C_GRID_ROW;				//得到当前英雄行数，从上往下第一行为0
		int col = index / C_GRID_ROW;				//得到当前英雄列数，从左往右第一列为0
		if (typeOne)
		{
			if (targetRow == row&&col <= targetCol&&targetCol <=col+range)
				return grid;
		}else{
			if (targetRow == row&&col >= targetCol&&targetCol >=col-range)
				return grid;
		}
	}
	return INVALID_GRID;
}

int MoveRule::Two_FiveTypeDispose(WarAlive* alive,int grid,bool typeTwo)
{
	int range = alive->moves.at(TypeFive);
	if(typeTwo)range = alive->moves.at(TypeTwo);
	bool Enemy = alive->getEnemy();
	if (!range)return INVALID_GRID;
	int targetRow = grid % C_GRID_ROW;
	int targetCol = grid / C_GRID_ROW;
	if (Enemy)
	{
		int index = alive->getGridIndex();				    //多格子标记的那个
		int row = index % C_GRID_ROW;
		int col = index / C_GRID_ROW;
		int Index = INVALID_GRID;
		//从小到大逐个判断，遇到有人则退出循环
		for (int i=1; i<=range ;i++)						//上对角 列大行小				
		{
			int changeCol = col-i;
			if (typeTwo)changeCol = col+i;
			if(changeCol >= C_GRID_COL||changeCol < 0||row-i < 0)break;
			int TarIndex = changeCol * C_GRID_ROW + row-i;
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		if(Index != INVALID_GRID )return Index;				//优先移动上对角

		for(int j=1; j<=range ;j++)							//下对角 列大行大
		{
			int changeCol = col-j;
			if (typeTwo)changeCol = col+j;
			if(changeCol >= C_GRID_COL||changeCol < 0||row+j >= C_GRID_ROW)break;
			int TarIndex = changeCol * C_GRID_ROW + row+j;
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		if(Index != INVALID_GRID )return Index;
	}else{
		int index = 0/*DataCenter::sharedData()->getWar()->getPoint(alive->getAliveID())*/;
		if (!index)return INVALID_GRID;
		int row = index % C_GRID_ROW;				//得到当前英雄行数，从上往下第一行为0
		int col = index / C_GRID_ROW;				//得到当前英雄列数，从左往右第一列为0
		for (int i=0;i<=range;i++)
		{
			if (typeTwo)
			{
				if (targetCol == col+i&&targetRow == row+i||targetCol == col+i&&targetRow == row-i)
					return grid;
			}else{
				if (targetCol == col-i&&targetRow == row-i||targetCol == col-i&&targetRow == row+i)
					return grid;
			}
		}
	}
	return INVALID_GRID;
}

int MoveRule::ThreeTypeDispose(WarAlive* alive,int grid)
{
	int range = alive->moves.at(TypeThree);
	bool Enemy = alive->getEnemy();
	if (!range)return INVALID_GRID;
	int targetRow = grid % C_GRID_ROW;
	int targetCol = grid / C_GRID_ROW;
	if (Enemy)
	{
		int index = alive->getGridIndex();
		int row = index % C_GRID_ROW;
		int col = index / C_GRID_ROW;
		int Index = INVALID_GRID;
		int uprow = row;
		for (int j=0; j<= range;j++)
		{
			uprow -= 1;
			if(uprow < 0) break;
			int TarIndex = col * C_GRID_ROW + uprow;
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		if(Index != INVALID_GRID )return Index;//优先上移
		int downrow = row;
		for (int i = 1;i <= range;i++)			//一个个区域逐个去进行判断，判断每一个区域的移动范围
		{
			downrow += 1;
			if(downrow >= C_GRID_ROW) break;
			int TarIndex = col * C_GRID_ROW + downrow;
			int Cgrid = MoveJudge(alive,TarIndex);
			if (Cgrid == INVALID_GRID)break;
			Index = Cgrid;
		}
		if(Index != INVALID_GRID )return Index;	
	}else{
		if (grid == 52)
			int q =0;
		int index = 0/*DataCenter::sharedData()->getWar()->getPoint(alive->getAliveID())*/;				//多格子怪物处理
		if (!index)return INVALID_GRID;
		int row = index % C_GRID_ROW;				//得到当前英雄行数，从上往下第一行为0
		int col = index / C_GRID_ROW;				//得到当前英雄列数，从左往右第一列为0
		if (targetCol == col&&targetRow <= row+range&&targetRow >= row-range)
			return grid;
	}
	return INVALID_GRID;
}

int MoveRule::MoveJudge(WarAlive* alive,int grid)
{
	WarAlive* targetAlive = DataCenter::sharedData()->getWar()->getAliveByGrid(grid);
	if( targetAlive == nullptr|| targetAlive == alive ||targetAlive->getEnemy())
	{
		return getMoveGrid(alive,grid);	
	}else{
		return INVALID_GRID;
	}
}

int MoveRule::getMoveGrid(WarAlive* alive,int grid)
{
	if (alive->grids.size()>1)							//在判断多个格子情况
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
		for (int j=1;j < alive->grids.size(); j++)
		{
			int us_point = alive->grids.at(j);
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
			WarAlive* p_Alive = DataCenter::sharedData()->getWar()->getAliveByGrid(us_point+c_Num);
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

int MoveRule::FrontBack(WarAlive* alive,int range,bool enemy)
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
				CCArray* arr = DataCenter::sharedData()->getWar()->getHeros(true);
				CCObject* obj = nullptr;
				bool pMoveGrid = false;
				CCARRAY_FOREACH(arr,obj)
				{
					WarAlive* alive = (WarAlive*)obj;
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

//返回-1是我方武将可移动，返回0表示不可移动，返回位置表示怪物的移动格子
bool MoveRule::HeroMoveDispose(WarAlive* alive,int grid)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	if (grid == INVALID_GRID || !alive)	return false;
	return true; 
#endif
	if (!alive->getBattle()&&!alive->getEnemy())
	{
		WarAlive* pAlive = DataCenter::sharedData()->getWar()->getAliveByGrid(grid);
		if (pAlive&&!pAlive->getEnemy())
			return false;
		if (grid >= /*C_STANDGRID*/96 )
			return true;
		return false;
	}
	if (grid == INVALID_GRID || !alive)	return false;
	WarAlive* target = DataCenter::sharedData()->getWar()->getAliveByGrid(grid);
	if (alive->getEnemy())
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		//if(MonstMoveAreaDispose(alive) == grid)return true;
		//return true;
#endif
		return false;
	}else{
		if(!alive->getMove())return false;
		if(target && target->getEnemy()) return false;					//目标格子有敌人
		if (alive->getCaptain())
		{
			if (grid >= C_CAPTAINGRID)
				return true;
			return false;
		}
		if (grid >= C_CAPTAINGRID)
			return false;
		else
			return true;
		//DataCenter::sharedData()->getWar()->addPoint(alive);			//记录武将当前回合站位
		if (One_FourTypeDispose(alive,grid)			== INVALID_GRID	&&	//不在所有区域范围内则返回false
			Two_FiveTypeDispose(alive,grid)			== INVALID_GRID	&&
			ThreeTypeDispose(alive,grid)			== INVALID_GRID	&&
			One_FourTypeDispose(alive,grid,false)	== INVALID_GRID	&&
			Two_FiveTypeDispose(alive,grid,false)	== INVALID_GRID	)return false;
		return true;
	}
}