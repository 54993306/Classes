
#ifndef _POP_ITEM_
#define _POP_ITEM_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
class CShopBuy: public BaseLayer
{
public:
	CREATE_LAYER(CShopBuy);
	CShopBuy();
	~CShopBuy();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	void showItemProperty(CItem *item);

	void updateArmorAttr(CItem * item);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void onBuy(CCObject* pSender);
	void setShopType(int type, int maxNum=0);
	void roleUpdate(const TMessage& tMsg);
	void addNum(CCObject* pSender);
	void subNum(CCObject* pSender);
	void sliderChange(CCObject* pSender, int nValue);
private:
	CLayout *m_ui;
	CLayout *m_armorLay;
	CLayout *m_itemLay;
	int m_buyType;
	CItem m_item;
	int m_buyNum;
	int m_maxNum;
};

#endif