#include "SetPanel.h"
#include "net/CNetClient.h"
#include "tools/ShowTexttip.h"
#include "common/color.h"
#include "GMessage.h"
SetPanel::SetPanel()
	:m_panel(nullptr)
{

}
SetPanel::~SetPanel()
{
	if(m_panel && m_panel->getParent())
	{
		m_panel->getParent()->removeChild(m_panel);
	}
	CC_SAFE_RELEASE(m_panel);
	m_panel = nullptr;
}
bool SetPanel::init()
{
	bool res = CCNode::init();
	do 
	{
		if(!res)break;
		CWidgetWindow* layout = CWidgetWindow::create();
		layout->setTag(1);
		layout->setTouchPriority(-4);
		addChild(layout);
		m_panel = LoadComponent("SetPanel.xaml");
		
		if(!m_panel) break;
		m_panel->retain();
		m_panel->setPosition(CCSizeCenter(m_panel->getContentSize()));
		this->setContentSize(m_panel->getContentSize());
		this->setAnchorPoint(CCPointCenter);
		layout->addChild(m_panel);
		for(int i = 1; i <=5; ++i)
		{
			CButton* button = (CButton*)m_panel->getChildByTag(i);
			button->setOnClickListener(this,ccw_click_selector(SetPanel::onClick));
		}
		res = true;
	} while (0);
	return res;
}
void SetPanel::onClick(CCObject* ob)
{
	CButton* btn = (CButton*)ob;
	int tag = btn->getTag();
	switch(tag)
	{
	case 1:
		{
			if(this->getParent())
				this->getParent()->removeChild(this);
		}break;
	case 2:
		{
			CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
			CCDirector::sharedDirector()->replaceScene(GETSCENE(LoginScene));
		}break;
	case 3:
		{
			CNetClient::getShareInstance()->sendDataType(RoleExitMsg);
			CCDirector::sharedDirector()->end();
		}break;
	case 4:
		{
			CCMessageBox(GETLANGSTR(1012), GETLANGSTR(1005));
			//ShowTexttip(U8("此功能尚未开放，敬请期待"),RGB_RED);
		}break;
	case 5:
		{
			CCMessageBox(GETLANGSTR(1012), GETLANGSTR(1005));
			//ShowTexttip(U8("此功能尚未开放，敬请期待"),RGB_RED);
		}break;
	default:
		break;
	}
}
bool SetPanel::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void SetPanel::onEnter()
{
	CCNode::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-3,true);
}
void SetPanel::onExit()
{
	CCNode::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCDirector::sharedDirector()->resume();
}