#include "CPaySelect.h"
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
#include "CPayList.h"
#include "model/DataCenter.h"
#include "common/CommonFunction.h"
#include "SDK/GamePlatfomDefine.h"


CPaySelect::CPaySelect():m_ui(nullptr),m_payType(PhonePay)
{

}

CPaySelect::~CPaySelect()
{

}

bool CPaySelect::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("PaySelect");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		std::string sXaml = "PaySelectGoogle.xaml";
		if(CJniHelper::getInstance()->getPlatform() == G_PLATFORM_AIS)
		{
			sXaml = "PaySelectAis.xaml";
		}

		m_ui = LoadComponent(sXaml.c_str());
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);

		CColorView *grad = CColorView::create(ccc4(53, 53, 53,255));
		grad->setPosition(VCENTER);
		grad->setContentSize(CCSize(1138,640));
		m_ui->addChild(grad,-1);

		CColorView *view = CColorView::create(ccc4(255, 255, 255,255));
		view->setPosition(VCENTER);
		view->setContentSize(CCSize(1138,400));
		m_ui->addChild(view,-1);

		this->setVisible(true);

		//updatePayUiShow( /*CheckDay(PaySelectDay)*/true );
		////schedule(schedule_selector(CPaySelect::updateForCheckDay), 1.0f);

		return true;
	}
	return false;
}

void CPaySelect::onEnter()
{
	BaseLayer::onEnter();

	CImageViewScale9 *bg1 = (CImageViewScale9*)m_ui->findWidgetById("bg1");
	CImageViewScale9 *bg2 = (CImageViewScale9*)m_ui->findWidgetById("bg2");
	CImageViewScale9 *bg3 = (CImageViewScale9*)m_ui->findWidgetById("bg3");

    bg1->setTouchEnabled(true);
	bg1->setOnClickListener(this, ccw_click_selector(CPaySelect::onPhonePay));
	bg2->setTouchEnabled(true);
	bg2->setOnClickListener(this, ccw_click_selector(CPaySelect::onCardPay));
	bg3->setTouchEnabled(true);
	bg3->setOnClickListener(this,ccw_click_selector(CPaySelect::onGoolgePay));

	
	CButton* google = (CButton*)m_ui->findWidgetById("google");
	google->setOnClickListener(this,ccw_click_selector(CPaySelect::onGoolgePay));
	//google->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	//google->getSelectedImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));

	//
	CButton* phone = (CButton*)m_ui->findWidgetById("phone");
	phone->setOnClickListener(this, ccw_click_selector(CPaySelect::onPhonePay));


	CButton* card = (CButton*)m_ui->findWidgetById("card");
	card->setOnClickListener(this, ccw_click_selector(CPaySelect::onCardPay));

	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CPaySelect::onClose));
	this->addChild(pClose, 999);

	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
}

void CPaySelect::onExit()
{
	BaseLayer::onExit();	

	NOTIFICATION->postNotification(SHOW_TOP_LAYER);
}


void CPaySelect::onPhonePay( CCObject * pSender )
{
	CPayList *phone = CPayList::create();
	LayerManager::instance()->push(phone);
	m_payType = PhonePay;

}

void CPaySelect::onCardPay( CCObject * pSender )
{
// 	onClose(nullptr);
	CVipCard *card = CVipCard::create();
	LayerManager::instance()->push(card);
	m_payType = CardPay;

}

void CPaySelect::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPaySelect::onGoolgePay( CCObject * pSender )
{
	CPayList *pGoogleList = CPayList::create();
	pGoogleList->setPayType(PayListTypeGoogle);
	LayerManager::instance()->push(pGoogleList);

	m_payType = GooglePay;
}

int CPaySelect::getPayType()
{
	return m_payType;
}

void CPaySelect::updateForCheckDay( float dt )
{
	if(CheckDay(PaySelectDay))
	{
		//重新刷新按钮状态
		updatePayUiShow( true );

		unschedule(schedule_selector(CPaySelect::updateForCheckDay));
	}
}

void CPaySelect::updatePayUiShow( bool bShowAll )
{
	const char *str[6] = { "google", "bg3", "desc_Copy10",  "card", "cardid", "bg2"};

	for(int i=0; i<6; i++)
	{
		CCNode *pNode =  m_ui->findWidgetById(str[i]);
		pNode->setVisible(bShowAll);
	}

}

