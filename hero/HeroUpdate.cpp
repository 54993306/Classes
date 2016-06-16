#include "HeroUpdate.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"

bool CHeroUpdate::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("HeroUpdate");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("HeroPromote.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}


void CHeroUpdate::levelSkillUp(CHero* hero,const Hero* pHero)
{
	m_hero = hero;
	m_reshero.readData(*pHero);

	CLabel *hp = (CLabel*)(m_ui->findWidgetById("hp"));
	hp->setString(ToString(hero->hp));
	CLabel *attack = (CLabel*)(m_ui->findWidgetById("atk"));
	attack->setString(ToString(hero->atk));
	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(ToString(hero->level));

	CLabel *addhp = (CLabel*)(m_ui->findWidgetById("addhp"));
	CLabel *addatk = (CLabel*)(m_ui->findWidgetById("addatk"));
	CLabel *addlevel = (CLabel*)(m_ui->findWidgetById("addlevel"));
	addhp->setString(ToString(pHero->hp()));
	addatk->setString(ToString(pHero->atk()));
	addlevel->setString(ToString(pHero->level()));

	int count = 0;
	if (m_hero->skill3.skillId>0)
	{
		count++;
	}
	if (m_hero->skill4.skillId>0)
	{
		count++;
	}
	m_tableView->setCountOfCell(count);
	m_tableView->reloadData();
} 

void CHeroUpdate::onEnter()
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
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroUpdate::tableviewDataSource));
	m_tableView->reloadData();
}

void CHeroUpdate::onExit()
{
	BaseLayer::onExit();
}

bool CHeroUpdate::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	CCSprite *bgSpr1 = (CCSprite*)m_ui->findWidgetById("bg1");

	if( !res && !bgSpr->boundingBox().containsPoint(pTouch->getLocation())&&!bgSpr1->boundingBox().containsPoint(pTouch->getLocation()))
	{
		res = true;
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	return res;
}

CCObject* CHeroUpdate::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CHeroUpdate::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i = 1; i <=16; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
	}
	CLabel *name1 = (CLabel*)pCell->findWidgetById("name1");
	CLabel *level1 = (CLabel*)pCell->findWidgetById("level1");
	CLabel *cost1 = (CLabel*)pCell->findWidgetById("cost1");
	CLabel *name2 = (CLabel*)pCell->findWidgetById("name2");
	CLabel *level2 = (CLabel*)pCell->findWidgetById("level2");		
	CLabel *cost2= (CLabel*)pCell->findWidgetById("cost2");

	if (uIdx==0)
	{
		name1->setString(m_hero->skill3.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill3.level)->getCString());
	
		name2->setString(m_reshero.skill3.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill3.level)->getCString());

		cost1->setString(ToString(m_hero->skill3.cost));
		cost2->setString(ToString(m_reshero.skill3.cost));

		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill3.skillId);
		if (cfg1)
		{
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(4);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);

				parent = pCell->getChildByTag(6);
				CImageView*spr1 = UICloneMgr::cloneImageView(spr);
				spr1->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr1);
			}
		}
	}
	else if (uIdx==1)
	{
		CLabel *skillType = (CLabel*)(pCell->findWidgetById("type"));
		skillType->setString(GETLANGSTR(273));

		name1->setString(m_hero->skill4.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill4.level)->getCString());
		name2->setString(m_hero->skill4.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill4.level)->getCString());

		cost1->setString(ToString(m_hero->skill4.cost));
		cost2->setString(ToString(m_reshero.skill4.cost));

		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill4.skillId);
		if (cfg1)
		{
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(4);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);

				parent = pCell->getChildByTag(6);
				CImageView*spr1 = UICloneMgr::cloneImageView(spr);
				spr1->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr1);
			}
		}
	}
}