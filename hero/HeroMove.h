
#ifndef __HEROMOVE_
#define __HEROMOVE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CHeroMove: public BaseLayer
{
public:
	CREATE_LAYER(CHeroMove);
    virtual bool init();
    void onEnter();
    void onClickButton(CCObject* pSender);
	
    void onExit();
    void showMoveDirection(CHero* hero);
	void displayMove(CLayout * upLay, int move1, int move2, int move3, int move4, int move5);
	void updateHeroInfo(const TMessage& tMsg);

protected:
private:
	CLayout *m_ui;
	CHero *m_hero;
};

#endif