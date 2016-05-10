
#ifndef __HERODPROMOTE_
#define __HERODPROMOTE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CHeroPromote: public BaseLayer
{
public:
	CREATE_LAYER(CHeroPromote);
	~CHeroPromote();
    virtual bool init();
    void onEnter();	
    void onExit();
	void levelSkillUp(CHero* hero,const Hero* pHero);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void addSkill(unsigned int uIdx, CTableViewCell * pCell);
protected:
private:
	CLayout *m_ui;
	CLayout *m_skillLay;
	CHero *m_hero;
	CHero m_reshero;
	CTableView *m_tableView;
	CLayout *m_cell;
};

#endif