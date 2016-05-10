#pragma once

#include "cocos2d.h"

#define SUB_PACKAGE_PATH							(CCFileUtils::sharedFileUtils()->getWritablePath()+"download")													//资源分包的文件保存路径

#define VERSION_UPDATE_PATH						(CCFileUtils::sharedFileUtils()->getWritablePath()+"download/versionUpdate")						//版本更新的文件保存路径

#define OTHER_DOWNLOAD_FILE_PATH		VERSION_UPDATE_PATH																														//其他单独文件的下载路径

#define VERSION_FILE "version_info.json"

inline long getCurrentTime()    
{    
	cc_timeval tv;
	CCTime::gettimeofdayCocos2d(&tv,NULL);
	//都转化为毫秒
	long  msec= tv.tv_sec*1000+tv.tv_usec/1000;
	CCLOG("getCurrentTime=%ld", msec);
	return msec;
}
