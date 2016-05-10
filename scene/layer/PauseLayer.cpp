#include "PauseLayer.h"
#include "Global.h"
PauseLayer::PauseLayer()
	:m_ui(nullptr)
{

}
PauseLayer::~PauseLayer()
{
	REMOVE_TARGET(m_ui);
	CC_SAFE_RELEASE(m_ui);
	m_ui = nullptr;
}
bool PauseLayer::init()
{
	bool res = CCNode::init();
	do 
	{
		CC_BREAK_IF(!res);
		CWidgetWindow* layout = CWidgetWindow::create();
		layout->setTag(1);
		addChild(layout);
		m_ui = LoadComponent("PauseLayer.xaml");
		m_ui->retain();
		//m_ui->setPosition(CCSizeCenter(m_ui->getContentSize()));
		m_ui->setPosition(VCENTER);
		m_ui->getChildByTag(1)->setVisible(false);

		this->setContentSize(m_ui->getContentSize());
		this->setAnchorPoint(CCPointCenter);
		layout->addChild(m_ui);
		res = true;
	} while (0);
	return res;
}
void PauseLayer::onEnter()
{
	CCNode::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-10,true);
	//CCDirector::sharedDirector()->pause();
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}
void PauseLayer::onExit()
{
	CCNode::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	//CCDirector::sharedDirector()->resume();
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
bool PauseLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	this->removeFromParentAndCleanup(true);
	return true;
}