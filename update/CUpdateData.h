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

inline void ChangePicture(CCSprite* pSprite)
{
	if(pSprite==nullptr)
	{
		return;
	}
	CCTexture2D* pTexture = pSprite->getTexture();
	CCTexture2D* pNewTexture = nullptr;

	int iRand = ((int)(CCRANDOM_0_1()*100))%9+1;//首次随机
	do
	{
		pNewTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warScene/LoadImage/%d.png", iRand)->getCString());
		iRand ++;
		iRand = iRand%9+1;
		//第二次做按顺序移动操作，保证两次肯定找到新图片
	}
	while(pNewTexture==pTexture || pNewTexture==nullptr);

	pSprite->setTexture(pNewTexture);

	pSprite->setZOrder(-2);

	CCSprite* pTranstionSprite = CCSprite::createWithTexture(pTexture);
	pTranstionSprite->setPosition(pSprite->getPosition());
	pTranstionSprite->setAnchorPoint(pSprite->getAnchorPoint());
	pTranstionSprite->setScale(pSprite->getScale());
	pSprite->getParent()->addChild(pTranstionSprite, pSprite->getZOrder()+1);
	pTranstionSprite->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.6f), CCRemoveSelf::create()));

}