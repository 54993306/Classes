#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"

enum PayListType
{
	PayListTypeDefault	=	1,		//googledefault
	PayListTypeGoogle		//googleyplay

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
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	CC_SYNTHESIZE(PayListType, m_payType, PayType);


private:
	void initUI();

private:
	void onPhonePay( CCObject * pSender );
	void onCardPay( CCObject * pSender );
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void updateForBuyForGoogle(float dt);

private:
	CLayout* m_ui;
	CTableView *m_tableView;
	CLayout *m_cell;
	vector<int> m_priceVec;

	int m_iIndexFlag;
};