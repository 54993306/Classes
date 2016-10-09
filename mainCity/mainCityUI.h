#ifndef __MAIN_CITYUI__
#define __MAIN_CITYUI__

#include "scene/layer/LayerManager.h"
#include "protos/protocol.h"
#include "CityData.h"
#include "task/TaskControl.h"
#include "activity/HttpLoadImage.h"

enum Btn_Type
{
// 	Shop_Btn = 1, //商城
// 	Bag_Btn = 2,  //背包
// 	Embat_Btn = 3, //布阵
// 	 = 4, //出战   
// 	
// 	Set_Btn = 5,  //设置
// 	Social_Btn = 6, //社交
// // 	Shelter_Btn = 7, //避难所
// 	Switch_Btn = 7, //openBtn

	Shop_Btn =1,
	HeroInfo_Btn ,
	Bag_Btn,
	Task_Btn,
	Uion_Btn,
	Battle_Btn,
//	Switch_Btn,
	Activity_Btn = 7,
	Friend_Btn=8,
// 	Mail_Btn,
	Sign_Btn,
	
};

class CTimeGift;

class CMainCityUI: public BaseLayer, public HttpLoadImageDelegate
{
public:
	CMainCityUI():m_bShowChapterFlag(false),m_pTimeGift(nullptr){};
	virtual bool init();
	CREATE_LAYER(CMainCityUI);

	void onEnter();
	void onExit();
// 	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
// 	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void updateRoleProperty(const TMessage& tMsg);
	void updateTaskNotice(const TMessage& tMsg);
	void changeLabel(CLabel * label, int coin);

	void show(CCObject* pObj);
	void hide(CCObject* pObj);
    void imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer);

	void checkShowActivity(CCObject* pObj);

private: 
	void runMoveAction(int fromTag, int toTag, float moveTime);
	void runMovePreAction(int tag);
	void hideBtn(CCNode *pSender);
	void hideLabel(CCNode *node);
	void onClickBtn(CCObject *pSender);
	void onAddResourceBtn(CCObject* pSender);
	void updateMailNotice(const TMessage& tMsg);
	void onHeadImgBtn(CCObject *pSender);
	void updateActionTime(float dt);
	void processNetMsg(int type, google::protobuf::Message *msg);
	void onVip(CCObject *pSender);
	void runTollgatepreviewCallBack();

	//自动弹出签到
	void autoShowSign();
private:
	CLayout *m_ui;
	CCPoint m_btnPos[8];  
	CCPoint m_preMovePos; //地图移动点击坐标
	float  m_moveX[9];

	CLabel *m_coinLabel;
	CLabel *m_moneyLabel;
	CLabel *m_foodLabel;

	bool m_bShowChapterFlag;
	map<int,CCity> m_cityMap;
	CTimeGift			*m_pTimeGift;
public:
	void onTimeWaitCityAction(CCNode* pSender);
	void updateOpenState(CityData *cityData);
	void updateFuctionOpen(const TMessage& tMsg);
	void updateGameTip(const TMessage& tMsg);
	void showNoticeTip(CGameTips * ct);
	void setShowHeadPart(bool isSHow);
	void showHead(const TMessage& tMsg);
	void exchangeMsg(int type, google::protobuf::Message *msg);
};


#endif