#pragma once

/******************************************************
*�ļ�����:	CJniHelper.h
*��д����:	2016-6-14-11:42
*��д����:	YPF
*��������:	c++ <--> android ͨ��
*******************************************************/

#include "cocos2d.h"

USING_NS_CC;

const int SHARE_APP = 0x1001;	//����
const int PAY_BUY = 0x1002;		//�Ʒ�
const int SHOW_TIPS = 0x1003;		//ϵͳ��ʾ
const int EXIT_APP = 0x1004;		//�˳���Ϸ
const int RESTART_APP = 0x1005;		//������Ϸ
const int SHOW_LOADING = 0x1006;		//loading
const int SHOW_VIDEO = 0x1007;		//video
const int CHECK_NETWORK = 0x1008;		//�����������
const int START_VIBRATOR = 0x1009;		//��ʼ��
const int STOP_VIBRATOR = 0x1010;			//ֹͣ��

class CJniListener {
public:
	virtual void jniResult(int nReqCode, int nResult) = 0;
};

class CJniHelper
{
public:

	CJniHelper();

	static CJniHelper* getInstance();

	static void destroy();

	 //�洢�汾��
	void saveVersion(const char* str);
	
	//�洢��Դ����·��
	void saveResourcesPath(const char* str);

	//������Ϸ
	void restartGame();

	//��ʾ���ؽ���
	void showLoading(bool bShow);

	//��ȡ������
	std::string getDeviceId();

	//������Ƶ
	void showVideo(int iTag);

	//ȷ����������
	bool checkNetWork();

	//�Ƿ�ΪWIFI����
	bool checkWifi();

	//�Ƿ�ΪWIFI����
	std::string getVersionName();

	//��ʼ��
	void startVibrator(int iMsec);
	//ֹͣ��
	void stopVibrator();

	//��ȡ���ұ�ǩ
	int getCountryType();

	//��ȡ����IPchai
	std::string getGateServerIp();
	//��ȡ���ض˿�
	int getGateServerPort();

	//�Ƿ���Ҫ���ز�ְ�
	bool isNeedDownloadPackage();

	void jniResult(int iReqCode, int iResult);

	void setJniListener(CJniListener* pListener);

private:
	static CJniHelper* m_pJniHelper;
	CJniListener* m_pJniResultListener;
};