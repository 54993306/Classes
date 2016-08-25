#pragma once

/******************************************************
*文件名称:	LoginLayerMix.h
*编写日期:	2016-6-29-18:19
*编写作者:	YPF
*功能描述:	facebook 登录界面
*******************************************************/

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
#include "scene/LoginScene.h"

using namespace cn::sharesdk;

const static char *LoginDayFaceBook = "2016-05-07-11";

class LoginLayerMix: public BaseLayer, public MessageHandler, public CJniListener, public DownloadPackageDelegate
{
public:
	LoginLayerMix();
	~LoginLayerMix();

	CREATE_FUNC(LoginLayerMix);

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
	bool LinkedGameServer( );
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
	void initUI();

	//初始化UI
	void initLogin();
	//初始化登录选择层
	void initSelectLoginLayer( );
	//初始化进入游戏层
	void initEnterGameLayer( );

	void removeLogin();

	//初始化loading效果
	void initLoadingEffect();

	//进入游戏
	void callBackInGame();

	//游戏退出
	void onGameExit(CCObject* obj);

	//点击下载更新包按钮回调
	void onClickDownloadPackage(CCObject* pSender);

	//从google下载最新包
	void downloadFromGoogle( CCObject* pSender );

	
	//google登录相关
	void googleUserInfoCallback(CCObject *object);
	//google授权
	bool checkGoogleAuthor();
	void onGoogleAuthor( );
	//google登录
	void onGoogleLogin( );
	void doGoogleLogin(float delt);

	//游客登录
	void onTouristLogin( );
	void doTouristLogin(float delt);

	//facebook登录相关
	void faceBookAuthorCallback(CCObject *object);
	void faceBookUserInfoCallback(CCObject *object);
	//facebook授权
	bool checkFacebookAuthor();
	void onFacebookAuthor( );
	//facebook登录
	void onFacebookLogin( );
	void doFacebookLogin(float delt);


	//登录选择
	void onSelectLogin( CCObject *pObj );

	//选择登录和进入游戏层互相切换
	void switchLayer( );

	//进入游戏
	void onEnterGame( CCObject *pObj );

	//选择登入方式
	void onReSelectLogin( CCObject *pObj );

private:
	//播放声音回调
	void callbackForSound();
	void callBackForSoundUp();

	//日期开放相关
	void updateForCheckDay(float dt);
	//滚动字幕
	void showLabelRoll(const char* sInfo);
	void hideLableRoll();


private:
	CLayout					*m_ui;
	CLayout					*m_pLoginSelectLay;
	CLayout					*m_pEnterGameLay;
	CCArmature				*m_pArmature;
	CWidgetWindow		*m_LayoutRoot;
	int							m_iCurrentType;
	ServerList					m_serverInfo;
	string						m_FaceBookAccount;
	string						m_FaceBookUserID;
	int							m_iSelectServerIndex;
	CLayout					*m_pCell;
	CTableView				*m_pTableView;
	CCNode					*m_pLoading;
	LoginType					m_iLoginType;
	CCNode					*m_pClose;
};
