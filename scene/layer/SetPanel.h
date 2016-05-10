#ifndef _SET_PANEL_H_
#define _SET_PANEL_H_
#include "AppUI.h"
class SetPanel:public CCNode,public CCTargetedTouchDelegate
{
public:
	SetPanel();
	virtual ~SetPanel();
	virtual bool init();
	CREATE_FUNC(SetPanel);
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
	void onClick(CCObject* ob);
protected:
	CLayout* m_panel;
};
#endif // !_SET_PANEL_H_
