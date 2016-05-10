
#ifndef _TIP_STUFF_
#define _TIP_STUFF_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
class CStuffTip: public BaseLayer
{
public:
	CREATE_LAYER(CStuffTip);
	CStuffTip();
	~CStuffTip();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	void showItemProperty(CItem *item);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void onJump(CCObject* pSender);
	void DoNotPopItem();
private:
	CLayout *m_ui;
	bool m_isOutClose;
};

#endif