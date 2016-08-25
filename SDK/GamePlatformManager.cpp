/******************************************************
*文件名称:	GamePlatformManager.cpp
*编写日期:	2016-7-15-10:13
*编写作者:	YPF
*功能描述:	游戏平台管理
*******************************************************/

#include "GamePlatformManager.h"
#include "scene/layer/LoginLayerDefault.h"
#include "scene/layer/LoginLayerMix.h"
#include "jni/CJniHelper.h"
#include "vip/CPaySelect.h"
#include "vip/CPayList.h"
#include "scene/CPopTip.h"
#include "update/CUpdateLayer.h"
#include "tools/ShowTexttip.h"
#include "GamePlatfomDefine.h"
#include "model/DataCenter.h"
#include "GamePlatfomCompile.h"
#include "ApplicationDefine.h"

#if COMPILE_UC
#include "scene/layer/LoginLayerUC.h"
#endif

GamePlatformManager *GamePlatformManager::m_pInstance = nullptr;

GamePlatformManager::GamePlatformManager()
	:m_iEventTag(GamePlatformEventNull)
{
	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(GamePlatformManager::updateForEvent), this, 0, kCCRepeatForever, 0, false);
}

GamePlatformManager::~GamePlatformManager()
{
	m_pInstance = nullptr;

	CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
}

GamePlatformManager * GamePlatformManager::getInstance()
{
	if( m_pInstance == nullptr )
	{
		m_pInstance = new GamePlatformManager;
		m_pInstance->init();
	}
	return m_pInstance;
}

void GamePlatformManager::destroy()
{
	//解绑网络回调处理
	//if(m_pInstance != nullptr)
	//{
	//	//GetTcpNet->unRegisterAllMsgHandler(m_pInstance);
	//}
	CC_SAFE_DELETE(m_pInstance);
	m_pInstance = nullptr;
}


void GamePlatformManager::init()
{
	//初始化

}

void GamePlatformManager::updateForEvent( float dt )
{
	switch (m_iEventTag)
	{
	case GamePlatformEventNull:
		break;
	case GamePlatformEventDownloadFromGoogle:
		{
			CJniHelper::getInstance()->downloadNewVersionFromGooglePlay();

			m_iEventTag = GamePlatformEventNull;
		}
		break;
	default:
		break;
	}
}


//平台初始化相关
void GamePlatformManager::InitPlatForm()
{
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UC:
		{
#if COMPILE_UC
			LoginLayerUC::uc_sdk_init();
#endif
		}break;
	default:
		break;
	}
}

BaseLayer *GamePlatformManager::getLoginLayer()
{
	//return LoginLayerMix::create();

	//平台登录相关
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UNKNOWN:
		{
			return LoginLayerDefault::create();
		}break;
	case G_PLATFORM_AIS:
	case G_PLATFORM_GOOGLE_PLAY:
	case G_PLATFORM_APP_STORE:
	case G_PLATFORM_FACEBOOK:
		{
			return LoginLayerMix::create();
		}break;
	//case G_PLATFORM_UC:
	//	{
	//		return LoginLayerUC::create();
	//	}break;
	default:
		break;
	}

	return nullptr;
}

//平台推出相关
bool GamePlatformManager::ExitPlatform()
{
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UC:
		{
#if COMPILE_UC
			//UC-退出
			LoginLayerUC::exit_with_uc();
			return true;
#endif
		}break;
	default:
		break;
	}

	return false;
}

//平台论坛悬浮窗开启
void GamePlatformManager::ShowPlatformFloatWindow()
{
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UC:
		{
#if COMPILE_UC
			LoginLayerUC::click_show_fb(nullptr);
#endif
		}break;
	default:
		break;
	}
}

//平台悬浮窗关闭
void GamePlatformManager::HidePlatformFloatWindow()
{
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UC:
		{
#if COMPILE_UC
			LoginLayerUC::click_hide_fb(nullptr);
#endif
		}break;
	default:
		break;
	}
}


//计费展示相关
void GamePlatformManager::ShowPayUI()
{
	//#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
	//	ShowPopTextTip(GETLANGSTR(191)); 
	//	return;
	//#endif

	//win32平台默认显示
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	CPaySelect *paySel = CPaySelect::create();
	LayerManager::instance()->push(paySel);
	return;
#endif


	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UNKNOWN:
		{
			CPaySelect *paySel = CPaySelect::create();
			LayerManager::instance()->push(paySel);
		}break;
	case G_PLATFORM_UC:
		{
			ShowPopTextTip(GETLANGSTR(2015));
		}break;
	case G_PLATFORM_GOOGLE_PLAY:
		{
			//只显示Google
			if(!DataCenter::sharedData()->getUser()->getUserData()->getGoogleBilling())
			{
				CPayList *pGoogleList = CPayList::create();
				pGoogleList->setPayType(PayListTypeGoogle);
				LayerManager::instance()->push(pGoogleList);
			}
			else
			{
				CPaySelect *paySel = CPaySelect::create();
				LayerManager::instance()->push(paySel);
			}
		}break;
	case G_PLATFORM_AIS:
		{
			CPaySelect *paySel = CPaySelect::create();
			LayerManager::instance()->push(paySel);
			return;
		}break;
	case G_PLATFORM_APP_STORE:
		{
			CPayList *pList = CPayList::create();
			pList->setPayType(PayListTypeApple);
			LayerManager::instance()->push(pList);
			return;
		}break;
	default:
		break;
	}
};


//平台版本更新
bool GamePlatformManager::VersionUpdateWithPlatform( std::string sServerInfo )
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//apk版本小于等于1.0.2
	if(CJniHelper::getInstance()->getVersionName().compare("1.0.2")<=0)
	{
		CUpdateLayer* pLayer = CUpdateLayer::create();
		CCDirector::sharedDirector()->getRunningScene()->addChild(pLayer, 999);
		//跳转下载
		pLayer->showHelpTips(GETLANGSTR(2025), true);
		return true;
	}
#endif

	//检查版本号，是否要更新
	bool isNeedUpdate = CUpdateLayer::checkUpdate( sServerInfo.c_str() );
	//创建更新
	if(!isNeedUpdate)
	{
		return false;
	}

	switch (G_PLATFORM_TARGET)
	{
	//case G_PLATFORM_GOOGLE_PLAY:
	//	{
	//		CPopTip *pop = CPopTip::create();
	//		LayerManager::instance()->pop();
	//		pop->setVisible(true);
	//		pop->addContentTip(CCString::createWithFormat("Please download new version %s !", sServerInfo.c_str())->getCString());
	//		pop->setTouchEnabled(true);
	//		pop->setTouchPriority(-100);
	//		pop->showConfirmOnly();
	//		pop->setButtonLisener(this, ccw_click_selector(GamePlatformManager::downloadFromGoogle));
	//		CCDirector::sharedDirector()->getRunningScene()->addChild(pop, 899);

	//	}break;
	default:
		{
#if VERSION_UPDATE
			CUpdateLayer *pLayer = CUpdateLayer::create();
			pLayer->setVersion(sServerInfo.c_str());
			CCDirector::sharedDirector()->getRunningScene()->addChild(pLayer, 999);
			//请求版本列表信息
			pLayer->requestVersionInfo();
#endif
		}
		break;
	}
	return true;
}

void GamePlatformManager::downloadFromGoogle( CCObject* pSender )
{
	if(m_iEventTag==GamePlatformEventNull)
	{
		m_iEventTag = GamePlatformEventDownloadFromGoogle;
	}
}
