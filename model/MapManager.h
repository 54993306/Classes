#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
/** 
*        col-> 
*      +-------+-------+-------+-------+-------+-------+-------+ 
*  row |00     |01     |02     |03     |04     |05     |...      
*   |  |       |       |       |       |       |       |        
*   v  |       |       |       |       |       |       |        
*      +-------+-------+-------+-------+-------+-------+-------+ 
*      |10     |11     |12     |13     |14     |15     |...      
*      |       |       |       |       |       |       |        
*      |       |       |       |       |       |       |        
*      +-------+-------+-------+-------+-------+-------+-------+ 
*      |20     |21     |22     |23     |24     |25     |...      
*      |       |(xp,yp)|       |       |       |       |        height = 92
*      |       |       |       |       |       |       |        
*      +-------+-------+-------+-------+-------+-------+-------+ 
*  y   |40     |41     |42     |43     |44     |45     |...     
*  ^   |       |       |       |       |       |       |        
*  |   |       |       |       |       |       |       |        
*  |   o-------+-------+-------+-------+-------+-------+-------+ 
*  |  origin                     width = 108
*  +------------>x 
* (0,0) 
*/  
//  [9/9/2014 xc_lin]
/**************************地图基础数据*************************/

class BaseMapData : public CCObject
{
public:
	BaseMapData();
	virtual ~BaseMapData(){};  
	CC_SYNTHESIZE(unsigned int,m_Id,MapID);
	CC_SYNTHESIZE(unsigned int,m_Width,Width);
	CC_SYNTHESIZE(unsigned int,m_Height,Height);
protected:
	inline bool init(){return true;}
protected:
	string m_MidRes;//中间层资源路径
};

/**************************战斗地图数据*************************/

class WarMapData:public BaseMapData
{
public:
	WarMapData(){};
	virtual ~WarMapData();
	CREATE_FUNC(WarMapData);
	const CCPoint& getPoint( int grid);
	void addPoint(int grid,float x,float y);
	int getGridIndex(const CCPoint& p);
	const CCRect& getRect(int grid);
	bool hasGrid(int grid);
protected:
	map<int,CCPoint> m_gridPoint;//格子对应位置
	map<int,CCRect>m_gridRect;
};

/**************************地图管理***********************************/
typedef map<unsigned int,BaseMapData*> MapType;
class MapManager : public CCObject
{
public:
	MapManager();
	virtual ~MapManager();
	CREATE_FUNC(MapManager);
public:
	WarMapData* getCurrWarMap();
	void addMap(BaseMapData* map);
	BaseMapData* getMap( unsigned int mapID);
	inline void setCurrWarMap(unsigned int mapID){m_currWarMap = mapID;}
	CC_SYNTHESIZE(int,m_row,Row);		//行数默认4行
	CC_SYNTHESIZE(int,m_col,Col);		//列数默认8列
	CC_SYNTHESIZE(int,m_mapwith,MapWith);
	CC_SYNTHESIZE(int,m_maphight,MapHight);	
	void initMap(int levelId);    //根据关卡id初始化战斗地图
	void clearMap();
protected:
	bool init();
	MapType m_list;
	unsigned int m_currWarMap;
};
#endif // !_MAP_MANAGER_H_
