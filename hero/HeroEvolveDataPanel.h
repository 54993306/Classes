#pragma once

/******************************************************
*文件名称:	HeroEvolveDataPanel.h
*编写日期:	2016-6-20-16:53
*编写作者:	YPF
*功能描述:	英雄进阶-球数据面板
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "HeroData.h"

using namespace std;

enum struct HeroEvolveEffectuUnlockType
{
	UnlockSkill,			//技能
	UnlockBox,			//插槽
	UnlockExtra			//其他-天赋技能？
};


class CHeroEvolveDataPanel: public BaseLayer
{
public:
	CHeroEvolveDataPanel();
	~CHeroEvolveDataPanel();
	CREATE_LAYER(CHeroEvolveDataPanel);
    virtual bool init();
    void onEnter();
    void onExit();

public:
	void show(CHero *pBaseHero, CHero *pHero = nullptr, bool bForceChange=false);
	void hide();

	void resetAllHeroData();

	CLayout* createSkillPanel(CHero *pBaseHero, CHero *pHero = nullptr, CLayout *pLay=nullptr);			//创建额外的技能面板层-目前给特效显示使用

	HeroEvolveEffectuUnlockType getUnLockTypeWithRank( int iRank );

private:
	void initDataUI();				//初始化数据UI
	void updateUI();				//更新UI显示

	vector<CSkill*> getRankOpenSkill(int iRank);			//获取当前等级开发的技能

	void insertOneSkillPanel();				//克隆并插入一个skillPanel
	void fitSkillPanelVec(int iSize);		//补充技能面板数量

	void showBaseInfo();				//显示基础数据
	void hideBaseInfo();				//隐藏基础数据
	void updateBaseInfo();			//更新基础数据

	void updateSkillInfo();			//更新技能数据

	void fillOneSkillInfo(CLayout *pLay, CSkill*, bool showExtra);			// 更新单个技能面板信息  || showExtra是否显示主动/被动技标签和cost

	bool isInitiativeSkill(CSkill *pSkill);				//是否主动技

	int getSkillTypeDesId(CSkill *pSkill);

	bool isRealSkill(CSkill *pSkill);					//是否为正常技能

	void autoFixPos();						//自动调整位置

	void hideAllSkillPanel();				//隐藏所有的技能面板

private:
	CLayout														*m_pUi;
	CHero															*m_pHero;							//英雄阶段数值信息
	CHero															*m_pBaseHero;						//英雄阶段技能等信息（不会变化）

	CLayout														*m_pCellLay;							//技能基础cell
	vector<CLayout*>										m_pSkillLayVec;							//技能信息列表, 最多三个信息
	CCPoint														m_pStartFlagPos;					//标记位置
	CLayout														*m_pBaseInfoLay;					//基础信息
	float																m_fBaseInfoPanelHeight;		//基础信息面板高度
	CImageViewScale9										*m_pBgImageScale9;				//可拉伸的背景框
};