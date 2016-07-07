#include "HeroExp.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "HeroControl.h"
#include "tools/ShowTexttip.h"

#include "HeroExpItem.h"
#include "GMessage.h"

CHeroExp::CHeroExp():m_useItem(nullptr),m_useCount(0),m_addExp(0),m_clickCellLab(nullptr),m_iSaveUse(0), m_pSender(nullptr) ,m_bSendBlock(false), m_bDataBack(true),m_bLevelUp(false)
{
	m_hero.exp = -1;
}

bool CHeroExp::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		m_ui = LoadComponent("HeroExp.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CHeroExp::onAddExp(CCObject* pSender)
{
	m_useItem = (CItem*)((CImageView*)pSender)->getUserData();
	if (m_useItem->itemNum<=0)
	{
		return;
	}
	m_iSaveUse = 1;
	m_pSender = pSender;
    ((CHeroAttribute*)(this->getParent()))->getDelegate()->onAddHeroExp(m_useItem->id,1);
}

void CHeroExp::onEnter()
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
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroExp::tableviewDataSource));
	m_tableView->reloadData();

	CNetClient::getShareInstance()->registerMsgHandler(RoleBag,this, CMsgHandler_selector(CHeroExp::processMessage));
	this->setOnTouchEndedAfterLongClickListener(this, ccw_afterlongclick_selector(CHeroExp::onlongClickExpEnd));

	m_pValue = (CLabel*)(m_ui->findWidgetById("level"));

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO, this, GameMsghandler_selector(CHeroExp::updateHeroInfo));	
}

CCObject* CHeroExp::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CHeroExp::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	CItem &item = m_itemData.itemList.at(uIdx); 

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item.itemId);

	for (int i = 1; i <=7; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		switch (i)
		{
		case 1:
			{
				CImageViewScale9 *bgView= (CImageViewScale9*)child;
				bgView->setTouchEnabled(true);
				bgView->setUserData(&m_itemData.itemList.at(uIdx));
			    bgView->setOnClickListener(this,ccw_click_selector(CHeroExp::onAddExp));
				bgView->setOnLongClickListener(this,ccw_longclick_selector(CHeroExp::onlongClickExpItem));
			}
			break;
		case 2: 
			{
				CCSprite *itembg = (CCSprite*)child;
				CCSprite *prop = getItemSprite(&item);
				if (!prop)
				{
					prop = CCSprite::create("prop/32003.png");
					CCLOG("[ ERROR ] CHeroExp::addTableCell Lost Image = %d",item.iconId);
				}
				if (prop)
				{
					prop->setPosition(ccp(itembg->getContentSize().width/2,itembg->getContentSize().height/2));
					itembg->addChild(prop);
				}
			}
			break;
		case 3:
			{
				CCSprite *itembg = (CCSprite*)child;
				itembg->setTexture(setItemQualityTexture(item.quality));
			}
			break;
		case 4:
			{
				CLabel *name = (CLabel*)child;
				if(itemData)
				{
					name->setString(itemData->itemName.c_str());
				}
			}
			break;
		case 5:
			{
				CLabel *exp = (CLabel*)child;
				exp->setString(ToString(item.prop.AddVal));
			}
			break;
		case 6:
			{
				CLabel *haveNum = (CLabel*)child;
				haveNum->setString(CCString::createWithFormat(GETLANGSTR(185),item.itemNum)->getCString());
			}
			break;
		case 7:
			{
// 				CImageView *addExp = (CImageView*)child;
// 				addExp->setTouchEnabled(true);
// 				addExp->setUserData(&m_itemData.itemList.at(uIdx));
//  				addExp->setOnClickListener(this,ccw_click_selector(CHeroExp::onAddExp));
// 				addExp->setOnLongClickListener(this,ccw_longclick_selector(CHeroExp::onlongClickExpItem));
// // 				addExp->setOnLongClickListener(this, ccw_longclick_selector(c));
// 				if (item.itemNum<=0)
// 				{
// 					addExp->setEnabled(false);
// 				}
			}
			break;

		default:
			break;
		}
	}
}

void CHeroExp::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

void CHeroExp::onGetMethod(CCObject* pSender)
{
// 	CItem *item = (CItem*)((CImageView*)pSender)->getUserData();
//     CHeroExpItem *expItem = CHeroExpItem::create();
// 	expItem->showItem(item);
// 	LayerManager::instance()->push(expItem);
}

void CHeroExp::updateItemCount()
{
	onAddExpSuccess();

	if (m_useItem->itemNum==0)
	{
		int size = m_itemData.itemList.size();
		for (int i = 0; i <size; i++)
		{
			if (m_useItem->id == m_itemData.itemList.at(i).id&&m_itemData.itemList.at(i).itemNum==0)
			{
				m_itemData.itemList.erase(m_itemData.itemList.begin()+i);
				break;
			}
		}
		m_tableView->setCountOfCell(m_itemData.itemList.size());
		m_tableView->reloadData();
	}
}

void CHeroExp::processMessage(int type, google::protobuf::Message *msg)
{
	WareHouseResponse *res = (WareHouseResponse*)msg;
	m_itemData.itemList.clear();
	m_itemData.read(*res);

	m_tableView->setCountOfCell(m_itemData.itemList.size());
	m_tableView->reloadData();
}

bool CHeroExp::onlongClickExpItem(CCObject* pSender, CCTouch* pTouch)
{
	m_iSaveUse = 0;
	m_useCount = 0;
	m_addExp = 0;
	m_useItem = (CItem*)((CImageView*)pSender)->getUserData();
	m_clickCellLab = (CLabel*)((CImageView*)pSender)->getParent()->getChildByTag(6);
	this->schedule(schedule_selector(CHeroExp::onAddUseItem), 0.1f);
	this->schedule(schedule_selector(CHeroExp::sendAddExp));
	m_pSender = pSender;
	return true;
}

void CHeroExp::onAddUseItem(float dt)
{
	//经验道具足够 而且 等级还未达上限
	if (m_useItem->itemNum >0 && m_hero.level<m_hero.maxLevel)
	{
		//不是最新数据，不再更新
		if(!m_bDataBack) return;

		m_useItem->itemNum--;
		m_useCount++;
		m_clickCellLab->setString(CCString::createWithFormat(GETLANGSTR(185), m_useItem->itemNum)->getCString());
		m_addExp += m_useItem->prop.AddVal;
		CCLOG("%d___%d__%d", m_hero.exp, m_addExp, m_hero.nextExp);
		//够升一级了，请求发送数据
		if (m_hero.exp + m_addExp > m_hero.nextExp)
		{
			m_vecSendData.push_back(m_useCount);
			m_useCount = 0;
			m_addExp = 0;
			m_bDataBack = false;
		}
	}
	else
	{
		this->unschedule(schedule_selector(CHeroExp::onAddUseItem));
	}

}

void CHeroExp::onlongClickExpEnd(CCObject* pSender, CCTouch* pTouch, float fDuration)
{
	this->unschedule(schedule_selector(CHeroExp::onAddUseItem));
	//((CHeroAttribute*)(this->getParent()))->getDelegate()->onAddHeroExp(m_useItem->id,m_useCount);
	//m_useItem->itemNum -= m_useCount;
	//m_useCount = 0;
	//updateItemCount();
}

void CHeroExp::onSetHero(CHero *hero, bool bReset)
{
	//相同数据，return掉
	if(m_hero.exp == hero->exp && m_hero.nextExp == hero->nextExp)
	{
		return;
	}

	if(bReset)
	{
		m_hero.exp = -1;
	}
	if (isVisible())
	{
		CNetClient::getShareInstance()->registerMsgHandler(RoleBag,this, CMsgHandler_selector(CHeroExp::processMessage));
	}
	updateHeroExp(hero);
}

void CHeroExp::onAddExpSuccess()
{
	CCLOG("CHeroExp::onAddExpSuccess");
	m_bDataBack = true;
	m_clickCellLab = (CLabel*)((CImageView*)m_pSender)->getParent()->getChildByTag(6);
	m_clickCellLab->setString(CCString::createWithFormat(GETLANGSTR(185),m_useItem->itemNum - m_iSaveUse)->getCString());
	m_useItem->itemNum-=m_iSaveUse;
	m_iSaveUse = 0;
}

void CHeroExp::sendAddExp(float dt)
{
	if(!m_bSendBlock)
	{
		if(m_vecSendData.size()>0)
		{
			m_bSendBlock = true;
			((CHeroAttribute*)(this->getParent()))->getDelegate()->onAddHeroExp(m_useItem->id, m_vecSendData.at(0));
			m_vecSendData.erase(m_vecSendData.begin());
			runAction(CCSequence::create(CCDelayTime::create(0.6f), CCCallFunc::create(this, callfunc_selector(CHeroExp::sendAddExpCallBack)),nullptr));
			CCLOG("CHeroExp::sendAddExp");
		}
	}
}

void CHeroExp::sendAddExpCallBack()
{
	m_bSendBlock = false;
	CCLOG("CHeroExp::sendAddExpCallBack");
}

void CHeroExp::updateHeroExp(CHero *hero)
{
	//是否升级
	m_bLevelUp = hero->level>m_hero.level && m_hero.exp!=-1;

	CProgressBar *process1 = (CProgressBar*)m_ui->findWidgetById("lv_process");
	float fPercent = hero->exp*100.0f/hero->nextExp;
	float dt = fabs(process1->getValue()-fPercent)*0.01f;
	process1->stopProgress();
	process1->stopAllActions();
	if(m_bLevelUp /*&& process1->getValue() != 0 */)
	{
		float dt = fabs(process1->getValue()-100.0f)*0.01f;
		process1->startProgress(100, dt);
		process1->runAction(CCSequence::create(CCDelayTime::create(dt), CCCallFunc::create(this, callfunc_selector(CHeroExp::updateHeroExpCallBack)),nullptr));
		m_pValue->setString(CCString::createWithFormat("%d/%d", m_hero.nextExp, m_hero.nextExp)->getCString());
	}
	else
	{
		process1->startProgress(fPercent, dt);
		m_pValue->setString(CCString::createWithFormat("%d/%d", hero->exp, hero->nextExp)->getCString());
	}

	m_hero = *hero;
}

void CHeroExp::updateHeroExpCallBack()
{
	CProgressBar *process1 = (CProgressBar*)m_ui->findWidgetById("lv_process");
	process1->stopProgress();
	process1->setValue(0);

	float fPercent = m_hero.exp*100.0f/m_hero.nextExp;
	float dt = fabs(process1->getValue()-fPercent)*0.01f;

	if(fPercent!=0)
	{	
		process1->startProgress(fPercent, dt);
	}

	m_pValue->setString(CCString::createWithFormat("%d/%d", m_hero.exp, m_hero.nextExp)->getCString());
}

void CHeroExp::updateHeroInfo( const TMessage& tMsg )
{
	CHero *hero = (CHero*)tMsg.lParam;

	onSetHero(hero, true);
}
