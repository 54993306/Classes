﻿/******************************************************
*文件名称:	LoginScene.cpp
*编写日期:	2016-6-29-16:51
*编写作者:	YPF
*功能描述:	登录场景
*******************************************************/

#include "LoginScene.h"
#include "scene/layer/LayerManager.h"
#include "SDK/GamePlatformManager.h"


//网关服务器
GateServer LoginScene::getGateServer()
{
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
	//const GateServer GATE_SERVER = {"125.227.163.145",	5203};//李辉服务器
	//const GateServer GATE_SERVER = {"192.168.0.9",			7303};//大毛服务器
	//const GateServer GATE_SERVER = {"127.0.0.1",					5203};//本机服务器
	//const GateServer GATE_SERVER = {"192.168.1.2",			5203};//局域网服务器
	//const GateServer GATE_SERVER = {"192.168.0.2",			5203};//香港局域网服务器 
	const GateServer GATE_SERVER =    {"103.31.20.79",			7304};//外网服务器
	//const GateServer GATE_SERVER = {"dashisha.lollab.co",			5203};//泰国-境外 
	//const GateServer GATE_SERVER = {"dashisha.lollab.co",			7303};//泰国-境外 
	//const GateServer GATE_SERVER = {"192.168.0.80",			7303};//小闫
	//const GateServer GATE_SERVER = {"192.168.0.8",			7303};//先成
#else if CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID	
	const GateServer GATE_SERVER = {
		CJniHelper::getInstance()->getGateServerIp(), 
		CJniHelper::getInstance()->getGateServerPort()
	};
#endif


	return GATE_SERVER;
}

BaseLayer * LoginScene::getLoginLayerWithGamePlatfom()
{
	return GamePlatformMgr->getLoginLayer();
}

LoginScene::LoginScene()
{

}

LoginScene::~LoginScene()
{

}

void LoginScene::onCreate()
{
	////检查用户名和密码是不是空的
	////初始默认本地的账号和密码
	//string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
	//string sPassword = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
	//if(sUserName.compare("") == 0 || sPassword.compare("") == 0)
	//{
	//	CCString *pStr1 = CCString::createWithFormat("ios%ld%d", getCurrentTime()/1000, CCRANDOM_0_1()*10000);
	//	CCString *pStr2 = CCString::createWithFormat("ios%ld", getCurrentTime()/(CCRANDOM_0_1()*1000));
	//	sUserName = pStr1->m_sString;
	//	sPassword = pStr2->m_sString;

	//	CCUserDefault::sharedUserDefault()->setStringForKey(USER_NAME, sUserName);
	//	CCUserDefault::sharedUserDefault()->setStringForKey(PASSWORD, sPassword);
	//	CCUserDefault::sharedUserDefault()->flush();
	//}

	//创建登录界面
	BaseLayer *pLayer = getLoginLayerWithGamePlatfom();
	if( pLayer !=nullptr)
	{
		pLayer->setVisible(true);
		this->addChild(pLayer);
	}
	else
	{
		CCLOG("platform login class missing");
	}
}

void LoginScene::onEnter()
{
	CScene::onEnter();
}

void LoginScene::onExit()
{
	CScene::onExit();
}

void LoginScene::showWarning()
{
	//黑色屏幕

}

void LoginScene::callBackForInit()
{

}
