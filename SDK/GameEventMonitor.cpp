/******************************************************
*文件名称:	GameEventMonitor.h
*编写日期:	2016-8-26-12:37
*编写作者:	YPF
*功能描述:	游戏事件监听
*******************************************************/

#include "GameEventMonitor.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#define CLASS_NAME "com/sdk/GameEventMonitorHelper"					//在Eclips里面找到src里面的相应路径

#endif

GameEventMonitor *GameEventMonitor::m_pInstance = nullptr;

GameEventMonitor::GameEventMonitor()
{
	//TODO
	CCLOG("cocos::GameEventMonitor::GameEventMonitor");

	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GameEventMonitor::updateForUiThread), this, 0, kCCRepeatForever, 0, false);
}

GameEventMonitor::~GameEventMonitor()
{
	//TODO
	CCLOG("cocos::GameEventMonitor::~GameEventMonitor");

	m_pInstance = nullptr;

	CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
}

GameEventMonitor * GameEventMonitor::getInstance()
{
	if( m_pInstance == nullptr )
	{
		m_pInstance = new GameEventMonitor;
	}
	return m_pInstance;
}

void GameEventMonitor::destroy()
{
	CC_SAFE_DELETE(m_pInstance);
	m_pInstance = nullptr;
}

void GameEventMonitor::updateForUiThread( float dt )
{

}

void GameEventMonitor::onLevelComplete( string sLevelName, string sDifficulty, int iTimeToComplete, int iPlayerState )
{
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "onLevelComplete", "(Ljava/lang/String;Ljava/lang/String;II)V")) {
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, sLevelName.c_str(), sDifficulty.c_str(), iTimeToComplete, iPlayerState);
	}
#endif
}
