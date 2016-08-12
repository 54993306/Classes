#pragma once
#include "cocos2d.h"

/************************************************************************
							µØÐÎ²ã				//  [9/9/2014 xc_lin]
************************************************************************/
namespace BattleSpace{
	class TerrainLayer : public cocos2d::CCNode
	{
	public:
		TerrainLayer();
		~TerrainLayer();
		CREATE_FUNC(TerrainLayer);
		void initTerrain();
		void addTerrainToGrid(int terrainID,char grid);
		void RemoveNullityTerrain(CCObject* ob);
	protected:
		bool init();

	private:

	};
};