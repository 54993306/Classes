/******************************************************************************
文件名: popupbox.cpp
编写者: csdn_viva@foxmail.com ccbase1.0
编写日期: 2013-5-2
功能描述: 弹出框实现
******************************************************************************/
#include "base.h"
#include "popupbox.h"

///构造函数，初始化默认值
CPopupBox::CPopupBox():
m_bIsCached(true)
{
	
}

///析构函数
CPopupBox::~CPopupBox()
{
	CCLOG("## [DEBUG] Release PopupBox [%s]", m_strClassName.c_str());
}

///当进入显示时调用
void CPopupBox::onEnter()
{
	CCLayerColor::setTouchEnabled(true);
	CCLayerColor::onEnter();
}

///当退出隐藏时调用
void CPopupBox::onExit()
{
	CCLayerColor::setTouchEnabled(false);
	CCLayerColor::onExit();
}

///重写事件分发函数
void CPopupBox::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), true);
}

///关闭本对话框
void CPopupBox::closeSelf()
{
	CSceneManager::sharedSceneManager()->closePopupBox(this->m_strClassName.c_str());
}
