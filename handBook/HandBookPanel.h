/******************************************************
*文件名称:	HandBookPanel.h
*编写日期:	2016-8-23-15:42
*编写作者:	YPF
*功能描述:	图鉴外框
*******************************************************/
#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "HandBookControl.h"


class CHandBookHeroLayer;
class CHandBookMonsterLayer;
class CHandBookEquipLayer;

class CHandBookPanel: public BaseLayer
{
public:
	CHandBookPanel();
	~CHandBookPanel();
	
	CREATE_LAYER(CHandBookPanel);

    virtual bool init();
    void onEnter();
    void onExit();

private:
	void onSwitchBtn(CCObject *pSender, bool bChecked);

	void showSelectRadioImg(int selIndex);

	void onClose(CCObject* pSender);

	void updateTableSelected(CTableView* pTable, int iSelected);

	void updateShowArea();

	void selectTabPanel(HandBookTabType iType, bool bForce=false);

private:
	CLayout											*m_ui;
	HandBookTabType							m_iType;
	CHandBookHeroLayer						*m_pHeroLayer;
	CHandBookMonsterLayer					*m_pMonsterLayer;
	CHandBookEquipLayer						*m_pEquipLayer;
};