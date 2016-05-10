#include "CJniHelper.h"
#include "../proj.android/jni/hellocpp/CJni.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

CJniHelper* CJniHelper::m_pJniHelper = nullptr;


CJniHelper::CJniHelper()
{
	m_pJniResultListener = nullptr;
}

CJniHelper* CJniHelper::getInstance()
{
	if(!m_pJniHelper)
	{
		m_pJniHelper = new CJniHelper;
	}
	return m_pJniHelper;
}

void CJniHelper::destroy()
{
	CC_SAFE_DELETE(m_pJniHelper);
	m_pJniHelper = nullptr;
}


void CJniHelper::saveVersion( const char* str )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(str != nullptr) 
	{
		c_save_version(str);
	}
#endif
}


void CJniHelper::saveResourcesPath( const char* str )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(str != nullptr) 
	{
		c_save_resources_path(str);
	}
#endif
}

void CJniHelper::restartGame()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	c_restart_game();
#endif
}

void CJniHelper::showLoading( bool bShow )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	c_show_loading(bShow);
#endif
}

void CJniHelper::showVideo( int iTag )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	c_show_video(iTag);
#endif
}


std::string CJniHelper::getDeviceId()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_get_device_id();
#endif
	return "demo_lollab";
}


bool CJniHelper::checkNetWork()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_check_network();
#endif
	return true;
}


bool CJniHelper::checkWifi()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_check_wifi();
#endif
	return true;
}


std::string CJniHelper::getVersionName()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_get_version_name();
#endif
	return "1.0.0";
}



void CJniHelper::jniResult( int iReqCode, int iResult )
{
	if(m_pJniResultListener != nullptr) 
	{
		m_pJniResultListener->jniResult(iReqCode, iResult);
	}
}

void CJniHelper::setJniListener( CJniListener* pListener )
{
	if(pListener!=nullptr)
	{
		m_pJniResultListener = pListener;
	}
}

void CJniHelper::startVibrator( int iMsec )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	c_start_vibrator(iMsec);
#endif
}

void CJniHelper::stopVibrator()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	c_stop_vibrator();
#endif
}

