#pragma once

/******************************************************
*文件名称:	StarUpItem.h
*编写日期:	2016-4-12-14:07
*编写作者:	YPF
*功能描述:	装备升星界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "CityData.h"
#include "bag/bagData.h"


struct  CRisingResponse
{
	int result ;		//升星结果（1 成功, 2 失败， 3 素材不足，4 达到最大星级, 5 数据错误）
	CItem armor ;		//要强化的装备数据
	CItem item1;		//初级道符
	CItem item2;		//精炼道符
	CItem item3;		//降魔道符
	int rate;		//基础成功机率
	int gold;		//费用
	bool bStarUp;	//是否可升星

	void read(const RisingResponse& res);
};

enum CStarUpItemChildTag
{
	CStarUpItemStarLayTag = 19,
};

//强化类
class CStarUpItem:public BaseLayer
{
public:
	~CStarUpItem();

	CStarUpItem();

	CREATE_LAYER(CStarUpItem);
	virtual bool init();
	void onEnter();

	void onExit();

	void strengthenResponse(int type, google::protobuf::Message *msg);

    void onSelectWeird(CItem* item); 

	void updateStrengthenItem();

	void updateBaseAttr();

	void updateItemProperty();
	void updateWeird(const TMessage& tMsg);
	void showSmeltLayer();
	void roleUpdate(const TMessage& tMsg);
	void changeToRinset();

	void sameEquip(CCObject* pObj);
	int getItemId();
protected:	
	void onStrengthen(CCObject* pSender);
	void onSelectItem(CCObject* pSender);
	void onSelectWeird(CCObject *pSender);
	void onItemTip(CCObject *pSender);

	void onUserStone(CCObject *pSender);
	void setAddHpString(CCNode* node);
	void setAddAtkString(CCNode* node);
	void setAddDefString(CCNode* node);
	void onGoldStrengthen(CCObject* pSender);
	void onConfirm(CCObject *pSender);

	void clearEquip();

private:
	void onSelectArmor(CItem* item);
	void showItem();
	void initItemInfo(CCPoint posStart, CLayout* pLayout[], int iCount);
	void hideNoneValueCell(int iCount, CLayout* pLayout[]);
	int updateInfoToUIAndReturnCount(CLayout* pLayout[], int iNameId[], int iValue[], int iValueAim[], int iMax, const char* sTitle);
	bool isMaxStar();
	void checkMoney();

	float showEffect(bool bSuccess);
	void showEffectCallBack1(CCNode* pSender);
	void showEffectCallBack(CCNode* pSender);
	void showTVLightCallBack();
private:
	CLayout *m_ui;

	CLayout *m_strenLeft;  
	CLayout *m_itemAttr;

	CItem m_item;

	CRisingResponse m_strengthItem;

	int m_selectOPType; //选择类型

	CItem m_weirdItem;

	bool m_isHpAdding;
	bool m_isAtkAdding;
	bool m_isDefAdding;

	bool m_isUseGold;

	CLayout* m_pCellInfo;
	CLayout* m_pBaseInfo[4];
};
