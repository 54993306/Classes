

#include "Tools/MZCursorTextField.h"

const static float DELTA = 1000;
bool MZCursorTextField::imeBool = false;

MZCursorTextField::MZCursorTextField(){
    CCTextFieldTTF();
    
    _pCursorAction = NULL;
    _pCursorSprite = NULL;
    
    _pInputText = NULL;
    _pStr = NULL;
}

MZCursorTextField::~MZCursorTextField(){
    delete _pInputText;
    delete _pStr;
}

void MZCursorTextField::onEnter(){
    CCTextFieldTTF::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
    this->setDelegate(this);
}

MZCursorTextField * MZCursorTextField::textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize)
{
    MZCursorTextField *pRet = new MZCursorTextField();
    
    if(pRet && pRet->initWithString("", fontName, fontSize))
    {
        pRet->autorelease();
        if (placeholder)
        {
            pRet->setPlaceHolder(placeholder);
        }
        pRet->initCursorSprite(fontSize);
        
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    
    return NULL;
}

void MZCursorTextField::initCursorSprite(int nHeight)
{
#ifdef CC_PLATFORM_WIN32
	//动态定义二维数组
	int **pixels ;
	unsigned int row = nHeight;
	unsigned int column = 4;

	pixels = new int*[row] ;
	for(unsigned int i = 0 ; i < row ; i ++ )
		pixels[i] = new int[column] ;
	//输入二维数组的值
	for(unsigned int i = 0 ; i < row ; i ++ )
		for(unsigned int j = 0 ; j < column ; j ++ )
			pixels[i][j] = 0xffffffff;
#if 0
	//输出二维数组
	cout<<"输出二位数组："<<endl;
	for(unsigned int i = 0 ; i < row ; i ++ )
	{
		for(unsigned int j = 0 ;j < column ; j ++ )
			cout<<pixels[i][j]<<' ' ;
		cout<<endl;
	}
#endif

#else
	int column = 4;
	int pixels[nHeight][column];
	for (int i=0; i < nHeight; ++i) {
		for (int j=0; j < column; ++j) {
			pixels[i][j] = 0xffffffff;
		}
	}
#endif  

    CCTexture2D *texture = new CCTexture2D();
	texture->autorelease();
    texture->initWithData(pixels, kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(column, nHeight));
    
    _pCursorSprite = CCSprite::createWithTexture(texture);
    CCSize winSize = getContentSize();
    _cursorPos = ccp(0, winSize.height / 2 + 2);
    _pCursorSprite->setPosition(_cursorPos);
    this->addChild(_pCursorSprite);
    
    _pCursorAction = CCRepeatForever::create((CCActionInterval *) CCSequence::create(CCFadeOut::create(0.25f), CCFadeIn::create(0.25f), NULL));
    
    _pCursorSprite->runAction(_pCursorAction);
    
    _pInputText = new std::string();

#ifdef CC_PLATFORM_WIN32
	//释放二维数组
	for(unsigned int i = 0 ; i < row ; i ++ )
		delete []pixels[i] ;
	delete []pixels ;
#endif
}

bool MZCursorTextField::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    _beginPos = pTouch->getLocationInView();
    _beginPos = CCDirector::sharedDirector()->convertToGL(_beginPos);
    
    return true;
}

CCRect MZCursorTextField::getRect(){
    CCSize size = getContentSize();
    return CCRectMake(-size.width / 2, -size.height / 2, size.width, size.height);
}

bool MZCursorTextField::isInTextField(cocos2d::CCTouch *pTouch)
{
    return getRect().containsPoint(convertTouchToNodeSpaceAR(pTouch));
}

void MZCursorTextField::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint endPos = pTouch->getLocationInView();
    endPos = CCDirector::sharedDirector()->convertToGL(endPos);

	CCSize size = CCDirector::sharedDirector()->getWinSize();

    CCRect rect = CCRectMake(size.width*0.35, size.height* 0.25, size.width*0.75, size.height* 0.35);
    CCPoint point = pTouch->getLocationInView();
    point = CCDirector::sharedDirector()->convertToGL(point);
    //删掉的
	/*
    if (!rect.containsPoint(point)) {
        _beginPos.x = _beginPos.y = -1;
        return;
    }
    */
    CCLOG("width: %f, height: %f.", getContentSize().width, getContentSize().height);
    
    isInTextField(pTouch) ? openIME() : closeIME();
}

bool MZCursorTextField::onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    if (_pInputText->empty()) {
        return false;
    }
    
    _pCursorSprite->setPositionX(getContentSize().width);
    
    return false;
}

bool MZCursorTextField::onTextFieldInsertText(cocos2d::CCTextFieldTTF *pSender, const char *text, int nLen)
{
    if ('\n' == *text)
    {
        return false;
    }
    
    if (_pInputText->length() <= 14) {
        _pInputText->append(text);
        setString(_pInputText->c_str());
        
        _pCursorSprite->setPositionX(getContentSize().width);
    }
    
    
    return true;
}

bool MZCursorTextField::onTextFieldDeleteBackward(cocos2d::CCTextFieldTTF *pSender, const char *delText, int nLen)
{
    _pInputText->resize(_pInputText->size() - nLen);
    setString(_pInputText->c_str());
    
    _pCursorSprite->setPositionX(getContentSize().width);
    
    if (_pInputText->empty()) {
        _pCursorSprite->setPositionX(0);
    }
    
    return false;
}

bool MZCursorTextField::onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    CCNotificationCenter::sharedNotificationCenter()->postNotification("ScreenAction", (CCObject* ) 1);
    
    if (_pInputText->empty()) {
        CCUserDefault::sharedUserDefault()->setStringForKey("NAME", "");
    }else{
        CCUserDefault::sharedUserDefault()->setStringForKey("NAME", _pInputText->c_str());
    }
    
    return false;
}

void MZCursorTextField::openIME()
{
    CCNotificationCenter::sharedNotificationCenter()->postNotification("ScreenAction", (CCObject* ) 0);
    _pCursorSprite->setVisible(true);
    this->attachWithIME();
}

void MZCursorTextField::closeIME()
{
    imeBool = true;
    _pCursorSprite->setVisible(false);
    this->detachWithIME();
}

void MZCursorTextField::onExit()
{
    
    CCTextFieldTTF::onExit();
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    
}

