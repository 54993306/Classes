#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_
/************************************************************* 
 *
 *
 *		Data : 2016.8.22
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 地图坐标管理器
 *
 *
 *************************************************************/

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
namespace BattleSpace
{
	class Coords;
	class CoordsManage : public CCObject
	{
	protected:
		CoordsManage();
		virtual bool init();
	public:
		virtual ~CoordsManage();
		CREATE_FUNC(CoordsManage);
	public:
		void initMap(int pStageID);					//根据关卡id初始化战斗地图
		int getCoordsRow() const;
		int getCoordsCol() const;
		int getCoordsWith() const;
		int getCoordsHight() const;
		void clearCoords();
	public:
		int getGridIndex(const CCPoint& pPoint) const;
		const CCPoint& getPoint( int pGrid) const;
		float CoordsMax() const;
		float CoordsMin() const;
	protected:
		void AddCoords(Coords* pCoords);
		Coords* getCoords(int pCoordsID);
	protected:
		map<int,Coords*> mCoordsMap;
		Coords* mDefultData;
		Coords* mCurrCoords;
	};
};
#endif // !_MAP_MANAGER_H_
