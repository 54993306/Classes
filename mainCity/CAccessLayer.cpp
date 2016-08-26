#include "CAccessLayer.h"
#include "sign/SignLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "UserDefaultData.h"
#include "set/ExchangeCode.h"
#include "LuckyWheel//LuckyWheelLayer.h"
//#include "activity/PairCardsActivity.h"
#include "common/CommonFunction.h"
#include "sign/PopItem.h"
#include "tools/ShowTexttip.h"


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

	setTouchPriority(LayerManager::instance()->getPriority());
	
	if(CCUserDefault::sharedUserDefault()->getBoolForKey(ACCESS_UI_OPEN))
	{
		m_pCheck = (CCheckBox*)m_ui->findWidgetById("check");
		m_pCheck->setChecked(true);
	}

	GetTcpNet->registerMsgHandler(ExchangeCodeMsg,this,CMsgHandler_selector(CAccessLayer::ProcessMsg));
}

void CAccessLayer::onExit()
{
	BaseLayer::onExit();

	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("skill/9062.plist");

	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CAccessLayer::initAccess()
{
	m_pCheck = (CCheckBox*)m_ui->findWidgetById("check");
	m_pCheck->setAnchorPoint(ccp(0.5f, 0.5f));
	m_pCheck->setPosition(m_pCheck->getPosition()+ccp(m_pCheck->getScaleX()*m_pCheck->getContentSize().width/2, m_pCheck->getScaleY()*m_pCheck->getContentSize().height/2));
	m_pCheck->setOnCheckListener(this, ccw_check_selector(CAccessLayer::onCheck));

	CImageViewScale9* pMaskTouch = (CImageViewScale9*)m_ui->findWidgetById("mask_touch");
	pMaskTouch->setTouchEnabled(true);
	int count = AccessTypeMax;
	for(unsigned int i=AccessTypeSign; i<count; i++)
	{
		CButton* pImage = (CButton*)m_ui->getChildByTag(i);
		pImage->setTouchEnabled(true);
		pImage->setOnClickListener(this, ccw_click_selector(CAccessLayer::onTouchAccess));
		pImage->setVisible(false);
		m_pButtonVec.push_back(pImage);
		if(i==AccessTypeSign)
		{
			pImage->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f,1.1f),CCScaleTo::create(0.3f,1.0f))));
		}

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
	PlayEffectSound(SFX_Button);

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
	case AccessExchange:
		{
			//CExchangeCode *exchange = CExchangeCode::create();
			//exchange->loadExchangeByType(CBCode);
			//LayerManager::instance()->push(exchange);

			CExchangeCode *exchange = CExchangeCode::create();
			exchange->loadExchangeByType(ExCode);
			LayerManager::instance()->push(exchange);
		}
		break;
	case AccessLuckyWheel:
		{
			CLuckyWheelLayer* wheel = CLuckyWheelLayer::create();
			LayerManager::instance()->push(wheel);
			wheel->setVisible(true);
			//GetTcpNet->sendData(LuckyWheelMsg);
			//GetTcpNet->sendData(LuckyWheelMsg,true);
			GetTcpNet->sendDataType(LuckyWheelMsg);//,true
		}
		break;
	//case AccessPairCards:
	//	{
	//		PairCardsActivityLayer* pair = PairCardsActivityLayer::create();
	//		LayerManager::instance()->push(pair);
	//		pair->setVisible(true);
	//	}
	//	
	//	break;
	default:
		break;
	}
}

void CAccessLayer::ProcessMsg( int type, google::protobuf::Message *msg )
{

	CardExchangeRes *res= (CardExchangeRes*)msg;
	int  ret = res->result();
	if (ret==1)
	{
		CGetPrizeRes prizers;
		prizers.result =1;
		for (int i = 0; i < res->prize_list_size(); i++)
		{  
			CPrize prize;
			prize.read(res->prize_list(i));
			prizers.prizeList.push_back(prize);
		}

		//弹框
		CPopItem *popItem = CPopItem::create();
		LayerManager::instance()->push(popItem);
		popItem->popPrizeRes(&prizers);

		//收集货币
		collectMoneyFromPrize(prizers);

	}
	//2 兑换码已使用，3 已兑换过该奖励，4 兑换码错误
	else if (ret==2)
	{
		ShowPopTextTip(GETLANGSTR(281));
	}
	else if (ret==3)
	{
		ShowPopTextTip(GETLANGSTR(282));
	}
	else if (ret==4)
	{
		ShowPopTextTip(GETLANGSTR(283));
	}
}