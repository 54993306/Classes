#ifndef _LUCKYWHEELLAYER_
#define _LUCKYWHEELLAYER_

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"

#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "LuckyWheel/LuckyWheelData.h"
//#include <stdlib.h>

class CMonsterInfo;
class CItemInfo;

class CLuckyWheelLayer : public BaseLayer
{
public:
	CLuckyWheelLayer(void);
	~CLuckyWheelLayer(void);
	CREATE_LAYER(CLuckyWheelLayer);
	virtual bool init();
	void onEnter();
	void onExit();
	void onClose(CCObject* obj);

	void onWheelBtnOnceClick(CCObject* obj);
	void onWheelBtnTenClick(CCObject* obj);
	void isOKTenBtn(CCObject* pSendr);

	void followAngle();
	void showOnceRotate();
	void showTenRotate();
	void callBackForTenActionGoing();
	void callBackForActionEnd();
	void callBackForShowGainPrize();

	void initUILightOffEffect();

	//网络回调
	void ProcessMsg(int type, google::protobuf::Message* msg);
	void updateLotteryItem();

	void initNoticeLabelShow();
	void updateNoticeLabelShow();
	//调度器
	void updateLightActionEffect(float delta);
	void updateNoticeLabelShowSchedule(float delta);

	void updateUserData();

	void onPress(CCObject* obj, CTouchPressState state);

private:
	CMonsterInfo* m_monsterInfo;
	CItemInfo* m_itemInfo;

	CLayout* m_ui;
	CLayout* m_item;
	CImageView* m_pointer;
	CImageView* m_prizeItemTag;
	CButton* m_onceBtn;
	CButton* m_tenBtn;
	CButton* m_closeBtn;

	int m_randomAng;
	int m_iRotateAngle;			//旋转角度
	unsigned int m_pItemListNum;	//道具数量
	int m_prizeIndex[10];		//道具索引

	int m_lotteryType;	//抽奖类型 1.once 2.ten
	int m_recordTime;	//记录次数
	bool m_isRotateAction;
	CPrize m_getOncePrize;			//抽一次获得
	vector<CPrize > m_getPrizeList;	//抽十次获取

	vector<CPrize > m_prizeList;
	vector<CNotice >m_noticeList;
	int m_onceNeedGold;
	int m_tenNeedGold;

	vector<CLayout* > m_noticeLab;
	int m_showNoticeLabNum;
	float m_minPos;
	float m_maxPos;

	int m_lightNum ; //记录light调度器次数
	int m_changelight;
	float m_iPrizeAngle;//光标显示的角度
};

#endif