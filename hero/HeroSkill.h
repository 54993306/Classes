
#ifndef __HEROSKILL_
#define __HEROSKILL_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "tollgate/ItemInfo.h"

class CHeroSkill: public BaseLayer
{
public:
	CREATE_LAYER(CHeroSkill);
	CHeroSkill();
    virtual bool init();
    void onEnter();
    void onClickSkill(CCObject* pSender);

	void showSkillType();

	void onSkillItem(CCObject* pSender);
	
	void onLvUp(CCObject* pSender);

    void onExit();
    void showSkill(CHero* hero, int type);

	void nodeOffset(CCNode * parent, float offset);

	void removeSkillIcon();

	void showOneStarSkill(CLayout * star1);

	void skillLevel();

	void levelUp(bool isSuccess);


	void showSpeciaBtn( bool bVisiable );		//特殊按钮，只有五星英雄可以点击
	void onSpecialBtnClick( CCObject *pObj );
	void showSpeciaPage( bool bVisiable );		//特殊层，进阶展示
	void showSpecialInfo( CCObject *pSender );

protected:
	void updateHeroInfo(const TMessage& tMsg);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void showSkillIcon(CSkill *skill, CImageView * mask1);

	void addFireAndNum(CCNode* pNode, int iNum);
	void processMsg(int type, google::protobuf::Message *msg);
	void onPress( CCObject* pSender, CTouchPressState iState );
	void failAction(float dt);
private:
	CLayout *m_ui;
	CLayout *m_ui_special;
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
	CItemInfo* m_pItemInfo;
	CCPoint m_point;
};

#endif