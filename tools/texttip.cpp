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

#include "texttip.h"
#include "ShowTexttip.h"
#include "commonDef.h"

texttip::texttip()
:m_pFlyLabel(NULL)
,m_pNodeContent(NULL)
,m_fontName(FONT_NAME)
,m_fontSize(25)
,m_fFlyingTime(0.0f)
,m_fStartFlyTime(0.0f)
,m_flyOffset(ccp(0, 0))
,m_color(ccc3(255, 255, 255))
{
}

texttip::~texttip()
{
	if ( m_type == MTB_TEXT )
	{
		TextTipManager::GetInstance().RemoveTextTip( this );
	}
}

bool texttip::init()
{
    bool bRet = false;

    do 
    {
        this->setContentSize(CCSizeMake(10, 10));
        this->setAnchorPoint(ccp(0.5, 0.5));
        bRet = true;
    } while (0);
    return bRet;
}

void texttip::doProcess()
{
    switch (m_type)
    {
    case MTB_TEXT:
		//修改了引擎底层的cclabelttf
		m_pFlyLabel = CCLabelTTF::create(m_strContent.c_str(), m_fontName.c_str(), m_fontSize);
		m_pFlyLabel->setColor(m_color);
		m_pFlyLabel->setFontSize(m_fontSize);
		m_pFlyLabel->setAnchorPoint(ccp(0.5, 0.5));
		m_pFlyLabel->setPosition(ccp(0.5, 0.5));
		this->addChild(m_pFlyLabel);
        break;
    case MTB_WARNING:
		{
			CCSprite* bg = NULL;
			bg = CCSprite::create("warScene/warning.png");
			bg->setAnchorPoint(ccp(0.5, 0.5));
			bg->setPosition(this->getAnchorPointInPoints());
			this->addChild(bg);
			if (m_pNodeContent)
			{
				m_pNodeContent->setAnchorPoint(ccp(0, 0.5));
				m_pNodeContent->setPosition(ccp( bg->getPositionX() - bg->getContentSize().width / 2 + 30 + 10,bg->getPositionY() ));
				this->addChild(m_pNodeContent);
			}else{
				m_pFlyLabel = CCLabelTTF::create(m_strContent.c_str(), m_fontName.c_str(), m_fontSize,CCSizeMake( bg->getContentSize().width - 30 - 40, 0),kCCTextAlignmentCenter );
				m_pFlyLabel->setColor(m_color);
				m_pFlyLabel->setAnchorPoint(ccp(0, 0.5));
				m_pFlyLabel->setPosition(ccp( bg->getPositionX() - bg->getContentSize().width / 2 + 30 + 10,bg->getPositionY() ));
				this->addChild(m_pFlyLabel);
			}
		}
		break;
    case MTB_OK:
        {
            CCSprite* bg = NULL;
            CCSprite* stateIcon = NULL;

            bg = CCSprite::create("WishTree/tipsbg.png");
            bg->setAnchorPoint(ccp(0.5, 0.5));
            bg->setPosition(this->getAnchorPointInPoints());
            this->addChild(bg);

            if (m_type == MTB_WARNING)
            {
                stateIcon = CCSprite::create("WishTree/waring.png");
            }else{
                stateIcon = CCSprite::create("WishTree/right.png");
            }
            stateIcon->setAnchorPoint(ccp(0, 0.5));
            stateIcon->setPosition(ccp( bg->getPositionX() - bg->getContentSize().width / 2 + 30, bg->getPositionY() ));
            this->addChild(stateIcon);
            if (m_pNodeContent)
            {
                m_pNodeContent->setAnchorPoint(ccp(0, 0.5));
                m_pNodeContent->setPosition(ccp( bg->getPositionX() - bg->getContentSize().width / 2 + 30 + stateIcon->getContentSize().width + 10,bg->getPositionY() ));
                this->addChild(m_pNodeContent);
            }else{
                m_pFlyLabel = CCLabelTTF::create(m_strContent.c_str(), m_fontName.c_str(), m_fontSize,CCSizeMake( bg->getContentSize().width - 30 - stateIcon->getContentSize().width - 40, 0),kCCTextAlignmentCenter );
                m_pFlyLabel->setColor(m_color);
                m_pFlyLabel->setAnchorPoint(ccp(0, 0.5));
                m_pFlyLabel->setPosition(ccp( bg->getPositionX() - bg->getContentSize().width / 2 + 30 + stateIcon->getContentSize().width + 10,bg->getPositionY() ));
                this->addChild(m_pFlyLabel);
            }
        }
        break;
	case ROUNDNUM:
		{
			CCSprite* bg = CCSprite::create("label/wave_jp.png");
			//bg->setAnchorPoint(ccp(0.5f, 0.5f));
			bg->setPositionX(bg->getPositionX()-30);
			this->addChild(bg);
			if (m_pNodeContent)
			{
				m_pNodeContent->setScale(0.95f);
				m_pNodeContent->setPosition(ccp(70,-35));
				this->addChild(m_pNodeContent);	
			}
					
		}
		break;
    }

    CCDelayTime * delay = CCDelayTime::create(m_fStartFlyTime);
    CCFiniteTimeAction* actionFly = CCSpawn::create(
        CCMoveBy::create(m_fFlyingTime, m_flyOffset),
        CCSequence::create(CCDelayTime::create(m_fFlyingTime * 0.7), CCFadeOut::create(m_fFlyingTime * 0.3), NULL),
        NULL);

    //消失后回调结束函数
    CCFiniteTimeAction* action = CCSequence::create(delay,
                                                    actionFly,
                                                    CCCallFuncN::create(this, callfuncN_selector(texttip::flyDone)),
                                                    NULL);

    if (m_type == MTB_TEXT)
        m_pFlyLabel->runAction(action);
    else
        this->runAction(action);
}

void texttip::flyDone( CCNode* node )
{
    this->removeFromParentAndCleanup(true);
}





TextTipManager::TextTipManager()
{

}

TextTipManager::~TextTipManager()
{
	m_TextTipsVec.clear();
}

TextTipManager& TextTipManager::GetInstance()
{
	static TextTipManager man;
	return man;
}

void TextTipManager::AddTextTip( texttip *pTip )
{
	if ( NULL == pTip )
	{
		return;
	}

	m_TextTipsVec.push_back( pTip );
	if ( m_TextTipsVec.size() == 1 )
	{
		DoProcess();
	}
}

void TextTipManager::RemoveTextTip( texttip *pTip )
{
	if ( NULL == pTip )
	{
		return;
	}

	std::vector<texttip*>::iterator it = m_TextTipsVec.begin();
	while ( it != m_TextTipsVec.end() )
	{
		if ( *it == pTip )
		{
			m_TextTipsVec.erase( it );
			return;
		}
		it++;
	}
}

void TextTipManager::DoProcess()
{
	CCDirector::sharedDirector()->getScheduler()->scheduleSelector( schedule_selector(TextTipManager::update), this, 0.5f, false );
}

void TextTipManager::update( float dt )
{
	std::vector<texttip*>::iterator it = m_TextTipsVec.begin();
	if ( it != m_TextTipsVec.end() )
	{
		texttip *pTip = *it;
		pTip->doProcess();
		m_TextTipsVec.erase( it );
	}

	if ( m_TextTipsVec.empty() )
	{
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector( schedule_selector(TextTipManager::update), this );
	}
}

