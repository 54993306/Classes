#include "HeroExpItem.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "HeroControl.h"
#include "tools/ShowTexttip.h"

bool CHeroExpItem::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("heroExpItemMask");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("HeroExpItem.xaml"); 
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CHeroExpItem::onEvolveBtn(CCObject* pSender)
{
    ((CHeroAttribute*)(this->getParent()))->getDelegate()->onEvolveHero();
}

bool CHeroExpItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);
	if (!res)
	{
		CCPoint locPoint = convertToNodeSpace(pTouch->getLocation());
		CCSprite *bg = (CCSprite*)(m_ui->findWidgetById("bg"));
		if (bg->boundingBox().containsPoint(locPoint))
		{
			res = true;
		}
		else 
		{
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
			res = true;
		}
	}
	return res;
}

void CHeroExpItem::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout *)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroExpItem::tableviewDataSource));
	m_tableView->reloadData();

}

CCObject* CHeroExpItem::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CHeroExpItem::evolveTask(const CEvolInfo& evInfo )
{
	CButton *evolve =(CButton*)(m_ui->findWidgetById("evolve"));
	evolve->setEnabled(evInfo.canEvol);

	m_evolInfo = evInfo;
	m_tableView->setCountOfCell(evInfo.questsList.size());
	m_tableView->reloadData();
}

void CHeroExpItem::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const CEvolQuest &evol = m_evolInfo.questsList.at(uIdx);
	const TaskData *task = DataCenter::sharedData()->getTask(Evole_Task)->getCfg(evol.id);
	for (int i = 1; i <=4; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView *bgView= (CImageView*)child;
			bgView->setTouchEnabled(true);
		    bgView->setUserData(&m_evolInfo.questsList.at(uIdx));
			bgView->setOnClickListener(this, ccw_click_selector(CHeroExpItem::onRecvTask));
		}
		else if (i==2)
		{
			CLabel *taskName = (CLabel*)child;
			taskName->setString(task->taskName.c_str());
		}
		else if (i==3)
		{
			CLabel *taskDesc = (CLabel*)child;
			taskDesc->setString(task->taskDesc.c_str());
		}
		else if (i==4)
		{
			child->setVisible(evol.pass);
		}
	}
}

void CHeroExpItem::onExit()
{
	BaseLayer::onExit();
}


void CHeroExpItem::onRecvTask(CCObject* pSender)
{
	CEvolQuest *evol = (CEvolQuest*)((CCNode*)pSender)->getUserData();

	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	if (evol->open&&data->getRoleAction()>=evol->action)
	{
		CPlayerControl::getInstance().sendEnterStage(evol->stageId,0,0,0, 0);
	}
	else if (!evol->open)
	{
		ShowPopTextTip(GETLANGSTR(183));
	}
	else if (data->getRoleAction()<evol->action)
	{
		ShowPopTextTip(GETLANGSTR(184));
	}
}

void CHeroExpItem::showItem(CItem* item)
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);

	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(item->quality));

	CCSprite *head = getItemSprite(item);
	if (!head)
	{
		head = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CHeroExpItem::showItem Lost Image = %d",item->iconId);
	}
	if (head)
	{
		head->setScale(82.0f/100.0f);
		head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
		itembg1->addChild(head);
	}

	CLabel *name = (CLabel *)(m_ui->findWidgetById("name"));
	if(itemData)
	{
		name->setString(itemData->itemName.c_str());
	}

	CLabel *num = (CLabel *)(m_ui->findWidgetById("num"));
	num->setString(ToString(item->itemNum));
}
