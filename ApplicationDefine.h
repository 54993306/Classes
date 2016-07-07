#pragma once

/******************************************************
*文件名称:	ApplicationDefine.h
*编写日期:	2016-6-14-11:53
*编写作者:	YPF
*功能描述:	游戏基础数据配置
*******************************************************/

#include "cocos2d.h"
#include <string>
#include "jni/CJniHelper.h"

//语言类型
enum struct AppLanguageType
{
	AppLangChineseCom= 1,		//中文繁体
	AppLangChineseSim,				//中文简体
	AppLangEnglish,						//英文
	AppLangThailand,					//泰文
	AppLangMax
};


//多语言资源路径
const std::string RES_PATH_WITH_LANG[(int)AppLanguageType::AppLangMax] = 
{
	"",
	"ChineseCom",
	"",						//默认
	"English",
	"Thailand"
};

//语言类型
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define APP_LANGUAGE				(AppLanguageType)(CJniHelper::getInstance()->getCountryType())
#else
#define APP_LANGUAGE				AppLanguageType::AppLangThailand
#endif

//图片加密
#define ENCRYPT_IMAGE				1

//显示帧数信息
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	#define DISPLAY_INFO					0
#else
	#define DISPLAY_INFO					1
#endif

//拆分包下载开启
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define DOWNLOAD_EXTRA_PACKAGE (CJniHelper::getInstance()->isNeedDownloadPackage())
#else
#define DOWNLOAD_EXTRA_PACKAGE 0
#endif

//更新开启
#define VERSION_UPDATE 1

//跳过网关服务器开起,直接连入游戏服务器
#define DIRECT_CONNECT_GAMESERVER 0

//扫描在使用的文件
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define OPEN_SCAN_USED_FILE		0
#else
#define OPEN_SCAN_USED_FILE		0
#endif

//服务器地址
struct GateServer
{
	std::string			sHost;
	int					iPort;
};