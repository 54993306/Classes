
#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

class HeroAdvance: public BaseLayer
{
public:
	CREATE_LAYER(HeroAdvance);
	HeroAdvance();
    virtual bool init();
    void onEnter();
	void onExit();

	void showEffect();
	void showEffectCallBack();

	void onClose(CCObject* pSender);
private:
	CLayout* m_ui;
};