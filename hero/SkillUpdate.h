
#ifndef __HEROSKILLUPDATE_
#define __HEROSKILLUPDATE_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"

class CSkillUpdate: public BaseLayer
{
public:
	CREATE_LAYER(CSkillUpdate);
	CSkillUpdate();
    virtual bool init();
    void onEnter();
    void heroSkill(CHero *hero, CSkill *skill);
	void onSkillItem(CCObject* pSender);
	
	void onLvUp(CCObject* pSender);

    void onExit();
    void showSkill(CHero* hero, int type);

	void nodeOffset(CCNode * parent, float offset);

	void removeSkillIcon();

	void showOneStarSkill(CLayout * star1);
	
	void skillLevel();

	void levelUp(bool isSuccess);
protected:
	void updateHeroInfo(const TMessage& tMsg);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void showSkillIcon(CSkill *skill, CImageView * mask1);

	void addFireAndNum(CCNode* pNode, int iNum);
	void processMsg(int type, google::protobuf::Message *msg);
	void skillActionExit(float dt);
	void showArmature(CCNode* node);
private:
	CLayout *m_ui;
	CLayout *m_skillLay;
	CLayout *m_mainSkillLay;
	CTableView *m_tableView;
	CHero *m_hero;
	int m_type;
	CLayout *m_cell;
	CBagData m_bagData;
	CItem *m_selectItem;
	int m_skillIndex;
	CCSprite *m_selectImg;
	int m_offsetX1;
	int m_offsetX2;
};

#endif