/******************************************************
*�ļ�����:	GameEventMonitor.h
*��д����:	2016-8-26-12:37
*��д����:	YPF
*��������:	��Ϸ�¼�����
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
	//��Ҫ��������Ϸ�¼�
	//�ؿ����
	void onLevelComplete( string sLevelName, string sDifficulty, int iTimeToComplete, int iPlayerState );

private:
	void updateForUiThread(float dt);

private:
	static GameEventMonitor			*m_pInstance;
};