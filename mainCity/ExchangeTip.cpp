#include "ExchangeTip.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

bool CExchangeTip::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayerExchangeTip");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("ExchangeTip.xaml");  //  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;

}

CExchangeTip::CExchangeTip()
{

}

void CExchangeTip::setBtnListener(CCObject *pListener, SEL_ClickHandler pHandler)
{
	CButton *ok = (CButton *)m_ui->findWidgetById("ok");
	ok->setOnClickListener(pListener,pHandler);
	CButton *cancel = (CButton *)m_ui->findWidgetById("cancel");
	cancel->setOnClickListener(pListener,pHandler);
}

void CExchangeTip::setData(int gold, int coin, int time/*=5*/)
{
	CLabel *goldLab = (CLabel *)m_ui->findWidgetById("gold");
	goldLab->setString(ToString(gold*time));
	CLabel *coinLab = (CLabel *)m_ui->findWidgetById("coin");
	coinLab->setString(ToString(coin*time));
	CLabel *timeLab = (CLabel *)m_ui->findWidgetById("time");
	timeLab->setString(CCString::createWithFormat(GETLANGSTR(1108), time)->getCString());
}


