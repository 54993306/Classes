#ifndef __CCWIDGET_RADIOBTNGROUP_H__
#define __CCWIDGET_RADIOBTNGROUP_H__

#include "cocos2d.h"
#include "RadioButton.h"
#include "Layout.h"
NS_CC_WIDGET_BEGIN
class CRadioBtnGroup:public CLayout
{
public:
	CRadioBtnGroup();
	~CRadioBtnGroup();
	bool init();

	CREATE_FUNC(CRadioBtnGroup);
	void addButton(CRadioButton *btn,int tag=-1);
	void selectButton(CRadioButton* btn, bool isSelect);
	//通过tag获取子控件
	CRadioButton* getItemByTag(int tag);
	//通过index获取子控件
	CRadioButton* getItmeByIndex(int index);

    CWidgetTouchModel onTouchBegan(CCTouch* pTouch);
    void onTouchMoved(CCTouch* pTouch, float fDuration);
    void onTouchEnded(CCTouch* pTouch, float fDuration);
    void onTouchCancelled(CCTouch* pTouch, float fDuration);

	void setOnClickListener(CCObject* pListener, SEL_ClickHandler pHandler);
	void setOnCheckListener(CCObject* pListener, SEL_CheckHandler pHandler);
protected:
	CRadioButton* itemForTouch(cocos2d::CCTouch * touch);
private:
	std::vector<CRadioButton*>* m_btnList;
	CRadioButton* m_currentBtn;
	CRadioButton* m_moveBtn;
};

NS_CC_WIDGET_END
#endif