#include "RadioBtnGroup.h"

NS_CC_WIDGET_BEGIN

CRadioBtnGroup::CRadioBtnGroup()
{
	m_btnList = new std::vector<CRadioButton*>();
	m_currentBtn = nullptr;
}
CRadioBtnGroup::~CRadioBtnGroup()
{
	CC_SAFE_DELETE(m_btnList);
}

bool CRadioBtnGroup::init()
{
	if (CLayout::init())
	{
// 		setAnchorPoint(CCPointZero);
// 		setPosition(CCPointZero);
// 		setContentSize(CCDirector::sharedDirector()->getWinSize());
		setContentSize(CCWIDGET_LAYOUT_DEFAULT_CONTENT_SIZE);
		setAnchorPoint(CCWIDGET_LAYOUT_DEFAULT_ANCHOR_POINT);
		return true;
	}
	return false;
}

void CRadioBtnGroup::addButton(CRadioButton *btn,int tag)
{
	if (btn)
	{
		if (tag==-1)
		{
			btn->setTag(m_btnList->size());
		}
		else
		{
			btn->setTag(tag);
		}
		addChild(btn);
		m_btnList->push_back(btn);
	}
	
}

void CRadioBtnGroup::selectButton(CRadioButton* btn, bool isSelect)
{
	btn->setChecked(isSelect);
}

CRadioButton* CRadioBtnGroup::getItemByTag(int tag)
{
	std::vector<CRadioButton*>::iterator iter = m_btnList->begin();
	for (; iter!= m_btnList->end(); ++iter)
	{
		if ((*iter)->getTag()==tag)
		{
			return *iter;
		}
	}
	return nullptr;
}

CRadioButton* CRadioBtnGroup::getItmeByIndex(int index)
{
	if (index>=0&&index<m_btnList->size())
	{
		CRadioButton *btn = (*m_btnList)[index];
		return btn;
	}
	return nullptr;
}

CRadioButton* CRadioBtnGroup::itemForTouch(cocos2d::CCTouch * touch)
{
	CCPoint localPoint = convertToNodeSpace(touch->getLocation());
	std::vector<CRadioButton*>::iterator iter = m_btnList->begin();

	for (; iter!= m_btnList->end(); ++iter)
	{
		if ((*iter)->isVisible()&&(*iter)->isTouchEnabled()&&(*iter)->boundingBox().containsPoint(localPoint))
		{
			return *iter;
		}
	}
	return nullptr;
}

CWidgetTouchModel CRadioBtnGroup::onTouchBegan(CCTouch* pTouch)
{
	m_currentBtn = itemForTouch(pTouch);
	if (m_currentBtn)
	{
		m_currentBtn->executeTouchBeganHandler(pTouch);
		return eWidgetTouchSustained;
	}
	return eWidgetTouchNone;
}

void CRadioBtnGroup::onTouchMoved(CCTouch* pTouch, float fDuration)
{
	CRadioButton* btn = itemForTouch(pTouch);
	if (btn)
	{
		m_moveBtn = btn;
		btn->executeTouchMovedHandler(pTouch,fDuration);
		for (int i=0; i<m_btnList->size(); ++i)
		{
			if ((*m_btnList)[i]!=btn)
			{
				//selectButton((*m_btnList)[i],false);
				(*m_btnList)[i]->executeTouchCancelledHandler(pTouch,fDuration);
			}
		}
	}
}

void CRadioBtnGroup::onTouchEnded(CCTouch* pTouch, float fDuration)
{
	CRadioButton *btn = itemForTouch(pTouch);
	if (m_currentBtn&&btn&&btn==m_currentBtn)
	{
		selectButton(m_currentBtn,true);
		for (int i=0; i<m_btnList->size(); ++i)
		{
			if ((*m_btnList)[i]!=btn && (*m_btnList)[i]->isChecked())
			{
				selectButton((*m_btnList)[i],false);
			}
		}
		m_currentBtn = nullptr;
	}
	else if (m_currentBtn&&btn)
	{
		btn->executeTouchCancelledHandler(pTouch,fDuration);
	}
	else if (m_moveBtn&&!btn)
	{
		m_moveBtn->executeTouchCancelledHandler(pTouch,fDuration);
	}
}

void CRadioBtnGroup::onTouchCancelled(CCTouch* pTouch, float fDuration)
{
	/*
	CRadioButton *btn = itemForTouch(pTouch);
	std::vector<CRadioButton*>::iterator iter = m_btnList->begin();
	for (; iter!=m_btnList->end();++iter)
	{
		if ((*iter)==btn)
		{
			btn->onTouchCancelled(pTouch,fDuration);
		}
		else
		{
			(*iter)->onTouchCancelled(pTouch,fDuration);
		}
	}*/
}

void CRadioBtnGroup::setOnClickListener(CCObject* pListener, SEL_ClickHandler pHandler)
{
	for (int i=0; i<m_btnList->size(); ++i)
	{
		CRadioButton *btn = (*m_btnList)[i];
		btn->setOnClickListener(pListener, pHandler);
	}
}

void CRadioBtnGroup::setOnCheckListener(CCObject* pListener, SEL_CheckHandler pHandler)
{
	for (int i=0; i<m_btnList->size(); ++i)
	{
		CRadioButton *btn = (*m_btnList)[i];
		btn->setOnCheckListener(pListener, pHandler);
	}
}

NS_CC_WIDGET_END