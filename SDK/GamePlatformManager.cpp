/******************************************************
*�ļ�����:	GamePlatformManager.cpp
*��д����:	2016-7-15-10:13
*��д����:	YPF
*��������:	��Ϸƽ̨����
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
	//�������ص�����
	//if(m_pInstance != nullptr)
	//{
	//	//GetTcpNet->unRegisterAllMsgHandler(m_pInstance);
	//}
	CC_SAFE_DELETE(m_pInstance);
	m_pInstance = nullptr;
}


void GamePlatformManager::init()
{
	//��ʼ��

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


//ƽ̨��ʼ�����
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

	//ƽ̨��¼���
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

//ƽ̨�Ƴ����
bool GamePlatformManager::ExitPlatform()
{
	switch (G_PLATFORM_TARGET)
	{
	case G_PLATFORM_UC:
		{
#if COMPILE_UC
			//UC-�˳�
			LoginLayerUC::exit_with_uc();
			return true;
#endif
		}break;
	default:
		break;
	}

	return false;
}

//ƽ̨��̳����������
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

//ƽ̨�������ر�
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


//�Ʒ�չʾ���
void GamePlatformManager::ShowPayUI()
{
	//#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
	//	ShowPopTextTip(GETLANGSTR(191)); 
	//	return;
	//#endif

	//win32ƽ̨Ĭ����ʾ
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
			//ֻ��ʾGoogle
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


//ƽ̨�汾����
bool GamePlatformManager::VersionUpdateWithPlatform( std::string sServerInfo )
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//apk�汾С�ڵ���1.0.2
	if(CJniHelper::getInstance()->getVersionName().compare("1.0.2")<=0)
	{
		CUpdateLayer* pLayer = CUpdateLayer::create();
		CCDirector::sharedDirector()->getRunningScene()->addChild(pLayer, 999);
		//��ת����
		pLayer->showHelpTips(GETLANGSTR(2025), true);
		return true;
	}
#endif

	//���汾�ţ��Ƿ�Ҫ����
	bool isNeedUpdate = CUpdateLayer::checkUpdate( sServerInfo.c_str() );
	//��������
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
			//����汾�б���Ϣ
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
