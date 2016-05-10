#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "bag/bagData.h"

class PropGetInfo: public BaseLayer
{
public:
	PropGetInfo();
	CREATE_LAYER(PropGetInfo);
    virtual bool init();

    void onEnter();
    void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

private:
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
    void addTableCell(unsigned int uIdx, CTableViewCell * pCell);
	void onRecvTask(CCObject* pSender);
	void updateItemInfo();
	void onClose(CCObject* pSender);
	
	void evolveTask(const vector<CDropStage>& stageList );
	void processMessage(int type, google::protobuf::Message *msg);

private:
	CLayout *m_ui;
	CLayout *m_cell;
	CTableView *m_tableView;
	vector<CDropStage> m_stageList;
	CItem m_item;
};