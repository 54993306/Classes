/************************************************************* 
*
*
*		Data : 2016.8.22
*	
*		Name : Coords
*
*		Author : Lin_Xiancheng
*
*		Description : 
*
*
*************************************************************/

#ifndef __Coords_H_
#define __Coords_H_
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
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

	enum struct sCoordsType
	{
		eNull					= 0,
		eRectangle				= 1,//默认战斗地图类型
	};

	class Coords : public CCObject
	{
	protected:
		Coords(sCoordsType pType);
		Coords();
	public:
		virtual ~Coords();
		virtual bool init();
		static Coords* createCoords(sCoordsType pType);
		static Coords* create();
		const sCoordsType getCoordsType() const;
	public:
		virtual void initGrids();
		virtual const CCPoint& getPoint( int grid) const;
		virtual void addPoint(int grid,float x,float y);
		virtual int getGridIndex(const CCPoint& p) const;
		virtual const CCRect& getRect(int grid) const;
		virtual bool hasGrid(int grid) const;
		CC_SYNTHESIZE(int,mCoordsID,MapID);
		CC_SYNTHESIZE(int,mCoordsWidth,CoordsWidth);		//这些信息应该从服务器或是配置上过来
		CC_SYNTHESIZE(int,mCoordsHeight,CoordsHeight);
		CC_SYNTHESIZE(int,mCoordsRow,CoordsRow);			//行数默认4行
		CC_SYNTHESIZE(int,mCoordsCol,CoordsCol);			//列数默认8列
	protected:
		map<int,CCPoint> mPointGrid;						//格子对应位置
		map<int,CCRect> mRectGrid;
		sCoordsType mCoordsType;
	};
};
#endif