#pragma once

/******************************************************
*文件名称:	CJniHelper.h
*编写日期:	2016-6-14-11:42
*编写作者:	YPF
*功能描述:	c++ <--> android 通信
*******************************************************/

#include "cocos2d.h"

USING_NS_CC;

const int SHARE_APP = 0x1001;	//分享
const int PAY_BUY = 0x1002;		//计费
const int SHOW_TIPS = 0x1003;		//系统提示
const int EXIT_APP = 0x1004;		//退出游戏
const int RESTART_APP = 0x1005;		//重启游戏
const int SHOW_LOADING = 0x1006;		//loading
const int SHOW_VIDEO = 0x1007;		//video
const int CHECK_NETWORK = 0x1008;		//检查网络连接
const int START_VIBRATOR = 0x1009;		//开始震动
const int STOP_VIBRATOR = 0x1010;			//停止震动

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

	 //存储版本号
	void saveVersion(const char* str);
	
	//存储资源下载路径
	void saveResourcesPath(const char* str);

	//重启游戏
	void restartGame();

	//显示加载界面
	void showLoading(bool bShow);

	//获取机器码
	std::string getDeviceId();

	//播放视频
	void showVideo(int iTag);

	//确认网络连接
	bool checkNetWork();

	//是否为WIFI网络
	bool checkWifi();

	//是否为WIFI网络
	std::string getVersionName();

	//开始震动
	void startVibrator(int iMsec);
	//停止震动
	void stopVibrator();

	//获取国家标签
	int getCountryType();

	//获取网关IPchai
	std::string getGateServerIp();
	//获取网关端口
	int getGateServerPort();

	//是否需要下载拆分包
	bool isNeedDownloadPackage();

	void jniResult(int iReqCode, int iResult);

	void setJniListener(CJniListener* pListener);

private:
	static CJniHelper* m_pJniHelper;
	CJniListener* m_pJniResultListener;
};