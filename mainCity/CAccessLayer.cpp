#include "CAccessLayer.h"
#include "sign/SignLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "UserDefaultData.h"

CAccessLayer::CAccessLayer():m_ui(nullptr),m_pMask(nullptr), m_pCheck(nullptr)
	,m_iLockIndex(0)
{

}

CAccessLayer::~CAccessLayer()
{

}

bool CAccessLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("access.xaml");
		m_ui->setPosition(ccp(winSize.width-1138/2, VCENTER.y));
		this->addChild(m_ui);
		this->setIsShowBlack(false);

		initAccess();

		setVisible(true);

		return true;
	}

	return false;
}

void CAccessLayer::onEnter()
{
	BaseLayer::onEnter();

	setTouchPriority(-2);

	if(CCUserDefault::sharedUserDefault()->getBoolForKey(ACCESS_UI_OPEN))
	{
		m_pCheck = (CCheckBox*)m_ui->findWidgetById("check");
		m_pCheck->setChecked(true);
	}
}

void CAccessLayer::onExit()
{
	BaseLayer::onExit();
}

void CAccessLayer::initAccess()
{
	m_pCheck = (CCheckBox*)m_ui->findWidgetById("check");
	m_pCheck->setAnchorPoint(ccp(0.5f, 0.5f));
	m_pCheck->setPosition(m_pCheck->getPosition()+ccp(m_pCheck->getScaleX()*m_pCheck->getContentSize().width/2, m_pCheck->getScaleY()*m_pCheck->getContentSize().height/2));
	m_pCheck->setOnCheckListener(this, ccw_check_selector(CAccessLayer::onCheck));

	CImageViewScale9* pMaskTouch = (CImageViewScale9*)m_ui->findWidgetById("mask_touch");
	pMaskTouch->setTouchEnabled(true);

	for(unsigned int i=AccessTypeSign; i<AccessTypeMax; i++)
	{
		CButton* pButton = (CButton*)m_ui->getChildByTag(i);
		pButton->setOnClickListener(this, ccw_click_selector(CAccessLayer::onTouchAccess));
		pButton->setVisible(false);
		m_pButtonVec.push_back(pButton);
	}

	m_pMask = (CImageViewScale9*)m_ui->findWidgetById("mask");
	m_pMask->setAnchorPoint(ccp(0.5f, 1.0f));
	m_pMask->setVisible(false);

}

void CAccessLayer::showAccess()
{
	int iSize = m_pButtonVec.size();
	CCPoint pBasePos = m_pCheck->getPosition();
	for(int i=0; i<iSize; i++)
	{
		CButton* pButton = m_pButtonVec[i];
		m_iLockIndex++;
		pButton->runAction(CCSequence::create(
			CCDelayTime::create(i*0.03f),
			CCShow::create(),
			CCMoveBy::create(0.05+i*0.03f, ccp(0, -70.0f*(i+1))),
			CCCallFunc::create(this, callfunc_selector(CAccessLayer::callBackForChecActionkEnd))
			,nullptr));
	}

	m_iLockIndex++;
	m_pMask->setVisible(true);
	schedule(schedule_selector(CAccessLayer::updateForScale9Mask), 0.02f);

	m_iAimHeight = 70.0f*iSize+30;
}

void CAccessLayer::hideAccess()
{
	int iSize = m_pButtonVec.size();
	CCPoint pBasePos = m_pCheck->getPosition();
	for(int i=0; i<iSize; i++)
	{
		CButton* pButton = m_pButtonVec[i];
		m_iLockIndex++;
		pButton->runAction(CCSequence::create(
			CCDelayTime::create((iSize-i)*0.03f),
			CCMoveBy::create(0.05+i*0.03f, ccp(0, 70.0f*(i+1))),
			CCHide::create(),
			CCCallFunc::create(this, callfunc_selector(CAccessLayer::callBackForChecActionkEnd))
			,nullptr));
	}

	m_iLockIndex++;
	schedule(schedule_selector(CAccessLayer::updateForScale9Mask), 0.02f);
	m_iAimHeight = 0;
}


void CAccessLayer::onCheck( CCObject *pSender, bool bChecked )
{
	m_pCheck->setEnabled(false);
	
	//动作
	m_iLockIndex++;
	m_pCheck->setVisible(true);
	m_pCheck->runAction(CCSequence::createWithTwoActions(
		CCRotateBy::create(0.15f, 180),
		CCCallFunc::create(this, callfunc_selector(CAccessLayer::callBackForChecActionkEnd))));

	PlayEffectSound(SFX_429);

	if(bChecked)
	{
		showAccess();
	}
	else
	{
		hideAccess();
	}
	CCUserDefault::sharedUserDefault()->setBoolForKey(ACCESS_UI_OPEN, bChecked);
	CCUserDefault::sharedUserDefault()->flush();
}


void CAccessLayer::callBackForChecActionkEnd()
{
	m_iLockIndex--;
	m_pCheck->setEnabled(m_iLockIndex<=0?true:false);
}


void CAccessLayer::updateForScale9Mask( float dt )
{
	float fSpeed = 40.0f;
	if(m_iAimHeight==0)
	{
		m_pMask->setContentSize(CCSize(m_pMask->getContentSize().width, m_pMask->getContentSize().height-fSpeed));
		if(m_pMask->getContentSize().height<m_iAimHeight)
		{
			m_pMask->setContentSize(CCSize(m_pMask->getContentSize().width, m_iAimHeight));
			m_pMask->setVisible(false);
			m_pMask->setTouchEnabled(false);
			unschedule(schedule_selector(CAccessLayer::updateForScale9Mask));
			callBackForChecActionkEnd();
		}
	}
	else
	{
		m_pMask->setContentSize(CCSize(m_pMask->getContentSize().width, m_pMask->getContentSize().height+fSpeed));
		if(m_pMask->getContentSize().height>m_iAimHeight)
		{
			m_pMask->setContentSize(CCSize(m_pMask->getContentSize().width, m_iAimHeight));
			unschedule(schedule_selector(CAccessLayer::updateForScale9Mask));
			callBackForChecActionkEnd();
			m_pMask->setTouchEnabled(true);
		}
	}
}



void CAccessLayer::onTouchAccess( CCObject *pSender )
{
	if(!m_pCheck->isEnabled()) return;

	CCNode* pNode = (CCNode*)pSender;

	int iTag = pNode->getTag();

	switch (iTag)
	{
	case AccessTypeSign:
		{
			CSignLayer *sign = CSignLayer::create();
			LayerManager::instance()->push(sign);
			GetTcpNet->sendDataType(SignDataMsg,true);
		}break;
	default:
		break;
	}
}