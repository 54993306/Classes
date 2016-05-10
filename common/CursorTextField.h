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
    // �����ʼλ��
    CCPoint m_beginPos;
    
    // ��꾫��
    CCSprite *m_pCursorSprite;
    
    // ��궯��
    CCAction *m_pCursorAction;
                 
    // �������
    CCPoint m_cursorPos;
    
    // ���������
    std::string *m_pInputText;

	//�ױ���
	CCSprite	*m_pSprBG;
	// �Ƿ������ʾ
	bool isPsw;
	//�����ַ��������ƣ�һ�����������ַ�
	int m_limitNum;
	//�������������С
	CCSize m_designedSize;
	int	m_nInputType;
public:
    CursorTextField();
    ~CursorTextField();
    
    // static 
	// placeholder��Ĭ�ϵ�Text����
	// fontName������
	// fontSize���ִ�С
	// touchSize��������Ч����
	// color������Լ�������ɫ
    static CursorTextField* textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize,CCSize touchSize,ccColor3B color);
    
    // CCLayer
    void onEnter();

	void setPriority(int nPriority);
	void onExit();
    
    // ��ʼ����꾫��
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
    
    // �ж��Ƿ�����TextField��
    bool isInTextField(CCTouch *pTouch);
    // �õ�TextField����
    CCRect getRect();
   
    // �����뷨
    void openIME();
    // �ر����뷨
    void closeIME();

	//�����Ƿ��Ǻ���ʾ
	bool getIsPsw();
	void setIsPsw(bool bFlag);
	//�����ַ��������ƣ�һ�����������ַ�
	void setLimitNum(int limitNum);
	int getLimitNum();
	//����������뷨�ĳߴ緶Χ
//	void setDesignedSize(CCSize size);
	CCSize getDesignedSize();

	std::string* getTextInput();
	void setTextInput(std::string str);

	void clearTextInput();
	void setCursorVisible(bool visible);
	void setInputType(int type);
};

#endif
