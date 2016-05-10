#include "RadioButton.h"

NS_CC_WIDGET_BEGIN

CRadioButton* CRadioButton::create()
{
	CRadioButton *pRet = new CRadioButton;
	if (pRet&&pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

CWidgetTouchModel CRadioButton::onTouchBegan(CCTouch* pTouch)
{
	CC_WIDGET_LONGCLICK_ONTOUCHBEGAN;

	if( isChecked() )
	{
		
	}
	else
	{
		if( m_pNormalPress )
		{
			SAFE_SET_VISIBLE(m_pNormalPress, true);
			SAFE_SET_VISIBLE(m_pNormal, false);
		}
	}

	return eWidgetTouchTransient;
}

void CRadioButton::onTouchMoved(CCTouch* pTouch, float fDuration)
{
	CC_WIDGET_LONGCLICK_ONTOUCHMOVED;

	CCPoint tPoint = m_pParent->convertToNodeSpace(pTouch->getLocation());
	if( boundingBox().containsPoint(tPoint) )
	{
		if(! isChecked() )
		{
			if( m_pNormalPress )
			{
				SAFE_SET_VISIBLE(m_pNormalPress, true);
				SAFE_SET_VISIBLE(m_pNormal, false);
			}
			else
			{
				SAFE_SET_VISIBLE(m_pNormal, true);
			}
		}
	}
	else
	{
		if( !isChecked() )
		{
			SAFE_SET_VISIBLE(m_pNormalPress, false);
			SAFE_SET_VISIBLE(m_pNormal, true);
		}
	}
}

void CRadioButton::onTouchEnded(CCTouch* pTouch, float fDuration)
{
	CC_WIDGET_LONGCLICK_ONTOUCHENDED;

	CCPoint tPoint = m_pParent->convertToNodeSpace(pTouch->getLocation());
	if( boundingBox().containsPoint(tPoint) )
	{
		if( !isChecked() )
		{
			setChecked(true);
			executeClickHandler(this);
		}
		
	}
	else
	{
		if( !isChecked() )
		{
			SAFE_SET_VISIBLE(m_pNormalPress, false);
			SAFE_SET_VISIBLE(m_pNormal, true);
			SAFE_SET_VISIBLE(m_pCheckedPress, false);
			SAFE_SET_VISIBLE(m_pChecked, false);
		}
	}
}

void CRadioButton::onTouchCancelled(CCTouch* pTouch, float fDuration)
{
	CC_WIDGET_LONGCLICK_ONTOUCHCANCELLED;

	if( !isChecked() )
	{
		SAFE_SET_VISIBLE(m_pNormalPress, false);
		SAFE_SET_VISIBLE(m_pNormal, true);
	}
}



NS_CC_WIDGET_END