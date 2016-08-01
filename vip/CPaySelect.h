#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum PayType
{
	PhonePay=1,
	CardPay,
	GooglePay
};

const static char *PaySelectDay = "2016-07-13-00";

class CPaySelect:public BaseLayer
{
public:
	CPaySelect();
	~CPaySelect();
	virtual bool init();
	CREATE_LAYER(CPaySelect);
	void onEnter();
	void onExit();
	void onClose( CCObject * pSender );
	int getPayType();
private:
	void updateForCheckDay(float dt);
	void updatePayUiShow( bool bShowAll );
	
private:
	void onPhonePay( CCObject * pSender );
	void onCardPay( CCObject * pSender );
	void onGoolgePay( CCObject * pSender );


private:
	CLayout* m_ui;
	int m_payType;
};