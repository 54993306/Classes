/************************************************************* 
 *
 *
 *		Data : 2016.8.23
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/Landform/TrapLayer.h"
#include "Battle/BattleCenter.h"
#include "Battle/CoordsManage.h"
#include "Battle/Landform/TrapManage.h"
#include "Battle/Landform/TrapObject.h"
#include "Battle/Landform/BattleTrap.h"
#include "Battle/ConstNum.h"

namespace BattleSpace
{
	TrapLayer::TrapLayer()
	{

	}

	TrapLayer::~TrapLayer()
	{
		
	}

	bool TrapLayer::init()
	{
		initBattleTrap();
		return CCNode::init();
	}

	void TrapLayer::initBattleTrap()
	{
		mTrapObjects = CCNode::create();
		this->addChild(mTrapObjects);
		map<int,BattleTrap*> tMasp = BattleTrapManage->getBattleTraps();
		for (auto tPair : tMasp)
		{
			TrapObject* tTrapObject = TrapObject::createTrapObject(tPair.second);
			const CCPoint& p = BattleCoords->getPoint(tPair.second->getGridIndex());
			tTrapObject->setPosition(ccpAdd(p,ccp(0,-GRID_HEIGHT/2)));
			mTrapObjects->addChild(tTrapObject);
		}
	}
}