
#include "Battle/RoleObject/RageObject.h"
#include "Battle/ConstNum.h"
#include "Battle/BaseRole.h"
namespace BattleSpace
{
	RageObject::RageObject():mProgress(nullptr),mBackdrop(nullptr),mPercent(0)
		,mRageMax(100),mRageValue(0)
	{}

	RageObject::~RageObject(){}

	bool RageObject::init()
	{
		mBackdrop = CCSprite::create("public/black.png");
		this->addChild(mBackdrop);

		CCSprite* tFrontImage = CCSprite::create("warScene/bar_yellow.png");
		mProgress = CCProgressTimer::create(tFrontImage);
		mProgress->setType(kCCProgressTimerTypeBar);		//条形进度条
		setDirection(R_left);
		mProgress->setPercentage(mRageValue);
		this->addChild(mProgress);
		return true;
	}

	void RageObject::setDirection(int pDirection)
	{
		switch (pDirection)
		{
		case R_up:						//向上为增
			{
				mProgress->setBarChangeRate(ccp(0,1));		//变化方向为Y轴
				mProgress->setMidpoint(ccp(0,0));			//中心点,越靠近中心点值越小
			}break;
		case R_Down:
			{
				mProgress->setBarChangeRate(ccp(0,1));
				mProgress->setMidpoint(ccp(0,1));
			}break;
		case R_left:
			{
				mProgress->setBarChangeRate(ccp(1,0));
				mProgress->setMidpoint(ccp(1,0));
			}break;
		case R_Right:
			{
				mProgress->setBarChangeRate(ccp(1,0));
				mProgress->setMidpoint(ccp(0,0));
			}break;
		}
	}

	void RageObject::setRageValue(float pRageValue)
	{
		if(mRageValue == pRageValue)return;
		mRageValue = pRageValue;
		updateRage();
	}

	void RageObject::setRageMax(float pMaxValue)
	{
		if(!pMaxValue || mRageMax == pMaxValue) 
			return;
		mRageMax = pMaxValue;
		updateRage();
	}

	void RageObject::setPercent( float pPercent )
	{
		if (mPercent == pPercent)
			return;
		mPercent = pPercent;
		mProgress->setPercentage(mPercent);
	}

	void RageObject::updateRage()
	{
		mPercent = mRageValue / mRageMax * 100;
		CCProgressFromTo* proto = CCProgressFromTo::create(0.3f,mProgress->getPercentage(),mPercent);
		mProgress->runAction(proto);
	}

	void RageObject::setDisplay(bool pDisplay)
	{
		mProgress->setVisible(pDisplay);
		mBackdrop->setVisible(pDisplay);
	}

	CCProgressTimer* RageObject::getPro()
	{
		return mProgress;
	}

}
