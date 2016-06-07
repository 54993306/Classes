
#ifndef __HEROEXPITEM_
#define __HEROEXPITEM_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "hero/HeroData.h"

class CHeroExpItem: public BaseLayer
{
public:
	CREATE_LAYER(CHeroExpItem);
    virtual bool init();
    void onEnter();
    void onEvolveBtn(CCObject* pSender);

    void onExit();
    void showMoveDirection(CHero* hero);
	void evolveTask(const CEvolInfo& evInfo );
	void showItem(CItem* item);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
protected:
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
    void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void onRecvTask(CCObject* pSender);

private:
	CLayout *m_ui;
	CHero *m_hero;
	CLayout *m_cell;
	CTableView *m_tableView;
	CEvolInfo m_evolInfo;
};

#endif