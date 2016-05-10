#pragma once


#include "AppUI.h"
#include "Global.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

#define WAR_RELIVE_TAG 5678

class WarRelive : public  BaseLayer
{
public:
	WarRelive();
	virtual bool init();
	CREATE_LAYER(WarRelive);

	void onEnter();
	void onExit();

	void onTouch(CCObject* pSender);

	void show(int iMoney);
	void hide();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

private:
	int m_iMoney;
};