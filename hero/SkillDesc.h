
#ifndef __SKILLDESC_
#define __SKILLDESC_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CSkillDesc: public BaseLayer
{
public:
	CREATE_LAYER(CSkillDesc);
    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
	
    void onExit();
    void showSkill(CHero* hero, int type);
	void updateSkillInfo(CSkill *skill, bool isPassive, int type);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected:
	void updateHeroInfo(const TMessage& tMsg);
private:
	CLayout *m_ui;
	CHero *m_hero;
	int m_type;

};

#endif