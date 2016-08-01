
#ifndef _ROLE_NAME_H
#define _ROLE_NAME_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "model/RoleNameGM.h"

class CursorTextField;

class CChangeName: public BaseLayer
{
public:
	CREATE_LAYER(CChangeName);
	CChangeName();
	~CChangeName();

    virtual bool init();
    void onEnter();
    void onExit();

	void onChangName(CCObject* pSender);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void onRandowName(CCObject* pSender);
private:
	CLayout *m_ui;
	CursorTextField* m_textFieldName;
	RoleNameGM m_nameGm;
};

#endif