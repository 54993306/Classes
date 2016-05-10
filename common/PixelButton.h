#ifndef CPIXEL_BUTTON
#define CPIXEL_BUTTON
// 
/*#include "cocos2d.h"*/
#include "platform/CCImage.h"
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "CCTK/cocos-widget.h"
USING_NS_CC_WIDGET;


class CPixelButton:public CButton
{
public:
	static CPixelButton* create(const char* pNormal, const char* pSelected /*= NULL*/, const char* pDisabled /*= NULL*/);
	void setImage(const char *pszFileName);
	CREATE_FUNC(CPixelButton);
	virtual ~CPixelButton();
protected:
	CCRect atlasRect();
public:
	virtual CWidgetTouchModel onTouchBegan(CCTouch *pTouch);
	

	CC_WIDGET_LONGCLICK_SCHEDULE(CPixelButton);
private:
	CCImage *m_image;
};
#endif // ! CPIXEL_SPRIRTE
