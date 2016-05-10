#pragma once

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