#include "WShopLayer.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/ShowTexttip.h"

#include "shop/ShopBuy.h"
#include "scene/CPopTip.h"
#include "Battle/AnimationManager.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"
#include "tools/UICloneMgr.h"
#include "common/color.h"
#include "mainCity/MainCityControl.h"


CWShopLayer::CWShopLayer():
m_ui(nullptr),m_selectItem(nullptr),m_buyType(3)
{

}

CWShopLayer::~CWShopLayer()
{

}

bool CWShopLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		////黑底
		//MaskLayer* pMaskLayer = MaskLayer::create("WorldBossMaskLayer");
		//pMaskLayer->setContentSize(winSize);
		//LayerManager::instance()->push(pMaskLayer);

		//内容
		setVisible(true);

		setIsShowBlack(false);

		return true;
	}

	return false;
}

void CWShopLayer::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_pageView = (CGridPageView*)m_ui->findWidgetById("scroll");
	m_pageView->setDirection(eScrollViewDirectionVertical);	
	m_pageView->setSizeOfCell(m_cell->getContentSize());
// 	m_pageView->setSizeOfCell(ccp(m_pageView->getContentSize().width/3,m_pageView->getContentSize().height/2));
//	m_pageView->setSizeOfCell(CCSizeMake(227,215));
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setRows(2);
	m_pageView->setColumns(3);
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CWShopLayer::gridviewDataSource));
	m_pageView->setAutoRelocateSpeed(500.0f);
	m_pageView->reloadData();

	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	CLabel *ghost = (CLabel *)(m_ui->findWidgetById("ghost"));
	ghost->setString(ToString(user->getRoleFood()));

	////绑定活动列表回调
//	GetTcpNet->registerMsgHandler(ActListMsg, this, CMsgHandler_selector(CWShopLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(Shop_Msg,this,CMsgHandler_selector(CWShopLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(Shop_Buy,this,CMsgHandler_selector(CWShopLayer::processNetMsg));
	GetTcpNet->registerMsgHandler(UpdateShopMsg,this,CMsgHandler_selector(CWShopLayer::processNetMsg));
}

void CWShopLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);


	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	if(isVisible())
	{
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);

}

void CWShopLayer::onComfirmUpdate(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;

	if (btn->getTag()==PopTipConfirm)
	{
		if(!CheckGold(m_updateGold))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}
		CMainCityControl::getInstance()->sendShopUpdate(m_buyType);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CWShopLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	
	if(!isVisible())
	{
		this->setVisible(true);
	}
	
	switch (type)
	{
		//商品列表
	case Shop_Msg:
		{
			ShopResponse *res = (ShopResponse*)msg;
			m_itemList.clear();
			m_updateGold = res->updategold();

			for (int i = 0; i < res->itemlist_size(); i++)
			{
				CItem item;
				item.read(res->itemlist(i));
				m_itemList.push_back(item);
			}

			if (m_itemList.size()>0)
			{
				this->setVisible(true);
			}
			else
			{
			//	onClose(nullptr);
				ShowPopTextTip(GETLANGSTR(284));
				return;
			}

			updateItemList(m_itemList);

		}
		break;

	case UpdateShopMsg:
		{
			UpdateShopRes *res = (UpdateShopRes*)msg;
			switch (res->result())
			{
			case 1:
				{
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					user->setRoleGold(user->getRoleGold() - m_updateGold);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					m_itemList.clear();
					m_updateGold = res->updategold();
					m_selectItem = nullptr;
					for (int i = 0; i < res->itemlist_size(); i++)
					{
						CItem item;
						item.read(res->itemlist(i));
						m_itemList.push_back(item);
					}
					updateItemList(m_itemList);
				}
				break;
			case 2:
				ShowPopTextTip(GETLANGSTR(194));
				break;
			case 3:
				ShowPopTextTip(GETLANGSTR(170));		
				break;
			default:
				break;
			}

		}
		break;

	case Shop_Buy:
		{
			BuyItemResponse *res = (BuyItemResponse*)msg;
			//res =1成功，2钱不够，3道具已售完，4数据错误，5材料不足，6背包空间不足
			if (res->result()==1)
			{
				if (res->has_role())
				{
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					user->read(res->role());
					CLabel *ghost = (CLabel *)(m_ui->findWidgetById("ghost"));
					ghost->setString(ToString(user->getRoleFood()));
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
				}

				//大于1000表示售完
			//	m_selectItem->itemNum += 1000;
				m_pageView->reloadData();
			}
			else if (res->result()==2)
			{
				switch (m_selectItem->moneyType)
				{
				case 1://元宝
					{
						//ShowPopTextTip(GETLANGSTR(203));
						CShowToBuyResource* pShow = CShowToBuyResource::create();
						pShow->showToBuyResourceByType(ShowBuyResourceGold);
					}
					break;
				case 2://金币
					{
						//ShowPopTextTip(GETLANGSTR(205));
						CShowToBuyResource* pShow = CShowToBuyResource::create();
						pShow->showToBuyResourceByType(ShowBuyResourceCoin);
					}
					break;
				case 3://友情点
					{
						ShowPopTextTip(GETLANGSTR(1016));
					}
					break;
				case 4://勋章
					{
						ShowPopTextTip(GETLANGSTR(240));
					}
					break;
				default:
					break;
				}
			}
			else if (res->result()==3)
			{
				ShowPopTextTip(GETLANGSTR(206));	
			}
			else if (res->result()==4)
			{
				ShowPopTextTip(GETLANGSTR(170));	
			}
			else if (res->result()==6)
			{
				ShowPopTextTip(GETLANGSTR(178));		
			}
		}
		break;
	default:
		break;
	}
}

CCObject* CWShopLayer::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridPageViewCell* pCell = (CGridPageViewCell*)pConvertCell;
	if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())
	{
		pCell = new CGridPageViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);

		addCell(pCell, uIdx);
	}
	else if (!pCell)
	{
		pCell = new CGridPageViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
	}
	else if (pCell)
	{
		pCell->removeAllChildren();

		if(uIdx>m_itemList.size())
		{
			return pCell;
		}
		addCell(pCell, uIdx);
	}
	return pCell;
}

void CWShopLayer::addCell(CGridPageViewCell* pCell, unsigned int uIdx)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	CItem &item = m_itemList[uIdx];
	item.itemNum=1;
	int count = 8;
	if (m_buyType==4)
	{
		count=9;
	}
	for (int i=0; i<count; ++i)
	{
		CCNode *node = (CCNode*)lay->getChildByTag(i+1);  //(CCNode*)lay->getChildren()->objectAtIndex(i);
		lay->removeChild(node);
		pCell->addChild(node);
		switch (i)
		{
		case 1:
			{

			}
			break;
		case 6:
		case 7:
			if (item.itemNum>=1000||item.itemNum==0)
			{
				if (m_selectItem&&m_selectItem->itemId == item.itemId)
				{				
					node->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.35f),CCShow::create()));
				}
			}
			node->setVisible(false);
			break;
		default:
			break;
		}

	}

	CImageView *buy = (CImageView*)(pCell->findWidgetById("bg"));
	buy->setUserData(&(m_itemList[uIdx]));
	if (item.itemNum >0&&item.itemNum<1000)
	{
		buy->setTouchEnabled(true);
	}
	buy->setOnClickListener(this,ccw_click_selector(CWShopLayer::onBuyItem));

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item.itemId);

	CLabel *name = (CLabel*)pCell->findWidgetById("name");
	std::string strName = "XXX";
	if(itemData)
	{
		strName = itemData->itemName;
	}

	CLabel *price = (CLabel*)pCell->findWidgetById("price");	
	name->setString(strName.c_str());
	price->setString(ToString(item.buyPrice));

	CCSprite *mask = (CCSprite*)(pCell->findWidgetById("mask"));
	mask->setTexture(setItemQualityTexture(item.quality));
	addItemLight(item.quality,mask,ccp(0,-2));

	CCSprite *itemSpr = getItemSprite(&item);
	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CWShopLayer::addCell Lost Image = %d",item.iconId);
	}
	if (itemSpr)
	{
		CCNode *mask =(CCNode*)(pCell->findWidgetById("con"));
		itemSpr->setPosition(ccp(mask->getContentSize().width/2, mask->getContentSize().height/2));
		mask->addChild(itemSpr);
	}

	CCSprite *coinIcon = (CCSprite*)(pCell->findWidgetById("coin"));
	coinIcon->setScale(0.7f);
	switch (item.moneyType)
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
	case 5:
		{
			CCTexture2D *texture =  CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("prop/%d.png", item.moneyType==4?2:5)->getCString());
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
	default:
		break;
	}
}

void CWShopLayer::updateItemList(vector<CItem>& itemList)
{
	m_pageView->setCountOfCell(itemList.size());
	m_pageView->reloadData();
}

void CWShopLayer::onBuyItem(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CImageView *btn = (CImageView*)pSender;
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	m_selectItem = (CItem*)btn->getUserData();
	CShopBuy *shopBuy = CShopBuy::create();	
	int num =0;
	if (m_selectItem->buyPrice>0)
	{
		if (m_buyType==3)
		{	
			num = data->getRoleFood()/m_selectItem->buyPrice;
		}
		else if (m_buyType==4)
		{
			num = data->getRolePoints()/m_selectItem->buyPrice;
		}
	}
	shopBuy->setShopType(m_buyType,num);
	shopBuy->showItemProperty(m_selectItem);
	LayerManager::instance()->push(shopBuy);
}

void CWShopLayer::loadUiByType(int type)
{
	m_buyType = type;
	if (type==3)
	{
		m_ui = LoadComponent("wShop.xaml");
	}
	else if (type == 4)
	{
		m_ui = LoadComponent("PvpShop.xaml");
		CImageView *gold = (CImageView*)m_ui->findWidgetById("gold");
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("prop/5.png");
		gold->setTexture(texture);
		gold->setTextureRect(CCRectMake(0,0,texture->getPixelsWide(),texture->getPixelsHigh()));
	}	
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui);
}
