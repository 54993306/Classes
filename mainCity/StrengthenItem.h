#ifndef __STRENGTHITEM_H__
#define __STRENGTHITEM_H__

#include "scene/layer/LayerManager.h"
#include "CityData.h"
#include "bag/bagData.h"

#include "protos/protocol.h"

enum CStrengthenItemType
{
	CStrengthenItemTypeItemLevelUp = 11,
	CStrengthenItemTypeItemReset = 13,
	CStrengthenItemTypeItemStarUp = 12,
	CStrengthenItemTypeOthers = 14
};

class CItemTip;

class CStrengthenItem: public BaseLayer
{
public:
	~CStrengthenItem(){}
	CStrengthenItem();
	CREATE_LAYER(CStrengthenItem);
	virtual bool init();
	void onEnter();
	void onClose(CCObject* pSender);
	void onSelectItem(CCObject* pSender);
	void onTouchItem(CCObject* pSender);
	void onFilterItem(CCObject* pSender);
	void onExit();
	void setSelectType(int type);
	void setOnSelectItemListener(CCObject* pListener,SEL_SelectItem pHandler);
	void setSelForHero(int toHero);
	CC_SYNTHESIZE(int, m_iEquipType, EquipType);
	void setAskType(CStrengthenItemType type);
protected:	
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void recItemData(int type, google::protobuf::Message *msg);
	void onSetFilter(const TMessage& tMsg);
	void processNetMessage(int type, google::protobuf::Message *msg);
private:
	CLayout *m_ui;
	CLayout *m_cell;
	CTableView *m_tableView;
	CBagData m_itemData;
	int m_type;
	CItem *m_item;

	vector<CItem*> m_itemList;
	vector<int> m_filterVec;
	CCSprite *m_selectCellImg;

	CCObject* m_pSelectListener;
	SEL_SelectItem m_itemSelectHander;
	int m_toHero;
	int m_iCountEquipStock;

	CItemTip* m_pItemTip;
	CStrengthenItemType m_iAskType;
};

#endif