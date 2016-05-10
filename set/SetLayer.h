
#ifndef _SETLAYER_H
#define _SETLAYER_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"
#include "common/CursorTextField.h"

#include "model/RoleNameGM.h"

#define SET_LAYER_HIDE "SET_LAYER_HIDE"
#define SET_LAYER_SHOW "SET_LAYER_SHOW"

class CSetLayer: public BaseLayer
{
public:
	CREATE_LAYER(CSetLayer);
	CSetLayer();
	~CSetLayer();

    virtual bool init();
    void onEnter();
    void onExit();

	void onExchange(CCObject* pSender);
	void onClose(CCObject* pSender);
	void onPushSet(CCObject* pSender);
	void onEffMusic(CCObject *pSender, bool bChecked);
	void onBgMusic(CCObject *pSender, bool bChecked);
	void showEffMusicLab(bool bChecked);
	void showBgMusicLab(bool bChecked);
	void ProcessMsg(int type, google::protobuf::Message *msg);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

private:
	void hide(CCObject* pObj);
	void show(CCObject* pObj);

private:
	CLayout *m_ui;
	bool m_bSoundLock;
};

#endif