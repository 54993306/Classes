#include "MapManager.h"
#include "Battle/ConstNum.h"
namespace BattleSpace{
	BaseMapData::BaseMapData()
		:m_Height(0)
		,m_Id(0)
		,m_Width(0)
	{}
	WarMapData::~WarMapData()
	{
		m_gridPoint.clear();
	}
	const CCPoint& WarMapData::getPoint( int grid)
	{
		do{
			if (m_gridPoint.size() == 0)
			{
				return INVALID_POINT;
			}
			map<int,CCPoint>::iterator iter = m_gridPoint.find(grid);
			if(iter != m_gridPoint.end())
			{
				return iter->second;
			}
		} while (0);
		return INVALID_POINT;
	}
	void WarMapData::addPoint(int grid,float x,float y)
	{
		if (!hasGrid(grid))
		{
			m_gridPoint[grid] = CCPoint(x,y);
			m_gridRect[grid] = CCRect(x - GRID_WIDTH / 2,y - GRID_HEIGHT / 2,GRID_WIDTH,GRID_HEIGHT);
		}
	}
	int WarMapData::getGridIndex(const CCPoint& p)
	{
		for(map<int,CCPoint>::iterator iter = m_gridPoint.begin(); iter != m_gridPoint.end(); ++iter)
		{
			CCRect rect(iter->second.x - GRID_WIDTH / 2,iter->second.y - GRID_HEIGHT / 2,GRID_WIDTH,GRID_HEIGHT);
			if(rect.containsPoint(p))
			{
				return iter->first;
			}
		}
		return INVALID_GRID;
	}

	const CCRect& WarMapData::getRect(int grid)
	{
		do{
			if (m_gridRect.empty())
				return NULL_RECT;
			map<int,CCRect>::iterator iter = m_gridRect.find(grid);
			if(iter != m_gridRect.end())
			{
				return iter->second;
			}
		} while (0);
		return NULL_RECT;
	}

	bool WarMapData::hasGrid(int grid)
	{
		map<int,CCPoint>::iterator iter = m_gridPoint.find(grid);
		if(iter != m_gridPoint.end())
		{
			return true;
		}
		return false;
	}

	MapManager::MapManager()
		:m_currWarMap(0)
		,m_row(4)				//地圖縱向總個數4
		,m_col(6)				//地圖的橫向總個數6
		,m_mapwith(924)
		,m_maphight(640)
	{}
	MapManager::~MapManager(){}

	void MapManager::clearMap()
	{
		for(MapType::iterator iter = m_list.begin(); iter != m_list.end();++iter)
		{
			CC_SAFE_RELEASE(iter->second);
			iter->second = nullptr;
		}
		m_list.clear();
	}

	bool MapManager::init()
	{
		return true;
	}
	//把地图的0号位置去掉
	void MapManager::initMap(int levelId)
	{
		setMapWith(90+32*(GRID_WIDTH+C_GRIDOFFSET_X));//格子从中间向两边绘制							//关卡地图的长度,之前设计是变化的现在不变了
		setCol(32);
		//地图以左下角为原点，向右为X正方向，向上为Y正方向,格子以左上角为第一个格子
		WarMapData* map = WarMapData::create();
		int i = 0;
		int j = 0;
		float MapMax_Y = float(m_maphight) - (float(m_maphight) - ((GRID_HEIGHT)*4))/2 - 20;		//地图最高处的Y值
		for(i = 0; i < m_col / 2; ++i)
		{
			for(j = 0; j < m_row; ++j)																//算左半屏点阵
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = ((m_col/2-1) - i )* C_GRID_ROW +j;
				map->addPoint(index, -dx ,MapMax_Y - dy);											//从中间开始往回算X值,从上往下算Y值
			}
		}
		for(i = 0; i < m_col / 2; ++i)																//算右半屏点阵
		{
			for(j = 0; j < m_row; ++j)
			{
				float dx = ( i + 1 )*GRID_WIDTH + i*C_GRIDOFFSET_X - GRID_WIDTH/2;
				float dy = ( j + 1 )*GRID_HEIGHT + j*C_GRIDOFFSET_Y - GRID_HEIGHT/2;
				int index = m_col*m_row/2+i*C_GRID_ROW+j;
				map->addPoint(index, dx ,MapMax_Y - dy);												//从中间开始往前算X值,从上往下算Y值
			}
		}
		map->setMapID(100+levelId);
		map->setHeight(m_maphight);
		map->setWidth(m_mapwith);
		addMap(map);
		setCurrWarMap(100+levelId);
	}

	void MapManager::addMap(BaseMapData* map)
	{
		do{
			if(!map) break;
			BaseMapData* tpMap = getMap(map->getMapID());
			if( tpMap == map ) break;
			if( tpMap )
			{
				CCLOG("MapManager has exists Map:%d",map->getMapID());
				break;
			}
			map->retain();
			m_list[map->getMapID()] = map;
		} while (0);
	}
	BaseMapData* MapManager::getMap( unsigned int mapID)
	{
		MapType::iterator iter = m_list.find(mapID);
		if( iter != m_list.end() )
		{
			return iter->second;
		}
		return nullptr;
	}
	WarMapData* MapManager::getCurrWarMap()
	{
		return (WarMapData*)getMap(m_currWarMap);
	}

}
