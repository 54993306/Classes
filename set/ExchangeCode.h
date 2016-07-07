
#ifndef _EXCHANGECODE_H
#define _EXCHANGECODE_H

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"
#include "common/CursorTextField.h"

#include "model/RoleNameGM.h"

enum ExchangeType
{
	ExCode = 1,
	CBCode
};

class CExchangeCode: public BaseLayer
{
public:
	CREATE_LAYER(CExchangeCode);
	CExchangeCode();
	~CExchangeCode();

    virtual bool init();
    void onEnter();
    void onExit();

	void onExchange(CCObject* pSender);
	void onClose(CCObject* pSender);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void loadExchangeByType(int type);
private:
	CLayout *m_ui;
	CursorTextField* m_textFieldName;
	CursorTextField* m_textFieldPwd;
	int m_type;
};

#endif