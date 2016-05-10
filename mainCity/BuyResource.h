#ifndef _BUYRESOURCE_
#define _BUYRESOURCE_

#include "scene/layer/LayerManager.h"
#include "AppUI.h"
#include "net/CNetClient.h"
#include "ExchangeData.h"

class BuyResource : public BaseLayer
{
public:
	BuyResource();
	~BuyResource();
	virtual bool init();
	void initByType(int but_type);
	CREATE_LAYER(BuyResource);
	void onClick(CCObject*ob);
	virtual void onEnter();
	virtual void onExit();
	void ProcessMsg(int type, google::protobuf::Message *msg);

	void runResAnimate(int index);

	void setExchangeType(int type);
	void exchangeToCoin();

	void updateExchange();

	void checkVip(CCObject* pObject);
	void exchangeOne(CCObject* pObject);
	void exchangeMore(CCObject* pObject);
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell* pCell);
	void exchangeToFood();
	void onContinueExchange(CCObject* pObject);
	void flyDone(CCNode *pNode);
	void resetPosition(CCNode *pNode);
	void updateTableOffset(float dt);
	void showCoinAnim(float dt);
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void roleUpdate(const TMessage& tMsg);
private:
	CLayout*  m_ui;
	int m_buyType;
	CLayout *m_vipConvertLay;
	CLayout *m_norConvertLay;
	CLayout *m_cell;
	CLayout *m_goldToCoinLay;

	CTableView *m_tableView;
	CExchangeInfo m_exchangeInfo;
	CExchangeRes m_exchangeRes;
	int m_exchangeType;
	int m_animIndex;
	int m_isPlay;
	int m_buyNum;
};

enum BuyResource_Type
{
	BUY_COIN = 1,		//粮食、铜钱、灵魂、金币
	BUY_FOOD,
	BUY_COPPER,
	BUY_GOLD,
};
#endif