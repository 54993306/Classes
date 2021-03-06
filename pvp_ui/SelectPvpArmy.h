﻿#ifndef __SELECT_PVP_ARMY_
#define __SELECT_PVP_ARMY_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "friend/Friend.h"
#include "army/union.h"

class CSelectPvpArmy:public BaseLayer
{
public:
	CSelectPvpArmy();
	~CSelectPvpArmy();
	CREATE_LAYER(CSelectPvpArmy);
	virtual bool init();
	void onEnter();
	void onExit();
	void setUnionList(vector<CUnion> unionList);
	void setStagId(int stageId, int questId=0);
	CC_SYNTHESIZE(int, m_WorldBoss,WordBoss);					//世界boss
	CC_SYNTHESIZE(int, m_iRoleId, RoleId);						//角色ID
	CC_SYNTHESIZE(bool, m_bRobot, IsRobot);				//机器人
	CC_SYNTHESIZE(std::string , m_sEnemyRoleName, EnemyRoleName);		//敌人名字
	CC_SYNTHESIZE(int ,				m_iEnemyRoleHead, EnemyRoleHead);			//敌人头像
	CC_SYNTHESIZE(std::string , m_sEnemyRoleFaceBook, EnemyRoleFacebook);		//敌人facebookId
	CC_SYNTHESIZE(bool, m_bIsReven, Reven);
protected:
	void onClose(CCObject* pSender);
	void onCombat(CCObject* pSender);

	void addHeroCell(unsigned int uIdx, CPageViewCell * pCell);
	void processNetMsg(int type, google::protobuf::Message *msg);

	CCObject* armyDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void updateMainSkillDesc();
	void onMyBat(CCObject *pSender, bool bChecked);
	void onFriendBat(CCObject *pSender, bool bChecked);
	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void updateHeroList();
	void updateCurrHeroList();
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);

	bool  findIsSelect(CHero * hero);

	void showSelectRadioImg(int selIndex);
	void onSelectHero(CCObject *pSender);
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void processMessage(int type, google::protobuf::Message *msg);
	void onClearHero(CCObject* pSender);
	void updateCost();
	void addHeroSelectCell(unsigned int uIdx, CGridViewCell* pCell);

private:
	void initAction();
	void showActionForArmy(float dt=0.0f);
	void showActionForArmyReverse(float dt=0.0f);
	void showActionForArmyCallBack();
	bool showAddCaptain();
	bool showRemoveCaptain();
	void showCaptainActionCallBack();
	bool checkShowCaptain();
	void willToBatthle(CCObject* pObj);
	void addOppTeamCell(unsigned int uIdx, CPageViewCell * pCell);
	CCObject* oppViewDataSource(CCObject* pConvertCell, unsigned int uIdx);
private:
	CLayout *m_ui;
	int  m_selectHero;
	CLayout *m_cell;
	CLayout *m_cell1;
	CLayout *m_selectLay;

	CGridPageView *m_friendPage;
	CPageView *m_armyView;

	int  m_selectType;
	int m_currPage;
	vector<CHero> m_friendHeroList;
	vector<CUnion> m_unionList;
	CUnion m_union;
	CHero *m_selectFriend;
	int m_friendId;
	int m_stageId;
	CHero m_hero;

	vector<CHero> m_heroList;
	vector<CHero> m_friendList;
	map<int,vector<CHero*>> m_heroMap;
	map<int,vector<CHero*>> m_friendMap;
	vector<CHero*> *m_currHeroList;
	CGridView *m_gridView;
	CPageView *m_oppTeamView;

	int m_frdId;
	vector<CHero> m_oppTeamList;
	int m_questId;

	//动画需要
	CLayout* m_pActiveLay;
	bool m_bCaptainShowAction;
	CCSprite* m_pTaiJi1;
	CCSprite* m_pTaiJi2;
	bool m_isWordBoss;
};

#endif // !__GOLDMINE_LAYER_
