#include "SelectEquipment.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "Global.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"

CSelectEquipment::CSelectEquipment():m_itemData(nullptr),m_preItem(nullptr),m_pSelectListener(nullptr),m_itemSelectHander(nullptr)
{

}


bool CSelectEquipment::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("selEqu");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("SelectItem.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CSelectEquipment::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->close("selEqu");

}

void CSelectEquipment::onEnter()
{
	BaseLayer::onEnter();

	CGridPageView *pageView = (CGridPageView*)m_ui->getChildByTag(9);
	pageView->setDirection(eScrollViewDirectionHorizontal);	
	pageView->setAnchorPoint(ccp(0,0));
	CCSize size = pageView->getContentSize();

	m_pageView = CGridPageView::create(size,CCSizeMake(size.width/6,size.height/4),0,
		this,ccw_datasource_adapter_selector(CSelectEquipment::gridviewDataSource));
	m_pageView->setPosition(pageView->getPosition());
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setRows(4);
	m_pageView->setColumns(6);
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setBounceable(false);

	m_pageView->setAutoRelocateSpeed(500.0f);
	m_ui->addChild(m_pageView);
	m_pageView->reloadData();

	// 	pageView->removeFromParent();

	CButton *btn= (CButton *)m_ui->getChildByTag(8);
	btn->setOnClickListener(this,ccw_click_selector(CSelectEquipment::onClose));
// 	GetTcpNet->registerMsgHandler(ChangeArmor,this,CMsgHandler_selector(CSelectEquipment::ProcessMsg));
	GetTcpNet->registerMsgHandler(RoleBag,this,CMsgHandler_selector(CSelectEquipment::recItemData));
}

CCObject* CSelectEquipment::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 

	CGridPageViewCell* pCell = (CGridPageViewCell*)pConvertCell;
	if (!pCell&&uIdx>=0)
	{
		pCell = new CGridPageViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);

		CButton *btn = CButton::create("common/cell.png","common/cell.png");
		btn->setAnchorPoint(CCPointZero);
		btn->setTag(uIdx);
		btn->setOnClickListener(this,ccw_click_selector(CSelectEquipment::onSelectBagItem));
		pCell->addChild(btn);

		if (uIdx==0)
		{

		}
		else if (m_itemData->itemList.size()>=uIdx)
		{
			CItem *item = &(m_itemData->itemList[uIdx-1]);

			CImageView *spr = CImageView::createWithTexture(setItemQualityTexture(item->quality));
			spr->setAnchorPoint(CCPointZero);
// 			spr->setPosition(ccp(6,6));
			
			btn->setTag(item->gridNum);
			int i = random_range(0,1);
			
			CImageView *prop = getItemSprite(item);
			if (!prop)
			{
				prop = CImageView::create("prop/101.png");
			}
			prop->setPosition(ccp(btn->boundingBox().size.width/2,btn->boundingBox().size.height/2));
			prop->setTouchEnabled(true);
			prop->setEnabled(true);
			/*prop->setOnLongClickListener(this,ccw_longclick_selector(CBagScene::onLongSelectItem));*/
			prop->setOnClickListener(this,ccw_click_selector(CSelectEquipment::onSelectBagItem));
			btn->addChild(prop);
			btn->addChild(spr);
		}
	}
	return pCell;
}

void CSelectEquipment::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CSelectEquipment::onSelectBagItem(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;	
	if (!m_pSelectListener)
	{	
		if (btn->getTag()<=m_itemData->itemList.size()&&btn->getTag()>0)
		{
			m_selectItem = &(m_itemData->itemList[btn->getTag()-1]);

// 			LayerManager::instance()->pop();
// 			LayerManager::instance()->close("selEqu");
		}
		else if (btn->getTag()==0)
		{
// 			if (m_preItem)
			{
				CPlayerControl::getInstance().sendChangeArmor(m_heroId,m_pos,0);
				LayerManager::instance()->pop();
				LayerManager::instance()->pop();
			}	
		}
	}
	else
	{
		if (btn->getTag()<=m_itemData->itemList.size()&&btn->getTag()>0)
		{
			m_selectItem = &(m_itemData->itemList[btn->getTag()-1]);
			(m_pSelectListener->*m_itemSelectHander)(m_selectItem);
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
		else if (btn->getTag()==0)
		{
			(m_pSelectListener->*m_itemSelectHander)(nullptr);
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
	}

}


bool CSelectEquipment::onLongSelectItem(CCObject* pSender, CCTouch* pTouch)
{
	return false;
}

CItem* CSelectEquipment::findItemByGrid(int uIdx)
{
	for (int i = 0; m_itemData &&i < m_itemData->itemList.size(); i++)
	{
		CItem *item = &(m_itemData->itemList[i]);
		if (item->gridNum== uIdx)
		{
			return item;
		}
	}
	return nullptr;
}

void CSelectEquipment::recItemData(int type, Message *msg)
{
	this->setVisible(true);
	WareHouseResponse *res = (WareHouseResponse*)msg;
	if (m_itemData)
	{
		delete m_itemData;
	}
	m_itemData = new CBagData();
	m_itemData->read(*res);
	m_maxGrid = res->maxnum();
	m_pageView->setCountOfCell(m_itemData->itemList.size()+1);
	m_pageView->reloadData();
}

void CSelectEquipment::setCurrentItem(int type, CItem* item)
{
	m_itemType = type;
	m_preItem = item;
}

void CSelectEquipment::setUnion(CUnion *nion)
{
	m_union = nion;
}

void CSelectEquipment::ProcessMsg(int type, google::protobuf::Message *msg)
{
	switch (type)
	{

	case ChangeArmor:
		{
//			Union *res = (Union *)msg;

//			CUnion nion;
//			nion.read(*res);

// 			if (res->has_hero())
// 			{
// // 				m_currNion->hero = nion.hero;
// 			}
// 			DoResult *res = (DoResult *)msg;
// 			if (res->result()==1)
// 			{
// 				ShowTexttip(U8("装备更换成功"),RGB_RED,0,VCENTER,2.0f,2.0f,0.0f,10.0f);
// 				CItem *item = new CItem();
// 				*item = *m_selectItem;
// 				CSceneManager::sharedSceneManager()->PostMessageA(CHANGE_EQUIPMENT,m_itemType,nullptr,item);
// 			}
// 			else
// 			{
// 				ShowTexttip(U8("装备更换失败"),RGB_RED,0,VCENTER,2.0f,2.0f,0.0f,10.0f);
// 			}
		}
		break;
	default:
		break;
	}
}

void CSelectEquipment::setOnSelectItemListener(CCObject* pListener,SEL_SelectItem pHandler)
{
	m_pSelectListener = pListener;
    m_itemSelectHander = pHandler;
}

void CSelectEquipment::setHeroArmor(int heroId, int pos)
{
	m_heroId = heroId;
	m_pos = pos;
}
