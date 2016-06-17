#include "TouchIntercept.h"


TouchIntercept::TouchIntercept(int _touchPriority, CCRect BigRect, CCRect SmallRect, bool _isOpposet,bool _isTest)
: m_touchPriority(_touchPriority)
, m_SmallRect(SmallRect)
, m_BigRect(BigRect)
, m_isOpposet(_isOpposet)
, m_isTest(_isTest)
{
	if (SmallRect.origin.x < BigRect.origin.x
		|| SmallRect.origin.y < BigRect.origin.y
		|| SmallRect.size.width > BigRect.size.width
		|| SmallRect.size.height > BigRect.size.height)//С�鲻�ڴ����
	{
		CCLOG("[ *ERROR ] TouchIntercept::TouchIntercept");
		m_isIntercept = false;
	}
	else
	{
		m_isIntercept = true;
	}
}

TouchIntercept::~TouchIntercept()
{

}

TouchIntercept* TouchIntercept::create(int _touchPriority, CCRect _content, CCRect _interceptSize ,bool _isOpposet /* =false */, bool _isTest /* = false */)
{
	TouchIntercept* pRet = new TouchIntercept(_touchPriority, _content, _interceptSize, _isOpposet, _isTest);
	pRet->autorelease();
	return pRet;
}

void TouchIntercept::onEnter()
{
	CCLayer::onEnter();
	setTouchEnabled(true);
	scheduleUpdate();
}

void TouchIntercept::onExit()
{
	unscheduleUpdate();
	setTouchEnabled(false);
	CCLayer::onExit();
}

void TouchIntercept::ccTouchEnded(CCTouch * pTouch, CCEvent* pEvent)
{

}

bool TouchIntercept::ccTouchBegan(CCTouch * pTouch, CCEvent* pEvent)
{
	if (!m_isIntercept)//��ʼ��ʧ�ܣ�������
	{
		return true;
	}
	CCPoint location = pTouch->getLocation();
	if (!isInIntercept(location) && isInContent(location))//����С�飬���ڴ������δ���
	{
		if (!m_isOpposet)
		{
			return true;
		}
		else
		{
			return false;
		}
	
	}
	else
	{
		if (!m_isOpposet)
		{
			return false;
		}
		else
		{
			return true;
		}
		
	}
	
}

void TouchIntercept::ccTouchMoved(CCTouch * pTouch, CCEvent* pEvent)
{

}

void TouchIntercept::draw()
{

}

void TouchIntercept::update(float dt)
{

}

void TouchIntercept::registerWithTouchDispatcher()
{
	for (CCNode* parent = getParent(); parent != NULL; parent = parent->getParent())
	{
		m_touchPriority--;
	}
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_touchPriority, true);
	CCLayer::registerWithTouchDispatcher();
	
	initView();
}

void TouchIntercept::initView()
{
	if(m_isTest)
	{
		CCScale9Sprite* black = CCScale9Sprite::create("public/guide/red.png");
		black->setAnchorPoint(ccp(0, 0));
		black->setPreferredSize(CCSizeMake(m_BigRect.size.width, m_BigRect.size.height));
		black->setPosition(ccp(m_BigRect.origin.x, m_BigRect.origin.y));
		addChild(black, 99999);

		CCScale9Sprite* blue = CCScale9Sprite::create("public/guide/blue.png");
		blue->setAnchorPoint(ccp(0, 0));
		blue->setPreferredSize(CCSizeMake(m_SmallRect.size.width, m_SmallRect.size.height));
		blue->setPosition(ccp(m_SmallRect.origin.x, m_SmallRect.origin.y));
		addChild(blue, 99999);
	}
	
}

bool TouchIntercept::isInIntercept(CCPoint location)
{
	if (location.x > m_SmallRect.origin.x
		&& location.x < m_SmallRect.origin.x + m_SmallRect.size.width
		&& location.y > m_SmallRect.origin.y
		&& location.y < m_SmallRect.origin.y + m_SmallRect.size.height)
	{
		return true;//��С����
	}
	else
	{
		return false;//��С����
	}
}

bool TouchIntercept::isInContent(CCPoint location)
{
	if (location.x > m_BigRect.origin.x
		&& location.x < m_BigRect.origin.x + m_BigRect.size.width
		&& location.y > m_BigRect.origin.y
		&& location.y < m_BigRect.origin.y + m_BigRect.size.height)
	{
		return true;//�ڴ����
	}
	else
	{
		return false;//�ڴ����
	}
}