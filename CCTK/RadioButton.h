#ifndef __CCWIDGET_RADIOBUTTON_H__
#define __CCWIDGET_RADIOBUTTON_H__

#include "cocos2d.h"
#include "CheckBox.h"
NS_CC_WIDGET_BEGIN
class CRadioButton:public CCheckBox
{
public:
	CRadioButton(){}
	virtual ~CRadioButton(){}
	static CRadioButton* create();
	virtual CWidgetTouchModel onTouchBegan(CCTouch* pTouch);
	virtual void onTouchMoved(CCTouch* pTouch, float fDuration);
	virtual void onTouchEnded(CCTouch* pTouch, float fDuration);
	virtual void onTouchCancelled(CCTouch* pTouch, float fDuration);

	CC_WIDGET_LONGCLICK_SCHEDULE(CRadioButton);
protected:
private:
};

NS_CC_WIDGET_END
#endif