/******************************************************
*文件名称:	HandBookHero.h
*编写日期:	2016-8-23-16:02
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#pragma once
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

//请求buff数据
//void inline AskForHandBookHero()
//{
//	GetTcpNet->sendDataType(BossPrizesMsg);
//}


class CHandBookEffectTip: public BaseLayer
{
public:
	CHandBookEffectTip();
	~CHandBookEffectTip();
	
	CREATE_LAYER(CHandBookEffectTip);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);


private:
	void updateUI();

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void onClickBlank( CCObject *pSender );
	void onClose(CCObject* pSender);

private:
	CLayout						*m_ui;
	CLayout						*m_pCell;
	CTableView					*m_pTableView;
	int								m_iSelectIndex;
};