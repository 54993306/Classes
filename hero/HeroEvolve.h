
#ifndef __HEROEVOLVE_
#define __HEROEVOLVE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "hero/HeroData.h"

class CNewHero;

class CHeroEvolve: public BaseLayer
{
public:
	CHeroEvolve();
	CREATE_LAYER(CHeroEvolve);
    virtual bool init();
    void onEnter();
    void onEvolveBtn(CCObject* pSender);
    void onExit();

	void showHeroHead(CHero *hero);

	void showHeroQuality(CHero * hero);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	bool checkHeroEvolveOpen(CHero * hero);

	void updateAttr(CHero * hero);

	void levelSkillUp(CHero* hero,const Hero* pHero);
	void showUpLevelAnimation();
	void runAnimation(float dt, CCNode * node);
	void compaseSuccess(CCObject* pObj);
	void showItem(CItem * item);
	void setNeedNum(int num);

	void updateNextQualityAttr(CHero* hero);

	void askForData();
	void askForDataCallBack();

	CC_SYNTHESIZE(bool, m_bAskNextQualityHero, AskNextQualityHero);

	void heroCall(CCObject* pSender);

	void showEvoleEffect();
	void showEvoleEffectCallBack();
	void frameEventCallBack(CCBone* pBone, const char* sEvent, int iTag1, int iTag2);
	void outCallBack();
protected:
	void updateHeroInfo(const TMessage& tMsg);
	void onClose(CCObject* pSender);

private:
	CLayout *m_ui;
	CHero *m_hero;
	CLayout *m_cell;
	CTableView *m_tableView;
	CItem m_item;
	int m_iType;
	int m_itemNeedNum;
	CNewHero* m_pNewHero;
	CCArmature* m_pHeroArmature;
};

#endif