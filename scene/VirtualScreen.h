#ifndef __VIRTUAL_SCREEN_H__
#define __VIRTUAL_SCREEN_H__

#include "cocos2d.h"
USING_NS_CC;

//使用中心点定位，是根据父容器大小的中心点来定位，而不是自己的大小
//CScene与CWidgetWindow会默认跟屏幕大小相同，所以XmlLayer的position要用VCENTER
const float VIRTUAL_FIXED_HEIGHT = 640;
const float DESIGN_WIDTH = 1138;
//虚拟屏幕尺寸,不同尺寸适配
#define VCENTER     VirtualScreen::Instance()->Center()
#define VTOP        VirtualScreen::Instance()->Top()
#define VRIGHT      VirtualScreen::Instance()->Right()
#define VBOTTOM     VirtualScreen::Instance()->Bottom()
#define VLEFT       VirtualScreen::Instance()->Left()
#define VSCALE      VirtualScreen::Instance()->Scale()
#define VXY(v)      VirtualScreen::Instance()->XY(v)
#define OFFSETX     (DESIGN_WIDTH-VirtualScreen::Instance()->VirtualSize().width)/2
class VirtualScreen
{
private:
	static VirtualScreen* instance;
	VirtualScreen() : scale(1.0f), top(0), right(0), bottom(0), left(0){}
	bool init();
	CCSize virtualSize;
	CCPoint center;
	float top;
	float right;
	float bottom;
	float left;
	float scale;
public:
	static VirtualScreen* Instance();
	void SetVirtualScreenWidth(float width);
	void SetVirtualScreenHeight(float height);
	CCSize VirtualSize();
	CCPoint Center();
	float Top();
	float Right();
	float Bottom();
	float Left();
	float Scale();
	float XY(float v);
	void release();
};

#endif  // __VIRTUAL_SCREEN_H__
