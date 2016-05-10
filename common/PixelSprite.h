#ifndef CPIXEL_SPRIRTE
#define CPIXEL_SPRIRTE
#include "AppUI.h"
#include "platform/CCImage.h"
class CPixelSprite:public CImageView
{
public:
    static CPixelSprite* create(const char *pszFileName);
	void setImage(const char *pszFileName);
	virtual ~CPixelSprite();
protected:
	CCRect atlasRect();
public:
	virtual CWidgetTouchModel onTouchBegan(CCTouch *pTouch);
	virtual void onTouchMoved(CCTouch *pTouch, float fDuration);
	virtual void onTouchEnded(CCTouch *pTouch, float fDuration);
	virtual void onTouchCancelled(CCTouch *pTouch, float fDuration);

	CC_WIDGET_LONGCLICK_SCHEDULE(CPixelSprite);
private:
	CCImage *m_image;
};
#endif // ! CPIXEL_SPRIRTE
