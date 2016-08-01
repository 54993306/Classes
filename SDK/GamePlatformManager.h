/******************************************************
*文件名称:	GamePlatformManager.h
*编写日期:	2016-7-15-10:02
*编写作者:	YPF
*功能描述:	游戏平台管理
*******************************************************/

#pragma once

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//事件
enum GamePlatformEvent
{
	GamePlatformEventNull,		
	GamePlatformEventDownloadFromGoogle,		//直接从google回调
};

#define GamePlatformMgr (GamePlatformManager::getInstance())

class BaseLayer;

class GamePlatformManager : public CCObject 
{
public:
	GamePlatformManager();
	~GamePlatformManager();

	static GamePlatformManager *getInstance();
	static void  destroy();

	void updateForEvent( float dt );

public:
	//平台初始化相关
	void InitPlatForm();

	//平台登录界面选用
	BaseLayer *getLoginLayer();

	//平台推出相关
	bool ExitPlatform();
	
	//平台论坛悬浮窗开启
	void ShowPlatformFloatWindow();

	//平台悬浮窗关闭
	void HidePlatformFloatWindow();

	//计费展示相关
	void ShowPayUI();

	//平台版本更新
	bool VersionUpdateWithPlatform( std::string sServerInfo );
	

	//从google下载最新包
	void downloadFromGoogle( CCObject* pSender );

public:
	void init();

private:
	static GamePlatformManager						*m_pInstance;
	GamePlatformEvent										m_iEventTag;					//event_tag
};