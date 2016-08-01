
#ifndef __SHOP_LAYER_
#define __SHOP_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"

class CShopLayer: public BaseLayer
{
public:
	CREATE_LAYER(CShopLayer);
	CShopLayer();
	~CShopLayer();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void addCell(CGridPageViewCell* pCell, unsigned int uIdx);

	void leaveTime(struct tm &lastTm, struct tm &timem, struct tm &now);

	void ProcessMsg(int type, google::protobuf::Message *msg);

	void setShopType(int type);

	void roleUpdate(const TMessage& tMsg);
protected:
	bool onLongSelectItem(CCObject* pSender, CCTouch* pTouch);
	void onSelectItem(CCObject* pSender);
	void onBuyItem(CCObject* pSender);
    void updateItemList(vector<CItem>& itemList);	
	void onSwitchBtn(CCObject *pSender, bool bChecked);

	void showSelectRadioImg(int selIndex);
	void onUpdateItem(CCObject* pSender);
	void onComfirmUpdate(CCObject* pSender);
	void updateTime(float dt);
private:
	CLayout *m_ui;
	CGridPageView *m_pageView;
	CLayout *m_cell;

	vector<CItem> m_itemList;

	map<int,vector<CItem>> m_itemMap;

	int m_buyType;

	CItem *m_selectItem;
	int m_updateGold;
	int m_min;
	CCArmature *m_armature;
};


#endif