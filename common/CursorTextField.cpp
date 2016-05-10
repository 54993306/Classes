#include "CursorTextField.h"

const static float DELTA = 5.0f;

CursorTextField::CursorTextField()
{
    CCTextFieldTTF();
    
    m_pCursorSprite = NULL;
    m_pCursorAction = NULL;
    
    m_pInputText = NULL;
	m_pSprBG=NULL;
	
	isPsw = false;
	m_designedSize = CCSize();
	m_limitNum = -1;
	m_nInputType=1;
}

CursorTextField::~CursorTextField()
{
    delete m_pInputText;
}

void CursorTextField::onEnter()
{
    CCTextFieldTTF::onEnter();
}

CursorTextField * CursorTextField::textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize,CCSize touchSize,ccColor3B color)
{
    CursorTextField *pRet = new CursorTextField();
    
    if(pRet && pRet->initWithString("", fontName, fontSize))
    {
        pRet->autorelease();
		pRet->setColor(color);
        if (placeholder)
        {
            pRet->setPlaceHolder(placeholder);
        }
        pRet->initCursorSprite(touchSize.height,touchSize,color);
        
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    
    return NULL;
}

void CursorTextField::initCursorSprite(int nHeight,CCSize touchSize,ccColor3B color)
{
/*	
	m_pSprBG = CCSprite::create("register_input.png");
	m_pSprBG->setAnchorPoint(CCPointZero);
	m_pSprBG->setPosition(ccp(0,0));
	this->addChild(m_pSprBG,0);
 */  
    // 初始化光标
	int column = 4;
	int *pixels=new int[nHeight*column];
	memset(pixels,0xff,nHeight*column*sizeof(int));


	CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(pixels, kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(column, nHeight));
	texture->autorelease();
	delete pixels;
    m_designedSize = touchSize;
	m_pCursorSprite=CCSprite::createWithTexture(texture);
//	CCSize winSize = getContentSize();
	CCSize winSize = m_designedSize;
    m_cursorPos = ccp(0, winSize.height / 2-3);
    m_pCursorSprite->setPosition(m_cursorPos);
	m_pCursorSprite->setColor(color);
    this->addChild(m_pCursorSprite);
    
    m_pCursorAction = CCRepeatForever::create((CCActionInterval *) CCSequence::create(CCFadeOut::create(0.25f), CCFadeIn::create(0.25f), NULL));
    
    m_pCursorSprite->runAction(m_pCursorAction);
	m_pCursorSprite->setVisible(false);
    
    m_pInputText = new std::string();
}
#define TOUCH_EXTENT_PX 10
bool CursorTextField::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if(!isVisible())
		return false;

	m_beginPos = pTouch->getLocation();

	CCRect rt;
	rt.origin=convertToWorldSpace(ccp(0,0));
	rt.size=CCSizeMake(m_designedSize.width,m_designedSize.height);
	rt.origin.x	-=TOUCH_EXTENT_PX;
	rt.origin.y	-=(TOUCH_EXTENT_PX+getContentSize().height/2);
	rt.size.width +=TOUCH_EXTENT_PX*2;
	rt.size.height+=TOUCH_EXTENT_PX*2;

	if(rt.containsPoint(m_beginPos))
	{
		openIME();
		return true;
	}
	closeIME();
    return false;
}

CCRect CursorTextField::getRect()
{
    CCSize size;
	if ((m_designedSize.width != 0) && (m_designedSize.height != 0))
	{
		size = m_designedSize;
	} 
	else
	{
		size = getContentSize();
	}

    //return  CCRectMake(0, 0, size.width, size.height);
	return  CCRectMake(-TOUCH_EXTENT_PX, -(TOUCH_EXTENT_PX+getContentSize().height/2), size.width+TOUCH_EXTENT_PX*2, size.height+TOUCH_EXTENT_PX*2);
}

bool CursorTextField::isInTextField(cocos2d::CCTouch *pTouch)
{
	return getRect().containsPoint(convertTouchToNodeSpaceAR(pTouch));
	//return CCRect::CCRectContainsPoint(getRect(), convertTouchToNodeSpaceAR(pTouch));
}

void CursorTextField::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	return;
// 	CCPoint endPos = pTouch->getLocationInView();
// 	
//     endPos = CCDirector::sharedDirector()->convertToGL(endPos);
//     
//     // 判断是否为点击事件
//      if (::abs(endPos.x - m_beginPos.x) > DELTA || 
//          ::abs(endPos.y - m_beginPos.y) > DELTA) 
//      {
//          // 不是点击事件
//          m_beginPos.x = m_beginPos.y = -1;
//          
//          return;
//      }
//     
//     // 判断是打开输入法还是关闭输入法
//     if(isInTextField(pTouch))
// 		openIME();
//  	else
//  		closeIME();
}

bool CursorTextField::onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    if (m_pInputText->empty()) 
	{
		m_pCursorSprite->setPositionX(0);
       return false;
    }    
    m_pCursorSprite->setPositionX(getContentSize().width);
// 	m_pCursorSprite->setPositionX(m_designedSize.width);
    
    return false;
}
//
bool CursorTextField::onTextFieldInsertText(cocos2d::CCTextFieldTTF *pSender, const char *text, int nLen)
{
    CCLOG("Width: %f", pSender->getContentSize().width);
    CCLOG("Text: %s", text);
    CCLOG("Length: %d", nLen);
	if ('\n' == *text)
	{
		return false;
	}
	if (m_pInputText->length() > (unsigned int)m_limitNum) 
	{
#ifndef _WIN32
		LOGD("%d",m_pInputText->length());
		LOGD("%d",m_limitNum);
#endif
		return true;
	}
	else
	{
		std::string tempStr = m_pInputText->substr();
		tempStr.append(text);
	}
	m_pInputText->append(text);
#ifndef _WIN32
	LOGD(text);
#endif
	if (isPsw) 
	{
		std::string tempStr;
		for (unsigned int i = 0; i < m_pInputText->size(); i++) 
		{
			tempStr.append("*");
		}
		setString(tempStr.c_str());
	}
	else 
	{
		setString(m_pInputText->c_str());
	}    
   
    
    m_pCursorSprite->setPositionX(getContentSize().width);
//	m_pCursorSprite->setPositionX(m_designedSize.width);
    
    return true;
}

bool CursorTextField::onTextFieldDeleteBackward(cocos2d::CCTextFieldTTF *pSender, const char *delText, int nLen)
{
//    m_pInputText->resize(m_pInputText->size() - nLen);
	if (m_pInputText->length()>0)
	{
		m_pInputText->resize(m_pInputText->length() - nLen);
		setString(m_pInputText->c_str());
	}
    
	if (isPsw) 
	{
		std::string tempStr;
		for (unsigned int i = 0; i < m_pInputText->size(); i++) 
		{
			tempStr.append("*");
		}
		setString(tempStr.c_str());
	}
	else 
	{
		setString(m_pInputText->c_str());
	}
    m_pCursorSprite->setPositionX(getContentSize().width);
//    m_pCursorSprite->setPositionX(m_designedSize.width);
    
    if (m_pInputText->empty()) 
	{
#ifndef _WIN32
		LOGD("not empty");
#endif
        m_pCursorSprite->setPositionX(0);
    }
	else
	{
#ifndef _WIN32
		LOGD("accept empty");
#endif
	}
    
    return false;
}

bool CursorTextField::onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    return false;
}
//----------------------------------------------------------
void CursorTextField::setCursorVisible(bool visible)
{
	m_pCursorSprite->setVisible(visible);
	if (m_pInputText->empty()) 
	{
		m_pCursorSprite->setPositionX(0);
	}    
}
//----------------------------------------------------------

void CursorTextField::openIME()
{
	CCLog("openIME()");
	m_pCursorSprite->setVisible(false);
    this->attachWithIME();
	this->m_pCursorSprite->setVisible(false);
}

void CursorTextField::closeIME()
{
	CCLog("closeIME()");
	m_pCursorSprite->setVisible(false);
    this->detachWithIME();
	this->m_pCursorSprite->setVisible(false);
}

void CursorTextField::onExit()
{
	closeIME();
    CCTextFieldTTF::onExit();
 //   CCTouchDispatcher::sharedDispatcher()->removeDelegate(this);
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}
bool CursorTextField::getIsPsw()
{
	return isPsw;
}
//设置星号显示否
void CursorTextField::setIsPsw( bool bFlag)
{
	isPsw = bFlag;
}
//设置字符长度
int CursorTextField::getLimitNum()
{
	return m_limitNum;
}
void CursorTextField::setLimitNum(int limitNum)
{
	m_limitNum = limitNum;
}
unsigned int CursorTextField::countLimitForInsertText()
{
	return m_limitNum;
}
//设置触摸弹出输入法的区域大小
//void CursorTextField::setDesignedSize(cocos2d::CCSize size)
//{
//	m_designedSize = size;
//}

CCSize CursorTextField::getDesignedSize()
{
	return m_designedSize;
}

std::string *CursorTextField::getTextInput()
{
	return m_pInputText;
}

void CursorTextField::setTextInput(std::string str)
{
	m_pInputText->clear();
	m_pInputText->append(str);
	setString(m_pInputText->c_str());
}

void CursorTextField::clearTextInput()
{
	m_pInputText->clear();
	setString("");
}

int CursorTextField::getContentInputType()
{
	return m_nInputType;
}

void CursorTextField::setInputType(int type)
{
	m_nInputType=type;
}

void CursorTextField::setPriority(int nPriority)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,nPriority,true);
	this->setDelegate(this);
}
