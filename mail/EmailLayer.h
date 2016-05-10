/******************************************************
*文件名称:	EmailLayer.h
*编写日期:	2016-4-21-14:21
*编写作者:	YPF
*功能描述:	邮件界面
*******************************************************/

#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"
#include "mail/EmailData.h"
#include "tollgate/Chapter.h"

class CMonsterInfo;
class CItemInfo;
class CMailDelegate;

class CMailLayer: public BaseLayer
{
public:
	CREATE_LAYER(CMailLayer);
	CMailLayer();
	~CMailLayer();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	CCObject* tableDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void updateHeroList(vector<CHero>& heroList);	

	void reloadEmailTableView(vector<CMail>* pMailList);	
	void showMailContent(const CMail &mail);

	

protected:
	void addEmailCell(unsigned int uIdx, CTableViewCell * pCell);
	void onCheckMail(CCObject* pSender);
	void onGetMail(CCObject* pSender);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	void onReadAllMail(CCObject* pSender);

	void onPressMailPrize(CCObject* pSender, CTouchPressState iState);


private:
	CLayout *m_ui;
	CLayout *m_mailCell;
	CLayout *m_mailLay;
	CTableView *m_tableView;
	CTableView *m_tableMail;
	vector<CMail> *m_mailList;
	CMailDelegate *m_delegate;
	CLayout *m_mailContentLay;
	CMail m_checkMail;
	CLayout *m_contentCell;
	CGetPrizeRes m_prizeRes;
	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;

};