
#ifndef __HEROBREAK_
#define __HEROBREAK_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CHeroBreak: public BaseLayer
{
public:
	CREATE_LAYER(CHeroBreak);
    virtual bool init();
    void onEnter();	
    void onExit();
	void levelSkillUp(CHero* hero,const Hero* pHero);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void progressChange(CCObject* pSender, int nValue);
	void hideflicker(CCNode* node);
	void runFlickerAction(float dt);
	void hideAnim(CCNode* node);
	void showIncrease(CCNode* node);
	void runStarAnim(float dt);
protected:
private:
	CLayout *m_ui;
	CHero *m_hero;
	CHero m_reshero;
	CCSprite *m_lightSpr;
	CCSprite *m_garrowSpr;
};

#endif