#include "StarUpItem.h"
#include "CityData.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "MainCityControl.h"
#include "model/DataCenter.h"

#include "army/SelectEquipment.h"
#include "GMessage.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"

#include "StrengthenItem.h"
#include "scene/CPopTip.h"
#include "bag/BagInfoTip.h"

#include "common/color.h"
#include "common/CheckMoney.h"
#include "Battle/AnimationManager.h"
#include "Resources.h"
#include "tools/CCShake.h"
#include "common/CGameSound.h"
#include "scene/layer/PropGetInfo.h"
#include "common/CommonFunction.h"
#include "common/CShowToBuyResource.h"
#include "tools/UICloneMgr.h"
#include "Strengthen.h"
#include "common/CCRollLabelAction.h"
#include "LevelUpItem.h"


void CRisingResponse::read( const RisingResponse& res )
{
	this->result = res.result();

	if (res.has_armor())
	{
		this->armor.read(res.armor());
	}
	if (res.has_item1())
	{
		this->item1.read(res.item1());
	}
	else
	{
		CItem it;
		this->item1 = it;
	}

	if (res.has_item2())
	{
		this->item2.read(res.item2());
	}
	else
	{
		CItem it;
		this->item2 = it;
	}

	if (res.has_item3())
	{
		this->item3.read(res.item3());
	}
	else
	{
		CItem it;
		this->item3 = it;
	}

	this->rate = res.rate();
	this->gold = res.gold();
	this->bStarUp = res.canrising();
}

using namespace BattleSpace;

CStarUpItem::CStarUpItem():m_strenLeft(nullptr),m_itemAttr(nullptr)
	,m_isHpAdding(false),m_isAtkAdding(false),m_isDefAdding(false),m_isUseGold(false)
	,m_pCellInfo(nullptr)
{
	for(unsigned int i=0; i<4; i++)
	{
		m_pBaseInfo[i] = nullptr;
	}
}


CStarUpItem::~CStarUpItem()
{
	CC_SAFE_RELEASE(m_pCellInfo);
}



bool CStarUpItem::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("StarUpItem.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		m_strenLeft = (CLayout*)(m_ui->findWidgetById("left"));
		m_itemAttr = (CLayout*)(m_ui->findWidgetById("attr"));

		this->setVisible(true);

		//获取cell
		m_pCellInfo = (CLayout*)m_ui->findWidgetById("cell_info");
		CC_SAFE_RETAIN(m_pCellInfo);
		m_pCellInfo->removeFromParentAndCleanup(false);

		CCPoint pBasePos = ccp(1281, 130);
		//初始化基础属性四条
		initItemInfo(pBasePos, m_pBaseInfo, 4);

		//添加动画
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("strengthen/5002.json", "strengthen/5002.atlas", 1);
		pSkeletonAnimation->setPosition(ccp(887, -2));
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_ui->addChild(pSkeletonAnimation, 3);
		m_pSpineHero = pSkeletonAnimation;

		return true;
	}
	return false;
}

void CStarUpItem::onEnter()
{ 
	BaseLayer::onEnter();

	//普通提升
	CButton *strength = dynamic_cast<CButton*>(m_ui->findWidgetById("strengthen"));
	strength->setOnClickListener(this,ccw_click_selector(CStarUpItem::onStrengthen));

	//一键提升
	CButton *strength1 = dynamic_cast<CButton*>(m_ui->findWidgetById("strengthen1"));
	strength1->setOnClickListener(this,ccw_click_selector(CStarUpItem::onGoldStrengthen));

	//选择装备
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->setOnClickListener(this,ccw_click_selector(CStarUpItem::onSelectItem));

	//选择特殊素材
	CButton *item4 = dynamic_cast<CButton*>(m_ui->findWidgetById("item4"));
	item4->setOnClickListener(this,ccw_click_selector(CStarUpItem::onSelectWeird));

	//选择普通素材1
	CButton *item1 = dynamic_cast<CButton*>(m_ui->findWidgetById("item1"));
	item1->setOnClickListener(this,ccw_click_selector(CStarUpItem::onItemTip));
	item1->setEnabled(false);
	//选择普通素材2
	CButton *item2 = dynamic_cast<CButton*>(m_ui->findWidgetById("item2"));
	item2->setOnClickListener(this,ccw_click_selector(CStarUpItem::onItemTip));
	item2->setEnabled(false);
	//选择普通素材3
	CButton *item3 = dynamic_cast<CButton*>(m_ui->findWidgetById("item3"));
	item3->setOnClickListener(this,ccw_click_selector(CStarUpItem::onItemTip));
	item3->setEnabled(false);


	//旋转圈
	CImageView *circleFire = (CImageView*)(m_ui->findWidgetById("fire_circle"));
	circleFire->setScale(1.95f);
	circleFire->runAction(CCRepeatForever::create(CCRotateBy::create(1.0f, 60)));
	circleFire->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(1.2f, 2.1f), CCScaleTo::create(1.2f, 1.95f))));


	GetTcpNet->registerMsgHandler(RisingDataRequestMsg, this, CMsgHandler_selector(CStarUpItem::strengthenResponse));
	GetTcpNet->registerMsgHandler(RisingRequestMsg, this, CMsgHandler_selector(CStarUpItem::strengthenResponse));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_WEIRD,this, GameMsghandler_selector(CStarUpItem::updateWeird));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CStarUpItem::roleUpdate));

	NOTIFICATION->addObserver(this, callfuncO_selector(CStarUpItem::sameEquip), SAME_EQUIP_STARUP_LEVELUP, nullptr);
}


void CStarUpItem::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_WEIRD,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);

	NOTIFICATION->removeAllObservers(this);

}

void CStarUpItem::onStrengthen(CCObject* pSender)
{
	//是否最大星级
	if(isMaxStar())
	{
		ShowPopTextTip(GETLANGSTR(1199));
		return;
	}

	//是否可升星
	if(!m_strengthItem.bStarUp)
	{
		ShowPopTextTip(GETLANGSTR(213));
		return;
	}

	//检查货币
	if(!CheckCoin(m_strengthItem.gold))
	{
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceCoin);
		return;
	}

	m_isUseGold =false;

	int iWeird = m_weirdItem.id>0? m_weirdItem.id:0;
	CPlayerControl::getInstance().sendArmorStartUp(m_strengthItem.armor.id, iWeird, m_isUseGold);
}

void CStarUpItem::onGoldStrengthen(CCObject* pSender)
{
	if(isMaxStar())
	{
		ShowPopTextTip(GETLANGSTR(1199));
		return;
	}

	if(m_strengthItem.gold<=0)
	{
		ShowPopTextTip(CCString::createWithFormat(GETLANGSTR(2003), 8)->getCString());
		return;
	}

	//检查货币
	if(!CheckGold(m_strengthItem.gold))
	{
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceGold);
		return;
	}

	m_isUseGold = true;

	CCString *str = CCString::createWithFormat(GETLANGSTR(1207), m_strengthItem.gold);

	ShowConfirmTextTip(str->getCString(), this,ccw_click_selector(CStarUpItem::onConfirm));	
}

void CStarUpItem::onConfirm(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		CPlayerControl::getInstance().sendArmorStartUp(m_strengthItem.armor.id, 0, true);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}


void CStarUpItem::strengthenResponse(int type, google::protobuf::Message *msg)
{
	switch (type)
	{
	case RisingDataRequestMsg:
		{
			RisingResponse *res = (RisingResponse*)msg;

			//升星数据
			if (res->has_armor())
			{	
				m_strengthItem.read(*res);
				//刷新数据
				showItem();
				NOTIFICATION->postNotification(SAME_EQUIP_STARUP_LEVELUP, this);
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(1199));
				return;
			}

		}break;
	case RisingRequestMsg:
		{
			RisingResponse *res = (RisingResponse*)msg;

			//升星结果（1 成功, 2 失败， 3 素材不足，4 达到最大星级, 5 数据错误）

			switch (res->result())
			{
			case 1:
				{
					//减去消耗，更新
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					if (m_isUseGold)
					{
						user->setRoleGold(user->getRoleGold()-m_strengthItem.gold);
					}
					else
					{
						user->setCoin(user->getCoin()-m_strengthItem.gold);
					}
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					//升星数据
					if (res->has_armor())
					{	
						m_strengthItem.read(*res);
					}
					//最大星级
					else
					{
						CItem* item = &(m_strengthItem.armor);
						item->armor.baseHp = item->armor.nextHp;
						item->armor.baseDef = item->armor.nextDef;
						item->armor.baseAtk = item->armor.nextAtk;
						item->armor.baseHit = item->armor.nextHit;
						item->armor.baseDex = item->armor.nextDex;
						item->armor.baseCrit = item->armor.nextCrit;
						item->armor.baseRenew = item->armor.nextRenew;
						item->armor.baseDodge = item->armor.nextDodge;
						item->iStar++;
					}

					//发送ITEM更新广播
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_ITEM_DATA, 3, nullptr, &(m_strengthItem.armor));

					//升星成功，显示特效
					float fTime = showEffect(true);

					this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CStarUpItem::showItem))));
				}
				break;
			case 2:
				{
					//减去消耗，更新
					UserData *user = DataCenter::sharedData()->getUser()->getUserData();
					if (m_isUseGold)
					{
						user->setRoleGold(user->getRoleGold()-m_strengthItem.gold);
					}
					else
					{
						user->setCoin(user->getCoin()-m_strengthItem.gold);
					}
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					//升星数据
					if (res->has_armor())
					{	
						m_strengthItem.read(*res);
					}

					//显示失败效果
					showEffect(false);

					showItem();
				}
				break;
			case 3:
				{
					ShowPopTextTip(GETLANGSTR(213));
				}
				break;
			case 4:
				{
					ShowPopTextTip(GETLANGSTR(1199));		
				}
				break;
			case 5:
				{
					ShowPopTextTip(GETLANGSTR(170));	
				}
				break;
			default:
				break;
			}

		}break;
	default:
		break;
	}
}

void CStarUpItem::onSelectItem(CCObject* pSender)
{
	m_selectOPType = 1;
	CStrengthenItem *selectItem = CStrengthenItem::create();
	selectItem->setVisible(false);
	selectItem->setSelectType(1);
	selectItem->setAskType(CStrengthenItemTypeItemStarUp);
	selectItem->setOnSelectItemListener(this, ccw_select_Item(CStarUpItem::onSelectArmor));
	LayerManager::instance()->push(selectItem);
	CPlayerControl::getInstance().sendRoleBag(12, true,0);
}


void CStarUpItem::onSelectArmor( CItem* item )
{
	if(!item)
	{
		return;
	}

	//最大星级
	if(item->iStar == 5)
	{
		ShowPopTextTip(GETLANGSTR(1199));		
		return;
	}

	if(item)
	{
		CPlayerControl::getInstance().askForArmorStartUp(item->id);
	}
}



void CStarUpItem::onSelectWeird(CItem* item)
{

	if (item)
	{
		m_weirdItem = *item;

		CLabel* pRateAdd = dynamic_cast<CLabel*>(m_ui->findWidgetById("rate_label_add"));
		//"提高强化成功率%d%%
		pRateAdd->setString(CCString::createWithFormat("+%d%%",m_weirdItem.prop.AddVal)->getCString());

	}
	else
	{
		CLabel* pRateAdd = dynamic_cast<CLabel*>(m_ui->findWidgetById("rate_label_add"));
		pRateAdd->setString("");
	}
}

void CStarUpItem::showItem()
{
	CLabel *tip = dynamic_cast<CLabel*>(m_ui->findWidgetById("tip"));
	tip->setVisible(false);

	m_pSpineHero->setVisible(false);

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(true);

	CCSprite *why = (CCSprite*)(m_ui->findWidgetById("why"));
	why->setVisible(false);

	updateItemProperty();
	updateBaseAttr();
	updateStrengthenItem();
}

void CStarUpItem::updateItemProperty()
{
	//设置名字
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(m_strengthItem.armor.itemId);

	CLabel *name = dynamic_cast<CLabel*>(m_ui->findWidgetById("name"));
	CCString *str = CCString::create("XXX");
	if(itemData)
	{
		str = CCString::createWithFormat("%s", itemData->itemName.c_str());
		name->setString(str->getCString());
	}

	//设置成功率
	CLabel* pRate = dynamic_cast<CLabel*>(m_ui->findWidgetById("rate_label"));
	pRate->setString(CCString::createWithFormat("%d%%", m_strengthItem.rate)->getCString());

	CLabel* pRateAdd = dynamic_cast<CLabel*>(m_ui->findWidgetById("rate_label_add"));
	pRateAdd->setString("");
}

//选择符文
void CStarUpItem::onSelectWeird(CCObject *pSender)
{
	m_selectOPType = 2;
	
	CSelectEquipment *selectEqu = CSelectEquipment::create();
	LayerManager::instance()->push(selectEqu);
	selectEqu->setOnSelectItemListener(this,ccw_select_Item(CStarUpItem::onSelectWeird));

	CPlayerControl::getInstance().sendRoleBag(2);
}

void CStarUpItem::updateWeird(const TMessage& tMsg)
{
	m_weirdItem = *(CItem*)tMsg.lParam;

	delete tMsg.lParam;
}

void CStarUpItem::onItemTip(CCObject *pSender)
{ 
	CCNode* pNode = (CCNode*)pSender;
	CItem* item =  (CItem*)(pNode->getUserData());
	PropGetInfo* pLayer = PropGetInfo::create();
	LayerManager::instance()->push(pLayer);
	CPlayerControl::getInstance().sendItemInfo(item->itemId);
}

void CStarUpItem::onUserStone(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CStarUpItem::updateBaseAttr()
{
	CItem* item = &(m_strengthItem.armor);

	//名字对应的langID
	int iNameId[8] = {42, 44, 43, 49, 45, 46, 48, 47};

	//基础属性
	int iBaseValue[8] = 
	{
		item->armor.baseHp, item->armor.baseDef, 
		item->armor.baseAtk, item->armor.baseHit,
		item->armor.baseDex, item->armor.baseCrit, 
		item->armor.baseRenew, item->armor.baseDodge
	};
	//目标属性
	int iAimValue[8] = 
	{
		item->armor.nextHp, item->armor.nextDef, 
		item->armor.nextAtk,	item->armor.nextHit,
		item->armor.nextDex,	item->armor.nextCrit, 
		item->armor.nextRenew, item->armor.nextDodge
	};

	//更新数据
	updateInfoToUIAndReturnCount(m_pBaseInfo, iNameId, iBaseValue, iAimValue, 8, nullptr);


	//设置当前星星和目标星星
	int iStar = item->iStar;
	//添加星星
	m_ui->removeChildByTag(494);
	m_ui->removeChildByTag(497);
	CLayout* pStarLayout1 = getStarLayout(iStar, 0.8f);
	pStarLayout1->setPosition(ccp(793-iStar*9, 476));
	m_ui->addChild(pStarLayout1, 120, 494);
	
	if(!isMaxStar())
	{
		CLayout* pStarLayout2 = getStarLayout(iStar+1, 0.8f);
		pStarLayout2->setPosition(ccp(989-(iStar+1)*7, 476));
		m_ui->addChild(pStarLayout2, 120, 497);
	}
	CCNode* pNode = (CCNode*)m_ui->findWidgetById("star_unknow");
	pNode->setVisible(isMaxStar());
}

void CStarUpItem::updateStrengthenItem()
{
	//图标
	CCSprite *prop = CCSprite::create(CCString::createWithFormat("prop/%d.png",m_strengthItem.armor.iconId)->getCString());
	if (!prop)
	{
		prop = CImageView::create("prop/32003.png");
	}
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->removeChildByTag(1);
	prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
	prop->setTag(1);
	btn->addChild(prop, -1);

	//品质
	((CCSprite*)btn->getNormalImage())->setTexture(setItemQualityTexture(m_strengthItem.armor.quality));

	//添加星星层
	btn->removeChildByTag(CStarUpItemStarLayTag);
	CLayout* pStarLayout = getStarLayout(m_strengthItem.armor.iStar);
	btn->addChild(pStarLayout, CStarUpItemStarLayTag, CStarUpItemStarLayTag);

	//设置等级
	CLabel* pLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	CCNode* pLevelBg = dynamic_cast<CCNode*>(m_ui->findWidgetById("level_mask"));
	if(m_strengthItem.armor.itemLevel>0)
	{
		pLevel->setVisible(true);
		pLevelBg->setVisible(true);

		pLevel->setString(CCString::createWithFormat("+%d", m_strengthItem.armor.itemLevel)->getCString());
	}
	else
	{
		pLevel->setVisible(false);
		pLevelBg->setVisible(false);
	}


	//是否显示勾玉强化
	CButton *strength1 = dynamic_cast<CButton*>(m_ui->findWidgetById("strengthen1"));
	CCNode *strength1Text = dynamic_cast<CCNode*>(m_ui->findWidgetById("btn_text"));
	CLabel* pGold = dynamic_cast<CLabel*>(m_ui->findWidgetById("gold"));
	CCNode* pGoldIcon = dynamic_cast<CCNode*>(m_ui->findWidgetById("gold_icon"));
	if(m_strengthItem.gold<=0)
	{
		//strength1->setVisible(false);
		//strength1Text->setVisible(false);
		if(pGold->isVisible())
		{
			strength1Text->setPositionX(strength1Text->getPositionX()-35);
		}
		
		pGold->setVisible(false);
		pGoldIcon->setVisible(false);
	}
	else
	{
		strength1->setVisible(true);
		strength1Text->setVisible(true);
		pGold->setVisible(true);
		pGoldIcon->setVisible(true);

		pGold->setString(ToString(m_strengthItem.gold));
		checkMoney();
	}


	//强化需要的材料
	CButton *item1 = dynamic_cast<CButton*>(m_ui->findWidgetById("item1"));
	item1->removeChildByTag(1);

	CLabel *num1 = (CLabel*)(m_ui->findWidgetById("num1"));
	CLabel *num2= (CLabel*)(m_ui->findWidgetById("num2"));
	CLabel *num3 = (CLabel*)(m_ui->findWidgetById("num3"));

	const ItemData * itemData1 = DataCenter::sharedData()->getItemDesc()->getCfg(m_strengthItem.item1.itemId);
	const ItemData * itemData2 = DataCenter::sharedData()->getItemDesc()->getCfg(m_strengthItem.item2.itemId);
	const ItemData * itemData3 = DataCenter::sharedData()->getItemDesc()->getCfg(m_strengthItem.item3.itemId);

	if (m_strengthItem.item1.iconId>0)
	{
		((CImageView*)item1->getNormalImage())->setTexture(setItemQualityTexture(m_strengthItem.item1.quality));
		item1->setEnabled(true);
		item1->setUserData(&m_strengthItem.item1);
		CCSprite *prop1 = CCSprite::create(GetImageName(1, m_strengthItem.item1.quality, m_strengthItem.item1.iconId).c_str());
		if (!prop1)
		{
			prop1 = CImageView::create("prop/32003.png");
		}

		item1->removeChildByTag(1);
		prop1->setPosition(ccp(item1->getContentSize().width/2,item1->getContentSize().height/2));
		prop1->setTag(1);
		item1->addChild(prop1, -1);

		CLabel *num = dynamic_cast<CLabel*>(m_ui->findWidgetById("num1"));
		num->setString(CCString::createWithFormat("%d/%d",m_strengthItem.item1.itemNum,m_strengthItem.item1.itparam)->getCString());

		if (m_strengthItem.item1.itemNum<m_strengthItem.item1.itparam)
		{
			num->setColor(ccRED);
			CButton *item1 = dynamic_cast<CButton*>(m_ui->findWidgetById("item1"));
			item1->setEnabled(true);
		}
		else
		{
			num->setColor(ccGREEN);
		}

	}
	else
	{
		item1->setEnabled(false);
		num1->setString("");
	}

	CButton *item2 = dynamic_cast<CButton*>(m_ui->findWidgetById("item2"));
	item2->removeChildByTag(1);
	if (m_strengthItem.item2.iconId>0)
	{
		((CImageView*)item2->getNormalImage())->setTexture(setItemQualityTexture(m_strengthItem.item2.quality));
		item2->setEnabled(true);
		item2->setUserData(&m_strengthItem.item2);

		CCSprite *prop1 = CCSprite::create(CCString::createWithFormat("prop/%d.png",m_strengthItem.item2.iconId)->getCString());
		if (!prop1)
		{
			prop1 = CImageView::create("prop/32003.png");
		}

		item2->removeChildByTag(1);
		prop1->setPosition(ccp(item2->getContentSize().width/2,item2->getContentSize().height/2));
		prop1->setTag(1);
		item2->addChild(prop1, -1);

		CLabel *num = dynamic_cast<CLabel*>(m_ui->findWidgetById("num2"));
		num->setString(CCString::createWithFormat("%d/%d",m_strengthItem.item2.itemNum,m_strengthItem.item2.itparam)->getCString());

		if (m_strengthItem.item2.itemNum<m_strengthItem.item2.itparam)
		{
			num->setColor(ccRED);
			CButton *item1 = dynamic_cast<CButton*>(m_ui->findWidgetById("item2"));
			item1->setEnabled(true);
		}
		else
		{
			num->setColor(ccGREEN);
		}

	}
	else
	{
		item2->setEnabled(false);
		num2->setString("");
	}

	CButton *item3 = dynamic_cast<CButton*>(m_ui->findWidgetById("item3"));
	item3->removeChildByTag(1);
	if (m_strengthItem.item3.iconId>0)
	{
		((CImageView*)item3->getNormalImage())->setTexture(setItemQualityTexture(m_strengthItem.item3.quality));
		item3->setVisible(true);
		item3->setEnabled(true);
		item3->setUserData(&m_strengthItem.item3);

		CCSprite *prop1 = CCSprite::create(CCString::createWithFormat("prop/%d.png",m_strengthItem.item3.iconId)->getCString());
		if (!prop1)
		{
			prop1 = CImageView::create("prop/32003.png");
		}

		item3->removeChildByTag(1);
		prop1->setPosition(ccp(item3->getContentSize().width/2,item3->getContentSize().height/2));
		prop1->setTag(1);
		item3->addChild(prop1, -1);

		CLabel *num = dynamic_cast<CLabel*>(m_ui->findWidgetById("num3"));
		num->setString(CCString::createWithFormat("%d/%d",m_strengthItem.item3.itemNum,m_strengthItem.item3.itparam)->getCString());

		if (m_strengthItem.item3.itemNum<m_strengthItem.item3.itparam)
		{
			num->setColor(ccRED);
			CButton *item1 = dynamic_cast<CButton*>(m_ui->findWidgetById("item3"));
			item1->setEnabled(true);
		}
		else
		{
			num->setColor(ccGREEN);
		}
	}
	else
	{
		item3->setEnabled(false);
		num3->setString("");
	}

	if(!m_isUseGold)
	{
		m_weirdItem.id =0;
	}

}

float CStarUpItem::showEffect(bool bSuccess)
{
	//屏蔽层
	MaskLayer* pLayer1 = MaskLayer::create("CStarUpItem::effect");
	pLayer1->setOpacity(0);
	LayerManager::instance()->push(pLayer1);
	MaskLayer* lay = MaskLayer::create("HammerEffect");
	lay->setContentSize(CCSizeMake(1138,640));
	lay->setAnchorPoint(ccp(0.5f, 0.5f));
	lay->setPosition(VCENTER);
	lay->setOpacity(0);
	LayerManager::instance()->push(lay);
	
	//拷贝星星
	//星星填充
	//星星放大
	//星星飞上去消失
	//成功就爆炸粒子弹成功，失败就不爆炸弹失败
	
	//参考星星
	CCSprite* pStar = (CCSprite*)m_ui->findWidgetById("star");
	CCPoint pos = pStar->getPosition()+ccp(pStar->getContentSize().width/2, pStar->getContentSize().height/2);

	CCSprite* pStarCopy = CCSprite::createWithTexture(pStar->getTexture());
	CCProgressTimer* pTimer = CCProgressTimer::create(pStarCopy);
	pTimer->setType(kCCProgressTimerTypeBar);
	pTimer->setMidpoint(ccp(0,0));
	pTimer->setBarChangeRate(ccp(0,1));
	pTimer->setPosition(pos);
	lay->addChild(pTimer);
	pTimer->runAction(CCSequence::create(
		CCProgressFromTo::create(0.6f, 0, 100),
		CCEaseBackInOut::create(CCScaleTo::create(0.1f, 1.3f)),
		CCMoveBy::create(0.2f, ccp(0, 189)),
		CCFadeOut::create(0.2f),
		nullptr));
	//CCParticleSystemQuad* pBindLz = CCParticleSystemQuad::create("lz/22.plist");
	//pBindLz->setPosition(ccp(pTimer->getContentSize().width/2, pTimer->getContentSize().height/2));
	//pTimer->addChild(pBindLz, 2);
	//pBindLz->setVisible(false);
	//pBindLz->runAction(CCSequence::create(CCDelayTime::create(0.75),CCShow::create(), /*CCRemoveSelf::create(), */nullptr));


	if(bSuccess)
	{
		//粒子
		lay->runAction(CCSequence::create(CCDelayTime::create(0.9f), CCCallFuncN::create(this, callfuncN_selector(CStarUpItem::showEffectCallBack1)),nullptr));
	}
	
	char* sTips = "strengthen/word_100.png";
	if(!bSuccess)
	{
		sTips = "strengthen/word_101.png";
	}

	//强化成功
	PlayEffectSound(SFX_432);
	CCSprite* pText = CCSprite::create(sTips);
	pText->setPosition(pos+ccp(0, 150));
	lay->addChild(pText, 3);
	pText->setVisible(false);
	pText->setScale(0.7f);
	pText->runAction(CCSequence::create(
		CCDelayTime::create(1.1f),
		CCShow::create(),
		CCFadeIn::create(0.2f),
		CCSpawn::createWithTwoActions(CCMoveBy::create(0.5f, ccp(0, 100)), CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.0f), CCFadeOut::create(0.3f))),
		nullptr));

	if(bSuccess)
	{
		lay->runAction(CCSequence::create(CCDelayTime::create(1.6f),CCCallFuncN::create(this, callfuncN_selector(CStarUpItem::showEffectCallBack)),nullptr));
		return 1.6f+0.5f;
	}
	else
	{
		lay->runAction(CCSequence::create(CCDelayTime::create(1.6f),CCCallFunc::create(this, callfunc_selector(CStarUpItem::showTVLightCallBack)),nullptr));
		return 1.6f;
	}
}

void CStarUpItem::showEffectCallBack1(CCNode* pSender)
{
	PlayEffectSound(SFX_409);

	CCNode* pNode = (CCNode*)m_ui->findWidgetById("why");
	//播放爆炸粒子
	CCParticleSystemQuad* pBomb = CCParticleSystemQuad::create("lz/abab.plist");
	pBomb->setPosition(pNode->getPosition());
	pBomb->setAutoRemoveOnFinish(true);
	pSender->addChild(pBomb, 10);

	//添加星星层
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->removeChildByTag(CStarUpItemStarLayTag);
	CLayout* pStarLayout = getStarLayout(m_strengthItem.armor.iStar);
	btn->addChild(pStarLayout, CStarUpItemStarLayTag, CStarUpItemStarLayTag);
	//获取最后一个星星，做动作
	CCSprite* pLastStar = (CCSprite*)pStarLayout->getChildren()->lastObject();
	pLastStar->runAction(CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 0.8f), CCScaleTo::create(0.2f, 0.4f)), 5));
	
	//获取参考位置
	CCSprite* pWhy = (CCSprite*)m_ui->findWidgetById("why");

	CCAnimation* pAnimation = AnimationManager::sharedAction()->getAnimation("9060");
	pAnimation->setDelayPerUnit(0.08f);
	CCSprite* pEffect = CCSprite::create("skill/9060.png");
	pEffect->setPosition(pWhy->getPosition());
	pEffect->runAction(CCSequence::create(CCAnimate::create(pAnimation), CCRemoveSelf::create(), nullptr));
	pEffect->setScale(1.75f);
	pSender->addChild(pEffect, 50);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(0.2f, 8.0f), CCScaleTo::create(0.0f, 1.0f)));

}

void CStarUpItem::showEffectCallBack( CCNode* pSender )
{
	PlayEffectSound(SFX_407);

	CCNode* pNode = (CCNode*)m_ui->findWidgetById("alpha_panel");
	float fTime = ShowTVLight(pNode, pSender);
	CCNodeRGBA* pWhitePanel = (CCNodeRGBA*)m_ui->findWidgetById("white_panel");
	pWhitePanel->setOpacity(0);
	pWhitePanel->runAction(CCSequence::create(CCDelayTime::create(fTime), CCFadeTo::create(0.15f, 180), CCFadeOut::create(0.03f), nullptr));

	pSender->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(fTime), CCCallFunc::create(this, callfunc_selector(CStarUpItem::showTVLightCallBack))));
}


void CStarUpItem::roleUpdate( const TMessage& tMsg )
{
	checkMoney();
}


void CStarUpItem::changeToRinset()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	CRadioButton *radioBtn2= (CRadioButton*)(radioGroup->getChildByTag(2));
	radioGroup->selectButton(radioBtn2, true);
	CRadioButton *radioBtn1= (CRadioButton*)(radioGroup->getChildByTag(1));
	radioGroup->selectButton(radioBtn1, false);
}

void CStarUpItem::initItemInfo( CCPoint posStart, CLayout* pLayout[], int iCount )
{
	for(int i=0; i<iCount; i++)
	{
		CLayout* pCopyLayout = UICloneMgr::cloneLayout(m_pCellInfo);
		pCopyLayout->setPosition(posStart+ccp(0, -i*40));
		m_itemAttr->addChild(pCopyLayout, 911);
		pLayout[i] = pCopyLayout;
		pLayout[i]->setVisible(false);
	}
}

void CStarUpItem::hideNoneValueCell( int iCount, CLayout* pLayout[] )
{
	for(unsigned int i=iCount; i<4; i++)
	{
		pLayout[i]->setVisible(false);
	}
}

int CStarUpItem::updateInfoToUIAndReturnCount( CLayout* pLayout[], int iNameId[], int iValue[], int iValueAim[], int iMax, const char* sTitle )
{
	int iMaxLineCount = 0;

	for(unsigned int i=0; i<iMax; i++)
	{
		//有值
		if(iValue[i] > 0)
		{
			pLayout[iMaxLineCount]->setVisible(true);
			//设置名称
			CLabel* pInfoType = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("info_type");
			pInfoType->setString(GETLANGSTR(iNameId[i]));
			//设置值大小
			CLabel* pValue = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("value");
			if(atoi(pValue->getString())!=iValue[i])
			{
				pValue->setString(ToString(iValue[i]));
				pValue->runAction(CCRollLabelAction::create(0.3f, 0, iValue[i], pValue));
			}
			//目标值大小
			CLabel* pAimValue = (CLabel*)pLayout[iMaxLineCount]->findWidgetById("aim_value");
			pAimValue->setString(ToString(iValueAim[i]));
			if(isMaxStar())
			{
				pAimValue->setString("????");
			}

			iMaxLineCount++;
		}

		//找到四个了，退出循环
		if(iMaxLineCount>=4)
		{
			break;
		}
	}
	//隐藏掉没有数据填充的cell
	hideNoneValueCell(iMaxLineCount, pLayout);

	//如果一个都没有，隐藏标题
	if(sTitle != nullptr)
	{
		CCNode* pNode = (CCNode*)m_ui->findWidgetById(sTitle);
		if(pNode)
		{
			pNode->setVisible(iMaxLineCount != 0);
		}
	}
	return iMaxLineCount;
}

bool CStarUpItem::isMaxStar()
{
	CItem* item = &(m_strengthItem.armor);
	return item->iStar == 5;
}

void CStarUpItem::checkMoney()
{
	CLabel *spand = dynamic_cast<CLabel*>(m_ui->findWidgetById("gold"));
	//检查货币数量
	if(!CheckCoin(atoi(spand->getString())))
	{
		spand->setColor(RGB_RED);
	}
	else
	{
		spand->setColor(RGB_WHITE);
	}
}

void CStarUpItem::showTVLightCallBack()
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CStarUpItem::sameEquip( CCObject* pObj )
{
	if(m_strengthItem.armor.itemId == 0)
	{
		return;
	}
	CLevelUpItem* pData = dynamic_cast<CLevelUpItem*>(pObj);
	if(pData)
	{
		int iId = pData->getItemId();
		if(m_strengthItem.armor.id == iId)
		{
			//清空当前数据
			clearEquip();
			m_strengthItem.armor.itemId = 0;
		}
	}
}

void CStarUpItem::clearEquip()
{
	CLabel *tip = dynamic_cast<CLabel*>(m_ui->findWidgetById("tip"));
	tip->setVisible(true);

	m_pSpineHero->setVisible(true);

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(false);

	CCSprite *why = (CCSprite*)(m_ui->findWidgetById("why"));
	why->setVisible(true);


	//当前星星和目标星星
	m_ui->removeChildByTag(494);
	m_ui->removeChildByTag(497);

	//装备图标
	CButton *btn = dynamic_cast<CButton*>(m_ui->findWidgetById("bgBtn"));
	btn->removeChildByTag(1);

	//等级
	CLabel* pLevel = dynamic_cast<CLabel*>(m_ui->findWidgetById("level"));
	CCNode* pLevelBg = dynamic_cast<CCNode*>(m_ui->findWidgetById("level_mask"));
	pLevel->setVisible(false);
	pLevelBg->setVisible(false);

	//星星层
	btn->removeChildByTag(CStarUpItemStarLayTag);
}

int CStarUpItem::getItemId()
{
	return m_strengthItem.armor.id;
}
