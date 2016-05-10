
#ifndef __HEROSKILL_
#define __HEROSKILL_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"


class CHeroSkill: public BaseLayer
{
public:
	CREATE_LAYER(CHeroSkill);
    virtual bool init();
    void onEnter();
    void onClickSkill(CCObject* pSender);
	
    void onExit();
    void showSkill(CHero* hero, int type);
	
protected:
	void updateHeroInfo(const TMessage& tMsg);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void addFireAndNum(CCNode* pNode, int iNum);
private:
	CLayout *m_ui;
	CLayout *m_skillLay;
	CLayout *m_mainSkillLay;
	CTableView *m_tableView;
	CHero *m_hero;
	int m_type;
	CLayout *m_cell;

};

#endif