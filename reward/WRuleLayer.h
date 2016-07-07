#pragma once
/******************************************************
*文件名称:	WRuleLayer.h
*编写日期:	2016-6-12-18:32
*编写作者:	YPF
*功能描述:	世界BOSS规则界面
*******************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

//请求规则奖励
void inline AskForRuleReward()
{
	GetTcpNet->sendDataType(BossPrizesMsg);
}


class CWRuleLayer: public BaseLayer
{
public:
	CWRuleLayer();
	~CWRuleLayer();
	
	CREATE_LAYER(CWRuleLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

private:
	void updateShowInfoScroll(CCSize size);
	void initRank(BossPrizes* pPrizes);
	void goRank(CCObject* pSender);
private:
	CLayout* m_ui;
	CLayout* m_pLayer;
	CScrollView* m_pScroll;
	CLayout* m_pCell;
};