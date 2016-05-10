#include "CSpecialProgress.h"


CSpecialProgress::CSpecialProgress():m_pStencilBar(nullptr),m_pClipNode(nullptr),m_pHead(nullptr),m_pLz(nullptr)
{

}

CSpecialProgress::~CSpecialProgress()
{

}


CProgressBar* CSpecialProgress::Create( const char* pProgress )
{
	CSpecialProgress* pRet = new CSpecialProgress();
	if( pRet && pRet->initWithFile(pProgress) )
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

CSpecialProgress* CSpecialProgress::CreateWithProgressBar( CProgressBar* pBar )
{
	CSpecialProgress* pRet = new CSpecialProgress();
	if( pRet &&  pRet->init() && pRet->copyFromProgressBar(pBar) )
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool CSpecialProgress::copyFromProgressBar(CProgressBar* pBar)
{
	if(pBar==nullptr)
	{
		return false;
	}

	CopyProgressBar(this, pBar);

	return true;
}

void CSpecialProgress::addHead( CCSprite* pSprite )
{
	if(m_pHead!=nullptr)
	{
		m_pHead->removeFromParentAndCleanup(true);
	}
	m_pHead = pSprite;
	this->addChild(pSprite, 11);
}

void CSpecialProgress::addParticleEffect( const char* pLz )
{
	//进度条效果
	if(m_pClipNode==nullptr)
	{
		m_pClipNode = CCClippingNode::create();
		m_pClipNode->setAlphaThreshold(0.05f);							//不设置则为矩形
		m_pClipNode->setContentSize(CCSizeMake(getProgressSprite()->getTexture()->getPixelsWide(), getProgressSprite()->getTexture()->getPixelsHigh()));
		m_pClipNode->setPosition(CCPointZero);
		m_pClipNode->setAnchorPoint(CCPointZero);

		if(m_pStencilBar==nullptr)
		{
			m_pStencilBar = CProgressBar::create();
			CopyProgressBar(m_pStencilBar, this);
			m_pStencilBar->setAnchorPoint(ccp(0.5f, 0.5f));
			m_pStencilBar->setScaleX(1.0f);
			m_pStencilBar->setPosition(ccp(m_pClipNode->getContentSize().width/2, m_pClipNode->getContentSize().height/2));
			m_pClipNode->setStencil(m_pStencilBar);							//设置的stencil必须是没有addchild的。		
		}

		this->addChild(m_pClipNode, 10);
	}
	
	if(m_pLz!=nullptr)
	{
		m_pLz->removeFromParentAndCleanup(true);
	}
	m_pLz = CCParticleSystemQuad::create(pLz);//填充粒子
	m_pLz->setPosition(ccp(m_pClipNode->getContentSize().width/2, m_pClipNode->getContentSize().height/2));
	m_pClipNode->addChild(m_pLz);
}

void CSpecialProgress::changeValueAndExecuteEvent( float nValue, bool bExeEvent )
{
	CProgressBar::changeValueAndExecuteEvent(nValue, bExeEvent);
	
}

void CSpecialProgress::updateForAdaptPos( float dt )
{
	int iValue = getValue();
	if(m_pStencilBar)
	{
		m_pStencilBar->setValue(iValue);
	}
	if(m_pHead)
	{
		m_pHead->setPosition(ccp(this->getContentSize().width*iValue/this->getMaxValue()-8, this->getContentSize().height/2));
	}
}

bool CSpecialProgress::init()
{
	CProgressBar::init();
	
	schedule(schedule_selector(CSpecialProgress::updateForAdaptPos));

	return true;
}


void CopyProgressBar(CProgressBar* pAim, CProgressBar* pFrom)
{
	if (pFrom->getBackgroundImage())
	{	
		pFrom->setBackgroundTexture(pFrom->getBackgroundImage()->getTexture());	
	}
	if(pFrom->getProgressSprite())
	{
		pAim->setProgressTexture(pFrom->getProgressSprite()->getTexture());
	}
	
	pAim->setMaxValue(pFrom->getMaxValue());
	pAim->setMinValue(pFrom->getMinValue());
	pAim->setValue(pFrom->getValue());

	pAim->setPosition(pFrom->getPosition());
	pAim->setAnchorPoint(pFrom->getAnchorPoint());
	pAim->setScaleX(pFrom->getScaleX());
	pAim->setScaleY(pFrom->getScaleY());
	pAim->setRotationX(pFrom->getRotationX());
	pAim->setRotationY(pFrom->getRotationY());
	pAim->setTag(pFrom->getTag());
	pAim->setVisible(pFrom->isVisible());
	pAim->setZOrder(pFrom->getZOrder());

	pAim->setEnabled(pFrom->isEnabled());
	pAim->setTouchEnabled(pFrom->isTouchEnabled());
	if (pFrom->getId())
	{
		pAim->setId(pFrom->getId());
	}
}
