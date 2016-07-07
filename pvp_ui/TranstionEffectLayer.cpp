#include "TranstionEffectLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"


CTranstionEffectLayer::CTranstionEffectLayer()
	:m_pBatchNode(nullptr),m_pPosStart(CCPointZero)
	,m_iIndexMax(0), m_iLineCount(0), m_iOverLineCount(0)
	,m_iLineDir(TranstionEffectLineDirectionHorizontal), m_iGrowDir(TranstionEffectGrowDirectionAdd)
	,m_iShowIndex(0)
	,m_pTarget(nullptr)
	,m_endCallBack(nullptr)
	,m_iRotation(0)
	,m_fTimeMove(0)
	,m_fTimeFade(0)
{

}

CTranstionEffectLayer::~CTranstionEffectLayer()
{

}

bool CTranstionEffectLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		this->setIsShowBlack(false);

		//effect
		m_pBatchNode = CCSpriteBatchNode::create("pvp/boxlight.png");
		m_pBatchNode->setPosition(CCPointZero);
		m_pBatchNode->setAnchorPoint(CCPointZero);
		this->addChild(m_pBatchNode);
		
		m_pPosStart = ccp(150, 10);

		m_iIndexMax = 12;

		m_iLineCount = 12;

		m_fTimeMove = 0.04f;

		m_fTimeFade = 0.55f;

		this->setVisible(true);

		return true;
	}

	return false;
}


void CTranstionEffectLayer::initLineGapAndGrowGap()
{
	switch (m_iLineDir)
	{
	case TranstionEffectLineDirectionVertical:
		{
			switch (m_iGrowDir)
			{
			case TranstionEffectGrowDirectionAdd:
				{
					m_pGrowGap = ccp(54, 0);
					m_pLineGap = ccp(29, 43);
				}
				break;
			case TranstionEffectGrowDirectionMinus:
				{
					m_pGrowGap = ccp(-54, 0);
					m_pLineGap = ccp(-29, 43);
				}
				break;
			default:
				break;
			}
		}break;
	case TranstionEffectLineDirectionHorizontal:
		{
			switch (m_iGrowDir)
			{
			case TranstionEffectGrowDirectionAdd:
				{
					m_pGrowGap = ccp(0, 54);
					m_pLineGap = ccp(43, 29);
				}
				break;
			case TranstionEffectGrowDirectionMinus:
				{
					m_pGrowGap = ccp(0, -54);
					m_pLineGap = ccp(43, -29);
				}
				break;
			default:
				break;
			}
		}break;
	default:
		break;
	}
}


void CTranstionEffectLayer::onEnter()
{
	BaseLayer::onEnter();

	initLineGapAndGrowGap();
}

void CTranstionEffectLayer::onExit()
{
	BaseLayer::onExit();
}



void CTranstionEffectLayer::onOpen(CCNode* pParent, int iZorder)
{
	pParent->addChild(this, iZorder);
}


void CTranstionEffectLayer::onClose( )
{
	if(m_pTarget != nullptr && m_endCallBack != nullptr)
	{
		(m_pTarget->*m_endCallBack)();
	}
	this->removeFromParentAndCleanup(true);
}

void CTranstionEffectLayer::showEffect()
{
	//n条增长线
	//每条线速度不同
	CCPoint pPos = m_pPosStart;
	for(int i=0; i<m_iLineCount; i++)
	{
		if(m_iLineDir == TranstionEffectLineDirectionHorizontal)
		{
			pPos = pPos + ccp(m_pLineGap.x, (i%2==1?1:-1)*m_pLineGap.y);
		}
		else
		{
			pPos = pPos + ccp((i%2==1?1:-1)*m_pLineGap.x, m_pLineGap.y);
		}

		showLine(pPos, 0, i==m_iLineCount-1?true:false);
	}
}

void CTranstionEffectLayer::showLine( CCPoint pPos, float fTime , bool bHideFirst)
{
	createElement(pPos, 0, bHideFirst);
}

void CTranstionEffectLayer::showLineElement( CCNode* pNode )
{
	int iIndex = pNode->getTag();
	if(iIndex>=m_iIndexMax-1)
	{
		m_iOverLineCount++;
		if(m_iOverLineCount>=m_iLineCount)
		{
			return;
		}
	}
	CCPoint pPos = pNode->getPosition() + m_pGrowGap;
	createElement(pPos, iIndex+1);
}

 CCSprite*  CTranstionEffectLayer::createElement( CCPoint pPos, int iIndex, bool bIsHide/*=false*/ )
{
	m_iShowIndex++;
	CCSprite* pSprite = CCSprite::create("pvp/boxlight.png");
	pSprite->setPosition(pPos);
	pSprite->setTag(iIndex);
	pSprite->setOpacity(CCRANDOM_0_1()*250);
	pSprite->setVisible(CCRANDOM_0_1()>0.9f?false:true);
	pSprite->setVisible(!bIsHide);
	pSprite->setRotation(m_iRotation);
	m_pBatchNode->addChild(pSprite);
	
	pSprite->runAction(CCSequence::createWithTwoActions(
		CCSpawn::createWithTwoActions(
		CCFadeOut::create(m_fTimeFade),
		CCSequence::createWithTwoActions(CCDelayTime::create(m_fTimeMove+CCRANDOM_0_1()*(m_fTimeMove*0.3f)), CCCallFuncN::create(this, callfuncN_selector(CTranstionEffectLayer::showLineElement))
		)),
		CCCallFuncN::create(this, callfuncN_selector(CTranstionEffectLayer::callBackforRemove))
		));
	return pSprite;
}

 void CTranstionEffectLayer::setStartPos( CCPoint startPos )
 {
	 m_pPosStart = startPos;
 }

 void CTranstionEffectLayer::bindCallback( CCObject* pObj, transitionEffect_end endCall )
 {
	 m_pTarget = pObj;
	 m_endCallBack = endCall;
 }

 void CTranstionEffectLayer::callBackforRemove( CCNode* pNode )
 {
	 pNode->removeFromParentAndCleanup(true);
	 m_iShowIndex--;
	 if(m_iShowIndex<=0)
	 {
		 onClose();
	 }
 }
