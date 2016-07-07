#pragma once

/******************************************************
*文件名称:	SelectChallengeLayer.h
*编写日期:	2016-6-13-17:16
*编写作者:	YPF
*功能描述:	选择挑战目标
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

const float fChallengePanelScale[3] = {1.0f, 0.9f, 0.8f};

class CSelectChallengeLayer: public BaseLayer
{
public:
	CSelectChallengeLayer();
	~CSelectChallengeLayer();
	
	CREATE_LAYER(CSelectChallengeLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

	void onOpen();
	void onClose(CCObject* pSender);

private:
	void onRobot(CCObject* pSender);
	void onRefresh(CCObject* pSender);
	void onCombat(CCObject* pSender);

	void initUIWithData();

	void updatePanelUI();
	void updateOnePanel(CLayout* pPanel, const Opponent& pOpponent, int iBgId);

	void panelActionShow();
	void panelActionShowCallBack();
	void panelActionReshow();
	void panelActionReshowDataChangeCallBack();
	void panelActionReshowCallBack();

	bool isUILock();

private:
	CLayout* m_ui;
	CLayout* m_pPanel[3];
	CLayout* m_pCell;

	//pvp数据备份
	PvpDataRes m_pPvpDataRes;

	bool m_bFirstInit;
	bool m_bGetNewData;

	int m_iLockIndex;
};