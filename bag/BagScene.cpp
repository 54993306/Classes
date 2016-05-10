#include "BagScene.h"
#include "Global.h"
#include "protos/protocol.h"
#include "common/CommonFunction.h"
#include "netcontrol/CPlayerControl.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"
#include "mainCity/FilterItem.h"
#include "common/CShowToBuyResource.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "mainCity/PanelTips.h"


//品质高优先/等级大优先/绑定英雄优先
bool SortBagItem(const CItem* pItem1, const CItem* pItem2)
{
	return 
		pItem1->quality*10000+pItem1->itemLevel*10+(pItem1->armor.hero>0?1:0)
		>
		pItem2->quality*10000+pItem2->itemLevel*10+(pItem2->armor.hero>0?1:0)
		;
}

CBagLayer::CBagLayer()
{
	m_bagData = nullptr;
	m_armorTip = nullptr;
	m_stuffTip = nullptr;
	m_selectType = 1;
	m_iSelectIndex = 1;
	m_pPanelTips = nullptr;

	for (int i=1; i<=7; i++)
	{
		m_filterVec.push_back(i);
	}
}

CBagLayer::~CBagLayer()
{
	CC_SAFE_DELETE(m_bagData);
}


bool CBagLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("Bagsce");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("bag.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//退出
		CButton* pClose = CButton::create("common/back.png", "common/back.png");
		pClose->getSelectedImage()->setScale(1.1f);
		pClose->setPosition(VLEFT+50, VTOP-50);
		pClose->setOnClickListener(this,ccw_click_selector(CBagLayer::onClose));
		this->addChild(pClose, 999);

		return true;
	}
	return false;
}

void CBagLayer::onEnter()
{
	BaseLayer::onEnter();
	m_cell =(CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

	m_pageView = (CGridView*)m_ui->getChildByTag(9);
	m_pageView->setDirection(eScrollViewDirectionVertical);
	m_pageView->setCountOfCell(0);
	m_pageView->setSizeOfCell(CCSizeMake(110,100));
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setColumns(3);
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setBounceable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CBagLayer::gridviewDataSource));
	m_pageView->reloadData();
	
	CButton* btn;
	btn = (CButton *)m_ui->getChildByTag(4);
	btn->setOnClickListener(this, ccw_click_selector(CBagLayer::onClickBtn));

	btn= (CButton *)m_ui->findWidgetById("filter");
	btn->setOnClickListener(this,ccw_click_selector(CBagLayer::onFilter));

	CCNode *lay = m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radio= (CRadioButton *)lay->getChildByTag(i+1);
		radio->setOnCheckListener(this, ccw_check_selector(CBagLayer::onChangeBagType));
	}

	m_selectImg = (CCSprite*)(m_ui->findWidgetById("selectImg"));
	//m_selectImg->setScale(0.9f);
	m_selectImg->retain();

	GetTcpNet->registerMsgHandler(RoleBag,this,CMsgHandler_selector(CBagLayer::bagItemData));
	GetTcpNet->registerMsgHandler(ExtendBag,this,CMsgHandler_selector(CBagLayer::processMsg));
	GetTcpNet->registerMsgHandler(ResetBag,this,CMsgHandler_selector(CBagLayer::processMsg));
	CSceneManager::sharedSceneManager()->addMsgObserver("updateFilter",this,GameMsghandler_selector(CBagLayer::onSetFilter));
	CSceneManager::sharedSceneManager()->addMsgObserver("decomposeArmor",this,GameMsghandler_selector(CBagLayer::decomposeArmorRes));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_ITEM_DATA, this, GameMsghandler_selector(CBagLayer::updateItemData));

}

void CBagLayer::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateFilter", this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("decomposeArmor", this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_ITEM_DATA, this);

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
}

void CBagLayer::onPageChange(CCObject* pSender, unsigned int nPageIdx)
{
// 	CCSprite *yellowSpr = (CCSprite*)m_ui->getChildByTag(55);
// 	CCSprite *pointSpr =(CCSprite*)m_ui->getChildByTag(50+nPageIdx);
// 	yellowSpr->setPosition(pointSpr->getPosition());
// 	switch (nPageIdx)
// 	{
// 	case 0:
// 		break;
// 
// 	case 1:
// 		break;
// 
// 	case 2:
// 		break;
// 
// 	case 3:
// 		break;
// 
// 	case 4:
// 		break;
// 	default:
//		break;
// 	}
}

void CBagLayer::decomposeArmorRes(const TMessage& tMsg)
{
	int armorId = tMsg.nMsg;

	vector<CItem>::iterator it = m_bagData->itemList.begin();
	for (; it!= m_bagData->itemList.end(); it++)
	{
		if ((*it).id==armorId)
		{
			m_bagData->itemList.erase(it);
			break;
		}
	}

	m_itemList.clear();

	for (int i = 0; i < m_bagData->itemList.size(); i++)
	{
		m_itemList.push_back(&m_bagData->itemList.at(i));
	}
	m_iSelectIndex = 1;

	std::sort(m_itemList.begin(), m_itemList.end(), SortBagItem);
	m_pageView->reloadData();
	
	const CItem *item = findItemByGrid(1);
	if (item)
	{
		m_armorTip->showItemProperty((CItem*)item);
		m_armorTip->setVisible(true);
	}
	else
	{
		m_armorTip->setVisible(false);
	}

	checkShowNoItemTip();
}

void CBagLayer::onClickBtn(CCObject *pSender)
{
	CButton *btn= (CButton *)pSender;
	switch (btn->getTag())
	{
	case 4:
		{
// 			CNetClient::getShareInstance()->sendDataType(ResetBag);
		}
		break;

	case 5:
		{
			if (m_pageView->getCountOfCell()<70)
			{	
				m_iSelectIndex = 1;
				m_pageView->setCountOfCell(m_pageView->getCountOfCell()+18);
			    m_pageView->reloadData();
			}
		}
		break;
	
	default:
		break;
	}
}

void CBagLayer::onFilter(CCObject *pSender)
{
	CButton *btn = (CButton *)pSender;
	if (m_selectType == Equipment_Type)
	{
		CFilterItem *filItem = CFilterItem::create();
		filItem->setFilterType(m_filterVec);
		LayerManager::instance()->push(filItem);
	}
}

void CBagLayer::onSetFilter(const TMessage& tMsg)
{
	m_itemList.clear();

	vector<int> *filt = (vector<int> *)tMsg.lParam;

	m_filterVec = *filt;

	for (int i=0; i<m_bagData->itemList.size(); ++i)
	{
		CItem *item =(CItem*)(&m_bagData->itemList.at(i));

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
			m_itemList.push_back(&m_bagData->itemList.at(i));
		}
	}

// 	m_pageView->setCountOfCell(m_itemList.size());
	m_iSelectIndex = 1;

	std::sort(m_itemList.begin(), m_itemList.end(), SortBagItem);
	m_pageView->reloadData();
	const CItem *item = findItemByGrid(1);
	if (item)
	{
		m_armorTip->showItemProperty((CItem*)item);
		m_armorTip->setVisible(true);
	}
	else if (m_armorTip)
	{
		m_armorTip->setVisible(false);
	}
}

CCObject* CBagLayer::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	CLayout *clone = UICloneMgr::cloneLayout(m_cell);
	if(!pCell)
	{
		pCell = new CGridViewCell();
		pCell->setTag(uIdx);
		pCell->autorelease();	
	}
	else 
	{
		pCell->removeAllChildren();
	}
	const CItem *item = this->findItemByGrid(uIdx+1);
	for (int i=1; i<=6;++i)
	{
		CCNode *child = clone->getChildByTag(i);
		clone->removeChild(child);
		pCell->addChild(child);

		switch (i)
		{
		case 1:
			{
				CImageView *btn = (CImageView*)child;
				btn->setTouchEnabled(true);
				if (item)
				{
					btn->setOnClickListener(this,ccw_click_selector(CBagLayer::onSelectBagItem));
					btn->setTag(uIdx+1);

					if(m_iSelectIndex==uIdx+1)
					{
						m_selectImg->removeFromParent();
						m_selectImg->setPosition(ccp(btn->getContentSize().width/2+6,btn->getContentSize().height/2 + 3));
						m_selectImg->setZOrder(100);
						pCell->addChild(m_selectImg, 99);
					}
					
					CImageView *prop = nullptr;
					prop = getItemSprite(item);
					prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
					btn->addChild(prop);
				}
			}
			break;
		case 2:
			{
				CImageView *mask = (CImageView*)child;
				if (item)
				{
					mask->setTexture(setItemQualityTexture(item->quality));

					if (item&&item->iStar>0&&item->itemType==2)
					{
						//添加星星
						CLayout* pStarLayout = getStarLayout(item->iStar);
						mask->addChild(pStarLayout);
					}

				}
				else
				{
					mask->setVisible(false);
					if (uIdx>=m_maxGrid)
					{
						CImageView *btn = (CImageView*)pCell->getChildByTag(1);
		 				CImageView *spr = CImageView::create("bagUI/beibaoicon_02.png");
						spr->setTouchEnabled(true);
						spr->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
						btn->addChild(spr);
 						btn->setOnClickListener(this,ccw_click_selector(CBagLayer::onUnlockBag));
					}
				}
			}
			break;
		case 3:
			{
				CLabel * pLabel = (CLabel*)child;
				if (item&&item->itemNum>0&&item->itemType!=2)
				{
					pLabel->setVisible(false);
					CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
					haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
					haveNum->setPosition(ccp(91, 3));
					haveNum->setScale(1.2f);
					pCell->removeChild(child);
					pCell->addChild(haveNum);

					haveNum->setString(CCString::createWithFormat("%d",item->itemNum)->getCString());
				}
				else
				{
					child->setVisible(false);
				}
			}
			break;
		case 4:
			{
				if (item&&item->itemType==2)
				{			
					child->setVisible(item->armor.hero>0);
				}
			}
			break;
		case 5:
			{
				CCNode * pBg = (CCNode*)child;
				if (item&&item->itemLevel>0&&item->itemType==2)
				{
					pBg->setVisible(true);
				}
				else
				{
					pBg->setVisible(false);
				}
			}
			break;
		case 6:
			{
				CLabel * pLabel = (CLabel*)child;
				if (item&&item->itemLevel>0&&item->itemType==2)
				{
					pLabel->setVisible(true);

					pLabel->setString(CCString::createWithFormat("+%d",item->itemLevel)->getCString());

					//if (item->itemLevel<item->armor.strenLv)
					//{				
					//	pLabel->setString(CCString::createWithFormat("Lv.%d",item->itemLevel)->getCString());
					//}
					//else
					//{
					//	pLabel->setString(CCString::createWithFormat("Lv.%s", GETLANGSTR(229))->getCString());
					//}
				}
				else
				{
					pLabel->setVisible(false);
				}
			}
			break;
		default:
			break;
		}
	}
	pCell->setScale(0.9f);
	return pCell;
}

void CBagLayer::onSelectBagItem(CCObject *pSender)
{
	PlayEffectSound(SFX_Button);

	CButton *btn = (CButton*)pSender;	
	const CItem *item = findItemByGrid(btn->getTag());
	m_iSelectIndex = btn->getTag();
	//showBookAnimate(m_ui);

	if (m_selectType == Equipment_Type)
	{
		m_armorTip->setVisible(true);
		m_armorTip->showItemProperty((CItem*)item);
	}
	else
	{
		m_stuffTip->showItemProperty((CItem*)item);
	}

	m_selectImg->removeFromParent();
	m_selectImg->setPosition(ccp(btn->getContentSize().width/2+6,btn->getContentSize().height/2 + 3));
	m_selectImg->setZOrder(100);
	btn->getParent()->addChild(m_selectImg);
}

void CBagLayer::onUnlockBag(CCObject *pSender)
{
	CNetClient::getShareInstance()->sendDataType(ExtendBag);
}

void CBagLayer::onLongSelectItem(CCObject* pSender, CCTouch* pTouch)
{

}

void CBagLayer::onChangeBagType(CCObject* pSender, bool isCheck)
{
	if (!isCheck) return;
	CButton *btn = (CButton*)pSender;
	m_selectType = btn->getTag();

	PlayEffectSound(SFX_Button);

	if (btn->getTag()==Prop_Type)
	{
	}
	//碎片
	else if (btn->getTag()==Suipian_Type)
	{
		CPlayerControl::getInstance().sendRoleBag(0);
		
		lockRadioButton();

		if (!m_stuffTip)
		{
			m_stuffTip = CStuffTip::create();
			m_stuffTip->setTouchPriority(LayerManager::instance()->getPriority()-1);
			m_stuffTip->DoNotPopItem();
			this->addChild(m_stuffTip);
		}

		m_stuffTip->setVisible(true);

		CButton* btn= (CButton *)m_ui->findWidgetById("filter");
		btn->setVisible(false);
		CLabel* label= (CLabel *)m_ui->findWidgetById("filter_text");
		label->setVisible(false);
		if (m_armorTip)
		{		
			m_armorTip->setVisible(false);
		}
	}
	//装备
	else if (btn->getTag() == Equipment_Type)
	{
		CPlayerControl::getInstance().sendRoleBag(1,true);
		
		lockRadioButton();

		m_stuffTip->setVisible(false);

		CButton* btn= (CButton *)m_ui->findWidgetById("filter");
		btn->setVisible(true);
		CLabel* label= (CLabel *)m_ui->findWidgetById("filter_text");
		label->setVisible(true);

		if (m_armorTip)
		{
			m_armorTip->setVisible(true);
		}
	}
}

void CBagLayer::bagItemData(int type, Message *msg)
{
	WareHouseResponse *res = (WareHouseResponse*)msg;
	if(!isVisible())
	{
		this->setVisible(true);
// 		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		//showBookAnimate(m_ui);
	}

	//CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	//radioGroup->setTouchEnabled(true);
	//radioGroup->stopAllActions();

	if (m_bagData)
	{
		delete m_bagData;
	}
	m_bagData = new CBagData();
	m_bagData->read(*res);

	m_maxGrid = m_bagData->maxNum;
	m_itemList.clear();

	for (int i = 0; i < m_bagData->itemList.size(); i++)
	{
		m_itemList.push_back(&m_bagData->itemList.at(i));
	}	

	m_iSelectIndex = 1;

	std::sort(m_itemList.begin(), m_itemList.end(), SortBagItem);
	m_pageView->setCountOfCell(m_bagData->maxNum+4);
	m_pageView->reloadData();

	if (m_bagData->itemList.size()>0)
	{
		m_selectImg->removeFromParent();
		CGridViewCell *cell = ((CGridViewCell*)m_pageView->getCells()->objectAtIndex(0));
		m_selectImg->setPosition(ccp(cell->getChildByTag(1)->getContentSize().width/2+6,cell->getChildByTag(1)->getContentSize().height/2 + 3));
		cell->addChild(m_selectImg); 

		if (!m_armorTip&&m_selectType == Equipment_Type)
		{
			m_armorTip = CItemTip::create();
			m_armorTip->setTouchPriority(LayerManager::instance()->getPriority()-1);
			this->addChild(m_armorTip);
			m_armorTip->showItemProperty((CItem*)findItemByGrid(m_iSelectIndex));
			m_armorTip->DoNotPopItem();
		}
		else if (m_armorTip&&m_selectType==Equipment_Type)
		{
			m_armorTip->showItemProperty((CItem*)findItemByGrid(m_iSelectIndex));
		}
		else
		{
			m_stuffTip->showItemProperty((CItem*)findItemByGrid(m_iSelectIndex));
			m_stuffTip->DoNotPopItem();
		}
	}
	else if (m_selectType==Suipian_Type)
	{
		m_stuffTip->setVisible(false);
	}
	else if (m_selectType==Equipment_Type&&m_armorTip)
	{
		m_armorTip->setVisible(false);
	}

	checkShowNoItemTip();

}

void CBagLayer::updatePage()
{
	int page =m_pageView->getCountOfCell()/(m_pageView->getRows()*m_pageView->getColumns());
    page = m_pageView->getCountOfCell()%(m_pageView->getRows()*m_pageView->getColumns())==0?page:page+1;
	for (int i = 0; i < 5; i++)
	{
		CCNode *point = m_ui->getChildByTag(50+i);
		if (i>page-1)
		{
			point->setVisible(false);
		}
	}
}

CItem* CBagLayer::findItemByGrid(int uIdx)
{
	if (uIdx<=m_itemList.size())
	{
		return m_itemList.at(uIdx-1);
	}
    return nullptr;
}

void CBagLayer::processMsg(int type, Message *msg)
{
	switch (type)
	{
	case ExtendBag:
		{
			ExtendResponse *res = (ExtendResponse*)msg;
			int isSucc = res->result();  

			//1 成功， 2 金币不足， 3 已达到背包格子最大数量上限
			if (isSucc==1)
			{
				m_maxGrid = res->gridnum();
				m_iSelectIndex = 1;
				m_pageView->setCountOfCell(res->gridnum()+5);
				m_pageView->reloadData();
			}
			else if (isSucc==2)
			{
				//ShowPopTextTip(GETLANGSTR(205));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceCoin);
			}
			else if (isSucc == 3)
			{
				ShowPopTextTip(GETLANGSTR(178));	
			}
		} 
		break;

	case ResetBag:
		{
			ResetResponse *res = (ResetResponse*)msg;
			if (res->result()&&m_bagData)
			{
				m_bagData->itemList.clear();
				for (int i = 0; i < res->itemlist_size(); i++)
				{
					const Item &item = res->itemlist(i);
					CItem it;
					it.read(item);
					m_bagData->itemList.push_back(it);
				}
				m_iSelectIndex = 1;
				m_pageView->reloadData();
				CCLOG("rest bag success");
			}

			checkShowNoItemTip();
		}
		break;
	default:
		break;
	}
}

void CBagLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}


void CBagLayer::lockRadioButton()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	radioGroup->setTouchEnabled(false);

	radioGroup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CCCallFunc::create(this, callfunc_selector(CBagLayer::callBackForRadioButton))));
}

void CBagLayer::callBackForRadioButton()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	radioGroup->setTouchEnabled(true);
}

void CBagLayer::updateItemData( const TMessage& tMsg )
{
	int iType = tMsg.nMsg;//1.升级2洗练3升星
	CItem* pItem = (CItem*)(tMsg.lParam);
	if(!pItem)
	{
		return;
	}
	CItem *item = findItemByGrid(m_iSelectIndex);
	if(pItem->id == item->id)
	{
		if(iType==1)
		{
			item->itemLevel = pItem->itemLevel;
			ReloadGridViewWithSameOffset(m_pageView);
		}
		else if(iType==3)
		{
			item->iStar = pItem->iStar;
			ReloadGridViewWithSameOffset(m_pageView);
		}
	}

}


void CBagLayer::checkShowNoItemTip()
{
	if(m_bagData->itemList.size()<=0)
	{
		m_pageView->setVisible(false);
		showNoItemTip(true, m_selectType==Suipian_Type?GETLANGSTR(1241):GETLANGSTR(1240));
	}
	else
	{
		m_pageView->setVisible(true);
		showNoItemTip(false, nullptr);
	}
}


void CBagLayer::showNoItemTip( bool bShow, const char* sInfo )
{
	if(bShow)
	{
		if(m_pPanelTips==nullptr)
		{
			m_pPanelTips = CPanelTips::create();
			this->addChild(m_pPanelTips);
		}
		m_pPanelTips->setVisible(true);
		m_pPanelTips->setString(sInfo);
	}
	else
	{
		if(m_pPanelTips!=nullptr)
		{
			m_pPanelTips->setVisible(false);
		}
	}
}



