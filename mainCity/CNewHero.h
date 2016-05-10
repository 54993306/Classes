#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "model/BattleData.h"
#include "mainCity/RecruitData.h"

class CNewHero : public BaseLayer
{
public:
	CNewHero();
	CREATE_LAYER(CNewHero);
	bool init();

	void onEnter();

	void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	CC_SYNTHESIZE(bool, m_bEvolve, Evolve);
public:
	void showNewHeroEffect(HeroLotteryData* pHero);
	void recoverPosition(CCNode *pNode);
	void showEvolveEffect();
private:
	void callBackForEffectShow();
	void showEffectCallBack();
	void hideOrShow(bool bShow);
private:
	CLayout *m_ui;
	CCSprite *m_pEffect;
	HeroLotteryData m_pHero;
	bool m_bShowing;
};