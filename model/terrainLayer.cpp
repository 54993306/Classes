
#include "terrainLayer.h"
#include "Battle/BattleRole.h"
#include "model/DataCenter.h"
#include "scene/effect/EffectObject.h"
#include "model/MapManager.h"
#include "model/WarManager.h"
#include "model/TerrainManager.h"
using namespace cocos2d;
namespace BattleSpace{
	TerrainLayer::TerrainLayer()
	{
	}

	TerrainLayer::~TerrainLayer()
	{
	}
	//有添加还有移除的过程。地形失效要移除地形
	void TerrainLayer::addTerrainToGrid(int terrainID,char grid)
	{
		WarMapData* map = DataCenter::sharedData()->getMap()->getCurrWarMap();
		const CCPoint& p = map->getPoint(grid);
		EffectObject* eff = EffectObject::create(ToString(terrainID),PLAYERTYPE::Repeat);
		eff->setPosition(p);
		eff->setScale(0.5f);
		eff->setTag(grid);
		this->addChild(eff);
	}

	void TerrainLayer::initTerrain()
	{
		CCArray* arr = DataCenter::sharedData()->getTer()->getTerrains();
		CCObject* object = nullptr;
		CCARRAY_FOREACH(arr,object)
		{
			Terrain* tar = (Terrain*)object;
			if(!tar) continue;
			addTerrainToGrid(tar->getTerEffect(),tar->getGridIndex());
		}
	}

	bool TerrainLayer::init()
	{
		bool res = CCNode::init();
		do 
		{
			initTerrain();
		} while (0);
		return res;
	}
	//移除无效地形
	void TerrainLayer::RemoveNullityTerrain(CCObject* ob)
	{
		CCInteger* grid = (CCInteger*)ob;
		Terrain* ter = DataCenter::sharedData()->getTer()->getTerrainByGrid(grid->getValue());
		if (!ter->getExistNum() || !ter->getTriggerNum())
		{
			this->removeChildByTag(grid->getValue());
			DataCenter::sharedData()->getTer()->removeTerrainById(grid->getValue());
		}
	}
}