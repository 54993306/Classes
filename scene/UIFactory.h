#ifndef __UIFACTORY_H__
#define __UIFACTORY_H__

//#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"
/*#include "CCTK/cocos-widget.h"*/
#include "AppUI.h"
#include "UITools.h"
USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_CC_WIDGET;

void SetButtonDefaultBackground( const char* source );
void SetButtonClickBackground( const char* source );
void SetBMFontDefault( const char* font );

CLayout* LoadComponent( const char* xml );
void SetButtonClickEvent( CCNode* designLayer, int tag, CCObject* pTarget, SEL_ClickHandler pHandler );

#define CREATE_FUNC_COLOR(__TYPE__) \
static __TYPE__* create() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->initWithColor(ccc4(0,0,0,0))) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

void LogTime(char* msg);

class SndButton : public CButton
{
public:
// 	virtual CWidgetTouchModel onTouchBegan(CCTouch *pTouch);
	SndButton():m_bPlaySound(true){}
	static SndButton* create(const char* pNormal, const char* pSelected = NULL, const char* pDisabled = NULL);
	virtual CWidgetTouchModel onTouchBegan(CCTouch* pTouch);
	void onTouchEnded(CCTouch* pTouch, float fDuration);
	CREATE_FUNC(SndButton);
	void setPlaySound(bool isPlay);
private:
	bool m_bPlaySound;
};

#endif  // __UIFACTORY_H__
