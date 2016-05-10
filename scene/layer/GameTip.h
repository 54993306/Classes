#ifndef _GAMETIP_H_
#define _GAMETIP_H_

#include "AppUI.h"
USING_NS_CC;
#include "LayerManager.h"

class GameTip: public BaseLayer
{
public:
	GameTip();
	~GameTip();
	bool init();
	virtual void onCreate();
	//DEFINE_POPUPBOX_CLASS(GameTip);
	CREATE_LAYER(GameTip);
	void showTip(const char *str, CCPoint point,CCSize contSize);
	void goBack(CCObject *obj);
	/*
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		return true;
	}*/
private:
	CScrollView *m_scroll;
	CLayout *m_ui;
};




#endif
