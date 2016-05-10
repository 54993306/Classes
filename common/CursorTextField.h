#ifndef CursorInputDemo_CursorTextField_h
#define CursorInputDemo_CursorTextField_h

#include "cocos2d.h"
#ifndef _WIN32
#include <android/log.h>
#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif



USING_NS_CC;

class CursorTextField: public CCTextFieldTTF, public CCTextFieldDelegate, public CCTouchDelegate
{
private:
    // 点击开始位置
    CCPoint m_beginPos;
    
    // 光标精灵
    CCSprite *m_pCursorSprite;
    
    // 光标动画
    CCAction *m_pCursorAction;
                 
    // 光标坐标
    CCPoint m_cursorPos;
    
    // 输入框内容
    std::string *m_pInputText;

	//底背景
	CCSprite	*m_pSprBG;
	// 是否加密显示
	bool isPsw;
	//设置字符长度限制，一个汉字三个字符
	int m_limitNum;
	//设置输入框触摸大小
	CCSize m_designedSize;
	int	m_nInputType;
public:
    CursorTextField();
    ~CursorTextField();
    
    // static 
	// placeholder：默认的Text内容
	// fontName：字体
	// fontSize：字大小
	// touchSize：触摸有效区域
	// color：光标以及字体颜色
    static CursorTextField* textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize,CCSize touchSize,ccColor3B color);
    
    // CCLayer
    void onEnter();

	void setPriority(int nPriority);
	void onExit();
    
    // 初始化光标精灵
    void initCursorSprite(int nHeight,CCSize touchSize,ccColor3B color);
    
    // CCTextFieldDelegate
    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF *pSender);
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
	virtual unsigned int countLimitForInsertText();
	virtual int getContentInputType();
//	virtual bool attachWithIME();
    
    // CCLayer Touch
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    // 判断是否点击在TextField处
    bool isInTextField(CCTouch *pTouch);
    // 得到TextField矩形
    CCRect getRect();
   
    // 打开输入法
    void openIME();
    // 关闭输入法
    void closeIME();

	//设置是否星号显示
	bool getIsPsw();
	void setIsPsw(bool bFlag);
	//设置字符长度限制，一个汉字三个字符
	void setLimitNum(int limitNum);
	int getLimitNum();
	//点击弹出输入法的尺寸范围
//	void setDesignedSize(CCSize size);
	CCSize getDesignedSize();

	std::string* getTextInput();
	void setTextInput(std::string str);

	void clearTextInput();
	void setCursorVisible(bool visible);
	void setInputType(int type);
};

#endif
