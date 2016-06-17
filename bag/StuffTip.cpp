#include "StuffTip.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "model/DataCenter.h"
#include "Battle/AnimationManager.h"
#include "common/CommonFunction.h"

#include "common/CGameSound.h"
#include "Resources.h"

CStuffTip::CStuffTip()
	:m_isOutClose(false)
{

}


bool CStuffTip::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);

		CColorView *colorView = CColorView::create(ccc4(0,0,0,200));
		colorView->setContentSize(CCSizeMake(1138,640));
		colorView->setPosition(VCENTER);
		colorView->setTag(100);
		this->addChild(colorView);

		m_ui = LoadComponent("StuffTip.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}

void CStuffTip::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CStuffTip::onEnter()
{
	BaseLayer::onEnter();

	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");
	CCSprite *mask= (CCSprite *)m_ui->findWidgetById("mask");
	bg->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15)))));
	mask->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15)))));

	//光影效果
	/*
	CCNode* pBg = (CCNode *)m_ui->findWidgetById("bg");
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9013");
	culAnim->setDelayPerUnit(0.1f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CCSprite* pSprite = CCSprite::create("skill/9013.png");
	pSprite->setPosition(ccp(215, 96));
	pBg->addChild(pSprite);
	pSprite->runAction(CCRepeatForever::create(pAnimate));
	*/
}

void CStuffTip::onExit()
{
	BaseLayer::onExit();
}


CStuffTip::~CStuffTip()
{
}

void CStuffTip::showItemProperty(CItem *item)
{
	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);

	CCSprite *mask= (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(item->quality));

	CLabel *name= (CLabel *)m_ui->findWidgetById("name");
	if(itemData)
	{
		name->setString(itemData->itemName.c_str());
	}
	

	CButton *jump = (CButton *)m_ui->findWidgetById("jump");
	jump->setUserData(item);
	jump->setOnClickListener(this,ccw_click_selector(CStuffTip::onJump));

	//
	CLabel *haveNum= (CLabel *)m_ui->findWidgetById("haveNum");
	haveNum->setString(ToString(item->itemNum));

	CCSprite *itemSpr = CImageView::create(GetImageName(1, item->quality, item->iconId).c_str());

	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CStuffTip::showItemProperty Lost Image = %d",item->iconId);
	}
	bg->removeChildByTag(1);
	itemSpr->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	itemSpr->setTag(1);
	bg->addChild(itemSpr);

	
	if (item->itemType!=2)
	{
		CLabel *desc = (CLabel*)m_ui->findWidgetById("desc");
		CLabel *desc1 = (CLabel*)m_ui->findWidgetById("desc1");
		if (itemData)
		{
			if(itemData->itemDesc.size()>0)
			desc->setString(itemData->itemDesc.c_str());
			if(itemData->itemDesc1.size()>0)
			desc1->setString(itemData->itemDesc1.c_str());
		}
		//else
		//{
		//	desc->setString("");
		//	desc1->setString("");
		//}
	}

	if (item->itemType)
	{
	}
}

void CStuffTip::DoNotPopItem()
{
	this->getChildByTag(100)->setVisible(false);
//	m_ui->setPosition(ccp(VCENTER.x+257,VCENTER.y));
	m_isOutClose = true;
}

void CStuffTip::onJump(CCObject* pSender)
{
	CItem *item = (CItem*)((CButton*)pSender)->getUserData();
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

bool CStuffTip::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res && !m_isOutClose)
	{
		CImageViewScale9 *bgView = (CImageViewScale9*)(m_ui->findWidgetById("bg"));
		if( bgView->boundingBox().containsPoint(pTouch->getLocation()))
		{
			res = true;
		}
		else
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
	return res;
}
