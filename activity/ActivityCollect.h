#pragma once

/******************************************************
*文件名称:	ActivityCollect.h
*编写日期:	2016-5-17-11:28
*编写作者:	YPF
*功能描述:	活动界面下的收集字的单独界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "ActivityControl.h"
#include "protos/protocol.h"

class CMonsterInfo;
class CItemInfo;

class CActivityCollectLayer: public BaseLayer
{
public:
	enum ExchangeType
	{
		ExchangeTypeNormal = 100,		//普通兑换
		ExchangeTypeByLevel = 101		//按等级兑换
	};

	CActivityCollectLayer();
	~CActivityCollectLayer();
	
	CREATE_LAYER(CActivityCollectLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	void initBaseData(google::protobuf::RepeatedPtrField< ::protos::ExList >& data, int iActId);

	//重置兑换信息
	void restData();

	CC_SYNTHESIZE(ExchangeType, m_iExchangeType, ExchangeType)

private:
	void initTableView();

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);


	void collectCellBlankClick(CCObject* pObj);
	void collectCellClick(CCObject* pObj);

	void updateTableSelected(CTableView* pTable, int iSelected);

	void onPress(CCObject* pSender, CTouchPressState iState);

	bool initListCellIcon(google::protobuf::RepeatedPtrField<Prize>* prizeList, CCPoint pStartPos, int iGapX, CCNode* pParent, bool bShowMaxData);

	bool initCellIconByData(CLayout *pCell, Prize* prize, bool bShowMaxData);

private:
	CLayout* m_ui;

	CLayout* m_cellIcon;
	CLayout* m_cellBlank;

	CTableView *m_tableView;

	int m_index_selected;

	google::protobuf::RepeatedPtrField< ::protos::ExList >* m_pData;
	int m_iActId;

	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;

	//当前兑换条目
	ExList* m_pCurrentExlist;
};