/******************************************************
*文件名称:	GamePlatfomDefine.h
*编写日期:	2016-6-29-16:38
*编写作者:	YPF
*功能描述:	第三方游戏发布平台标识
*******************************************************/
#pragma once

#include "cocos2d.h"
#include "jni/CJniHelper.h"

// 第三方游戏发布平台宏定义
#define G_PLATFORM_UNKNOWN							0				//公司内部自定义
#define G_PLATFORM_AIS										1				//AIS
#define G_PLATFORM_GOOGLE_PLAY						2				//GooglePlay
#define G_PLATFORM_APP_STORE							3				//AppStore
#define G_PLATFORM_FACEBOOK							4				//facebook
#define G_PLATFORM_UC											5				//UC
#define G_PLATFORM_360										6				//360


#ifndef FACEBOOKSHARE
#define FACEBOOKSHARE
#endif // !FACEBOOKSHARE


//选用支付平台
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define G_PLATFORM_TARGET						G_PLATFORM_UNKNOWN
#else
#define G_PLATFORM_TARGET						(CJniHelper::getInstance()->getPlatform())
#endif