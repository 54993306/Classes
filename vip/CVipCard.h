#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

class CursorTextField;

class CVipCard:public BaseLayer
{
public:
	CVipCard();
	~CVipCard();
	virtual bool init();
	CREATE_LAYER(CVipCard);
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
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
private:
	CLayout* m_ui;
	CursorTextField* m_cardText;
	CursorTextField* m_pwdText;
	int m_iCardId;
	std::string m_sDeviceId;
	std::string m_sTelNumber;
	bool m_bStep1Success;
	int m_iTimeRemain;
	CTableView *m_tableView;
	CLayout *m_cell;
};