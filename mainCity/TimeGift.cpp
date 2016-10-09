/******************************************************
*文件名称:	TimeGift.cpp
*编写日期:	2016-8-29-12:48
*编写作者:	YPF
*功能描述:	在线礼包
*******************************************************/

#include "TimeGift.h"
#include "model/DataCenter.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/UICloneMgr.h"
#include "vip/VipLayer.h"
#include "ExchangeTip.h"
#include "GMessage.h" 
#include "tools/ShowTexttip.h"
#include "sign/PopItem.h"
#include "common/CommonFunction.h"


CTimeGift::CTimeGift()
	:m_pUI(nullptr),m_iTime(0),m_pSkeletonAnimation(nullptr)
{}

CTimeGift::~CTimeGift()
{}

bool CTimeGift::init()
{
	if (BaseLayer::init())
	{

		m_pUI = LoadComponent("timeGift.xaml");  //  
		m_pUI->setPosition(VCENTER);
		this->addChild(m_pUI);

		this->setVisible(true);
		this->setIsShowBlack(false);

		//礼包
		CImageView *pGift = (CImageView *)m_pUI->findWidgetById("gift");
		pGift->setTouchEnabled(true);
		pGift->setOnClickListener(this, ccw_click_selector(CTimeGift::onGift));
		pGift->setOpacity(0);

		m_pSkeletonAnimation = SkeletonAnimation::createWithFile("mainCity/timeGift.json", "mainCity/timeGift.atlas", 1);
		m_pSkeletonAnimation->setPosition(pGift->getPosition()+ccp(40, 50));
		m_pSkeletonAnimation->setAnimation(0, "stand1", true);
		m_pUI->addChild(m_pSkeletonAnimation, 3);

		//时间
		m_pText = (CLabel *)m_pUI->findWidgetById("time");
		m_pText->setString("00:00:00");

		schedule(schedule_selector(CTimeGift::updateForTime), 1.0f);

		return true;
	}
	return false;
}


void CTimeGift::onClose(CCObject*ob)
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CTimeGift::onEnter()
{
	BaseLayer::onEnter();

	NOTIFICATION->addObserver(this, callfuncO_selector(CTimeGift::timeInBackground), TIME_IN_BACKGROUND, nullptr);

	GetTcpNet->registerMsgHandler(OnlinePrizeMsg,this,CMsgHandler_selector(CTimeGift::ProcessMsg));
}

void CTimeGift::onExit()
{
	BaseLayer::onExit();

	NOTIFICATION->removeAllObservers(this);

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
}


void CTimeGift::ProcessMsg(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
	case OnlinePrizeMsg:
		{
			OnlinePrize* res = (OnlinePrize*)msg;
			int iResult = res->result();
			//领取结果 (1 成功， 2 在线时间未达到，3 领取错误)
			switch (iResult)
			{
			case 1://成功
				{
					m_pSkeletonAnimation->setToSetupPose();
					m_pSkeletonAnimation->setAnimation(0, "stand1", true);

					//解析购买成功后的数据
					CGetPrizeRes prizers;
					prizers.result =1;
					for (int i = 0; i < res->prizes_size(); i++)
					{  
						CPrize prize;
						prize.read(res->prizes(i));
						prizers.prizeList.push_back(prize);
					}

					//弹框
					CPopItem *popItem = CPopItem::create();
					LayerManager::instance()->push(popItem);
					popItem->popPrizeRes(&prizers);

					//收集货币
					collectMoneyFromPrize(prizers);

					//重置时间
					int iNextTime = res->nexttime();
					switch (iNextTime)
					{
					case -1:{ this->removeFromParentAndCleanup(true); };break;
					default:{ this->setTime(iNextTime); }break;
					}

				}break;
			case 2://领取时间未到
				{
					ShowPopTextTip(GETLANGSTR(2058));
				}break;
			case 3://数据错误
				{
					ShowPopTextTip(GETLANGSTR(207));
				}break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void CTimeGift::onGift( CCObject *pSender )
{
	//TODO
	if (m_iTime<=0)
	{
		AskForOnlinePrize();
	}
	//else
	//{
	//	ShowPopTextTip(GETLANGSTR(2058));
	//}
}

void CTimeGift::setTime( int iTime )
{
	m_iTime = iTime;
	updateString();
}

void CTimeGift::updateForTime( float dt )
{
	//TODO
	if (m_iTime <= 0)
	{
		return;
	}

	//更新时间
	updateTime();
	//更新字符串
	updateString();
}

void CTimeGift::updateTime()
{
	m_iTime--;
}

void CTimeGift::updateString()
{
	if (m_iTime==0)
	{
		m_pText->setString(GETLANGSTR(2061));
		m_pSkeletonAnimation->setAnimation(0, "stand2", true);
	}
	else
	{
		int iHour = m_iTime/(3600);
		int iMinute = (m_iTime/60)%60;
		int iSecond = m_iTime%60;
		m_pText->setString(CCString::createWithFormat("%02d:%02d:%02d", iHour, iMinute, iSecond)->getCString());
	}
}

void CTimeGift::timeInBackground( CCObject* pObj )
{
	//TODO
	CCInteger* pValue = dynamic_cast<CCInteger*>(pObj);
	int iValue = pValue->getValue()/1000;
	m_iTime -= iValue;
	m_iTime = m_iTime<0?0:m_iTime;
	updateString();
}
