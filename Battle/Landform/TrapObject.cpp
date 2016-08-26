/************************************************************* 
 *
 *
 *		Data : 2016.8.21
 *	
 *		Name : 
 *
 *		Author : Lin_Xiancheng
 *
 *		Description : 
 *
 *
 *************************************************************/

#include "Battle/Landform/TrapObject.h"
#include "Battle/SpineDataManage.h"
#include "Battle/BattleCenter.h"
#include "Battle/Landform/BattleTrap.h"
#include "scene/UITools.h"
namespace BattleSpace
{
	TrapObject::TrapObject( BattleTrap* pData ):mData(pData),mTrapNode(nullptr){}

	TrapObject::~TrapObject()
	{
		//CC_SAFE_RELEASE(mSpine);
		//mSpine = nullptr;
	}

	TrapObject* TrapObject::createTrapObject(BattleTrap* pData)
	{
		TrapObject* tTrapObject = new TrapObject(pData);
		if (tTrapObject && tTrapObject->init())
		{
			tTrapObject->autorelease();
			return tTrapObject;
		}else{
			delete tTrapObject;
			tTrapObject = nullptr;
			return nullptr;
		}
	}

	bool TrapObject::init()
	{
		if (SpineManage->isSpineModel(mData->getTrapModel()))
		{
			initSpineTrap();
		}else{
			initImageTrap();
		}
		return true;
	}

	void TrapObject::initSpineTrap()
	{
		SpineData* tData = SpineManage->getSpineData(ToString(mData->getTrapModel()));
		if (!tData)
		{
			tData = SpineManage->getSpineData("10000");
			CCLOG("[ *ERROR ]  TrapObject::init Spine Model=%d IS NULL",mData->getTrapModel()); 
		}
		SkeletonAnimation*  Animation = SkeletonAnimation::createWithData(tData->first);
		CCAssert(Animation,"TrapObject::init Spine NULL");
		Animation->setAnimation(0,"standby",true);
		Animation->setRotation(mData->getRotation());
		mTrapNode = Animation;
		this->addChild(mTrapNode);
	}

	void TrapObject::initImageTrap()
	{
		char path[60] = {0};
		sprintf(path,"Trap/%d.png",mData->getTrapModel());
		CCSprite* tTrapImage = CCSprite::create(path);
		tTrapImage->setRotation(mData->getRotation());
		mTrapNode = tTrapImage;
		this->addChild(mTrapNode);
	}

}