#pragma once

/******************************************************
*文件名称:	LevelUpItem.h
*编写日期:	2016-4-12-14:06
*编写作者:	YPF
*功能描述:	装备升级界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "CityData.h"
#include "bag/bagData.h"
#include "SmeltArmor.h"

//强化类
class CLevelUpItem:public BaseLayer
{
public:
	~CLevelUpItem();

	CLevelUpItem();

	CREATE_LAYER(CLevelUpItem);
	virtual bool init();
	void onEnter();

	void onExit();

	void strengthenResponse(int type, google::protobuf::Message *msg);

	void setIsStrengthen(bool isEnable);

	void updateHero(const TMessage& tMsg);
	
	void showItem();

	void updateStrengthenItem();

	void updateBaseAttr();

	void updateItemProperty();
	void updateWeird(const TMessage& tMsg);
	void showSmeltLayer();
	void roleUpdate( const TMessage& tMsg );
	void sameEquip(CCObject* pObj);
	int getItemId();
protected:	
	void onStrengthen(CCObject* pSender);
	void onSelectItem(CCObject* pSender);
	void onSelectWeird(CCObject *pSender);
	void onItemTip(CCObject *pSender);
	void onClose(CCObject *pSender);
	void onUserStone(CCObject *pSender);
	void setAddHpString(CCNode* node);
	void setAddAtkString(CCNode* node);
	void setAddDefString(CCNode* node);
	void onOneKeyLevelUp(CCObject* pSender);
	void onConfirm(CCObject *pSender);

	float showHammerEffect(int iTimesHit=2);
	void showHammerSoundCallBack(CCNode* pSender);
	void showHammerShakeCallBack();
	void showHammerEffectCallBack(CCNode* pSender);

	bool isMaxLevel();

	void clearEquip();

private:
	void initItemInfo(CCPoint posStart, CLayout* pLayout[], int iCount);
	void hideNoneValueCell(int iCount, CLayout* pLayout[]);
	int updateInfoToUIAndReturnCount(CLayout* pLayout[], int iNameId[], int iValue[], int iValueAim[], int iMax, const char* sTitle);
	void checkMoney();
	void selectITemCallBack(CItem* pItem);
	void showTVLightCallBack();
private:
	CLayout *m_ui;

	CLayout *m_strenLeft;  
	CLayout *m_itemAttr;

	CSmeltArmor *m_smeltLay;   //洗练层

	CItem m_item;
	CStrengthenResponse m_strengthItem;
	int m_selectOPType; //选择类型

	CItem m_weirdItem;

	bool m_isHpAdding;
	bool m_isAtkAdding;
	bool m_isDefAdding;

	bool m_isOneKey;

	CLayout* m_pCellInfo;
	CLayout* m_pBaseInfo[4];
};