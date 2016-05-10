#pragma once

#include "cocos2d.h"
#include <string>

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
#define APP_LANGUAGE				AppLanguageType::AppLangChineseSim

//图片加密
#define ENCRYPT_IMAGE				1

//显示帧数信息
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	#define DISPLAY_INFO					1
#else
	#define DISPLAY_INFO					0
#endif

//拆分包下载
#define DOWNLOAD_EXTRA_PACKAGE 1

//更新开启
#define VERSION_UPDATE 1

//跳过网关服务器开起,直接连入游戏服务器
#define DIRECT_CONNECT_GAMESERVER 0

//服务器地址
struct GateServer
{
	std::string		sHost;
	int					iPort;
};