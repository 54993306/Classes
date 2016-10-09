/******************************************************
*�ļ�����:	GameEventMonitor.h
*��д����:	2016-8-26-12:37
*��д����:	YPF
*��������:	��Ϸ�¼�����
*******************************************************/

#include "GameEventMonitor.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

#define CLASS_NAME "com/sdk/GameEventMonitorHelper"					//��Eclips�����ҵ�src�������Ӧ·��

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
	CCLOG("level:%s", sLevelName.c_str());
	CCLOG("difficulty:%s", sDifficulty.c_str());
	CCLOG("time:%d", iTimeToComplete);
	CCLOG("state:%d", iPlayerState);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "onLevelComplete", "(Ljava/lang/String;Ljava/lang/String;II)V")) {
		jstring jStr1 = minfo.env->NewStringUTF(sLevelName.c_str());
		jstring jStr2 = minfo.env->NewStringUTF(sDifficulty.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jStr1, jStr2, iTimeToComplete, iPlayerState);
		minfo.env->DeleteLocalRef(jStr1);
		minfo.env->DeleteLocalRef(jStr2);
	}
#endif
}

void GameEventMonitor::onAisPayComplete( string sProductId, int iPrice )
{
	CCLOG("sProductId:%s", sProductId.c_str());
	CCLOG("iPrice:%d", iPrice);
#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	if(JniHelper::getStaticMethodInfo(minfo, CLASS_NAME, "aisPaySuccess", "(Ljava/lang/String;I)V")) {
		jstring jStr1 = minfo.env->NewStringUTF(sProductId.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jStr1, iPrice);
		minfo.env->DeleteLocalRef(jStr1);
	}
#endif
}
