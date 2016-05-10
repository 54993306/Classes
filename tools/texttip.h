// ***************************************************************
//  FileName: texttip.h 
//  Version:  1.0   
//  Date: 
//  Author: linxiancheng
//  Copyright (C) 2014 U1
//  --------------------------------------------------------------
//  Description:  文字提示
//  -------------------------------------------------------------
//  History: 
//  -------------------------------------------------------------
// ***************************************************************

#pragma once

#include "cocos2d.h"

#include <string>

using namespace cocos2d;

class texttip : public CCNode
{
public:
    texttip();
    ~texttip();

    CREATE_FUNC(texttip);

    bool init();

    void doProcess();

    void setFlyState(float startFlyTime, float flyingTime,  CCPoint offset,int type ,int fontSize = 25)
    {
        m_fStartFlyTime = startFlyTime;
        m_fFlyingTime = flyingTime;
        m_flyOffset = offset;
		m_type = type;
		m_fontSize = fontSize;
    }
    inline void setFontName(const char * fontName) { m_fontName = fontName; }
    inline void setFontSize(int fontSize) { m_fontSize = fontSize; }
    inline void setFont(const char * fontName, int fontSize)
    {
        setFontName(fontName);
        setFontSize(fontSize);
    }
    void setContent(const char * content) { m_strContent = content; }
    void setContent(CCNode * content) { m_pNodeContent = content; }
    
    void setFontColor(const ccColor3B & color) { m_color = color; }

private:
    void flyDone(CCNode* node);

private:
    CCLabelTTF *            m_pFlyLabel;

    std::string             m_strContent;
    CCNode *                m_pNodeContent;
    std::string             m_fontName;
    int                     m_fontSize;

    float                   m_fStartFlyTime;
    float                   m_fFlyingTime;
    CCPoint                 m_flyOffset;
    ccColor3B               m_color;
	int						m_type;
};

class TextTipManager : public CCObject
{
private:
	TextTipManager();
	~TextTipManager();

public:
	static TextTipManager& GetInstance();
	void AddTextTip( texttip *pTip );
	void RemoveTextTip( texttip *pTip );
	void DoProcess();
	virtual void update( float dt );

private:
	std::vector<texttip*> m_TextTipsVec;
};