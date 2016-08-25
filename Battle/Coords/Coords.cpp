/************************************************************* 
*
*
*		Data : 2016.8.22
*	
*		Name : 
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/
#include "Battle/Coords/Coords.h"
#include "Battle/ConstNum.h"

namespace BattleSpace
{
	Coords::Coords(sCoordsType pType):mCoordsType(pType),mCoordsHeight(640),mCoordsID(0),mCoordsRow(4),mCoordsCol(32)
	{
		mCoordsWidth = 90+32*(GRID_WIDTH+C_GRIDOFFSET_X);
	}

	Coords::Coords():mCoordsHeight(640),mCoordsID(0),mCoordsRow(4),mCoordsCol(32),mCoordsType(sCoordsType::eRectangle)
	{
		mCoordsWidth = 90+32*(GRID_WIDTH+C_GRIDOFFSET_X);
	}

	Coords::~Coords()
	{
		mPointGrid.clear();
		mRectGrid.clear();
	}

	Coords* Coords::create()
	{
		Coords* tCoords = new Coords();
		if (tCoords && tCoords->init())
		{
			tCoords->autorelease();
			return tCoords;
		}else{
			delete tCoords;
			tCoords = nullptr;
			return nullptr;
		}
	}
	//这里可以进行多态化的处理
	Coords* Coords::createCoords( sCoordsType pType )
	{
		switch (pType)
		{
		case BattleSpace::sCoordsType::eNull:
			{
				return Coords::create();
			}break;
		case BattleSpace::sCoordsType::eRectangle:
			{
				return Coords::create();
			}break;
		default:
			{
				return Coords::create();
			}break;
		}
	}

	const sCoordsType Coords::getCoordsType() const
	{
		return mCoordsType;
	}

	bool Coords::init()
	{
		initGrids();
		return true;
	}
	//地图以左下角为原点，向右为X正方向，向上为Y正方向,格子以左上角为第一个格子
	void Coords::initGrids()
	{
		int i = 0;
		int j = 0;
		float MapMax_Y = float(mCoordsHeight) - (float(mCoordsHeight) - ((GRID_HEIGHT)*4))/2 - 20;			//地图最高处的Y值
		for(i = 0; i < mCoordsCol / 2; ++i)
		{
			for(j = 0; j < mCoordsRow; ++j)																//算左半屏点阵
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = ((mCoordsCol/2-1) - i )* C_GRID_ROW +j;
				addPoint(index, -dx ,MapMax_Y - dy);											//从中间开始往回算X值,从上往下算Y值
			}
		}
		for(i = 0; i < mCoordsCol / 2; ++i)																//算右半屏点阵
		{
			for(j = 0; j < mCoordsRow; ++j)
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = mCoordsCol*mCoordsRow/2+i*C_GRID_ROW+j;
				addPoint(index, dx ,MapMax_Y - dy);												//从中间开始往前算X值,从上往下算Y值
			}
		}
	}

	const CCPoint& Coords::getPoint( int grid) const
	{
		if (mPointGrid.empty())
			return INVALID_POINT;
		map<int,CCPoint>::const_iterator iter = mPointGrid.find(grid);
		if(iter != mPointGrid.end())
			return iter->second;
		return INVALID_POINT;
	}

	void Coords::addPoint(int grid,float x,float y)
	{
		if (!hasGrid(grid))
		{
			mPointGrid[grid] = CCPoint(x,y);
			mRectGrid[grid] = CCRect(x - GRID_WIDTH / 2,y - GRID_HEIGHT / 2,GRID_WIDTH,GRID_HEIGHT);
		}
	}

	int Coords::getGridIndex(const CCPoint& p) const
	{
		for (auto tPair : mRectGrid)
		{
			if (tPair.second.containsPoint(p))
				return tPair.first;
		}
		return INVALID_GRID;
	}

	const CCRect& Coords::getRect(int pGrid) const
	{
		if ( mRectGrid.empty() )
			return NULL_RECT;
		map<int,CCRect>::const_iterator iter = mRectGrid.find(pGrid);
		if(iter != mRectGrid.end())
			return iter->second;
		return NULL_RECT;
	}

	bool Coords::hasGrid(int grid)const
	{
		map<int,CCPoint>::const_iterator iter = mPointGrid.find(grid);
		if(iter != mPointGrid.end())
			return true;
		return false;
	}
}