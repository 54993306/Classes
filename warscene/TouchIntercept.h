

// ***************************************************************
// �������ز�
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
	CCRect m_SmallRect;			//С��(��������)
	CCRect m_BigRect;				//���
	void initView();
	bool isInIntercept(CCPoint location);//�Ƿ���С������
	bool isInContent(CCPoint location);//�Ƿ��ڴ������
	bool m_isIntercept;//�Ƿ������
	bool m_isOpposet;//�Ƿ�ת������
	bool m_isTest;//�Ƿ���ʾ������ɫ�飬���ڶ�λ
};