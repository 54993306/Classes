/******************************************************
*文件名称:	GamePlatfomDefine.h
*编写日期:	2016-6-29-16:38
*编写作者:	YPF
*功能描述:	第三方游戏发布平台标识
*******************************************************/
#pragma once

#include "cocos2d.h"

// 第三方游戏发布平台宏定义
#define G_PLATFORM_UNKNOWN						1				//公司内部自定义
#define G_PLATFORM_FACEBOOK						2				//facebook
#define G_PLATFORM_UC										3				//UC
#define G_PLATFORM_360									4				//360

//选用支付平台
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define G_PLATFORM_TARGET						G_PLATFORM_UNKNOWN
#else
#define G_PLATFORM_TARGET						G_PLATFORM_UNKNOWN
#endif



#if G_PLATFORM_TARGET == G_PLATFORM_UNKNOWN
#include "scene/layer/LoginLayerDefault.h"
#endif
#if G_PLATFORM_TARGET == G_PLATFORM_FACEBOOK
#include "scene/layer/LoginLayerFaceBook.h"
#endif
#if G_PLATFORM_TARGET == G_PLATFORM_UC
#include "scene/layer/LoginLayerUC.h"
#endif
#if G_PLATFORM_TARGET == G_PLATFORM_360
#include "scene/layer/LoginLayer360.h"
#endif