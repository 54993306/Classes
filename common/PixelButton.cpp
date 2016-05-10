#include "PixelButton.h"

CPixelButton* CPixelButton::create(const char* pNormal, const char* pSelected /*= NULL*/, const char* pDisabled /*= NULL*/)
{
	CPixelButton* pRet = new CPixelButton();
	if( pRet && pRet->initWithFile(pNormal, pSelected, pDisabled) )
	{
		pRet->autorelease();
		pRet->setImage(pNormal);
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

void CPixelButton::setImage(const char *pszFileName)
{
	m_image= new CCImage();
	m_image->initWithImageFileThreadSafe(CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFileName).c_str());
	this->setContentSize(CCSizeMake(m_image->getWidth(),m_image->getHeight()));
}

CCRect CPixelButton::atlasRect()
{
	CCSize cSize = this->getContentSize();
	CCPoint point = this->getAnchorPointInPoints();
	return CCRectMake( -point.x, -point.y, cSize.width,cSize.height);
}


CWidgetTouchModel CPixelButton::onTouchBegan(CCTouch *pTouch)
{
	CC_WIDGET_LONGCLICK_ONTOUCHBEGAN;

// 	CCPoint locPoint = convertToNodeSpace(pTouch->getLocation());

	if (this->atlasRect().containsPoint(convertTouchToNodeSpaceAR(pTouch))) 
	{

		ccColor4B c = {0, 0, 0, 0};

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		CCPoint touchPoint = pTouch->getLocationInView();

		CCSize cSize = this->getContentSize();
		CCPoint point =this->getAnchorPointInPoints();
		point = ccp(cSize.width - point.x,cSize.height- point.y);
		CCPoint pos(this->getPositionX() - point.x,winSize.height-this->getPositionY()- point.y);

		CCPoint localPoint = ccp(touchPoint.x - pos.x,
			touchPoint.y -pos.y);

		float scaleFactor = CCDirector::sharedDirector()->getContentScaleFactor();
		unsigned int x = localPoint.x  * scaleFactor, y = localPoint.y * scaleFactor;

		float _width = this->getContentSize().width*scaleFactor;

		unsigned char *data_ = m_image->getData();


		unsigned int *pixel = (unsigned int *)data_;
		pixel = pixel + (y * (int)_width)* 1 + x * 1;

		c.r = *pixel & 0xff;
		c.g = (*pixel >> 8) & 0xff;
		c.b = (*pixel >> 16) & 0xff;
		c.a = (*pixel >> 24) & 0xff;

		if (c.a == 0) {
			CCLog("ccTouchBegan");
			return eWidgetTouchNone;
		}else
		{
			return eWidgetTouchSustained;
		}

	}
	return eWidgetTouchNone;
}

CPixelButton::~CPixelButton()
{
	CC_SAFE_DELETE(m_image);
}
