
#ifndef __SIGN_LAYER_
#define __SIGN_LAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"
#include "SignData.h"

class CMonsterInfo;
class CItemInfo;

class CSignLayer: public BaseLayer
{
public:
	CREATE_LAYER(CSignLayer);
	CSignLayer();
	~CSignLayer();

    virtual bool init();
    void onEnter();
    void onClose(CCObject* pSender);
    void onExit();

	CCObject* gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx);

	void updateHeroList(vector<CHero>& heroList);	

	void ProcessMsg(int type, google::protobuf::Message *msg);

	void updateTotoalSign();

	void updateSignDays();

	void onPress(CCObject* pSender, CTouchPressState iState);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void showFirstLoginEffect();

private:
	void showEffect();
	void showEffectCallBack();

	void hideEffect();
	void hideEffectCallBack();

	void popSelf();

protected:
   	void addGridCell(unsigned int uIdx, CGridViewCell* pCell);
	CCSprite* getPrize(const CSign &sign);
	void onResignDay(CCObject *pSender);

	void showHeroBook(CCObject* pSender, bool bChecked);
	//更新滑动区域
	void updateShowInfoScroll();

	void showCheckDayToCenter();

	void initHeroData();

	void onBtnResignDay(CCObject* pSender);
	void onBtnSignDay(CCObject *pSender);

	int getCurrentSignIndex();

	int autoNextSignStatusChange();

private:
	CLayout *m_ui;
	CGridView *m_gridView;
	CLayout *m_cell;
	CSignData m_signData;
	int m_resignGold;
	CLayout *m_signLay;
	bool m_bIsFirst;

	CMonsterInfo* m_pMonsterInfo;
	CItemInfo* m_pItemInfo;

	//展示滚动区域
	CScrollView* m_show_info_scroll;
	CLabel* m_pInfo1;//姓名（火）
	CLabel* m_pInfo2;//详细描述
	bool m_bIsReSign;	//是否为补签操作
	bool m_bTouchLock;
};

#endif