#pragma once

/******************************************************
*文件名称:	PvpMenuLayer.h
*编写日期:	2016-6-13-18:10
*编写作者:	YPF
*功能描述:	异步PVP主功能菜单
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"

enum PvpMenuLayerTabType
{
	PvpMenuLayerTabNull = -1,						//null
	PvpMenuLayerTabDefense = 0,				//防守
	PvpMenuLayerTabRule,								//规则
	PvpMenuLayerTabShop,							//商店
	PvpMenuLayerTabRecord,						//记录
	PvpMenuLayerTabMax					
};

class CSelectDefense;
class CPvpRuleLayer;
class CWShopLayer;
class CPvpRecord;

class CPvpMenuLayer: public BaseLayer
{
public:
	CPvpMenuLayer();
	~CPvpMenuLayer();
	
	CREATE_LAYER(CPvpMenuLayer);

    virtual bool init();
    void onEnter();
    void onExit();

private:

	//信息读取特效
	void showOpenEffect();

	void showOpenEffectCallback();

	void onSwitchBtn(CCObject *pSender, bool bChecked);

	void onClose(CCObject* pSender);

	void updateTableSelected(CTableView* pTable, int iSelected);

	void updateShowArea();

	void selectTabPanel(PvpMenuLayerTabType iType, bool bForce=false);

private:
	CLayout* m_ui;
	PvpMenuLayerTabType m_iType;

	CSelectDefense* m_pSelectDefenseLayer;
	CPvpRuleLayer* m_pPvpRuleLayer;
	CWShopLayer* m_pShopLayer;
	CPvpRecord* m_pPvpRecordLayer;

	CCNode* m_pCurrentLayer;
};