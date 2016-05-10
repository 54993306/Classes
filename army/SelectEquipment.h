
#ifndef __SELECTEQUIPMENT_LAYER_
#define __SELECTEQUIPMENT_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "union.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"

class CEmbattleControl;




class CSelectEquipment: public BaseLayer
{
public:
	CREATE_LAYER(CSelectEquipment);
	CSelectEquipment();
    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void setCurrentItem(int type, CItem* item);
	void setUnion(CUnion *nion);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void setOnSelectItemListener(CCObject* pListener,SEL_SelectItem pHandler);
	void setHeroArmor(int heroId, int pos);
protected:
	void onSelectBagItem(CCObject *pSender);
	bool onLongSelectItem(CCObject* pSender, CCTouch* pTouch);
	CItem* findItemByGrid(int uIdx);
	void recItemData(int type, Message *msg);
	
private:
	CLayout *m_ui;
	CGridPageView *m_pageView;
	CBagData *m_itemData;
	CItem *m_preItem; //现有的装备
	int m_itemType;
	int m_maxGrid;
	CUnion *m_union;

	CItem *m_selectItem;

	CCObject* m_pSelectListener;
	SEL_SelectItem m_itemSelectHander;

	int m_heroId;
	int m_pos;
};


#endif