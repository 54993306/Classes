#pragma once

/******************************************************
*文件名称:	CUpdateData.h
*编写日期:	2016-6-14-11:51
*编写作者:	YPF
*功能描述:	更新相关的基础数据和函数
*******************************************************/

#include "cocos2d.h"

USING_NS_CC;

#define WRITEABLE_PATH									(CCFileUtils::sharedFileUtils()->getWritablePath())

#define SUB_PACKAGE_PATH							(CCString::createWithFormat("%s%s", WRITEABLE_PATH.c_str(), "download")->m_sString)													//资源分包的文件保存路径

#define VERSION_UPDATE_PATH						(CCString::createWithFormat("%s%s", WRITEABLE_PATH.c_str(), "download/versionUpdate")->m_sString)						//版本更新的文件保存路径
#define FBIMG_PATH						(CCString::createWithFormat("%s%s", WRITEABLE_PATH.c_str(), "download/fbImg")->m_sString)						//版本更新的文件保存路径

#define OTHER_DOWNLOAD_FILE_PATH		VERSION_UPDATE_PATH																																										//其他单独文件的下载路径

#define VERSION_FILE "version_info.json"

#define PACKAGE_VERSION_FILE "package_version_info.json"

inline long getCurrentTime()    
{    
	cc_timeval tv;
	CCTime::gettimeofdayCocos2d(&tv,NULL);
	//都转化为毫秒
	long  msec= tv.tv_sec*1000+tv.tv_usec/1000;
	CCLOG("getCurrentTime=%ld", msec);
	return msec;
}