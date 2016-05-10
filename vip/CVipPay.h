#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

class CursorTextField;

class CVipPay:public BaseLayer
{
public:
	CVipPay();
	~CVipPay();
	virtual bool init();
	CREATE_LAYER(CVipPay);
	void onEnter();
	void onExit();
	void showErrorCode(string sErrorCode);
	void setCardId(int iCardId);
	void setStep1Success(bool bSuccess);
	void showTimeClock();
	void onClose( CCObject * pSender );
	bool getStep1Success();
	void enableSmsCodeBtn();

private:
	void hideErrorCode();
	void updateClock(float dt);
	void hideTimeClock();
	void disEnableSmsCodeBtn();
	
private:
	void onConfirm( CCObject * pSender );
	void onCancel( CCObject * pSender );
	void onGetSecurityCode(CCObject * pSender);
	
private:
	CLayout* m_ui;
	CursorTextField* m_pTelInput;
	CursorTextField* m_pCodeInput;
	int m_iCardId;
	std::string m_sDeviceId;
	std::string m_sTelNumber;
	bool m_bStep1Success;
	int m_iTimeRemain;
};