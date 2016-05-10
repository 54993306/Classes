
#ifndef __HEROUPDATE_
#define __HEROUPDATE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CHeroUpdate: public BaseLayer
{
public:
	CREATE_LAYER(CHeroUpdate);
    virtual bool init();
    void onEnter();	
    void onExit();
	void levelSkillUp(CHero* hero,const Hero* pHero);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
protected:
private:
	CLayout *m_ui;
	CHero *m_hero;
	CHero m_reshero;
	CLayout *m_cell;
	CTableView *m_tableView;
};

#endif