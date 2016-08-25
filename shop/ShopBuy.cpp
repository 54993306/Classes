#include "ShopBuy.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"
#include "Battle/AnimationManager.h"

#include "common/CGameSound.h"

CShopBuy::CShopBuy()
	:m_buyNum(1),m_maxNum(1)
{

}
using namespace BattleSpace	;

bool CShopBuy::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("ShopBuyMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		this->setVisible(true);
		return true;
	}
	return false;
}

void CShopBuy::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CShopBuy::onEnter()
{
	BaseLayer::onEnter();

	//加特效
	{
		CCAnimation *batAnim = AnimationManager::sharedAction()->getAnimation("8050");//9016
		batAnim->setDelayPerUnit(0.05f);
		CCSprite *batLight = createAnimationSprite("skill/8050.png", ccp(1138/2, 640/2), batAnim, true);
		batLight->setScale(2.0f);
		m_ui->addChild(batLight, -1);

		PlayEffectSound(SFX_432);
	}

	{
		CCAnimation *batAnim = AnimationManager::sharedAction()->getAnimation("9017");
		batAnim->setDelayPerUnit(0.05f);
		CCSprite *batLight = createAnimationSprite("skill/9017.png", ccp(1138/2, 605), batAnim, true);
		m_ui->addChild(batLight, 99);
	}

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CShopBuy::roleUpdate));

}

void CShopBuy::onExit()
{
	BaseLayer::onExit();

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}


CShopBuy::~CShopBuy()
{
}

void CShopBuy::showItemProperty(CItem *item)
{
	m_item = *item;

	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);

	CCSprite *mask= (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(item->quality));

	CLabel *name= (CLabel *)m_ui->findWidgetById("name");

	if(itemData)
	{
		name->setString(itemData->itemName.c_str());
	}
	
// 
	CLabel *itemnum= (CLabel *)m_ui->findWidgetById("buyNum");
	itemnum->setString(ToString(item->itemNum));

	CLabel *havenum= (CLabel *)m_ui->findWidgetById("haveNum");
	havenum->setString(ToString(item->itparam));

	CLabel *price= (CLabel *)m_ui->findWidgetById("price");
	price->setString(ToString(item->buyPrice*item->itemNum));

	CButton *buy = (CButton *)m_ui->findWidgetById("buy");
	buy->setUserData(item);
	buy->setOnClickListener(this,ccw_click_selector(CShopBuy::onBuy));

	CCSprite *itemSpr =getItemSprite(item);
	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CShopBuy::showItemProperty Lost Image = %d",item->iconId);
	}
	if (itemSpr)
	{	
		itemSpr->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
		bg->addChild(itemSpr);
	}

	CCSprite *coinIcon = (CCSprite*)(m_ui->findWidgetById("coin"));
	switch (item->moneyType)
	{
	case 1:
		break;
	case 2:
		{  
			CCTexture2D *texture =  CCTextureCache::sharedTextureCache()->addImage("mainCity/icon_11.png");
			coinIcon->setTexture(texture);
			coinIcon->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));

			//检查货币数量
			if(!CheckCoin(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	case 3:
		{
			CCTexture2D *texture =  CCTextureCache::sharedTextureCache()->addImage("public/xinxing.png");
			coinIcon->setTexture(texture);
			coinIcon->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));

			//检查货币数量
			if(!CheckGold(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	case 4:
		{
			CCTexture2D *texture =  CCTextureCache::sharedTextureCache()->addImage("worldBoss/icon_ghost.png");
			coinIcon->setTexture(texture);
			coinIcon->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
			//检查货币数量
			if(!CheckGhost(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	case 5:
		{
			CCTexture2D *texture =  CCTextureCache::sharedTextureCache()->addImage("prop/5.png");
			coinIcon->setTexture(texture);
			coinIcon->setTextureRect(CCRectMake(0,0,texture->getContentSize().width,texture->getContentSize().height));
			//检查货币数量
			if(!CheckRolePoint(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	default:
		break;
	}
	
	if (item->itemType==5)
	{
		m_itemLay->setVisible(false);
		m_armorLay->setVisible(true);

		updateArmorAttr(item);
		CLabel *desc = (CLabel*)m_armorLay->findWidgetById("desc3");
		if (itemData)
		{
			desc->setString(itemData->itemDesc1.c_str());
		}
		else
		{
			desc->setString("");
		}
	}
	else
	{
		m_itemLay->setVisible(true);
		m_armorLay->setVisible(false);
		CLabel *desc1 = (CLabel*)m_itemLay->findWidgetById("desc1");
		CLabel *desc2 = (CLabel*)m_itemLay->findWidgetById("desc2");

		if (itemData)
		{
			desc1->setString(itemData->itemDesc.c_str());
			desc2->setString(itemData->itemDesc1.c_str());
		}
		else
		{
			desc1->setString("");
			desc2->setString("");
		}
	}

}

void CShopBuy::onBuy(CCObject* pSender)
{
	CItem *item = (CItem*)((CButton*)pSender)->getUserData();

	switch (item->moneyType)
	{
	case 1://元宝
		{
			if(!CheckGold(item->buyPrice))
			{
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
				return;
			}
		}
		break;
	case 2://金币
		{
			if(!CheckCoin(item->buyPrice))
			{
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceCoin);
				return;
			}
		}
		break;
	case 3://友情点
		{
			ShowPopTextTip(GETLANGSTR(1016));
		}
		break;
	case 4://勋章
		{
			if (DataCenter::sharedData()->getUser()->getUserData()->getRoleFood()<item->buyPrice)
			{			
				ShowPopTextTip(GETLANGSTR(1224));
				return;
			}
		}
		break;
	case 5:
		{
			if (DataCenter::sharedData()->getUser()->getUserData()->getRolePoints()<item->buyPrice)
			{			
				ShowPopTextTip(GETLANGSTR(2046));
				return;
			}
		}
		break;
	default:
		break;
	}

	if (m_buyType==3)
	{
		if (m_buyNum>0)
		{	
			CMainCityControl::getInstance()->sendShopBuyItem(item->itemId,m_buyNum,m_buyType);
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
		else
		{
			ShowPopTextTip("stone not enough ");
		}
	}
	else
	{
	   CMainCityControl::getInstance()->sendShopBuyItem(item->itemId,item->itemNum,m_buyType);
	   LayerManager::instance()->pop();
	   LayerManager::instance()->pop();
	}

	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8050");
	CCSprite *light = CCSprite::create("skill/8050.png");
	light->setPosition(VCENTER);
	light->setScale(2.2f);
	light->runAction(CCSequence::createWithTwoActions(CCAnimate::create(lightAnim)->reverse(),CCRemoveSelf::create()));
	CSceneManager::sharedSceneManager()->getCurrScene()->addChild(light);
}

bool CShopBuy::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	if( !res )
	{
		CImageViewScale9 *bgView = (CImageViewScale9*)(m_ui->findWidgetById("bg"));

		CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

		if( bgView->boundingBox().containsPoint(pTouchPos))
		{
			res = true;
		}
		else
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}
	return true;
}

void CShopBuy::updateArmorAttr(CItem * item)
{
	CLabel *hp = (CLabel*)(m_ui->findWidgetById("hp"));
	hp->setString(ToString(item->armor.baseHp));

	CLabel *attack = (CLabel*)(m_ui->findWidgetById("attack"));
	attack->setString(ToString(item->armor.baseAtk));

	CLabel *deffend = (CLabel*)(m_ui->findWidgetById("defend"));
	deffend->setString(CCString::createWithFormat("%d%%",item->armor.baseDef)->getCString());

	CLabel *crit = (CLabel*)(m_ui->findWidgetById("crit"));
	crit->setString(ToString(item->armor.baseCrit));

	CLabel *hit = (CLabel*)(m_ui->findWidgetById("hit"));
	hit->setString(ToString(item->armor.baseHit));

	CLabel *dodge = (CLabel*)(m_ui->findWidgetById("dodge"));
	dodge->setString(ToString(item->armor.baseDodge));

	CLabel *firstAtk = (CLabel*)(m_ui->findWidgetById("firstAtk"));
	firstAtk->setString(CCString::createWithFormat("%d%%",item->armor.baseDex)->getCString());

	CLabel *addhp = (CLabel*)(m_ui->findWidgetById("addhp"));

	CLabel *addatk = (CLabel*)(m_ui->findWidgetById("addatk"));

	CLabel *adddef = (CLabel*)(m_ui->findWidgetById("adddef"));

	CLabel *addcrit = (CLabel*)(m_ui->findWidgetById("addcrit"));

	CLabel *addhit = (CLabel*)(m_ui->findWidgetById("addhit"));

	CLabel *adddodge = (CLabel*)(m_ui->findWidgetById("adddodge"));

	CLabel *dex = (CLabel*)(m_ui->findWidgetById("addact"));

	if (item->armor.addHp>0)
	{
		addhp->setString(CCString::createWithFormat("+%d",item->armor.addHp)->getCString());
		addhp->setPositionX(hp->getPositionX()+hp->getContentSize().width);
	}
	else
	{
		addhp->setString("");
	}

	if (item->armor.addDef>0)
	{
		adddef->setString(CCString::createWithFormat("+%d%%",item->armor.addDef)->getCString());
		adddef->setPositionX(deffend->getPositionX()+deffend->getContentSize().width);
	}
	else
	{
		adddef->setString("");
	}

	if (item->armor.addAtk>0)
	{
		addatk->setString(CCString::createWithFormat("+%d",item->armor.addAtk)->getCString());
		addatk->setPositionX(attack->getPositionX()+attack->getContentSize().width);
	}
	else
	{
		addatk->setString("");
	}

	// 		if (item->armor.addHit>0)
	// 		{
	// 			addhit->setString(CCString::createWithFormat("+%d",item->armor.addHit)->getCString());
	// 			addhit->setPositionX(hit->getPositionX()+hit->getContentSize().width);
	// 		}
	// 		else
	{
		addhit->setString("");
	}

	// 		if (item->armor.addCrit>0)
	// 		{
	// 			addcrit->setString(CCString::createWithFormat("+%d",item->armor.addCrit)->getCString());
	// 			addcrit->setPositionX(crit->getPositionX()+crit->getContentSize().width);
	// 		}
	// 		else
	{
		addcrit->setString("");
	}

	// 		if (item->armor.addDodge>0)
	// 		{
	// 			adddodge->setString(CCString::createWithFormat("+%d",item->armor.addDodge)->getCString());
	// 			adddodge->setPositionX(dodge->getPositionX()+dodge->getContentSize().width);
	// 		}
	// 		else
	{
		adddodge->setString("");
	}

	// 		if (item->armor.addDex>0)
	// 		{
	// 			dex->setString(CCString::createWithFormat("+%d",item->armor.addDex)->getCString());
	// 			dex->setPositionX(firstAtk->getPositionX()+firstAtk->getContentSize().width);
	// 		}
	// 		else
	{
		dex->setString("");
	}
}

void CShopBuy::setShopType(int type, int maxNum/*=0*/)
{
	m_buyType = type;

	if (type<4)
	{
		m_ui = LoadComponent("ShopBuy.xaml"); 
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
	}
	else
	{
		m_ui = LoadComponent("PvpShopBuy.xaml"); 
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
	}
	
	m_armorLay =(CLayout *)m_ui->findWidgetById("armor");
	m_itemLay =(CLayout *)m_ui->findWidgetById("item");

	CLayout *sliderLay = (CLayout*)(m_ui->findWidgetById("blid"));
	CCNode *bg =m_ui->findWidgetById("bg");
	CCNode *bg1 =m_ui->findWidgetById("bg1");

	if (type==3||type==4)
	{	
		bg->setVisible(type==3);
		bg1->setVisible(type==4);
		CSlider *slider = (CSlider*)(m_ui->findWidgetById("slider"));
		sliderLay->setVisible(true);
		slider->setOnValueChangedListener(this,ccw_valuechanged_selector(CShopBuy::sliderChange));
		slider->setMaxValue(maxNum);
		m_maxNum = maxNum;
		CImageView *sub = (CImageView*)(m_ui->findWidgetById("sub"));
		sub->setTouchEnabled(true);
		sub->setOnClickListener(this,ccw_click_selector(CShopBuy::subNum));
		CImageView *add = (CImageView*)(m_ui->findWidgetById("add"));
		add->setTouchEnabled(true);
		add->setOnClickListener(this,ccw_click_selector(CShopBuy::addNum));
		if (maxNum==0)
		{
			//sliderLay->setVisible(false);
			slider->setDragable(false);
			slider->setMaxValue(1);
			slider->setMinValue(1);
		}
		else
		{
			slider->setMinValue(1);
		}
	}
	else
	{
		sliderLay->setVisible(false);
	}
}

void CShopBuy::sliderChange(CCObject* pSender, int nValue)
{
	m_buyNum = nValue;
	CLabel *buyNum = (CLabel*)(m_ui->findWidgetById("buyNum"));
	buyNum->setString(ToString(m_buyNum));

	CLabel *price =(CLabel*)(m_ui->findWidgetById("price"));
	price->setString(ToString(m_buyNum*m_item.buyPrice));
}

void CShopBuy::addNum(CCObject* pSender)
{
	if (m_buyNum<m_maxNum)
	{
		m_buyNum++;
		sliderChange(nullptr,m_buyNum);
	}
}

void CShopBuy::subNum(CCObject* pSender)
{
	if (m_buyNum>1)
	{
		m_buyNum--;
		sliderChange(nullptr,m_buyNum);
	}
}

void CShopBuy::roleUpdate( const TMessage& tMsg )
{
	CLabel *price= (CLabel *)m_ui->findWidgetById("price");

	switch (m_item.moneyType)
	{
	case 1:
		break;
	case 2:
		{  
			//检查货币数量
			if(!CheckCoin(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	case 3:
		{
			//检查货币数量
			if(!CheckGold(atoi(price->getString())))
			{
				price->setColor(RGB_RED);
			}
			else
			{
				price->setColor(RGB_WHITE);
			}
		}
		break;
	default:
		break;
	}
}


