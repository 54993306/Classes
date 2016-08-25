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
#include <spine/spine-cocos2dx.h>
#include "activity/HttpLoadImage.h"

using namespace spine;

const float fChallengePanelScale[3] = {1.0f, 0.9f, 0.8f};

void inline AskForGoToSelectPvpArmy( int iRoleId, bool isRobot)
{
	OppTeamReq *pReq = new OppTeamReq;
	pReq->set_role_id(iRoleId);
	pReq->set_is_robot(isRobot);
	GetTcpNet->sendData(pReq, PvpOppTeamMsg);
	delete pReq;
}

class CPvpMenuLayer;

class CSelectChallengeLayer: public BaseLayer,public  HttpLoadImageDelegate
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


	void showOpenEffect();
	void callBackForShowOpenEffect();

	void showChallenge( CCObject *pSender );
	void hideChanllenge( CCObject *pSender );
	void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);
private:
	CLayout* m_ui;
	CLayout* m_pPanel[3];
	SkeletonAnimation* m_pMoveLight[3];
	SkeletonAnimation* m_pBaseLight[3];

	CLayout* m_pCell;

	//pvp数据备份
	PvpDataRes m_pPvpDataRes;

	bool m_bFirstInit;
	bool m_bGetNewData;

	int m_iLockIndex;

	SkeletonAnimation			*m_pRobot;
	int m_iTimesCombat;
	std::map<std::string, CImageView *> m_pMapHeadBg;
};