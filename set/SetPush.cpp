#include "SetPush.h"
#include "common/CGameSound.h"

CSetPush::CSetPush()
{

}

bool CSetPush::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("SetPush");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("setPush.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;

}


void CSetPush::onExchange(CCObject* pSender)
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSetPush::onOpenOff(CCObject* pSender, bool isCheck)
{
	
}

void CSetPush::onEnter()
{
	BaseLayer::onEnter();

	CButton *close = (CButton*)(m_ui->findWidgetById("close"));
	close->setOnClickListener(this,ccw_click_selector(CSetPush::onExchange));
	
	CToggleView *action = (CToggleView*)(m_ui->findWidgetById("action"));
	action->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));

	CToggleView *reward = (CToggleView*)(m_ui->findWidgetById("reward"));
	reward->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));

	CToggleView *garrsion = (CToggleView*)(m_ui->findWidgetById("garrsion"));
	garrsion->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));

	CToggleView *shop = (CToggleView*)(m_ui->findWidgetById("shop"));
	shop->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));
	CToggleView *fullaction = (CToggleView*)(m_ui->findWidgetById("fullaction"));
	fullaction->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));
	CToggleView *actTip = (CToggleView*)(m_ui->findWidgetById("actTip"));
	actTip->setOnCheckListener(this,ccw_check_selector(CSetPush::onOpenOff));

}

void CSetPush::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSetPush::onExit()
{
	BaseLayer::onExit();
}


CSetPush::~CSetPush()
{

}



