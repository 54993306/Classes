#pragma once

/******************************************************
*文件名称:	SignLayer.h
*编写日期:	2016-6-14-11:44
*编写作者:	YPF
*功能描述:	签到界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/CityData.h"
#include "net/CNetClient.h"
#include "bag/bagData.h"
#include "SignData.h"

class CMonsterInfo;
class CItemInfo;

//特效类
class ILineEffect : public CCNode
{
public:
	virtual void showEffect() = 0;
	virtual void hideEffect() = 0;
	virtual bool isTouchLock() = 0;
	virtual void bindUI(CLayout* ui) = 0;
	virtual void bindRectEffect(CCNode* pNode) = 0;
};

class CLineLightEffect : public ILineEffect
{
public:
	CLineLightEffect():m_ui(nullptr), m_bTouchLock(false),m_pRectEffectNode(nullptr){};

	bool isTouchLock(){ return m_bTouchLock;};
	void bindUI(CLayout* ui){m_ui = ui;};
	void bindRectEffect(CCNode* pNode){m_pRectEffectNode = pNode;};

	void showEffect();
	void showEffectCallBack();

private:
	void hideEffect();
	void hideEffectCallBack();
	void pop();
	void showRectEffect();

private:
	bool m_bTouchLock;
	CLayout* m_ui;
	CCNode* m_pRectEffectNode;
};

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

	void showTipsOnNextTotalSign();

	//总签到获取效果开箱+弹框(暂停一帧处理)
	void callBackForTotalSignEffect();
	void callBackForTotalSignEffectOneFrameLater();
	void callBackForTotalSignEffectEnd();

	//总签到奖励领取完后更新签到状况
	void callBackForTotalSignUiUpdate();

	//检测可开宝箱
	int checkTotalBox();

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

	ILineEffect* m_pLineEffect;
	CCSprite* m_pYellowTip;
	CLabel* m_pYellowTipText;
	CCClippingNode* m_pClip;
};