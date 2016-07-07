/******************************************************
*文件名称:	LoginLayerUC.h
*编写日期:	2016-6-29-22:35
*编写作者:	YPF
*功能描述:	UC登录
*******************************************************/

#pragma once

#include "AppUI.h"
#include "scene/layer/LayerManager.h"
#include "net/MessageHandler.h"
#include "net/CNetClient.h"
#include "CocoStudio/GUI/UIWidgets/UITextField.h"
#include "cocos-ext.h"
#include "protos/gate_protocol.pb.h"
#include "C2DXShareSDK/C2DXShareSDK.h"
#include "jni/CJniHelper.h"
#include "common/CursorTextField.h"
#include "update/CDownloadPackageDelegate.h"
#include "ApplicationDefine.h"
#include "UserDefaultData.h"
#include "update/CUpdateData.h"

#include "CUCGameSdk.h"
#include "UCSdkCallback.h"
#include "UCSdk.h"

using namespace ucgamesdk;

#define LOG "UC_PLATFORM_PLUGIN" 

//检查用户名和密码是不是空的
void inline checkUsernameAndPassword()
{
	//初始默认本地的账号和密码
	string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
	if(sUserName.compare("") == 0 || sPassword.compare("") == 0)
	{
		CCString *pStr1 = CCString::createWithFormat("uc%ld%d", getCurrentTime()/1000, CCRANDOM_0_1()*10000);
		CCString *pStr2 = CCString::createWithFormat("cu%ld%d", getCurrentTime()/(CCRANDOM_0_1()*1000), CCRANDOM_0_1()*10000);
		sUserName = pStr1->m_sString;
		sPassword = pStr2->m_sString;

		CCUserDefault::sharedUserDefault()->setStringForKey(USER_NAME, sUserName);
		CCUserDefault::sharedUserDefault()->setStringForKey(PASSWORD, sPassword);
		CCUserDefault::sharedUserDefault()->flush();
	}
	//CCLOG("LoginLayerUC::linkedGameServerSuccess()_sUserName_%s", sUserName.c_str());
	//CCLOG("LoginLayerUC::linkedGameServerSuccess()_sPassword_%s", sPassword.c_str());
}

//登录自家服务器
void inline LOGIN_GAME(  string sExtra )
{
	checkUsernameAndPassword();
	string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
	//CCLOG("Login__________%s____________%s__________%s", sUserName.c_str(), sPassword.c_str(), sExtra.c_str());
	CNetClient::getShareInstance()->sendLogin(sUserName, sPassword, "", false, 0, sExtra);
	CCLOG("CNetClient::getShareInstance()->sendLogin");
};


class LoginLayerUC:  public BaseLayer, public MessageHandler, public CJniListener, public DownloadPackageDelegate
{
public:
	LoginLayerUC();
	~LoginLayerUC();
	CREATE_FUNC(LoginLayerUC);

	virtual bool init();
	
	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	//jni回调，处理视频播放完的事件
	void jniResult(int nReqCode, int nResult);

	//更新包下载完的回调
	void downloadPackageEnd(bool bAnswer);

private:
	//播视频
	void showVideo();

private:
	//骨骼帧事件监听
	void frameEventCallBack(CCBone* pBone, const char* sEvent, int iTag1, int iTag2);
	//骨骼动作事件监听
	void movementCallBack(CCArmature* pAramture, MovementEventType type, const char* sData);

private:
	//连接到网关服务器，请求服务器列表
	void LinkedServerListNet();
	//连接到游戏服务器
	void LinkedGameServer(const Server& server);
	//循环请求连接到网关服务器
	void scheduleForConnectGateServer(float dt);
	//网络请求回调
	bool ProcessMsg(int type, google::protobuf::Message *msg);

	//table相关
	CCObject* tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx);
	void addTableCell(unsigned int uIdx, CTableViewCell * pCell);

	//根据name获取server下标
	int getServerIndex(const std::string& sName);
	//更新选择的服务器在UI界面的显示
	void updateSelectServer();

	//选择某一个服务器
	void selectServer(CCObject* pObj);

	//显示/隐藏服务器列表
	void showServerList();
	void hideServerList();

	//检查服务器状态
	bool checkServerStatus(const Server &server);

private:
	//初始化/移除登录UI
	void initLogin();
	void removeLogin();
	//初始化输入框
	void addLoginField();

	//初始化loading效果
	void initLoadingEffect();

	//快速登录按钮回调
	void onQuickLogin(CCObject* obj);
	//游戏服务器没连接上则请求连接，然后延时再次快速登录
	void reConnectLogin(float dt);
	//成功连接上服务器
	void linkedGameServerSuccess();

	//进入游戏
	void callBackInGame();

	//游戏退出
	void onGameExit(CCObject* obj);

	//点击下载更新包按钮回调
	void onClickDownloadPackage(CCObject* pSender);

	//跳过网关直接进入游戏服务器
	void directConnectGameServer();

	void updateForCheckDay(float dt);

private:
	//播放声音回调
	void callbackForSound();
	void callBackForSoundUp();

	void showLabelRoll(const char* sInfo);
	void hideLableRoll();

	bool checkDay();


	//。。。。。。。。。。。。。。
public:
	
	static void uc_sdk_init();

	static void click_pay(CCObject* pSender);
	static void click_show_fb(CCObject* pSender);
	static void click_hide_fb(CCObject* pSender);

	static void exit_with_uc();

	//UC
private:
	
	void click_enter_uc(CCObject* pSender);
	//void click_hide_fb(CCObject* pSender);
	void click_logout(CCObject* pSender);
	void click_login(CCObject* pSender);
	//void click_pay(CCObject* pSender);
	void click_bbs(CCObject* pSender);
	//void click_show_fb(CCObject* pSender);
	void click_submit_ed(CCObject* pSender);
	void click_upoint(CCObject* pSender);

	static bool check_login_status();

	void loginMyGame( CCObject* pObj );
	void updateForLogin(float dt);

private:
	CLayout *m_ui;
	CLayout *m_pLoginLay;
	CLayout *m_pCurrentServerUI;
	CCArmature* m_armature;
	CWidgetWindow* m_LayoutRoot;
	int m_iCurrentType;
	ServerList m_serverInfo;

	int m_iSelectServerIndex;
	CLayout* m_pCell;
	CTableView* m_pTableView;
	CCNode* m_pLoading;
};
