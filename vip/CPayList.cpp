#include "CPayList.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "common/CursorTextField.h"
#include "UserDefaultData.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "jni/CJniHelper.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "GMessage.h"
#include "model/DataDefine.h"
#include "CVipPay.h"
#include "CVipCard.h"
#include "tools/UICloneMgr.h"
#include "SDK/PayDefine.h"

CPayList::CPayList():m_ui(nullptr),m_payType(PayListTypeDefault),m_iIndexFlag(-1)
{

}

CPayList::~CPayList()
{

}

bool CPayList::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CPayListMask");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);
		pMaskLayer->setOpacity(255);
		pMaskLayer->setIsShowBlack(true);

		this->setVisible(true);

		//其他放到onEnter里去

		return true;
	}
	return false;
}

void CPayList::onEnter()
{
	BaseLayer::onEnter();
	initUI();
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
}


void CPayList::initUI()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_priceVec.push_back(50);
	m_priceVec.push_back(100);
	m_priceVec.push_back(300);
	m_priceVec.push_back(500);
	m_priceVec.push_back(1000);

	string strXaml ="PayList.xaml";
	switch (m_payType)
	{
	case PayListTypeGoogle:
		{
			strXaml = "GooglePayList.xaml";
			m_priceVec.clear();
			m_priceVec.push_back(50);
			m_priceVec.push_back(100);
			m_priceVec.push_back(300);
			m_priceVec.push_back(500);
			m_priceVec.push_back(1000);
			m_priceVec.push_back(3000);
		}break;
	case PayListTypeApple:
		{
			strXaml = "ApplePayList.xaml";
			m_priceVec.clear();
			m_priceVec.push_back(35);
			m_priceVec.push_back(179);
			m_priceVec.push_back(349);
			m_priceVec.push_back(699);
			m_priceVec.push_back(1750);
			m_priceVec.push_back(3500);
		}break;
	default:
		break;
	}

	m_ui = LoadComponent(strXaml.c_str());
	m_ui->setTag(1);
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui, 2);

	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CPayList::onClose));
	this->addChild(pClose, 999);

	//5个按钮
	for(int i=0; i<m_priceVec.size(); i++)
	{
		CButton *pButton = (CButton *)m_ui->findWidgetById(CCString::createWithFormat("btn_%d", i+1)->getCString());
		pButton->setTag(i+1);
		pButton->setOnClickListener(this,ccw_click_selector(CPayList::onPhonePay));
		pButton->setUserData(&m_priceVec.at(i));
	}
}


void CPayList::onExit()
{
	BaseLayer::onExit();	

	NOTIFICATION->postNotification(SHOW_TOP_LAYER);
}

void CPayList::onPhonePay( CCObject * pSender )
{
	CButton *pBtn = (CButton*)pSender;
	int iTag = pBtn->getTag();
	int *price = (int*)(pBtn->getUserData());

	switch (m_payType)
	{
	case PayListTypeDefault:
		{
			CVipPay *phone = CVipPay::create();
			LayerManager::instance()->push(phone);
			
			phone->setPayItem(iTag, *price);
		}
		break;
	case PayListTypeGoogle:
		{
			if(m_iIndexFlag == -1)
			{
				int iIndex = pBtn->getTag();
				CCLOG("[ warning ] : PayListTypeGoogle--iIndex--%d", iIndex);
				m_iIndexFlag = iIndex;
				scheduleOnce(schedule_selector(CPayList::updateForBuyForGoogle), 0.1f);
			}
		}
		break;
	case PayListTypeApple:
		{
			//TODO

		}
		break;
	default:
		break;
	}

}

void CPayList::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPayList::updateForBuyForGoogle( float dt )
{
	if(m_iIndexFlag != -1)
	{
		GooglePaySDK::getInstance()->pay(G_PAY, (GAME_PRODUCT)(m_iIndexFlag), m_iIndexFlag);
		m_iIndexFlag = -1;
	}
}
