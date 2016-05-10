#include "StrengthenItem.h"
#include "tools/UICloneMgr.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "GMessage.h"

#include "FilterItem.h"
#include "tools/ShowTexttip.h"
#include "common/CommonFunction.h"
#include "model/DataCenter.h"
#include "guide/GuideManager.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "Hero/HeroList.h"

#include "bag/ItemTip.h"

//品质高优先/等级大优先/绑定英雄优先
bool SortItem(const CItem* pItem1, const CItem* pItem2)
{
	return 
		pItem1->quality*10000+pItem1->itemLevel*10+(pItem1->armor.hero>0?1:0)
		>
		pItem2->quality*10000+pItem2->itemLevel*10+(pItem2->armor.hero>0?1:0)
		;
}

CStrengthenItem::CStrengthenItem():m_type(0),m_item(nullptr),m_pSelectListener(nullptr),m_itemSelectHander(nullptr),m_toHero(0),m_iCountEquipStock(0),m_iEquipType(0)
	,m_pItemTip(nullptr),m_iAskType(CStrengthenItemTypeOthers)
{
	for (int i=1; i<=7; i++)
	{
		m_filterVec.push_back(i);
	}
}

bool CStrengthenItem::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("StrengthenItem");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("StrengItem.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//添加ItemTip
		m_pItemTip = CItemTip::create();
		m_ui->findWidgetById("attr")->addChild(m_pItemTip, -1);
		CCPoint pos = m_ui->convertToNodeSpace(m_pItemTip->getPosition());
		m_pItemTip->setPosition(pos+ccp(36, 30));
		m_pItemTip->DoNotPopItem();
		m_pItemTip->hideBg();
		//m_pItemTip->hideHeroEquipHead();
		m_pItemTip->hideButton();

		return true;
	}
	return false;
}

void CStrengthenItem::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CStrengthenItem::tableviewDataSource));
	m_tableView->reloadData();

	CButton *select = (CButton*)(m_ui->findWidgetById("select"));
	select->setOnClickListener(this,ccw_click_selector(CStrengthenItem::onSelectItem));

	CButton *filter = (CButton*)(m_ui->findWidgetById("filter"));
	filter->setOnClickListener(this,ccw_click_selector(CStrengthenItem::onFilterItem));

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CStrengthenItem::onClose));
	this->addChild(pClose, 999);

    m_selectCellImg = (CCSprite*)m_ui->findWidgetById("selimg");
	m_selectCellImg->retain();

	GetTcpNet->registerMsgHandler(RoleBag,this,CMsgHandler_selector(CStrengthenItem::recItemData));
	GetTcpNet->registerMsgHandler(ArmorListMsg,this,CMsgHandler_selector(CStrengthenItem::processNetMessage));

	CSceneManager::sharedSceneManager()->addMsgObserver("updateFilter",this,GameMsghandler_selector(CStrengthenItem::onSetFilter));
}


void CStrengthenItem::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateFilter",this);
}

void CStrengthenItem::onSetFilter(const TMessage& tMsg)
{
	m_itemList.clear();

	vector<int> *filt = (vector<int> *)tMsg.lParam;

	m_filterVec = *filt;

	for (int i=0; i<m_itemData.itemList.size(); ++i)
	{
		CItem *item =(CItem*)(&m_itemData.itemList.at(i));

		bool isAdd = false;
		for (int j = 0; j < filt->size(); j++)
		{
			
			if (item->armor.armorType == filt->at(j))
			{
				if (tMsg.nMsg == 2)
				{
					isAdd = true;
				}
				else if (tMsg.nMsg == 0&&tMsg.nMsg==item->armor.hero)
				{
					isAdd = true;
				}
				else if (tMsg.nMsg == 1&& item->armor.hero>0)
				{  
					isAdd = true;
				}
				else
				{
					isAdd = false;
				}
				break;
			}
		}
		if (isAdd)
		{
			m_itemList.push_back(&m_itemData.itemList.at(i));
		}
	}

	//排序
	std::sort(m_itemList.begin(), m_itemList.end(), SortItem);
	m_tableView->setCountOfCell(m_itemList.size());
	m_tableView->reloadData();

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(false);

	if (m_itemList.size()>0)
	{
		CTableViewCell *cell = (CTableViewCell*)m_tableView->getCells()->objectAtIndex(0);
		onTouchItem(cell->getChildByTag(1));
	}
}


CCObject* CStrengthenItem::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

void CStrengthenItem::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	CItem &item =  *(m_itemList.at(uIdx));// m_itemData.itemList.at(uIdx);

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item.itemId);

	for (int i = 1; i <=11; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		if (!child) continue;
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView *bgImg = (CImageView*)child;
			if (m_toHero>0)
			{
				if (!item.canUse&&item.armor.hero!=m_toHero)
				{
					CColorView *colorView = CColorView::create(ccc4(0,0,0,100));
					colorView->setContentSize(bgImg->getContentSize());
 					colorView->setPosition(bgImg->getPosition());
					pCell->addChild(colorView,1);
				}
				
				if(item.armor.hero<=0)
				{
					//记录库存
					m_iCountEquipStock++;
				}
			}
			
			bgImg->setTouchEnabled(true);
			bgImg->setUserData(m_itemList.at(uIdx));
			bgImg->setOnClickListener(this,ccw_click_selector(CStrengthenItem::onTouchItem));
		}
		else if (i==2)
		{
			((CButton*)child)->setEnabled(false);
			CCSprite *mask = CCSprite::createWithTexture(setItemQualityTexture(item.quality));
			mask->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
			child->addChild(mask, 10);

			CCSprite *prop = CCSprite::create(GetImageName(1, item.quality, item.iconId).c_str());
			if (!prop)
			{
				prop = CImageView::create("prop/32003.png");
			}
			prop->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
			child->addChild(prop);

			//添加星星
			CLayout* pStarLayout = getStarLayout(item.iStar);
			child->addChild(pStarLayout, 10);
		}
		else if (i==3)
		{
			CLabel *label = (CLabel*)child;
			if(itemData)
			{
				label->setString(itemData->itemName.c_str());
			}
		}
		else if (i==4)
		{
			CLabel *label = (CLabel*)child;
			
			if (item.itemLevel==0)
			{
				label->removeFromParent();
			}
			else
			{
				label->setString(CCString::createWithFormat("+%d",item.itemLevel)->getCString());
				label->setPositionX(pCell->getChildByTag(3)->getPositionX()+pCell->getChildByTag(3)->getContentSize().width+10);
			}
		}
		else if (i==5)
		{
			CLabel *label = (CLabel*)child;
			if (item.itemLevel==0)
			{
				label->setVisible(false);
			}
			else
			{
				label->setVisible(true);
				//if (item.itemLevel==item.armor.strenLv)
				//{
				//	label->setString(CCString::createWithFormat("Lv.%s",GETLANGSTR(229))->getCString());
				//}
				//else
				//{
				//	label->setString(CCString::createWithFormat("Lv.%d",item.itemLevel)->getCString());
				//}
				label->setString(CCString::createWithFormat("+%d",item.itemLevel)->getCString());
			}
		}
		else if (i==6)
		{
			CCSprite *head = (CCSprite*)child;
			if (item.armor.hero>0)
			{
				CCSprite *img = CCSprite::create(getImageName(&item.armor).c_str());
				if(!img)
				{
					img = CCSprite::create("headIcon/101.png");
					CCLOG("ERROR CStrengthenItem::addTableCell");
				}

				CCSprite *mask = (CCSprite*) lay->getChildByTag(7);
				lay->removeChild(mask);
				pCell->addChild(mask);

				img->setScale(0.53f);
				img->setPosition(ccp(head->getContentSize().width/2,head->getContentSize().height/2+14));
				head->addChild(img);
			}
			else
			{
				pCell->removeChild(child);
			}
		}
		else if (i==7)
		{
			pCell->removeChild(child);
		}
		else if (i==8)
		{
			child->setVisible(item.stren);
		}
		else if(i==9)
		{
			if (item.itemLevel==0)
			{
				child->setVisible(false);
			}
			else
			{
				child->setVisible(true);
			}
		}
		else if (i==11)
		{
			CLabelAtlas* pLabel = (CLabelAtlas*)child;
			pLabel->setAnchorPoint(ccp(0, 0.5f));
			pLabel->setString(ToString(item.armor.combat));
		}
	}
}

void CStrengthenItem::recItemData(int type, google::protobuf::Message *msg)
{
	//this->setVisible(true);

	WareHouseResponse *res = (WareHouseResponse*)msg;
	m_itemData.read(*res);

	for (int i=0; i<m_itemData.itemList.size(); ++i)
	{
		if (m_itemData.itemList.at(i).canUse)
		{
			m_itemList.push_back(&(m_itemData.itemList.at(i)));
		}
	}
	bool isInsert = false;
	for (int i=0; i<m_itemData.itemList.size(); ++i)
	{
		if (!m_itemData.itemList.at(i).canUse)
		{
			if (CGuideManager::getInstance()->getIsRunGuide()&&(CGuideManager::getInstance()->getCurrTask()==5||CGuideManager::getInstance()->getCurrTask()==20)&&m_itemData.itemList.at(i).itemId==11011&&!isInsert)
			{
				m_itemList.insert(m_itemList.begin(),&(m_itemData.itemList.at(i)));
				isInsert =true;
			}
			else
			{
				m_itemList.push_back(&(m_itemData.itemList.at(i)));
			}
		}
	}

	//排序
	if (!CGuideManager::getInstance()->getIsRunGuide())
	{
		std::sort(m_itemList.begin(), m_itemList.end(), SortItem);
	}
	m_tableView->setCountOfCell(m_itemData.itemList.size());
	m_tableView->reloadData();

	if (m_itemList.size()>0)
	{
		CTableViewCell *cell = (CTableViewCell*)m_tableView->getCells()->objectAtIndex(0);
		onTouchItem(cell->getChildByTag(1));
		this->setVisible(true);
	}
	else
	{
		int iType = m_iAskType;

		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
		
		switch (iType)
		{
		case CStrengthenItemTypeItemLevelUp:ShowPopTextTip(GETLANGSTR(1225));
			break;
		case CStrengthenItemTypeItemReset:ShowPopTextTip(GETLANGSTR(1226));
			break;
		case CStrengthenItemTypeItemStarUp:ShowPopTextTip(GETLANGSTR(1227));
			break;
		case CStrengthenItemTypeOthers:ShowPopTextTip(GETLANGSTR(220));
			break;
		default:
			break;
		}
	}
}


void CStrengthenItem::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CStrengthenItem::onSelectItem(CCObject* pSender)
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();

	if(m_pSelectListener && m_itemSelectHander)
	{
		if (m_type==2&&m_toHero==m_item->armor.hero)
		{
			//卸下
			EquipStockData* pData = new EquipStockData;
			pData->m_iCount = m_iCountEquipStock+1;
			pData->m_iIndex = m_iEquipType-1;
			NOTIFICATION->postNotification(UPDATE_EQUIP_STOCK_COUNT,  pData);
			delete pData;

			(m_pSelectListener->*m_itemSelectHander)(nullptr);

		}
		else
		{
			//穿上
			if(m_item->armor.hero == 0)
			{
				EquipStockData* pData = new EquipStockData;
				pData->m_iCount = m_iCountEquipStock-1;
				pData->m_iIndex = m_iEquipType-1;
				NOTIFICATION->postNotification(UPDATE_EQUIP_STOCK_COUNT,  pData);
				delete pData;
			}

			(m_pSelectListener->*m_itemSelectHander)(m_item);
		}
	}
}


void CStrengthenItem::setSelectType(int type)
{
	this->m_type = type;
}

void CStrengthenItem::onFilterItem(CCObject* pSender)
{
	CFilterItem *filItem = CFilterItem::create();
	filItem->setFilterType(m_filterVec);
	LayerManager::instance()->push(filItem);
}

void CStrengthenItem::onTouchItem(CCObject* pSender)
{
	if(isVisible())
	{
		PlayEffectSound(SFX_Button);
	}

	CImageView * img = (CImageView*)pSender;
	m_item = (CItem*)img->getUserData();

	m_pItemTip->showItemProperty(m_item);

	CLayout *attribute = (CLayout*)(m_ui->findWidgetById("attr"));
	attribute->setVisible(true);

	m_selectCellImg->removeFromParent();
	img->addChild(m_selectCellImg);
	m_selectCellImg->setVisible(true);
	m_selectCellImg->setPosition(ccp(img->getContentSize().width/2,0/*img->getContentSize().height/2*/));

	CButton *select = (CButton*)(m_ui->findWidgetById("select")); //sel_font
	CLabel *selfont = (CLabel*)(m_ui->findWidgetById("sel_font"));
	if (m_toHero==m_item->armor.hero&&m_toHero>0)
	{
		select->setVisible(true);
		selfont->setVisible(false);
	}
	else if (m_toHero>0&&!m_item->canUse)
	{
		select->setVisible(false);
		selfont->setVisible(false);
	}
	else
	{
		select->setVisible(true);
		selfont->setVisible(true);
	}

	if (m_type==2)
	{
		if (m_toHero>0)
		{
			CImageView *uninstall = (CImageView*)(m_ui->findWidgetById("uninstall"));
			CImageView *change = (CImageView*)(m_ui->findWidgetById("change"));
			CImageView *selfont = (CImageView*)(m_ui->findWidgetById("sel_font"));
			uninstall->setVisible(m_toHero==m_item->armor.hero);
			change->setVisible(m_toHero!=m_item->armor.hero&&m_item->armor.hero>0&&m_item->canUse);
			selfont->setVisible(m_item->armor.hero==0&&m_item->canUse);
		}
	}
}

void CStrengthenItem::setOnSelectItemListener(CCObject* pListener,SEL_SelectItem pHandler)
{
	m_pSelectListener = pListener;
	m_itemSelectHander = pHandler;
}

void CStrengthenItem::setSelForHero(int toHero)
{
	m_toHero = toHero;
}

void CStrengthenItem::processNetMessage(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);
	ArmorListRes * res = (ArmorListRes*)msg;
	for (int i = 0; i < res->armorlist_size(); i++)
	{
		CItem item;
		item.read(res->armorlist(i));
		//if (item.canUse)
		{
			m_itemData.itemList.push_back(item);
		}
	}

	for (int j = 0; j < m_itemData.itemList.size(); j++)
	{
		m_itemList.push_back(&(m_itemData.itemList.at(j)));
	}

	//排序
	std::sort(m_itemList.begin(), m_itemList.end(), SortItem);

	m_tableView->setCountOfCell(m_itemData.itemList.size());
	m_tableView->reloadData();

	if (m_itemList.size()>0)
	{
		CTableViewCell *cell = (CTableViewCell*)m_tableView->getCells()->objectAtIndex(0);
		onTouchItem(cell->getChildByTag(1));
		this->setVisible(true);
	}
}

void CStrengthenItem::setAskType( CStrengthenItemType type )
{
	m_iAskType = type;
}
