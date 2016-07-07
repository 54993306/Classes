#pragma once
/******************************************************
*文件名称:	WShopLayer.h
*编写日期:	2016-6-12-18:32
*编写作者:	YPF
*功能描述:	世界BOSS商店界面
*******************************************************/
#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"


class CWShopLayer: public BaseLayer
{
public:
	CWShopLayer();
	~CWShopLayer();
	
	CREATE_LAYER(CWShopLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addCell(CGridPageViewCell* pCell, unsigned int uIdx);
	void onBuyItem(CCObject* pSender);
	void onComfirmUpdate(CCObject* pSender);
	void updateItemList(vector<CItem>& itemList);
	void loadUiByType(int type);
private:
	CLayout* m_ui;
	CGridPageView *m_pageView;
	CLayout *m_cell;

	vector<CItem> m_itemList;

	map<int,vector<CItem>> m_itemMap;

	int m_buyType;

	CItem *m_selectItem;
	int m_updateGold;
	int m_min;
};