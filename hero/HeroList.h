
#ifndef __HERO_LIST_
#define __HERO_LIST_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"

class CNewHero;

class EquipStockData : public CCObject
{
public:

	EquipStockData():m_iIndex(0),m_iCount(0){};

	int m_iIndex;
	int m_iCount;
};

class CHeroList: public BaseLayer
{
public:
	CREATE_LAYER(CHeroList);
	CHeroList();
	~CHeroList();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void updateHeroList(vector<CHero>& heroList, int insertIndex);	
	void showNewHeroEffect(CHero *hero);
	void runOpenAnim();

	void updateEquipStockCount(CCObject* pObj);

	void compaseSuccess(CCObject* pObj);
protected:
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);
	void onSelectHero(CCObject *pSender);
	void addCallCell(unsigned int uIdx, CGridViewCell* pCell);
	void onGetItem(CCObject *pSender);
	void recoverPosition(CCNode *pNode);
	void onRecruitConfirm(CCObject *pSender);
	void runStoneAnim(CCNode *pNode);

	void addArmorForCell(CGridViewCell* pCell, CLayout* pLayout, CHero* pHero);

private:
	CLayout *m_ui;
	CGridView *m_gridView;
	CLayout *m_cell;
	CLayout *m_cell1;
	CLayout *m_emptyLay;
	vector<CHero> m_heroList;
	map<int,vector<CHero*>> m_heroMap;

	vector<CHero*> *m_currHeroList;

	map<int,vector<CItem>> m_itemMap;

	CHero *m_selectHero;
	CHero* m_pSelectCallHero;
	CLayout *m_subLay;
	CNewHero* m_pNewHeroEffect;
	int m_iEquipStock[4];
};

#endif