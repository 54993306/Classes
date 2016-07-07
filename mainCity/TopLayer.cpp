#include "TopLayer.h"
#include "common/RollLabel.h"
#include "model/DataCenter.h"
#include "common/color.h"
#include "Global.h"
#include "GMessage.h"
#include "common/ProgressLabel.h"
#include "tollgate/TollgateLayer.h"
#include "netcontrol/CPlayerControl.h"
#include "shop/ShopLayer.h"
#include "MainCityControl.h"
#include "bag/BagScene.h"
#include "task/CTaskLayer.h"
#include "mail/EmailLayer.h"
#include "mail/EmailControl.h"
#include "CCTK/scenemanager.h"
#include "hero/HeroControl.h"
#include "roleImg/RoleInfo.h"
#include "friend/FriendLayer.h"
#include "vip/VipLayer.h"
#include "task/TaskControl.h"
#include "mainCity/BuyResource.h"
#include "tools/ShowTexttip.h"
#include "scene/CPopTip.h"
#include "Battle/AnimationManager.h"
#include "common/CGameSound.h"

bool CTopLayer::init()
{
	if (BaseLayer::init())
	{
// 		MaskLayer* lay = MaskLayer::create("MaskLayer");
// 		LayerManager::instance()->push(lay);
		this->setIsShowBlack(false);
		m_ui = LoadComponent("TopLay.xaml");
		m_ui->setPosition(VCENTER);
		m_ui->setTag(1);
		this->addChild(m_ui);
		
		show(nullptr);

		return true;
	}
	return false;
}

using namespace BattleSpace;
void CTopLayer::onEnter()
{
	BaseLayer::onEnter();
	this->setTouchPriority(-100);
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();

	m_coinLabel = (CLabel*)m_ui->getChildByTag(13);
	m_coinLabel->setString(ToString(user->getCoin()));

	m_moneyLabel = (CLabel*)m_ui->getChildByTag(14);
	m_moneyLabel->setString(ToString(user->getRoleGold()));
	// 
	m_foodLabel = (CLabel*)m_ui->getChildByTag(17);
	//m_foodLabel->setString(ToString(user->getRoleFood()));
	m_foodLabel->setString(ToString(user->getFriends()));

	CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
	action->setString(CCString::createWithFormat("%d/%d",user->getRoleAction(),user->getActionLimit())->getCString());

	CButton* add_food = (CButton*)m_ui->findWidgetById("btn_add_food");
	CButton* add_copper = (CButton*)m_ui->findWidgetById("btn_add_copper");
	CButton* add_soul = (CButton*)m_ui->findWidgetById("btn_add_soul");
	CButton* add_gold = (CButton*)m_ui->findWidgetById("btn_add_gold");
	CButton* add_friendship = (CButton*)m_ui->findWidgetById("btn_add_friendship");
	add_food->getSelectedImage()->setScale(1.1f);
	add_copper->getSelectedImage()->setScale(1.1f);
	add_soul->getSelectedImage()->setScale(1.1f);
	add_gold->getSelectedImage()->setScale(1.1f);
	add_friendship->getSelectedImage()->setScale(1.1f);
	add_food->setOnClickListener(this, ccw_click_selector(CTopLayer::onAddResourceBtn));
	add_copper->setOnClickListener(this, ccw_click_selector(CTopLayer::onAddResourceBtn));
	add_soul->setOnClickListener(this, ccw_click_selector(CTopLayer::onAddResourceBtn));
	add_gold->setOnClickListener(this, ccw_click_selector(CTopLayer::onAddResourceBtn));
	add_friendship->setOnClickListener(this, ccw_click_selector(CTopLayer::onAddResourceBtn));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO,this,GameMsghandler_selector(CTopLayer::updateRoleProperty));

	//绑定场景隐藏和显示的消息
	NOTIFICATION->addObserver(this, callfuncO_selector(CTopLayer::show), SHOW_TOP_LAYER, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CTopLayer::hide), HIDE_TOP_LAYER, nullptr);

	CImageView *coin = (CImageView*)(m_ui->findWidgetById("coin"));
	CCAnimation *coinAnim = AnimationManager::sharedAction()->getAnimation("7033");
	coinAnim->setDelayPerUnit(0.3f);
	coin->runAction(CCRepeatForever::create(CCAnimate::create(coinAnim)));

	CImageView *gold = (CImageView*)(m_ui->findWidgetById("gold"));
	CCAnimation *goldAnim = AnimationManager::sharedAction()->getAnimation("7034");
	goldAnim->setDelayPerUnit(0.3f);
	gold->runAction(CCRepeatForever::create(CCAnimate::create(goldAnim)));
}

void CTopLayer::updateActionTime(float dt)
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	if (data->getRoleAction() < data->getActionLimit())
	{
		data->setRoleAction(data->getRoleAction()+1);
		CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
		action->setString(CCString::createWithFormat("%d/%d",data->getRoleAction(),data->getActionLimit())->getCString());
		data->setActionTime(data->getActionTime() + data->getInterval()*60*60);
	}
	else
	{
		this->unschedule(schedule_selector(CTopLayer::updateActionTime));
	}
}

void CTopLayer::updateRoleProperty(const TMessage& tMsg)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();

	CCScaleTo* sc1 = CCScaleTo::create(0.15f,1.3f);
	CCScaleTo* sc2 = CCScaleTo::create(0.35f,1.0f);
	CCSequence* sqes = CCSequence::create(sc1,sc2,nullptr);

	if (user->getCoin()!=atoi(m_coinLabel->getString()))
	{
		CProgressLabel *prolab = CProgressLabel::create();
		prolab->setLabel(m_coinLabel);
		prolab->changeValueTo(user->getCoin(),0.5f);
		CCSequence* sqes1 = (CCSequence*)sqes->copy();
		m_coinLabel->runAction(sqes1);
		this->addChild(prolab);

		CLabel *label = (CLabel*)m_ui->getChildByTag(130);
		int coin = user->getCoin()-atoi(m_coinLabel->getString());
		changeLabel(label, coin);
	}
	
	if (user->getRoleGold()!=atoi(m_moneyLabel->getString()))
	{
		CProgressLabel *prolab = CProgressLabel::create();
		prolab->setLabel(m_moneyLabel);
		prolab->changeValueTo(user->getRoleGold(),0.5f);
		CCSequence* sqes1 = (CCSequence*)sqes->copy();
		m_moneyLabel->runAction(sqes1);
		this->addChild(prolab);

		CLabel *label = (CLabel*)m_ui->getChildByTag(140);
		int coin = user->getRoleGold()-atoi(m_moneyLabel->getString());
		changeLabel(label, coin);
	}
	/*
	if (user->getRoleFood()!=atoi(m_foodLabel->getString()))
	{
		CProgressLabel *prolab = CProgressLabel::create();
		prolab->setLabel(m_foodLabel);
		prolab->changeValueTo(user->getRoleFood(),0.5f);
		CCSequence* sqes1 = (CCSequence*)sqes->copy();
		m_foodLabel->runAction(sqes1);
		this->addChild(prolab);

		CLabel *label = (CLabel*)m_ui->getChildByTag(170);
		int coin = user->getRoleFood()-atoi(m_foodLabel->getString());
		changeLabel(label, coin);
	}
	*/
	if (user->getFriends()!=atoi(m_foodLabel->getString()))
	{
		CProgressLabel *prolab = CProgressLabel::create();
		prolab->setLabel(m_foodLabel);
		prolab->changeValueTo(user->getFriends(),0.5f);
		CCSequence* sqes1 = (CCSequence*)sqes->copy();
		m_foodLabel->runAction(sqes1);
		this->addChild(prolab);

		CLabel *label = (CLabel*)m_ui->getChildByTag(170);
		int coin = user->getFriends()-atoi(m_foodLabel->getString());
		changeLabel(label, coin);
	}

	CLabel *action = (CLabel*)(m_ui->findWidgetById("action"));
	if (user->getRoleAction()!=atoi(action->getString()))
	{
		CCSequence* sqes2 = (CCSequence*)sqes->copy();
		action->runAction(sqes2);
		action->setString(CCString::createWithFormat("%d/%d",user->getRoleAction(), user->getActionLimit())->getCString());
	}

}

void CTopLayer::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(TASK_NOTICE,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(MAIL_NOTICE,this);
	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);
}

void CTopLayer::changeLabel(CLabel * label, int coin)
{
	label->setVisible(true);
	label->setString(ToString(coin));
	CProgressLabel *prolab = CProgressLabel::create();
	prolab->setLabel(label);
	prolab->changeValueTo(0,0.5f);
	this->addChild(prolab);

	CCDelayTime *delay = CCDelayTime::create(0.5f);
	CCCallFuncN *call = CCCallFuncN::create(this,callfuncN_selector(CTopLayer::hideLabel));
	label->runAction(CCSequence::createWithTwoActions(delay,call));
}

void CTopLayer::hideLabel(CCNode *node)
{
	node->setVisible(false);
}

void CTopLayer::onAddResourceBtn(CCObject* pSender)
{
	CButton* btn = (CButton*)pSender;
	if (!strcmp(btn->getId(),"btn_add_gold"))
	{
		CVipLayer *vipLayer= CVipLayer::create();
		if(LayerManager::instance()->push(vipLayer))
		{
			GetTcpNet->sendDataType(VipInfoMsg,true);
			GetTcpNet->sendDataType(VipShopMsg,true);
			return;
		}
	}
 /*	return;*/
	

	BuyResource *layer = dynamic_cast<BuyResource*>(LayerManager::instance()->getLayer("BuyResource"));
	if (layer)
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	if(dynamic_cast<CPopTip*>(LayerManager::instance()->getLayer("BuyResourceText")))
	{
		return;
	}

	if (!strcmp(btn->getId(),"btn_add_food"))
	{
		BuyResource* bufLayer = BuyResource::create();
		bufLayer->setExchangeType(BUY_FOOD);
		LayerManager::instance()->push(bufLayer);
		CPlayerControl::getInstance().sendConvert(2);
	}
	else if (!strcmp(btn->getId(),"btn_add_copper"))
	{
		BuyResource* bufLayer = BuyResource::create();
		bufLayer->setExchangeType(BUY_COIN);
		LayerManager::instance()->push(bufLayer);
		CPlayerControl::getInstance().sendConvert(1);
	}else if (!strcmp(btn->getId(),"btn_add_soul"))
	{
		
	}else if (!strcmp(btn->getId(),"btn_add_gold"))
	{
	//	LayerManager::instance()->pop();
	}
	else if(!strcmp(btn->getId(),"btn_add_friendship"))
	{
		CPopTip* TopLayerText = ShowPopTextTip(GETLANGSTR(1114),ccWHITE,false);
		TopLayerText->setName("BuyResourceText");
	}
	else{
	//	LayerManager::instance()->pop();
		CCLOG("[ *ERROR ] CMainCityUI::onAddResourceBtn");
	}
}

void CTopLayer::show( CCObject* pObj )
{
	m_iIndexForShow++;
	if(m_iIndexForShow>0)
	{
		this->setVisible(true);
	}
	CCLOG("CTopLayer::show_%d", m_iIndexForShow);
}

void CTopLayer::hide( CCObject* pObj )
{
	m_iIndexForShow--;
	if(m_iIndexForShow<=0)
	{
		this->setVisible(false);
	}
	CCLOG("CTopLayer::hide_%d", m_iIndexForShow);
}
