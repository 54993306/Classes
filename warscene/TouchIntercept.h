

// ***************************************************************
// 触摸拦截层
// ***************************************************************
#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class TouchIntercept : public CCLayer
{
public:
	TouchIntercept(int _touchPriority, CCRect BigRect, CCRect SmallRect, bool _isOppopset, bool isTest);
	~TouchIntercept();
	static TouchIntercept* create(int _touchPriority, CCRect BigRect, CCRect SmallRect, bool _isOppopset = false, bool isTest = false);
	virtual void onEnter();
	virtual void onExit();
	virtual void draw();
	virtual void update(float dt);
	virtual void ccTouchEnded(CCTouch * pTouch, CCEvent* pEvent);
	virtual bool ccTouchBegan(CCTouch * pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch * pTouch, CCEvent* pEvent);
	virtual void registerWithTouchDispatcher();
private:
	int m_touchPriority;
	CCRect m_SmallRect;			//小块(拦截区域)
	CCRect m_BigRect;				//大块
	void initView();
	bool isInIntercept(CCPoint location);//是否在小块外面
	bool isInContent(CCPoint location);//是否在大块外面
	bool m_isIntercept;//是否会屏蔽
	bool m_isOpposet;//是否反转屏蔽区
	bool m_isTest;//是否显示测试颜色块，便于定位
};