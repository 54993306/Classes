
#ifndef __PVPRECORD_LAYER_
#define __PVPRECORD_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "activity/HttpLoadImage.h"

class CPvpRecord: public BaseLayer
{
public:
	CREATE_LAYER(CPvpRecord);
	CPvpRecord();
	~CPvpRecord();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);	
	void onToggle(CCObject* pSender);
	void setOptionType(int type);
protected:
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void onSelect(CCObject* pSender);
private:
	CLayout *m_ui;
	CTableView *m_tableView;
	CLayout *m_cell;
	CLayout *m_heroLay;
	CCSize m_tableSize;
};

#endif