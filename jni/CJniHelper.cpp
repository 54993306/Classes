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
	return "1.0.1";
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
	m_pJniResultListener = pListener;
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

int CJniHelper::getCountryType()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_get_country_type();
#endif
	CCAssert(false, "Wrong : should be call on android platform!");
	return 4;
}

std::string CJniHelper::getGateServerIp()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_get_gate_server_ip();
#endif
	CCAssert(false, "Wrong : should be call on android platform!");
	return "";
}

int CJniHelper::getGateServerPort()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_get_gate_server_port();
#endif
	CCAssert(false, "Wrong : should be call on android platform!");
	return 8888;
}

bool CJniHelper::isNeedDownloadPackage()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return c_is_need_download_package();
#endif
	CCAssert(false, "Wrong : should be call on android platform!");
	return true;
}

int CJniHelper::getPlatform()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//获取需要更新的所有版本
	std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
	if(sDefalutVersion.compare("1.0.2")>0)
	{
		//1.0.2以下的包，没有平台判断方法，
		return c_getPlatform();
	}
#endif
	return 0;
}

void CJniHelper::downloadNewVersionFromGooglePlay()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//获取需要更新的所有版本
	std::string sDefalutVersion = CJniHelper::getInstance()->getVersionName();
	if(sDefalutVersion.compare("1.0.2")>0)
	{
		//1.0.2以下的包，没有平台判断方法，
		return c_down_new_version_from_google_play();
	}
#endif
}

