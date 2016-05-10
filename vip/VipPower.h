
#ifndef __VIP_POWER_
#define __VIP_POWER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"
#include "VipData.h"

class CMonsterInfo;
class CItemInfo;

class CVipPower: public BaseLayer
{
public:
	CREATE_LAYER(CVipPower);
	CVipPower();
	~CVipPower();

    virtual bool init();
    void onEnter();
	void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void updateVipPriv(CVipInfoRes *pVipInfo);	
	void ProcessMsg(int type, google::protobuf::Message *msg);

	void onPress(CCObject* pSender, CTouchPressState iState);

protected:
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void onLeft(CCObject* pSender);
	void onRight(CCObject* pSender);
	void enableBtn(CCNode *pNode);
private:
	CLayout *m_ui;
	CGridView *m_gridView;
	CTableView *m_tableView;
	CLayout *m_cell;
	CLayout *m_privlcell;

	CVipDesc m_vipDesc;
	CVipData m_vip;
	int m_vipLev;

	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;
};

#endif