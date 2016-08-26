/******************************************************
*文件名称:	GameEventMonitor.h
*编写日期:	2016-8-26-12:37
*编写作者:	YPF
*功能描述:	游戏事件监听
*******************************************************/

#pragma once

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class GameEventMonitor : public CCObject 
{
public:
	GameEventMonitor();
	~GameEventMonitor();

	static GameEventMonitor *getInstance();
	static void  destroy();

public:
	//需要监听的游戏事件
	//关卡完成
	void onLevelComplete( string sLevelName, string sDifficulty, int iTimeToComplete, int iPlayerState );

private:
	void updateForUiThread(float dt);

private:
	static GameEventMonitor			*m_pInstance;
};