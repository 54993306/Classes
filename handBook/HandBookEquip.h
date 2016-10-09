/******************************************************
*文件名称:	HandBookEquip.h
*编写日期:	2016-8-23-16:02
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#pragma once
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

class CHandBookEquipLayer: public BaseLayer
{
public:
	CHandBookEquipLayer();
	~CHandBookEquipLayer();

	CREATE_LAYER(CHandBookEquipLayer);

	virtual bool init();
	void onEnter();
	void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

private:
	CLayout				*m_ui;
	CLayout				*m_pCell;
	CLayout				*m_pScroll;

};