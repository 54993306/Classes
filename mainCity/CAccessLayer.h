/******************************************************
*文件名称:	CAccessLayer.h
*编写日期:	2016-4-21-15:54
*编写作者:	YPF
*功能描述:	主城其他功能入口，签到，打折等
*******************************************************/

#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"

USING_NS_CC;

//入口类型
enum AccessType
{
	AccessTypeSign = 1,
	AccessExchange,
	AccessLuckyWheel,
	AccessTypeMax,
	
};

class CAccessLayer: public BaseLayer
{
public:
	CREATE_LAYER(CAccessLayer);
	CAccessLayer();
	~CAccessLayer();

    virtual bool init();
    void onEnter();
    void onExit();

private:
	void initAccess();
	void showAccess();
	void hideAccess();
	void onCheck(CCObject *pSender, bool bChecked);
	void onTouchAccess(CCObject *pSender);
	void callBackForChecActionkEnd();

	void updateForScale9Mask(float dt);

	void ProcessMsg(int type, google::protobuf::Message *msg);

private:
	CLayout *m_ui;
	vector<CButton*> m_pButtonVec;
	CImageViewScale9* m_pMask;
	CCheckBox* m_pCheck;
	int m_iLockIndex;
	int m_iAimHeight;
};