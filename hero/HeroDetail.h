
#ifndef __HERODETAIL_
#define __HERODETAIL_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"

class CHeroDetail: public BaseLayer
{
public:
	~CHeroDetail();
	CREATE_LAYER(CHeroDetail);
    virtual bool init();
    void onEnter();
    void onClickButton(CCObject* pSender);
	
    void onExit();
    void showHeroInfo(CHero* hero);
	void updateHeroInfo(const TMessage& tMsg);
protected:
private:
	CLayout *m_ui;
	CHero *m_hero;
};

#endif