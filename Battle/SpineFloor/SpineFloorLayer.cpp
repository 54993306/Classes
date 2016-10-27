/************************************************************* 
 *
 *	
 *		Name : SpineFloorLayer
 *
 *		Author : Lin_Xiancheng
 *
 *		Time : 	10/22/2016 
 *
 *		Description : 
 *
 *
 *************************************************************/
#include "Battle/SpineFloor/SpineFloorLayer.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleCenter.h"
#include "Battle/CoordsManage.h"
#include "Battle/BattleMessage.h"
#include "Battle/SpineFloor/SpineEffect.h"

namespace BattleSpace
{
	SpineFloorLayer::SpineFloorLayer()
	{

	}

	SpineFloorLayer::~SpineFloorLayer()
	{
		bNotification->removeAllObservers(this);
	}

	bool SpineFloorLayer::init()
	{
		bNotification->addObserver(this,callfuncO_selector(SpineFloorLayer::initSpineEffect),MsgCreateSpineFloor,nullptr);
		return true ;
	}
	//在此处得到消息创建spine效果，创建出来后加上去即可
	void SpineFloorLayer::initSpineEffect( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		for (auto tGrid : tRole->mSkillArea)
		{	
			SpineEffect* tEffect = SpineEffect::createSpineEffect(tRole,tGrid);
			const CCPoint& p = BattleCoords->getPoint(tGrid);
			tEffect->setPosition(p);
			this->addChild(tEffect);
		}
	}

};