#include "WaitLayer.h"
#include "common/color.h"
#include "Battle/EffectObject.h"
using namespace BattleSpace;
bool WaitLayer::init()
{
	if (CPopupBox::init())
	{
// 		CColorView *clorView = CColorView::create(ccc4(0,0,0,200));
// 		clorView->setContentSize(CCSizeMake(CCDirector::sharedDirector()->getWinSize().width,250));
// 		clorView->setPosition(VCENTER);
// 		this->addChild(clorView);
		m_ui = LoadComponent("loadWait.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

	    m_loadEffect = EffectObject::create("loading",sPlayType::eRepeat);
		m_loadEffect->setPositionX(VCENTER.x);
		m_loadEffect->setPositionY(VCENTER.y+50.0f);
		m_loadEffect->setEffAnchorPoint(0.5f,0.5f);
		m_loadEffect->setScale(0.6f);
		this->addChild(m_loadEffect);

		for (int i = 0; i < 4; i++)
		{
			CCNode *point = m_ui->getChildByTag(i+1);
			point->setVisible(false);
		}
	}
	return true;
}
void WaitLayer::onOpen(const PARAM param)
{
	char *str = (char*)param;
	/*
	CCLabelTTF *label = CCLabelTTF::create(str,"Arial",20);
	this->addChild(label);
	label->setPosition(ccp(VCENTER.x, VCENTER.y-30));
	label->setColor(RGB_GOLD);*/

	//m_loadEffect->play(1);
	for (int i = 0; i < 4; i++)
	{
		CCNode *point = m_ui->getChildByTag(i+1);
		point->setVisible(false);
	}


	for (int i = 0; i < 4; i++)
	{
		CCNode *point = m_ui->getChildByTag(i+1);
		CCDelayTime *delay = CCDelayTime::create(0.4*i);
		CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(WaitLayer::showNode));
	
		if (i==3)
		{
			CCDelayTime *delay1 = CCDelayTime::create(0.4f);
			CCCallFuncN *call1 = CCCallFuncN::create(this, callfuncN_selector(WaitLayer::hideNode));
			point->runAction(CCRepeatForever::create(CCSequence::create(delay,call, delay1, call1, nullptr)));
		}
		else
		{
			point->runAction(CCRepeatForever::create(CCSequence::create(delay,call, nullptr)));
		}

	}

	m_ui->setVisible(false);
	m_loadEffect->setVisible(false);
	this->setVisible(true);
	this->scheduleOnce(schedule_selector(WaitLayer::showSelf),1.5f);	
	
}

void WaitLayer::showSelf(float delt)
{
	m_ui->setVisible(true);
	m_loadEffect->setVisible(true);
	this->unschedule(schedule_selector(WaitLayer::showSelf));
}

void WaitLayer::showNode(CCNode *pSender)
{
	pSender->setVisible(true);
}

void WaitLayer::hideNode(CCNode *pSender)
{
	for (int i = 0; i < 4; i++)
	{
		CCNode *point = m_ui->getChildByTag(i+1);
		point->setVisible(false);
	}
}

bool WaitLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	 return true; 
}

void WaitLayer::onEnter()
{
	setTouchEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
	CCNode::onEnter();
}

