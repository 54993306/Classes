
#ifndef __BAG_SCENE__
#define __BAG_SCENE__
#include "AppUI.h"
#include "BagInfoTip.h"
#include "bagData.h"
#include "net/CNetClient.h"
#include "ItemTip.h"
#include "StuffTip.h"

enum BagBtnType
{
	Equipment_Type=1,
	Suipian_Type=2,
	Prop_Type=3,
};

class CPanelTips;

class CBagLayer:public BaseLayer
{
public:
	CBagLayer();
	~CBagLayer();
	CREATE_FUNC(CBagLayer);
	virtual bool init();
	void onEnter();	
	void onExit();
	void onClickBtn(CCObject *pSender);
    void bagItemData(int type, Message *msg);
	void processMsg(int type, Message *msg);
	void updateItemData(const TMessage& tMsg);
	
protected:
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void onPageChange(CCObject* pSender, unsigned int nPageIdx);
	//选择道具显示属性
	void onSelectBagItem(CCObject *pSender);
	void onUnlockBag(CCObject *pSender);
	void onLongSelectItem(CCObject* pSender, CCTouch* pTouch);
	void onChangeBagType(CCObject* pSender, bool isCheck);
	CItem* findItemByGrid(int uIdx);
	void updatePage();
	//按道具类型过滤
	void onFilter(CCObject *pSender);
	void onSetFilter(const TMessage& tMsg);
	//分解道具消息处理
	void decomposeArmorRes(const TMessage& tMsg);
	void onClose(CCObject* pSender);
	void lockRadioButton();
	void callBackForRadioButton();

private:
	void checkShowNoItemTip();
	void showNoItemTip(bool bShow, const char* sInfo);

private:
	CLayout *m_ui;
	CGridView *m_pageView;
	CBagData *m_bagData;
	int m_maxGrid;
	CLayout *m_cell;
	CItemTip *m_armorTip;
	CStuffTip *m_stuffTip;
	int m_selectType;
	CCSprite *m_selectImg;
	vector<int> m_filterVec;
	vector<CItem*> m_itemList;
	int m_iSelectIndex;

	CPanelTips* m_pPanelTips;
};


#endif