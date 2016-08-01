#pragma once
/******************************************************
*文件名称:	PvpRuleLayer.h
*编写日期:	2016-6-14-11:34
*编写作者:	YPF
*功能描述:	PVP 规则界面
*******************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

//请求规则奖励
void inline AskForPvpRule()
{
	GetTcpNet->sendDataType(PvpPrizeListMsg);
}


class CPvpRuleLayer: public BaseLayer
{
public:
	CPvpRuleLayer();
	~CPvpRuleLayer();
	
	CREATE_LAYER(CPvpRuleLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

private:
	void updateShowInfoScroll(CCSize size);
	void initRank(PvpPrizeList* pPrizes);
	void goRank(CCObject* pSender);

private:
	CLayout* m_ui;
	CLayout* m_pLayer;
	CScrollView* m_pScroll;
	CLayout* m_pCell;
};