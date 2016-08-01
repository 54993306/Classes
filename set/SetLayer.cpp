#include "SetLayer.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "common/MultLanguage.h"
#include "ExchangeCode.h"
#include "SetPush.h"
#include "tollgate/chapter.h"
#include "sign/PopItem.h"
#include "common/CGameSound.h"
#include "common/CommonFunction.h"

#include "Resources.h"
#include "common/CursorTextField.h"

CSetLayer::CSetLayer():m_bSoundLock(false)
{

}

bool CSetLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("SetLayermask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		//lay->setVisible(false);

		m_ui = LoadComponent("Set.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);
		
		this->setIsShowBlack(false);
		this->setVisible(true);

		return true;
	}
	return false;

}

void CSetLayer::onEnter()
{
	BaseLayer::onEnter();
	CButton *push = (CButton*)(m_ui->findWidgetById("push"));
	push->setOnClickListener(this,ccw_click_selector(CSetLayer::onPushSet));
	CButton *exchange = (CButton*)(m_ui->findWidgetById("exchange"));
	exchange->setOnClickListener(this,ccw_click_selector(CSetLayer::onExchange));


	////退出
	//CButton* pClose = CButton::create("common/back.png", "common/back.png");
	//pClose->getSelectedImage()->setScale(1.1f);
	//pClose->setPosition(VLEFT+50, VTOP-50);
	//pClose->setOnClickListener(this,ccw_click_selector(CSetLayer::onClose));	
	//this->addChild(pClose, 999);


	CToggleView *effMusic = (CToggleView*)(m_ui->findWidgetById("effMusic"));
	effMusic->setOnCheckListener(this,ccw_check_selector(CSetLayer::onEffMusic));
	CToggleView *bgMusic = (CToggleView*)(m_ui->findWidgetById("bgMusic"));
	bgMusic->setOnCheckListener(this,ccw_check_selector(CSetLayer::onBgMusic));

	bool ison = GameSound->isEffectOn();
	effMusic->setChecked(ison);
	showEffMusicLab(ison);
	ison = GameSound->isMusicOn();
	bgMusic->setChecked(ison);
	showBgMusicLab(ison);

	m_bSoundLock = true;

	GetTcpNet->registerMsgHandler(ExchangeCodeMsg,this,CMsgHandler_selector(CSetLayer::ProcessMsg));

	NOTIFICATION->addObserver(this, callfuncO_selector(CSetLayer::hide), SET_LAYER_HIDE, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CSetLayer::show), SET_LAYER_SHOW, nullptr);

	NOTIFICATION->postNotification(HIDE_ROLE_INFO);
}

void CSetLayer::onEffMusic(CCObject *pSender, bool bChecked)
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}
	
	showEffMusicLab(bChecked);
}

void CSetLayer::onBgMusic(CCObject *pSender, bool bChecked)
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Button);
	}

	showBgMusicLab(bChecked);
}

void CSetLayer::onExchange(CCObject* pSender)
{
	//if(m_bSoundLock)
	//{
	//	PlayEffectSound(SFX_Button);
	//}

	CExchangeCode *exchange = CExchangeCode::create();
	exchange->loadExchangeByType(ExCode);
	LayerManager::instance()->push(exchange);
}

void CSetLayer::onPushSet(CCObject* pSender)
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Ensure);
	}

	CSetPush *push = CSetPush::create();
	LayerManager::instance()->push(push);
}

void CSetLayer::onClose(CCObject* pSender)
{
	if(m_bSoundLock)
	{
		PlayEffectSound(SFX_Ensure);
	}

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSetLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);

	NOTIFICATION->removeAllObservers(this);

	NOTIFICATION->postNotification(SHOW_ROLE_INFO);
}

CSetLayer::~CSetLayer()
{

}

void CSetLayer::showEffMusicLab(bool bChecked)
{
	CLabel *efflab = (CLabel*)(m_ui->findWidgetById("effmusicLab"));
	if (bChecked)
	{
		efflab->setString(GETLANGSTR(1006));
	}
	else
	{
		efflab->setString(GETLANGSTR(1007));
	}

	GameSound->setEffectState(bChecked);
}

void CSetLayer::showBgMusicLab(bool bChecked)
{
	CLabel *bglab = (CLabel*)(m_ui->findWidgetById("bgmusicLab"));
	if (bChecked)
	{
		bglab->setString(GETLANGSTR(1008));
	}
	else
	{
		bglab->setString(GETLANGSTR(1009));
	}

	GameSound->setMusicState(bChecked);
}

void CSetLayer::ProcessMsg(int type, google::protobuf::Message *msg)
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

bool CSetLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

		CImageView *bgView1 = (CImageView*)(m_ui->findWidgetById("bg"));

		if( bgView1->boundingBox().containsPoint(pTouchPos))
		{
			res = true;
		}
		else
		{
			onClose(nullptr);
		}
	}
	return true;
}

void CSetLayer::hide(CCObject* pObj)
{
	setVisible(false);
}

void CSetLayer::show(CCObject* pObj)
{
	setVisible(true);
}


