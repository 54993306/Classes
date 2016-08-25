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
	//������Խ��ж�̬���Ĵ���
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
	//��ͼ�����½�Ϊԭ�㣬����ΪX����������ΪY������,���������Ͻ�Ϊ��һ������
	void Coords::initGrids()
	{
		int i = 0;
		int j = 0;
		float MapMax_Y = float(mCoordsHeight) - (float(mCoordsHeight) - ((GRID_HEIGHT)*4))/2 - 20;			//��ͼ��ߴ���Yֵ
		for(i = 0; i < mCoordsCol / 2; ++i)
		{
			for(j = 0; j < mCoordsRow; ++j)																//�����������
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = ((mCoordsCol/2-1) - i )* C_GRID_ROW +j;
				addPoint(index, -dx ,MapMax_Y - dy);											//���м俪ʼ������Xֵ,����������Yֵ
			}
		}
		for(i = 0; i < mCoordsCol / 2; ++i)																//���Ұ�������
		{
			for(j = 0; j < mCoordsRow; ++j)
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = mCoordsCol*mCoordsRow/2+i*C_GRID_ROW+j;
				addPoint(index, dx ,MapMax_Y - dy);												//���м俪ʼ��ǰ��Xֵ,����������Yֵ
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