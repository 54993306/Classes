
#include "terrainLayer.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleCenter.h"
#include "Battle/EffectObject.h"
#include "Battle/MapManager.h"
#include "Battle/WarManager.h"
#include "Battle/Landform/TerrainManager.h"
using namespace cocos2d;
namespace BattleSpace{
	TerrainLayer::TerrainLayer()
	{
	}

	TerrainLayer::~TerrainLayer()
	{
	}
	//����ӻ����Ƴ��Ĺ��̡�����ʧЧҪ�Ƴ�����
	void TerrainLayer::addTerrainToGrid(int terrainID,char grid)
	{
		WarMapData* map = ManageCenter->getMap()->getCurrWarMap();
		const CCPoint& p = map->getPoint(grid);
		EffectObject* eff = EffectObject::create(ToString(terrainID),sPlayType::eRepeat);
		eff->setPosition(p);
		eff->setScale(0.5f);
		eff->setTag(grid);
		this->addChild(eff);
	}

	void TerrainLayer::initTerrain()
	{
		CCArray* arr =ManageCenter->getTer()->getTerrains();
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
	//�Ƴ���Ч����
	void TerrainLayer::RemoveNullityTerrain(CCObject* ob)
	{
		CCInteger* grid = (CCInteger*)ob;
		Terrain* ter = ManageCenter->getTer()->getTerrainByGrid(grid->getValue());
		if (!ter->getExistNum() || !ter->getTriggerNum())
		{
			this->removeChildByTag(grid->getValue());
			ManageCenter->getTer()->removeTerrainById(grid->getValue());
		}
	}
}