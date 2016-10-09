#ifndef _PAIRCARDSACTIVITY_
#define _PAIRCARDSACTIVITY_

/************************************************************************/
/*
	文件名称：PairCardsActivity.h
	编写作者：WYS
	编写日期：16-8-26
	功能表述：翻牌活动层
*/
/************************************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"

#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "LuckyActive/LuckyWheelData.h"

enum GameState
{
	unStartGame = 1,
	StartGame,
	gameOver
};

class CMonsterInfo;
class CItemInfo;

#define SHOWPRIZELIST_SIZE 20

class PairCardsActivityLayer : public BaseLayer
{
public:
	PairCardsActivityLayer();
	~PairCardsActivityLayer();
	CREATE_LAYER(PairCardsActivityLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClose(CCObject* obj);
	void callFuncClose(CCObject* pSender);

	void ProcessMsg(int type ,google::protobuf::Message* msg);
	void gameRuleExplain(CCObject* pSender);
	void initCards();
	void initCardsData();
	void updateCardsData();
	void judegeHaveSameCardsAndAction(CPrize* item);
	void setCardsListTouchEnableTo(bool enable);
	void setImageAllChildVisibleTo(CImageView* image,bool visible);
	void updateUserData();

	void onPress(CCObject* obj, CTouchPressState state);
	void updateOneCards();
	void startAction();
	void callFuncNOpenCardsAction(CCNode* node);
	void callFuncNOpenCardsOverAction(CCNode* node);
	void isBuyOpenCardsNum(CCObject* pSender);
	void showPrizePopItem();
	void openAllCards();
	void callFuncNShowPrizePopItemAction(CCNode* node);
	void callFuncSetBtnToTure();

	void onStartGameBtnClick(CCObject* obj);
	void startGame();
	void isGiveUpThisGame(CCObject* pSender);
	void callFuncStartGameAction(CCNode* node);
	void callFuncShowPos();
	void callFuncStartGameOverAction();

	void initNotice();
	void updateNoticeLabelShow(float delta);

private:
	CItemInfo* m_itemInfo;
	CMonsterInfo* m_monsterInfo;

	CLayout* m_ui;
	CImageView* m_cardsImage;
	CImageView* m_cardsBlueFrame;
	CLabel* m_cardsNameLab;
	CImageView* m_cardsPosImage;

	CImageView* m_redPoint;
	CLabel* m_currFreeTimesLab;
	CLabel* m_noticeLabel;
	CButton* m_startGameBtn;
	CButton* m_closeBtn;
	CCTexture2D* m_texture_on;
	CCTexture2D* m_texture_off;

	//CPrize m_prize;
	vector<CImageView* > m_ImageList;
	vector<CCPoint > m_cardsPos;	//记录卡牌位置
	vector<CPrize > m_prizeItemList;	//接收卡牌列表
	vector<CPrize >m_showItemList;	//已打开卡牌的列表
	vector<CPrize >m_getPrizeList;	//获得的资源列表
	vector<CNotice > m_noticeList;//接收公告列表
	vector<CLabel* >m_noticeLab;

	bool m_isFree;	//是否免费
	int m_needGold_GameTimes;	//非免费次数花费
	int m_needGold_otherPairCardTimes;	//额外翻牌花费
	int m_gameTimes_free;	//每局免费翻牌数
	int m_gameTimes_Buy;	//每局可购买次数

	int m_cardsIndex;//18张卡牌索引值
	int m_index_click[50];//已打开卡牌的索引数组
	int m_gameNum;//游戏次数
	int m_cardsNum;//卡片个数
	int m_gameState;	//游戏状态
	int m_openCardsNum;	//打开卡片个数
	int m_currCard;//记录发牌动画时card

	bool isShowPrizePopItem;	//是否显示奖励弹框
	int m_record_onPressTimes;	//记录点击卡牌次数
	
	float m_pairTime;	//卡牌scale时间
	float m_minPosY;
	float m_maxPosY;
	int m_currNoticeLab;	//当前显示公告条数
	bool m_isUpdateNotice;

	bool m_isInit;
};

#endif 