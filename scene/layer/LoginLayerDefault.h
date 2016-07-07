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

class LoginLayerDefault:  public BaseLayer, public MessageHandler, public CJniListener, public DownloadPackageDelegate
{
public:
	LoginLayerDefault();
	~LoginLayerDefault();
	CREATE_FUNC(LoginLayerDefault);

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
	void scheduleForConnect(float dt);
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

	//显示或隐藏登录UI
	void showOrHideLogin(CCObject* obj);

	//初始化loading效果
	void initLoadingEffect();

	//注册
	void onRegister(CCObject* obj);

	//登录按钮回调
	void onLogin(CCObject* obj);
	//游戏服务器没连接上则请求连接，然后延时再次登录
	void doLogin(float delt);

	//快速登录按钮回调
	void onQuickLogin(CCObject* obj);
	//游戏服务器没连接上则请求连接，然后延时再次快速登录
	void reConnectLogin(float dt);

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

	
private:
	CLayout *m_ui;
	CLayout *m_logLay;
	CCArmature* m_armature;
	CWidgetWindow* m_LayoutRoot;
	int m_iCurrentType;
	ServerList m_serverInfo;

	CursorTextField* m_textFiledAccount;
	CursorTextField* m_textFiledPassword;
	CursorTextField* m_accessCodeText;
	
	bool m_isRegister;
	int m_iSelectServerIndex;
	CLayout* m_pCell;
	CTableView* m_pTableView;
	CCNode* m_pLoading;
};
