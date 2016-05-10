#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "HeroData.h"

class CHeroCall: public BaseLayer
{
public:
	CHeroCall();

	CREATE_LAYER(CHeroCall);

    virtual bool init();

    void onEnter();
    void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void setNeedNum(int num);

private:
	void processMessage(int type, google::protobuf::Message *msg);
	void compaseSuccess(CCObject* pObj);
	void onClose(CCObject* pSender);
	void onRecvTask(CCObject* pSender);
	void updateHeroInfo(const TMessage& tMsg);

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void showItem(CItem * item);
	void evolveTask(const CItem& item);

	void showHeroHead(CHero *hero);
	void showHeroQuality(CHero * hero);

private:
	CLayout *m_ui;
	CHero *m_hero;
	CLayout *m_cell;
	CTableView *m_tableView;
	CItem m_item;
	int m_itemNeedNum;
};