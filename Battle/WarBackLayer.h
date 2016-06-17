#pragma once


#include "AppUI.h"
#include "Global.h"
#include "scene/layer/LayerManager.h"

class WarBackLayer : public  BaseLayer
{
public:
	WarBackLayer();
	virtual bool init();
	CREATE_LAYER(WarBackLayer);

	void onEnter();

	void onExit();

	void touchExit();

	void returnCity(CCObject* ob);

	void onExitBattle(CCObject* pSender);
	void onResumeBattle(CCObject* pSender);
	void onEffMusic(CCObject *pSender, bool bChecked);
	void onBgMusic(CCObject *pSender, bool bChecked);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void show();
	void hide();

	void showEffMusicLab(bool bChecked);
	void showBgMusicLab(bool bChecked);

	void setMoneyAndBox(int iMoney, int iBox);

private:
	CLayout* m_ui;
	bool m_bSoundLock;
};

enum WarBackTag
{
	tips_tag
};