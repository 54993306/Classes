#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "activity/HttpLoadImage.h"
class CursorTextField;

class CVipPay:public BaseLayer,public HttpLoadImageDelegate
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

	void setConfirmState(bool bSuccess);

	void showTimeClock();
	void onClose( CCObject * pSender );
	bool getStep1Success();
	void enableSmsCodeBtn();
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);
	void showVerifyCode(const char* str);
	void setPayItem(int iTag, int price);

	void onPaySuccess();

private:
	void hideErrorCode();
	void updateClock(float dt);
	void hideTimeClock();
	void disEnableSmsCodeBtn();
	
private:
	void onConfirm( CCObject * pSender );
	void onCancel( CCObject * pSender );
	void onGetSecurityCode(CCObject * pSender);
	bool isFloatStr(const char* str);
	void onCheckMoney(CCObject *pSender, bool bChecked);
private:
	CLayout* m_ui;
	CursorTextField* m_pTelInput;
	CursorTextField* m_pCodeInput;
	int m_iCardId;
	std::string m_sDeviceId;
	std::string m_sTelNumber;
	bool m_bStep1Success;
	int m_iTimeRemain;
	int m_buyMoney;
	int m_payStep;
	int m_iTag;			//购买标记展示图片
};