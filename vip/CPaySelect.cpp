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

		m_ui = LoadComponent("PaySelect.xaml");
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
}

void CPaySelect::onExit()
{
	BaseLayer::onExit();	
	if (LayerManager::instance()->getLayer("CActivityLayer"))
	{
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
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
