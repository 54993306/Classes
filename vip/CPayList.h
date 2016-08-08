#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum PayListType
{
	PayListTypeDefault	=	1,		//googledefault
	PayListTypeGoogle,		//googleyplay
	PayListTypeApple		//appleStore
};

class CPayList:public BaseLayer
{
public:
	CPayList();
	~CPayList();
	virtual bool init();
	CREATE_LAYER(CPayList);
	void onEnter();
	void onExit();
	void onClose( CCObject * pSender );

	CC_SYNTHESIZE(PayListType, m_payType, PayType);


private:
	void initUI();

private:
	void onPhonePay( CCObject * pSender );
	void updateForBuyForGoogle(float dt);

private:
	CLayout* m_ui;
	vector<int> m_priceVec;

	int m_iIndexFlag;
};