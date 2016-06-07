#include "ShopLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "ShopBuy.h"
#include "scene/CPopTip.h"
#include "scene/AnimationManager.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"

#include "common/CGameSound.h"
#include "Resources.h"


CShopLayer::CShopLayer():
	m_buyType(1)
	,m_selectItem(nullptr)
	,m_updateGold(0)
{

}

using namespace BattleSpace;
bool CShopLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("shopmask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("shop.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CShopLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
}

void CShopLayer::onUpdateItem(CCObject* pSender)
{
	CCString *str = CCString::createWithFormat(GETLANGSTR(26),m_updateGold);
	ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(CShopLayer::onComfirmUpdate));
}

void CShopLayer::onComfirmUpdate(CCObject* pSender)
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


void CShopLayer::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	m_pageView = (CGridPageView*)m_ui->findWidgetById("scroll");
	m_pageView->setDirection(eScrollViewDirectionVertical);	
	m_pageView->setSizeOfCell(ccp(m_pageView->getContentSize().width/3,m_pageView->getContentSize().height/2));
	m_pageView->setSizeOfCell(CCSizeMake(227,215));
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setRows(2);
	m_pageView->setColumns(3);
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
// 	m_pageView->setBounceable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CShopLayer::gridviewDataSource));
	m_pageView->setAutoRelocateSpeed(500.0f);
	m_pageView->setBounceable(false);
	m_pageView->reloadData();

	// 	pageView->removeFromParent();


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("close");
	pClose->setOnClickListener(this,ccw_click_selector(CShopLayer::onClose));
	m_ui->addChild(pClose, 999);

	CButton *update = (CButton *)m_ui->findWidgetById("update");
	update->setOnClickListener(this,ccw_click_selector(CShopLayer::onUpdateItem));

	GetTcpNet->registerMsgHandler(Shop_Msg,this,CMsgHandler_selector(CShopLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(Shop_Buy,this,CMsgHandler_selector(CShopLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(UpdateShopMsg,this,CMsgHandler_selector(CShopLayer::ProcessMsg));
	this->setVisible(false);

	if (m_buyType==2)
	{
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("skill/shangchengbaobao0.png","skill/shangchengbaobao0.plist","skill/shangchengbaobao.ExportJson");
		m_armature = CCArmature::create("shangchengbaobao");
		m_armature->getAnimation()->play("xunhuan",-1,-1,1);
		m_armature->setPosition(ccp(100,100));
		m_armature->setScaleX(-1.2f);
		m_armature->setScaleY(1.2f);
		m_ui->addChild(m_armature);
	}

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CShopLayer::roleUpdate));

}

CCObject* CShopLayer::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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



void CShopLayer::onSelectItem(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
}

void CShopLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;

	int selIndex = 2*btn->getTag();
	
	if (bChecked)
	{
		showSelectRadioImg(selIndex);

		CMainCityControl::getInstance()->sendShopRequest(btn->getTag());

		m_buyType = btn->getTag();

		switch (btn->getTag())
		{
		case 1:
			{

			}
			break;
		case 2:
			{
				
			}
			break;
		default:
			break;
		}
	}
}

void CShopLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	BaseLayer::onExit();
	//NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
}


void CShopLayer::updateItemList(vector<CItem>& itemList)
{
	m_pageView->setCountOfCell(itemList.size());
	m_pageView->reloadData();
}

void CShopLayer::ProcessMsg(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);
		//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
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
				onClose(nullptr);
				ShowPopTextTip(GETLANGSTR(284));
				return;
			}

			updateItemList(m_itemList);
			if (res->has_updatetime())
			{				
				CLabel *timelab = (CLabel*)m_ui->findWidgetById("time");
				
				
				if (m_buyType==1)
				{	
					tm tim;
					time_t  t = res->updatetime()/1000;
					tim = *localtime(&t);			
					if (tim.tm_min>9)
					{					
						timelab->setString(CCString::createWithFormat("%d:%d",tim.tm_hour,tim.tm_min)->getCString());
					}
					else
					{
						timelab->setString(CCString::createWithFormat("%d:0%d",tim.tm_hour,tim.tm_min)->getCString());
					}
				}
				else
				{
// #ifdef _WIN32
// 						
// 					time_t tnow = time(nullptr);
// 					t = t - tnow;
// #else
// 					time_t t =  res->updatetime();
// 					struct cc_timeval now;  
// 					CCTime::gettimeofdayCocos2d(&now, NULL);  
// 					time_t tnow = now.tv_sec*1000 + now.tv_usec/1000;
// 					t = (t - tnow)/1000;
// #endif
					time_t t  = res->updatetime()/1000;		
					int  hour = floor(t / 3600);
					m_min = floor((t - hour * 3600) / 60);
					int sec = floor(t - hour * 3600 - m_min * 60);
					timelab->setString(CCString::createWithFormat("%d%s", m_min, GETLANGSTR(1013))->getCString());
					this->schedule(schedule_selector(CShopLayer::updateTime),60.0f);
				}
			}
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
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
				}

				//大于1000表示售完
				m_selectItem->itemNum += 1000;
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

void CShopLayer::updateTime(float dt)
{
	m_min--;
	if (m_min<=0)
	{
		this->unschedule(schedule_selector(CShopLayer::updateTime));
		return;
	}
	CLabel *timelab = (CLabel*)m_ui->findWidgetById("time");
	timelab->setString(CCString::createWithFormat("%d%s", m_min, GETLANGSTR(1013))->getCString());
}

void CShopLayer::onBuyItem(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CImageView *btn = (CImageView*)pSender;
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	m_selectItem = (CItem*)btn->getUserData();
	CShopBuy *shopBuy = CShopBuy::create();	
	shopBuy->setShopType(m_buyType);
	shopBuy->showItemProperty(m_selectItem);
	LayerManager::instance()->push(shopBuy);
}

void CShopLayer::showSelectRadioImg(int selIndex)
{
	for (int i=1; i<=6; ++i)
	{
		CCNode *spr = nullptr;
		if (i%2==0)
		{
			spr= (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(true);
		}
		else
		{
			spr = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("switch%d",i)->getCString());
			spr->setVisible(false);
		}
		if (selIndex ==i)
		{
			spr->setVisible(false);
		}
		else if (selIndex==i+1)
		{
			spr->setVisible(true);
		}
	}
}

void CShopLayer::leaveTime(struct tm &lastTm, struct tm &timem, struct tm &now)
{
	lastTm.tm_year = timem.tm_year - now.tm_year;
	lastTm.tm_mon = timem.tm_mon - now.tm_mon;
	lastTm.tm_mday = timem.tm_mday - now.tm_mday;
	lastTm.tm_hour = timem.tm_hour - now.tm_hour;
	lastTm.tm_min = timem.tm_min - now.tm_min;
	lastTm.tm_sec = timem.tm_sec - now.tm_sec;
}

CShopLayer::~CShopLayer()
{
	CC_SAFE_RELEASE(m_cell);
}

void CShopLayer::addCell(CGridPageViewCell* pCell, unsigned int uIdx)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	CItem &item = m_itemList[uIdx];
	for (int i=0; i<8; ++i)
	{
		CCNode *node = (CCNode*)lay->getChildByTag(i+1);  //(CCNode*)lay->getChildren()->objectAtIndex(i);
		lay->removeChild(node);
		pCell->addChild(node);
		switch (i)
		{
		case 1:
			{
				if (item.itemNum>=1000&&m_selectItem&&m_selectItem->itemId == item.itemId)
				{	
					item.itemNum = 0;//限定只播一次，不然数据更新，reload的时候，这里还会播动画
					CCAnimation *fireAnim1= AnimationManager::sharedAction()->getAnimation("7031");
					fireAnim1->setDelayPerUnit(0.85f);
					CCPoint pos1 = ccp(pCell->getContentSize().width/2-5,pCell->getContentSize().height/2+8);
					CCSprite *fire = CCSprite::create("skill/7031.png");
					fire->setPosition(pos1);
					fire->runAction(CCSequence::createWithTwoActions(CCAnimate::create(fireAnim1),CCRemoveSelf::create()));
					fire->setTag(7031);
					pCell->addChild(fire,5);
				}
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
				else
				{
     				node->setVisible(true);
				}
			}
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
	buy->setOnClickListener(this,ccw_click_selector(CShopLayer::onBuyItem));

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item.itemId);

	CLabel *name = (CLabel*)pCell->findWidgetById("name");
	std::string strName = "XXX";
	if(itemData)
	{
		 strName = itemData->itemName;
	}
	
	CLabel *price = (CLabel*)pCell->findWidgetById("price");
	if (item.itemNum>=1000)
	{
		CCString *str = CCString::createWithFormat(U8("× %d"),item.itemNum-1000);
		name->setString(strName.append(str->getCString()).c_str());
		price->setString(ToString(item.buyPrice*(item.itemNum-1000)));
	}
	else
	{
		CCString *str = CCString::createWithFormat(U8("× %d"),item.itemNum);
		name->setString(strName.append(str->getCString()).c_str());
		price->setString(ToString(item.buyPrice*item.itemNum));
	}
	
	CCSprite *mask = (CCSprite*)(pCell->findWidgetById("mask"));
	mask->setTexture(setItemQualityTexture(item.quality));
	addItemLight(item.quality,mask,ccp(0,-2));

	CCSprite *itemSpr = getItemSprite(&item);
	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CShopLayer::addCell Lost Image = %d",item.iconId);
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
		}
		break;
	
	default:
		break;
	}
}

void CShopLayer::setShopType(int type)
{
	m_buyType = type;
	CCSprite *bg = (CCSprite*)(m_ui->findWidgetById("norbg"));		
	CCSprite *bg1 = (CCSprite*)(m_ui->findWidgetById("redbg"));

	if (type==1)
	{
		bg->setVisible(true);
		bg1->setVisible(false);
	}
	else if (type==2)
	{
		bg->setVisible(false);
		bg1->setVisible(true);
	}
}

void CShopLayer::roleUpdate( const TMessage& tMsg )
{
	m_pageView->reloadData();
}
