#include "HeroDetail.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"

bool CHeroDetail::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		m_ui = LoadComponent("HeroDetail.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		//m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
		//m_cell->retain();
		//m_ui->removeChild(m_cell);

		//m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
		//m_tableView->setDirection(eScrollViewDirectionVertical);
		//m_tableView->setSizeOfCell(m_cell->getContentSize());
		////m_tableView->setSizeOfCell(CCSizeMake(790,115));
		//m_tableView->setBounceable(false);
		//m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroDetail::tableviewDataSource));	
		//m_tableView->setCountOfCell(8);

		return true;
	}
	return false;
}

void CHeroDetail::onClickButton(CCObject* pSender)
{

}

void CHeroDetail::showHeroInfo(CHero* hero)
{
	m_hero = hero;
	CLayout *font = (CLayout*)(m_ui->findWidgetById("font"));
	CLabel *hp = (CLabel*)(font->findWidgetById("hp"));
	hp->setString(ToString(hero->hp));

	CLabel *attack = (CLabel*)(font->findWidgetById("attack"));
	attack->setString(ToString(hero->atk));

	CLabel *deffend = (CLabel*)(font->findWidgetById("defend"));
	deffend->setString(ToString(hero->def));
	deffend->setVisible(hero->def>0||hero->addDef);
	CLabel *def_font = (CLabel*)(font->findWidgetById("def_font"));
	def_font->setVisible(hero->def>0||hero->addDef);

	CLabel *crit = (CLabel*)(font->findWidgetById("crit"));
	crit->setString(CCString::createWithFormat("%d", hero->crit)->getCString());

	CLabel *hit = (CLabel*)(font->findWidgetById("hit"));
	hit->setString(ToString(hero->hit));

	CLabel *dodge = (CLabel*)(font->findWidgetById("dodge"));
	dodge->setString(ToString(hero->dodge));
	
	CLabel *firstAtk = (CLabel*)(font->findWidgetById("firstAtk"));
	firstAtk->setString(ToString(hero->renew));

	CLabel *addhp = (CLabel*)(font->findWidgetById("addhp"));

	CLabel *addatk = (CLabel*)(font->findWidgetById("addatk"));

	CLabel *adddef = (CLabel*)(font->findWidgetById("adddef"));

	CLabel *addcrit = (CLabel*)(font->findWidgetById("addcrit"));

	CLabel *addhit = (CLabel*)(font->findWidgetById("addhit"));

	CLabel *adddodge = (CLabel*)(font->findWidgetById("adddodge"));

	CLabel *dex = (CLabel*)(font->findWidgetById("addact"));

	CLabel *dexf = (CLabel*)(font->findWidgetById("dex"));
	CLabel *adddexf = (CLabel*)(font->findWidgetById("adddex"));
	CLabel *dexfont = (CLabel*)(font->findWidgetById("dex_font"));
	dexf->setString(CCString::createWithFormat("%d",hero->dex)->getCString());

	if (hero->addDex>0)
	{
		adddexf->setString(CCString::createWithFormat("+%d",hero->addDex)->getCString());
		//adddexf->setPositionX(dexf->getPositionX()+dexf->getContentSize().width+10);
	}
	else
	{
		adddexf->setString("");
	}

	if (hero->addHp>0)
	{
		addhp->setString(CCString::createWithFormat("+%d",hero->addHp)->getCString());
		//addhp->setPositionX(hp->getPositionX()+hp->getContentSize().width+10);
	}
	else
	{
		addhp->setString("");
	}
	
	if (hero->addDef>0)
	{
		adddef->setString(CCString::createWithFormat("+%d",hero->addDef)->getCString());
		//adddef->setPositionX(deffend->getPositionX()+deffend->getContentSize().width+10);
	}
	else
	{
		adddef->setString("");
	}

	if (hero->addAtk>0)
	{
		addatk->setString(CCString::createWithFormat("+%d",hero->addAtk)->getCString());
		//addatk->setPositionX(attack->getPositionX()+attack->getContentSize().width+10);
	}
	else
	{
		addatk->setString("");
	}

	if (hero->addHit>0)
	{
		addhit->setString(CCString::createWithFormat("+%d",hero->addHit)->getCString());
		//addhit->setPositionX(hit->getPositionX()+hit->getContentSize().width+10);
	}
	else
	{
		addhit->setString("");
	}

	if (hero->addCrit>0)
	{
		addcrit->setString(CCString::createWithFormat("+%d",hero->addCrit)->getCString());
		//addcrit->setPositionX(crit->getPositionX()+crit->getContentSize().width+10);
	}
	else
	{
		addcrit->setString("");
	}

	if (hero->addDodge>0)
	{
		adddodge->setString(CCString::createWithFormat("+%d",hero->addDodge)->getCString());
		//adddodge->setPositionX(dodge->getPositionX()+dodge->getContentSize().width+10);
	}
	else
	{
		adddodge->setString("");
	}

	if (hero->addRenew>0)
	{
		dex->setString(CCString::createWithFormat("+%d",hero->addRenew)->getCString());
		//dex->setPositionX(firstAtk->getPositionX()+firstAtk->getContentSize().width+10);
	}
	else
	{
		dex->setString("");
	}

	int count = 8;
	if (hero->def<=0&&hero->addDef<=0)
	{
		count--;
	}
	//m_tableView->setCountOfCell(count);
	//m_tableView->reloadData();
} 

void CHeroDetail::onEnter()
{
	BaseLayer::onEnter();
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroDetail::updateHeroInfo));
}


//CCObject* CHeroDetail::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
//{
//	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
//	if (!pCell)
//	{
//		pCell = new CTableViewCell();
//		pCell->autorelease();
//		pCell->setTag(uIdx);
//		addTableCell(uIdx,pCell);
//	}
//	else
//	{
//		pCell->removeAllChildren();
//		addTableCell(uIdx,pCell);
//	}
//	return pCell;
//}


//void CHeroDetail::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
//{
//	if (uIdx == 0)
//	{	
//		CLayout *lay = UICloneMgr::cloneLayout(m_cell);	
//		while (lay->getChildrenCount()>0)
//		{
//			CCNode * node = (CCNode*)lay->getChildren()->objectAtIndex(0);
//			lay->removeChild(node);
//			pCell->addChild(node);
//		}
//	}
//	else
//	{
//		CLayout *layout = (CLayout *)m_ui->findWidgetById(CCString::createWithFormat("_%d",uIdx)->getCString());
//		CLayout *lay = UICloneMgr::cloneLayout(layout);	
//		while (lay->getChildrenCount()>0)
//		{
//			CCNode * node = (CCNode*)lay->getChildren()->objectAtIndex(0);
//			node->setPositionY(m_cell->getChildByTag(1)->getPositionY());
//			lay->removeChild(node);
//			pCell->addChild(node);
//		}
//	}
//}

void CHeroDetail::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	showHeroInfo(hero);
}

void CHeroDetail::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

CHeroDetail::~CHeroDetail()
{
	
}

