#include "sweep.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"

bool CSweepLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("MaskLayerRep");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("SweepIntro.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;

}

CSweepLayer::CSweepLayer():m_posIndex(0),m_number(0)
{
}

void CSweepLayer::onBeginSweep(CCObject* pSender)
{

}

void CSweepLayer::onEnter()
{

	BaseLayer::onEnter();

	

	CButton *btn = (CButton*)m_ui->findWidgetById("sweep");
	btn->setOnClickListener(this,ccw_click_selector(CSweepLayer::onBeginSweep));
	
	CButton *sub = (CButton*)m_ui->findWidgetById("sub");
	sub->setOnClickListener(this,ccw_click_selector(CSweepLayer::onSub));

	CButton *add = (CButton*)m_ui->findWidgetById("add");
	add->setOnClickListener(this,ccw_click_selector(CSweepLayer::onAdd));

	m_numTextField = CursorTextField::textFieldWithPlaceHolder("5", "arial", 18,CCSize(200,35),ccWHITE);
	m_numTextField->setPosition(ccpAdd(sub->getPosition(),ccp(50,0)));
	m_ui->addChild(m_numTextField);
	
	m_number = 5;
}

void CSweepLayer::onExit()
{
	BaseLayer::onExit();
}

void CSweepLayer::setPosIndex(int index)
{
	m_posIndex = index;
}

void CSweepLayer::onSub(CCObject* pSender)
{
	m_number--;
	updateSweepNum();
}

void CSweepLayer::onAdd(CCObject* pSender)
{
	m_number++;
	updateSweepNum();
}

void CSweepLayer::updateSweepNum()
{
	m_numTextField->setString(CCString::createWithFormat("%d",m_number)->getCString());
	CLabel *num = (CLabel*)m_ui->findWidgetById("num");
	num->setString(CCString::createWithFormat("%d",m_number)->getCString());
}

