#include "GameTip.h"

GameTip::GameTip()
{

}

GameTip::~GameTip()
{

}
bool GameTip::init()
{
	if (BaseLayer::init())
	{
		CWidgetWindow* layout = CWidgetWindow::create();
		layout->setTag(1);
		addChild(layout);
		m_ui = LoadComponent("GameTip.xaml");
		layout->addChild(m_ui);

		CButton *btn2 = (CButton *)m_ui->getChildByTag(2);
		btn2->setOnClickListener(this,ccw_click_selector(GameTip::goBack));
	}
	return true;
}

void GameTip::onCreate()
{

}


void GameTip::goBack(CCObject *obj)
{
	this->removeFromParentAndCleanup(true);
}

void GameTip::showTip(const char *str, CCPoint point,CCSize contSize)
{
	/*
	CCSprite *spr = CCSprite::create("loginScene/tipBox.png");
	spr->setPosition(VCENTER);
	m_ui->addChild(spr);
	*/
	m_scroll = CScrollView::create(contSize);
		
	m_scroll->setContainerSize(contSize);
		
	m_scroll->setDirection(eScrollViewDirectionVertical);
		
	m_scroll->setPosition(ccp(point.x+10, point.y+30));
		
	m_scroll->setColor(ccWHITE);
		
	m_ui->addChild(m_scroll);

	CCLabelTTF *label = CCLabelTTF::create(str,
		"Arial",15,CCSizeMake(400,0),kCCTextAlignmentLeft);
	label->setAnchorPoint(ccp(0,1));
	label->setPosition(ccp(0,m_scroll->getContentSize().height));
	m_scroll->getContainer()->addChild(label);
	//m_scroll->setContentOffset(ccp(0,0));
	//m_scroll->setContentOffsetToBottom();
}
