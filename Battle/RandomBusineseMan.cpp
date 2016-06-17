#include "RandomBusineseMan.h"

RandomBusineseMan::RandomBusineseMan(){
	m_bIsTouchEnabled = false;
	m_fTime = 1.0f;
}

RandomBusineseMan::~RandomBusineseMan(){
}

bool RandomBusineseMan::init(){

	bool res = BaseLayer::init();
	
	setVisible(true);

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//黑底
	CCLayerColor* pMaskLayer = CCLayerColor::create(ccc4(0, 0, 0, 160));
	pMaskLayer->setContentSize(winSize);
	this->addChild(pMaskLayer, 1);

	//配置内容
	m_ui = LoadComponent("randomBusineseMan.xaml");
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui, 2);

	//标题
	CCSprite* pTitleBg = (CCSprite*)m_ui->findWidgetById("title_bg");
	pTitleBg->setVisible(false);
	//文字
	CCSprite* pTitle = (CCSprite*)m_ui->findWidgetById("text");
	pTitle->setVisible(false);
	pTitleBg->runAction(
		CCSequence::create(
		CCDelayTime::create(0.4f),
		CCShow::create(),
		CCEaseBackOut::create(CCMoveTo::create(0.3f, pTitleBg->getPosition()))
		,nullptr)
		);
	pTitle->runAction(
		CCSequence::create(
		CCDelayTime::create(0.4f),
		CCShow::create(),
		CCEaseBackOut::create(CCMoveTo::create(0.3f, pTitleBg->getPosition()))
		,nullptr)
		);
	pTitleBg->setPositionY(pTitleBg->getPositionY()+300);
	pTitle->setPositionY(pTitle->getPositionY()+300);

	//旋转光
	CCSprite *light = (CCSprite*)m_ui->findWidgetById("light");
	light->runAction(CCRepeatForever::create(CCRotateBy::create(4.0f, 360)));
	light->setScale(0.5f);
	light->setOpacity(0);
	light->runAction
		(CCSequence::create(
		CCSpawn::create(CCEaseBackOut::create(CCScaleTo::create(0.45f, 2.1f)), CCFadeIn::create(0.15f), nullptr),
		CCScaleTo::create(0.2f, 1.7f)
		,nullptr)
		);

	//头像
	CCSprite *head = (CCSprite*)m_ui->findWidgetById("head");
	head->setScale(0.5f);
	head->setOpacity(0);
	head->runAction
		(CCSequence::create(
		CCSpawn::create(CCEaseBackOut::create(CCScaleTo::create(0.4f, 1.5f)), CCFadeIn::create(0.1f), nullptr),
		CCScaleTo::create(0.2f, 1.2f),
		CCCallFuncN::create(this, callfuncN_selector(RandomBusineseMan::callBackHead))
		,nullptr)
		);

	//退出用图片
	CImageView *btnView= (CImageView *)m_ui->findWidgetById("btn");
	btnView->setTouchEnabled(true);
	btnView->setOnClickListener(this,ccw_click_selector(RandomBusineseMan::touchHide));

	//触摸屏蔽时间段
	scheduleOnce(schedule_selector(RandomBusineseMan::scheduleCallBack), m_fTime);

	return res;
}

void RandomBusineseMan::touchHide( CCObject* pSender )
{
	if(m_bIsTouchEnabled)
	{
		removeFromParentAndCleanup(true);
	}
}

void RandomBusineseMan::onEnter()
{
	BaseLayer::onEnter();

	setTouchPriority(-14);
}

void RandomBusineseMan::onExit()
{
	BaseLayer::onExit();
}

void RandomBusineseMan::scheduleCallBack( float dt )
{
	m_bIsTouchEnabled = true;
}

void RandomBusineseMan::callBackHead( CCNode* pSender )
{
	CCSprite* pHead = (CCSprite*)pSender;

	pHead->runAction(CCRepeatForever::create(
		CCSequence::create(CCRotateBy::create(0.5f, -5), CCRotateBy::create(0.9f, 10), CCRotateBy::create(0.5f, -5), nullptr)
		));
}
