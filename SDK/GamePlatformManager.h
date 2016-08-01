/******************************************************
*�ļ�����:	GamePlatformManager.h
*��д����:	2016-7-15-10:02
*��д����:	YPF
*��������:	��Ϸƽ̨����
*******************************************************/

#pragma once

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//�¼�
enum GamePlatformEvent
{
	GamePlatformEventNull,		
	GamePlatformEventDownloadFromGoogle,		//ֱ�Ӵ�google�ص�
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
	//ƽ̨��ʼ�����
	void InitPlatForm();

	//ƽ̨��¼����ѡ��
	BaseLayer *getLoginLayer();

	//ƽ̨�Ƴ����
	bool ExitPlatform();
	
	//ƽ̨��̳����������
	void ShowPlatformFloatWindow();

	//ƽ̨�������ر�
	void HidePlatformFloatWindow();

	//�Ʒ�չʾ���
	void ShowPayUI();

	//ƽ̨�汾����
	bool VersionUpdateWithPlatform( std::string sServerInfo );
	

	//��google�������°�
	void downloadFromGoogle( CCObject* pSender );

public:
	void init();

private:
	static GamePlatformManager						*m_pInstance;
	GamePlatformEvent										m_iEventTag;					//event_tag
};