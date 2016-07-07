#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "model/BattleData.h"
#include "mainCity/RecruitData.h"

typedef void (CCObject::*newHeroEffectCallBack)();
#define newHeroEffectCallBack_selector(C_FUN) (newHeroEffectCallBack)(&C_FUN)

class CNewHero : public BaseLayer
{
public:
	CNewHero();
	CREATE_LAYER(CNewHero);
	bool init();

	void onEnter();

	void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);

	CC_SYNTHESIZE(bool, m_bEvolve, Evolve);

	void bindCallback(CCObject* pTarget, newHeroEffectCallBack pFunCall);

public:
	void showNewHeroEffect(HeroLotteryData* pHero);
	void recoverPosition(CCNode *pNode);
	void showEvolveEffect();
	void setIsShare(bool isShare);
private:
	void callBackForEffectShow();
	void showEffectCallBack();
	void hideOrShow(bool bShow);
	void shareFb( CCObject* pObj );
	void effectFinish();

	void checkCallBackBind();

private:
	CLayout *m_ui;
	CCSprite *m_pEffect;
	HeroLotteryData m_pHero;
	bool m_bShowing;
	bool m_isFinish;
	bool m_isShare;
	CCObject* m_pTarget;
	newHeroEffectCallBack m_pNewHeroEffectCallback;
};