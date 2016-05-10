#include "HeroAdvance.h"
#include "common/CGameSound.h"
#include "Resources.h"

HeroAdvance::HeroAdvance()
{
}

bool HeroAdvance::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("HeroAdvanceMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("advance.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}

void HeroAdvance::onEnter()
{
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(HeroAdvance::onClose));
	this->addChild(pClose, 999);
}

void HeroAdvance::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}


void HeroAdvance::onExit()
{
	BaseLayer::onExit();

}

void HeroAdvance::showEffect()
{
	//动作
	//默认显示石像
	CImageView* pStone1 = (CImageView*)m_ui->findWidgetById("stone_1");
	CImageView* pStone2 = (CImageView*)m_ui->findWidgetById("stone_2");

	//石像出现
	pStone1->setPositionX(pStone1->getPositionX()-300);
	pStone2->setPositionX(pStone2->getPositionX()+300);

	pStone1->runAction(CCMoveBy::create(0.2f, ccp(300, 0)));
	pStone2->runAction(CCMoveBy::create(0.2f, ccp(-300, 0)));

	//


	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.2f), CCCallFunc::create(this, callfunc_selector(HeroAdvance::showEffectCallBack))));
}

void HeroAdvance::showEffectCallBack()
{
	setTouchEnabled(true);

	//m_pBody->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.6f, 1.05f), CCScaleTo::create(0.6f, 1.0f))));
}