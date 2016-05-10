#include "BackLayer.h"
#include "scene/layer/SetPanel.h"
#include "scene/layer/SetPanel.h"
#include "scene/CPopTip.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "guide/GuideManager.h"
#include "model/DataCenter.h"
#include "task/TaskControl.h"
using namespace cocos2d;

BackLayer::BackLayer(void)
	:m_setPanel(nullptr),m_backTip(nullptr)
{
}

BackLayer::~BackLayer(void)
{
}

bool BackLayer::init()
{
	this->setKeypadEnabled(true);
	return true;
}

void BackLayer::keyBackClicked()
{
	CScene *scene = CSceneManager::sharedSceneManager()->getCurrScene();
	if (CGuideManager::getInstance()->getIsRunGuide()) return;

	const string &name = scene->getClassName();

	if (!m_backTip)
	{
		if (LayerManager::instance()->getLayerCount()==0&&name=="CMainScene")
		{
			m_backTip = CPopTip::create();
			m_backTip->addContentTip(GETLANGSTR(5));
			m_backTip->setTouchEnabled(true);
			m_backTip->setTouchPriority(-100);
 			m_backTip->setButtonLisener(this,ccw_click_selector(BackLayer::onExitGame));
			this->addChild(m_backTip,10,1);

			return;
		}
		else if(LayerManager::instance()->getLayerCount()==0&&name=="WarScene")
		{
			CCNotificationCenter::sharedNotificationCenter()->postNotification(SHOW_PLAN_PANEL);
		}
		else if (LayerManager::instance()->getLayerCount()==0&&name=="LoginScene")
		{
			m_backTip = CPopTip::create();
			m_backTip->addContentTip(GETLANGSTR(5));
			m_backTip->setTouchEnabled(true);
			m_backTip->setTouchPriority(-100);
			m_backTip->setButtonLisener(this,ccw_click_selector(BackLayer::onExitGame));
			this->addChild(m_backTip,10,1);
		}
		else
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
}

void BackLayer::removePanel(cocos2d::CCObject* ob)
{
	//CC_SAFE_RELEASE(m_setPanel);
	m_setPanel = nullptr;
}


void BackLayer::onReturnLogin(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		//销毁多余数据，确保不影响切换账号重新登录
		DataCenter::destroy();
		CTaskControl::getInstance()->resetGameTips();
		CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
		CNetClient::getShareInstance()->setIsCloseConn(true);
		CCDirector::sharedDirector()->replaceScene(GETSCENE(LoginScene));
	}                                    
	else
	{
		LayerManager::instance()->pop();
		m_backTip->removeFromParent();
		m_backTip = nullptr;
	}
}

void BackLayer::onExitGame(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
		CCDirector::sharedDirector()->end();
	}                                    
	else
	{
		LayerManager::instance()->pop();
		m_backTip->removeFromParent();
		m_backTip = nullptr;
	}
}
