#include "VirtualScreen.h"
#include "Global.h"

//虚拟屏幕尺寸,不同尺寸适配
VirtualScreen* VirtualScreen::instance = NULL;
VirtualScreen* VirtualScreen::Instance()
{
	if ( instance == NULL )
	{
		instance = new VirtualScreen();
		instance->init();
	}
	return instance;
}
void VirtualScreen::release()
{
	delete this;
}
bool VirtualScreen::init()
{
	//this->SetVirtualScreenWidth(960.0f);	//设虚拟宽960
	this->SetVirtualScreenHeight(VIRTUAL_FIXED_HEIGHT);
	return true;
}
void VirtualScreen::SetVirtualScreenWidth(float width)
{
	CCSize deviceSize = CCDirector::sharedDirector()->getWinSizeInPixels();	//假设设备是横向
	this->scale = deviceSize.width / width;
	this->virtualSize.width = width;
	this->virtualSize.height = deviceSize.height / this->scale;
	CCEGLView::sharedOpenGLView()->setDesignResolutionSize(this->virtualSize.width, this->virtualSize.height, kResolutionNoBorder);	//原理，把宽对齐，把多余的上下去掉，越方留边越多(mini)
	CCLOG("Virtual width: %0.2f, Virtual height: %0.2f, Scale: %0.2f", this->virtualSize.width, this->virtualSize.height, this->scale);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	this->center.x = winSize.width / 2;
	this->center.y = winSize.height / 2;
	this->left = 0;
	this->right = this->virtualSize.width - 1;
	this->top = this->virtualSize.height - 1;
	this->bottom = 0;
}
void VirtualScreen::SetVirtualScreenHeight(float height)						//假设设备是横向
{
	CCSize deviceSize = CCDirector::sharedDirector()->getWinSizeInPixels();		//得到屏幕尺寸
	this->scale = deviceSize.height / height;									//得到实际高和设计高度比率
	this->virtualSize.height = height;											//设定虚拟屏幕高为实际屏幕高度
	this->virtualSize.width = deviceSize.width / this->scale;					//
	CCEGLView::sharedOpenGLView()->setDesignResolutionSize(this->virtualSize.width, this->virtualSize.height, kResolutionFixedHeight);	//原理，把宽对齐，把多余的上下去掉，越方留边越多(mini)
	CCLOG("Virtual width: %0.2f, Virtual height: %0.2f, Scale: %0.2f", this->virtualSize.width, this->virtualSize.height, this->scale);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	this->center.x = winSize.width / 2;
	this->center.y = winSize.height / 2;
	this->left = 0;
	this->right = this->virtualSize.width - 1;
	this->top = this->virtualSize.height - 1;
	this->bottom = 0;
}
CCSize VirtualScreen::VirtualSize()	{ return this->virtualSize; }
float VirtualScreen::Scale()		{ return this->scale;	}
CCPoint VirtualScreen::Center()		{ return this->center;	}
float VirtualScreen::Top()			{ return this->top;		}
float VirtualScreen::Right()		{ return this->right;	}
float VirtualScreen::Bottom()		{ return this->bottom;	}
float VirtualScreen::Left()			{ return this->left;	}
float VirtualScreen::XY(float v)	{ return this->scale * v; }