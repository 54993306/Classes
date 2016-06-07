
#ifndef __HEROEXP_
#define __HEROEXP_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "hero/HeroData.h"

class CHeroExp: public BaseLayer
{
public:
	CREATE_LAYER(CHeroExp);
	CHeroExp();
    virtual bool init();
    void onEnter();
    void onAddExp(CCObject* pSender);

    void onExit();
	void updateItemCount();
	void onSetHero(CHero *hero, bool bReset=false);

	void onAddExpSuccess();

	void sendAddExp(float dt);
	void sendAddExpCallBack();

	void updateHeroInfo(const TMessage& tMsg);
protected:
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
    void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void onGetMethod(CCObject* pSender);

	bool onlongClickExpItem(CCObject* pSender, CCTouch* pTouch);
	void onlongClickExpEnd(CCObject* pSender, CCTouch* pTouch, float fDuration);

	void processMessage(int type, google::protobuf::Message *msg);
	void onAddUseItem(float dt);
	void isUseExpItem(CCObject* pSender);
	void updateHeroExp(CHero *hero);
	void updateHeroExpCallBack();
private:
	CLayout *m_ui;
	CHero m_hero;
	CLayout *m_cell;
	CTableView *m_tableView;

	CBagData m_itemData;
	CItem *m_useItem;
	int  m_useCount;
	CLabel *m_clickCellLab;
	int  m_addExp;
	int m_iSaveUse;
	CCObject* m_pSender;
	bool m_bSendBlock;
	vector<int> m_vecSendData;
	bool m_bDataBack;
	CLabel* m_pValue;
	bool m_bLevelUp;
};

#endif