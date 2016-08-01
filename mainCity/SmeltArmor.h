#pragma once

/******************************************************
*文件名称:	SmeltArmor.h
*编写日期:	2016-4-12-13:45
*编写作者:	YPF
*功能描述:	装备洗练界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "bag/bagData.h"
#include <spine/spine-cocos2dx.h>

using namespace spine;

class CSmeltArmor: public BaseLayer
{
public:
	CREATE_LAYER(CSmeltArmor);
	CSmeltArmor();
	~CSmeltArmor();
    virtual bool init();
    void onEnter();
    void onExit();
	void onSelectArmor(CItem* item);

	void initSpandAndCheckBox();

	void showArmorAttr();
	void proceesMessage(int type, google::protobuf::Message *msg);
	void roleUpdate(const TMessage& tMsg);
	void updateStar(const TMessage& tMsg);

protected:
	void onCheckAttr(CCObject *pSender, bool bChecked);
	void onSelectItem(CCObject* pSender);
	void onRefineItem(CCObject* pSender);
	void setYellowBgState(CCheckBox* pCheck, bool isCheck);
	void showMoveAction(const char* name);

	void initFire();
	void showFireWithType(int iType);

	float showStrengthEffect();
	void showStrengthSoundCallBack();
	void showSmoke(CCNode* pSender);
	void showStrengthCallBack(CCNode* pSender);

	void showTVLightCallBack();
private:
	void initItemInfo(CCPoint posStart, CLayout* pLayout[], int iCount);
	void hideNoneValueCell(int iCount, CLayout* pLayout[]);
	int updateInfoToUIAndReturnCount(CLayout* pLayout[], int iNameId[], int iValue[], const char* sRange[], int iMax, const char* sTitle);

	void showSmeltDataToUI();
	void checkMoney();
private:
	CLayout *m_ui;
	CLayout *m_attr;
	CCArray *m_filterArr;
	bool m_noEquip;  //未装备
	bool m_yesEquip;
	int m_coin;
	int m_diamond;
	CItem m_armor;
	//洗练火
	CCSprite* m_pFire1;
	CCSprite* m_pFire2;
	int m_iStrengthType;

	CLayout* m_pCellInfo;
	CLayout* m_pBaseInfo[4];

	int m_iCount;

	SkeletonAnimation *m_pSpineHero;
};