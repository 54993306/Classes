﻿
#ifndef _SETPUSH_H
#define _SETPUSH_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

class CSetPush: public BaseLayer
{
public:
	CREATE_LAYER(CSetPush);
	CSetPush();
	~CSetPush();

    virtual bool init();
    void onEnter();
    void onExit();

	void onExchange(CCObject* pSender);
	void onClose(CCObject* pSender);
	void onOpenOff(CCObject* pSender, bool isCheck);
private:
	CLayout *m_ui;
};

#endif