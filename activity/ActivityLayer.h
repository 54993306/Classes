#pragma once
/******************************************************
*文件名称:	ActivityLayer.h
*编写日期:	2016-6-14-10:58
*编写作者:	YPF
*功能描述:	活动界面
*******************************************************/

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "ActivityData.h"
#include "HttpLoadImage.h"
#include "ActivityControl.h"

class ILineEffect;

class CActivityCollectLayer;

class CActivityLayer: public BaseLayer, public HttpLoadImageDelegate
{
public:
	CActivityLayer();
	~CActivityLayer();
	
	CREATE_LAYER(CActivityLayer);

    virtual bool init();
    void onEnter();
    void onExit();

	//网络回调
	void processNetMsg(int type, google::protobuf::Message *msg);

	//选项卡切换
	void selectTabPanel(ActivityTabType iType, bool bForce=false);

	//更新活动列表
	void updateBattleList(const TMessage& tMsg);

	//更新新品上架列表
	void updateNewSaleList(const TMessage& tMsg);

	//绑定图片加载回调
	void imageLoadSuccessCallBack(string sTag,  vector<char>* pBuffer=nullptr);

private:
	void initTabPart();

	void onSwitchBtn(CCObject *pSender, bool bChecked);
	void showSelectRadioImg(int selIndex);

	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	void onClose(CCObject* pSender);

	void onJoinActivity(CCObject* pSender);

	void activityCellClick(CCObject* pObj);

	void updateTableSelected(CTableView* pTable, int iSelected);

	void updateShowArea();

	//拼接字符串
	string linkUrlImageTag(ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag imageTag);

	//获取图片的唯一存储路径
	string getImageSavePath(ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag imageTag);

	//更新showInfo图片
	void updateShowInfoPicWithTexture(CCTexture2D* pTexture);

	//更新滑动区域
	void updateShowInfoScroll();

	//检查是否有数据
	bool checkDataHere(ActivityTabType iType);

	//通过id寻找对应的activity
	CActivity& getActivityByActId(ActivityTabType iType, int iActId);

	void buyGiftPopTipBack(CCObject* pSender);

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

private:
	string getImageReadPath(ActivityTabType activityType, CActivity& activity, ActivityLoadImageTag imageTag);
	/*int threadFunc();*/
	void hideAllCollectLayer();
	CActivityCollectLayer* getCActivityCollectLayerById(int iId);

private:
	CLayout* m_ui;

	CRadioButton* m_pRadioButton[ActivityTabMax];

	CLayout* m_cell[ActivityTabMax];

	CTableView *m_tableView[ActivityTabMax];

	int m_index_selected[ActivityTabMax];

	ActivityTabType m_activity_type;

	vector<CActivity> m_activity_list[ActivityTabMax];

	string m_base_url[ActivityTabMax];

	//展示图片
	CCSprite* m_show_info_sprite;
	//展示滚动区域
	CScrollView* m_show_info_scroll;

	//活动按钮
	CButton* m_join_activity;

	//按钮标签
	CLabel* m_join_activity_label;

	int m_iRequestLimit;

	CCNode* m_pLoading;

	ILineEffect* m_pLineEffect;
	//HttpGetImg* p1;
	std::map<int, CActivityCollectLayer*> m_mapCollectLayer;
};