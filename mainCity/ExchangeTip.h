
#ifndef __EXCHANGE_LAYER_
#define __EXCHANGE_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"


class CExchangeTip: public BaseLayer
{
public:
	CREATE_LAYER(CExchangeTip);
	CExchangeTip();
    virtual bool init();
	void setData(int gold, int coin, int  time=5);
	void setBtnListener(CCObject *pListener, SEL_ClickHandler pHandler);
protected:
private:
	CLayout *m_ui;
};


#endif