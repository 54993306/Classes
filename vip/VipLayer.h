
#ifndef __VIP_LAYER_
#define __VIP_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"
#include "VipPower.h"

class CVipLayer: public BaseLayer
{
public:
	CREATE_LAYER(CVipLayer);
	CVipLayer();
	~CVipLayer();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();
	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);	
	void onToggle(CCObject* pSender);
	void setOptionType(int type);

	void updateMoney(int iAll);
protected:
	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);
	void ProcessMsg(int type, google::protobuf::Message *msg);
	void resVipShop(int type, google::protobuf::Message *msg);
	void onBuy(CCObject* pSender);
	void buyCardRes(int type, google::protobuf::Message *msg);
	void onConfirmBuy(CCObject* pSender);
	void updateBuyInfo(const TMessage& tMsg);
	void onPay(CCObject* pSender);
	void rechargeRes(int type, google::protobuf::Message *msg);

	void paySuccess(int money, RechargeRes *res);

	void onConfirmRecharge(CCObject* pSender);
	void updateRoleProperty(const TMessage& tMsg);
private:
	CLayout *m_ui;
	CGridView *m_gridView;
	CLayout *m_cell;
	CVipPower *m_power;
	CVipInfoRes m_vipInfo;
	vector<CGoldCard> m_goldCardList;
	int m_optionType;
	CGoldCard *m_card;
	bool m_bArchive;
};

#endif