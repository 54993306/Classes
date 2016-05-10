

#ifndef __client1__MZCursorTextField__
#define __client1__MZCursorTextField__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class MZCursorTextField
: public CCTextFieldTTF
, public CCTextFieldDelegate
, public CCTouchDelegate
{
public:
    MZCursorTextField();
    ~MZCursorTextField();
    
    static MZCursorTextField* textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize);
    
    void onEnter();
    
    void onExit();
    
    CCRect getRect();
    
    void openIME();
    
    void closeIME();
    
    void initCursorSprite(int nHeight);
    
    bool isInTextField(CCTouch *pTouch);
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF *pSender);
    
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
    
public:
    
    static bool imeBool;
	std::string*    _pInputText;
    
private:
    
    std::string*    _pStr;
    

    
    CCPoint         _beginPos;
    
    CCPoint         _cursorPos;
    
    CCSprite*       _pCursorSprite;
    
    CCAction*       _pCursorAction;
    
};


#endif /* defined(__client1__MZCursorTextField__) */
