#ifndef _LUCKYWHEELLAYER_
#define _LUCKYWHEELLAYER_
/************************************************************************/
/* 
	文件名称：LuckyWheelLayer.h
	编写作者：WYS
	编写日期：16-8-15
	功能表述：转盘抽奖活动层
*/
/************************************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "common/CommonFunction.h"

#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "LuckyActive/LuckyWheelData.h"

//人物动画
#include <spine/spine-cocos2dx.h>
using namespace spine;

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
	void onUpdateItemListClick(CCObject* pSender);

	void updateUserData();
	void onPress(CCObject* obj, CTouchPressState state);

	void onWheelBtnOnceClick(CCObject* obj);
	void onWheelBtnTenClick(CCObject* obj);
	void isOKTenBtn(CCObject* pSendr);

	void randomItemArray();//随机排列奖励列表
	void followAngle();//黄标跟随指针
	void showOnceRotate();
	void showTenRotate();
	void callBackForTenActionGoing();
	void callBackForActionEnd();
	void callBackForShowGainPrize();

	void ProcessMsg(int type, google::protobuf::Message* msg);//网络回调
	void updateLotteryItem();	//更新奖励列表
	void initNoticeLabelShow();	//初始化公告显示
	void updateNoticeLabelShow();//更新公告显示

	void initUILightOffEffect();//初始化灯光效果
	void updateLightActionEffect(float delta);//灯光显示效果调度器
	void updateNoticeLabelShowSchedule(float delta);//更新公告信息滚动调度器

private:
	CMonsterInfo* m_monsterInfo;
	CItemInfo* m_itemInfo;

	CLayout* m_ui;
	CLayout* m_item;
	CLayout* m_pLayout;

	CImageView* m_pointer;
	CImageView* m_prizeItemTag;
	CButton* m_onceBtn;
	CButton* m_tenBtn;
	CButton* m_closeBtn;
	
	int m_randomAng;	//随机角度
	int m_iRotateAngle;	//旋转角度
	unsigned int m_pItemListNum;	//道具数量
	vector<CPrize > m_prizeList;	//接收奖励Item列表
	int m_itemRandomIndex[10];		//随机道具索引
	int m_prizeIndex[10];		//奖励索引

	int m_lotteryType;			//抽奖类型 1.once 2.ten
	int m_recordTime;			//记录次数
	bool m_isRotateAction;//是否在旋转
	CPrize m_getOncePrize;			//抽一次获得
	vector<CPrize > m_getPrizeList;	//抽十次获取列表
	
	vector<CNotice >m_noticeList;//接收公告列表信息
	CNotice m_judgePrize;//当获取列表list达到最大数，判断接收奖励的列表信息的新信息
	int m_onceNeedGold;
	int m_tenNeedGold;

	vector<CLayout* > m_noticeLab;//公告Table
	int m_currNoticeLab;//当前显示第几条公告信息
	int m_alreadyNoticeLabNum;//已显示所有公告
	int m_oldNoticeLabNum;//刷新前一次公告数量
	bool m_isUpdateNotice;//判断公告调度器是否在执行

	float m_minPos;
	float m_maxPos;

	int m_lightNum ; //记录light调度器次数
	int m_changelight;//记录UI调度器次数
	float m_iPrizeAngle;//光标显示的角度

	int m_startRotateTime;//记录开始旋转时间
	bool m_isSlow;	//是否在减速

	bool m_isUpdateItemList;//是否更新Item列表
};

#endif