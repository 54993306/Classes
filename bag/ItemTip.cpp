#include "ItemTip.h"
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
#include "model/DataCenter.h"
#include "mainCity/Strengthen.h"
#include "sign/PopItem.h"
#include "scene/CPopTip.h"
#include "scene/AnimationManager.h"
#include "mainCity/MainCityControl.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CommonFunction.h"

CItemTip::CItemTip()
	:m_isOutClose(false)
	,m_iMaxLineCount(0)
	,m_pCellInfo(nullptr)
{
	for(unsigned int i=0; i<4; i++)
	{
		m_pBaseInfo[i] = nullptr;
		m_pSpecialInfo[i] = nullptr;
	}
}


CItemTip::~CItemTip()
{
	CC_SAFE_RELEASE(m_pCellInfo);
}



bool CItemTip::init()
{
	if (BaseLayer::init())
	{
// 		MaskLayer* lay = MaskLayer::create("ItemTipMask");
// 		lay->setContentSize(CCSizeMake(2824,640));
// 		LayerManager::instance()->push(lay);

		this->setIsShowBlack(false);

		CColorView *colorView = CColorView::create(ccc4(0,0,0,200));
		colorView->setContentSize(CCSizeMake(1138,640));
		colorView->setPosition(VCENTER);
		colorView->setTag(100);
		this->addChild(colorView);

		m_ui = LoadComponent("ItemTip.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

		//获取cell
		m_pCellInfo = (CLayout*)m_ui->findWidgetById("cell_info");
		CC_SAFE_RETAIN(m_pCellInfo);
		m_pCellInfo->removeFromParentAndCleanup(false);

		CCPoint pBasePos = ITEM_INFO_START_POS;
		//初始化基础属性四条
		initItemInfo(pBasePos, m_pBaseInfo, 4);

		pBasePos.x += 200;
		//初始化特殊属性四条
		initItemInfo(pBasePos, m_pSpecialInfo, 4);

		//特殊属性字体用蓝色
		for(unsigned int i=0; i<4; i++)
		{
			CLabel* pName = (CLabel*)m_pSpecialInfo[i]->findWidgetById("info_type");
			pName->setColor(ccc3(121, 173, 212));
			CLabel* pValue = (CLabel*)m_pSpecialInfo[i]->findWidgetById("value");
			pValue->setColor(ccc3(121, 173, 212));
		}

		return true;
	}
	return false;
}

void CItemTip::DoNotPopItem()
{
	this->getChildByTag(100)->setVisible(false);
	//m_ui->setPosition(ccp(VCENTER.x+257,VCENTER.y));
	m_isOutClose = true;
}


void CItemTip::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CItemTip::onEnter()
{
	BaseLayer::onEnter();

	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");
	bg->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0, 15)), CCMoveBy::create(0.7f, ccp(0, -15)))));

	//光影效果
	/*
	CCNode* pBg = (CCNode *)m_ui->findWidgetById("bg1");
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9013");
	culAnim->setDelayPerUnit(0.1f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CCSprite* pSprite = CCSprite::create();
	pSprite->setPosition(ccp(215, 96));
	pBg->addChild(pSprite);

	pSprite->runAction(CCRepeatForever::create(pAnimate));
	*/
	GetTcpNet->registerMsgHandler(DecomposeData,this,CMsgHandler_selector(CItemTip::processNetMsg));
	GetTcpNet->registerMsgHandler(DecomposeArmor,this,CMsgHandler_selector(CItemTip::processNetMsg));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_ITEM_DATA, this, GameMsghandler_selector(CItemTip::updateItemData));
}

void CItemTip::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_ITEM_DATA, this);
}


void CItemTip::showItemProperty(CItem *item)
{
	m_selectItem = item;

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);

	//背景
	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");
	bg->setOpacity(0);

	//品质
	CCSprite *mask= (CCSprite *)m_ui->findWidgetById("mask");
	mask->setTexture(setItemQualityTexture(item->quality));

	//名称
	CLabel *name= (CLabel *)m_ui->findWidgetById("name");
	if(itemData)
	{
		name->setString(itemData->itemName.c_str());
	}

	//类型
	CLabel* pType= (CLabel *)m_ui->findWidgetById("type");
	pType->setString(getArmorTypeStr(item->armor.armorType).c_str());

	//分解
	CButton *resolve = (CButton *)m_ui->findWidgetById("resolve");
	resolve->setOnClickListener(this,ccw_click_selector(CItemTip::onResolve));

	//强化
	CButton *strength = (CButton *)m_ui->findWidgetById("strength");
	strength->setOnClickListener(this,ccw_click_selector(CItemTip::onStrength));
	
	//物品icon
	CCSprite *itemSpr = CCSprite::create(CCString::createWithFormat("prop/%d.png",item->iconId)->getCString());
	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CItemTip::showItemProperty Lost Image = %d",item->iconId);
	}
	if (itemSpr)
	{
		bg->removeChildByTag(111);
		itemSpr->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
		itemSpr->setTag(111);
		itemSpr->setScale(1.3f);
		bg->addChild(itemSpr);
	}

	//如果是属性，则更新属性
	if (item->itemType==2)
	{
		updateArmorAttr(item);

		//描述
		CLabel *desc = (CLabel*)m_ui->findWidgetById("desc");
		if (itemData)
		{
			desc->setString(itemData->itemDesc.c_str());
		}
		else
		{
			desc->setString("");
		}
	}

	//是否被英雄佩戴
	CCSprite *isEqu = (CCSprite*)m_ui->findWidgetById("isEqu");
	isEqu->setVisible(item->armor.hero!=0);
	if (item->armor.hero>0)
	{
		isEqu->removeChildByTag(1);
		CCSprite *img = CCSprite::create(CCString::createWithFormat("headIcon/%d.png",item->armor.hero)->getCString());
		if(!img)
		{
			img = CCSprite::create("headIcon/101.png");
		}
		img->setScale(0.53f);
		img->setPosition(ccp(isEqu->getContentSize().width/2,isEqu->getContentSize().height/2+14));
		img->setTag(1);
		isEqu->addChild(img);
	}

}

void CItemTip::onResolve(CCObject* pSender)
{
	if (m_selectItem->armor.hero>0)
	{
		ShowConfirmTextTip(GETLANGSTR(187),this,ccw_click_selector(CItemTip::onConfirmDecompose));
	}
	else
	{
		lockResolve();
		CPlayerControl::getInstance().sendDecomposeArmor(m_selectItem->id,DecomposeData);
	}
}

void CItemTip::onConfirmDecompose(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		lockResolve();
		CPlayerControl::getInstance().sendDecomposeArmor(m_selectItem->id,DecomposeData);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CItemTip::onStrength(CCObject* pSender)
{
	if (CMainCityControl::getInstance()->isCityOpen(4))
	{
		CStrengthen *strength = CStrengthen::create();
		LayerManager::instance()->push(strength);
		CPlayerControl::getInstance().sendStrengthenArmor(m_selectItem->id,0);
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(202));
	}
	
}

bool CItemTip::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
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

void CItemTip::updateArmorAttr(CItem * item)
{
	m_iMaxLineCount = 0;

// 	42生命
// 		43攻击
// 	44	防御
// 	45	攻速
// 		46暴击
// 		47闪避
// 		48回复
// 	49	命中

	//optional int32 baseHp = 5;		//生命
	//optional int32 baseDef = 6;		//防御
	//optional int32 baseAtk = 7;		//攻击
	//optional int32 baseHit = 8;		//命中
	//optional int32 baseDex = 9;		//敏捷
	//optional int32 baseCrit = 10;		//暴击
	//optional int32 baseRenew = 11;		//回复
	//optional int32 baseDodge = 12;		//闪避

	//optional int32 addHp = 13;		//当前强化血量加成 or 特殊血量属性
	//optional int32 addDef = 14;		//当前强化防御加成
	//optional int32 addAtk = 15;		//当前强化攻击加成
	//optional int32 addHit = 16;		//当前强化命中加成
	//optional int32 addDex = 17;		//当前强化敏捷加成
	//optional int32 addCrit = 18;		//当前强化暴击加成
	//optional int32 addRenew = 19;		//当前强化回复加成
	//optional int32 addDodge = 20;		//当前强化闪避加成

	int iNameId[8] = {42, 44, 43, 49, 45, 46, 48, 47};

	//基础属性
	int iBaseValue[8] = 
	{
		item->armor.baseHp, item->armor.baseDef, item->armor.baseAtk, item->armor.baseHit,
		item->armor.baseDex, item->armor.baseCrit, item->armor.baseRenew, item->armor.baseDodge
	};
	//特殊属性
	int iSpecialValue[8] = 
	{
		item->armor.addHp, item->armor.addDef, item->armor.addAtk, item->armor.addHit,
		item->armor.addDex, item->armor.addCrit, item->armor.addRenew, item->armor.addDodge
	};
	
	int iMaxLineCountBase = updateInfoToUIAndReturnCount(m_pBaseInfo, iNameId, iBaseValue, 8, "base_title");
	int iMaxLineCountSpecial = updateInfoToUIAndReturnCount(m_pSpecialInfo, iNameId, iSpecialValue, 8, "special_title");

	m_iMaxLineCount = iMaxLineCountBase>iMaxLineCountSpecial?iMaxLineCountBase:iMaxLineCountSpecial;

	//更新线和描述的位置
	updatePosOfLineAndDesc();

	//等级
	CLabel *itemLev= (CLabel *)m_ui->findWidgetById("level");
	CCNode *pMaskBg= (CCNode *)m_ui->findWidgetById("mask_level");
	if(item->itemLevel == 0)
	{
		itemLev->setVisible(false);
		pMaskBg->setVisible(false);
	}
	else
	{
		itemLev->setVisible(true);
		pMaskBg->setVisible(true);
		itemLev->setString(CCString::createWithFormat("+%d",item->itemLevel)->getCString());
	}

	//战力
	CLabel* pFight= (CLabel *)m_ui->findWidgetById("power");
	pFight->setString(ToString(item->armor.combat));

	//星级
	CImageView* pImageView = (CImageView*)(m_ui->findWidgetById("bg_item"));
	CLayout* pStarLay = getStarLayoutWithBlackBase(item->iStar, 0.6f);
	pImageView->removeAllChildren();
	pStarLay->setPosition(pStarLay->getPosition()+ccp(185, 73));
	pImageView->addChild(pStarLay);
}


void CItemTip::processNetMsg(int type, google::protobuf::Message *msg)
{
	if (type==DecomposeData)
	{
		this->setVisible(true);

		DecomposeRes *res = (DecomposeRes*)msg;
		vector<CItem> itemList;
		for (int i = 0; i < res->items_size(); i++)
		{
			CItem item;
			item.read(res->items(i));
			itemList.push_back(item);
		}

		//金币更新到界面
		UserData *data = DataCenter::sharedData()->getUser()->getUserData();
		data->setCoin(data->getCoin()+res->coin());
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

		//增加金币
		CItem item;
		item.itemNum = res->coin();
		item.iconId = 0;
		item.quality = 5;
		item.iStar = 0;
		itemList.push_back(item);

		CPopItem *popItem = CPopItem::create();
		LayerManager::instance()->push(popItem);
		popItem->popItemList(itemList);
		popItem->setBtnHandler(this,ccw_click_selector(CItemTip::onDecomposeBtn));

		PlayEffectSound(SFX_432);
	}
	else
	{
		DecomposeRes *res = (DecomposeRes*)msg;
		if (res->result()==1)
		{
			CSceneManager::sharedSceneManager()->PostMessageA("decomposeArmor",m_selectItem->id,nullptr,nullptr);
		}
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
}

void CItemTip::onDecomposeBtn(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==1)
	{
		CPlayerControl::getInstance().sendDecomposeArmor(m_selectItem->id,DecomposeArmor);
	}
	else
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	
}

void CItemTip::lockResolve()
{
	CButton *resolve = (CButton *)m_ui->findWidgetById("resolve");
	resolve->setTouchEnabled(false);
	resolve->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFunc::create(this, callfunc_selector(CItemTip::callBackResolve))));
}

void CItemTip::callBackResolve()
{
	CButton *resolve = (CButton *)m_ui->findWidgetById("resolve");
	resolve->setTouchEnabled(true);
}

void CItemTip::initItemInfo( CCPoint posStart, CLayout* pLayout[], int iCount )
{
	for(int i=0; i<iCount; i++)
	{
		CLayout* pCopyLayout = UICloneMgr::cloneLayout(m_pCellInfo);
		pCopyLayout->setPosition(posStart+ccp(0, -i*ITEM_INFO_HEIGHT));
		m_ui->addChild(pCopyLayout, 10);
		pLayout[i] = pCopyLayout;
	}
}

void CItemTip::updatePosOfLineAndDesc()
{
	int iY = ccp(554, 300).y-m_iMaxLineCount*ITEM_INFO_HEIGHT+15;
	CCNode* pLine = (CCNode*)m_ui->findWidgetById("line");
	pLine->setPositionY(iY);
	CCNode* pDesc = (CCNode*)m_ui->findWidgetById("desc");
	pDesc->setAnchorPoint(ccp(0, 1));
	pDesc->setPositionY(iY-7);
}

void CItemTip::hideNoneValueCell( int iCount, CLayout* pLayout[] )
{
	for(unsigned int i=iCount; i<4; i++)
	{
		pLayout[i]->setVisible(false);
	}
}

int CItemTip::updateInfoToUIAndReturnCount( CLayout* pLayout[], int iNameId[], int iValue[], int iMax, const char* sTitle )
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
			pValue->setString(ToString(iValue[i]));

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
	CCNode* pNode = (CCNode*)m_ui->findWidgetById(sTitle);
	pNode->setVisible(iMaxLineCount != 0);

	return iMaxLineCount;
}

void CItemTip::hideHeroEquipHead()
{
	CCNode* pNode = (CCNode*)m_ui->findWidgetById("isEqu");
	pNode->setPositionY(9900);
}

void CItemTip::hideButton()
{
	const char* str[4] = {"itemLev_Copy", "itemLev_Copy1", "resolve", "strength"};
	for(unsigned int i=0; i<4; i++)
	{
		CCNode* pNode = (CCNode*)m_ui->findWidgetById(str[i]);
		pNode->setPositionY(9900);
	}
}

void CItemTip::hideBg()
{
	CCNode* pNode = (CCNode*)m_ui->findWidgetById("bg1");
	pNode->setPositionY(9900);
}

void CItemTip::updateItemData( const TMessage& tMsg )
{
	int iType = tMsg.nMsg;//1.升级2洗练3升星
	CItem* pItem = (CItem*)(tMsg.lParam);
	if(!pItem)
	{
		return;
	}

	//战力更新
	m_selectItem->armor.combat = pItem->armor.combat;

	switch (iType)
	{
	case 1:
		{
			//更新等级+基础属性
			m_selectItem->itemLevel = pItem->itemLevel;
			CopyBaseAttr(m_selectItem, pItem);
		}break;
	case 2:
		{
			//更新特殊属性
			CopySpecialAttr(m_selectItem, pItem);
		}break;
	case 3:
		{
			//更新星级+基础属性
			m_selectItem->iStar = pItem->iStar;
			CopyBaseAttr(m_selectItem, pItem);
		}break;
	default:
		break;
	}

	//做更新
	updateArmorAttr(m_selectItem);
}
