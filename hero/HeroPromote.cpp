#include "HeroPromote.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"

bool CHeroPromote::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("HeroPromote");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("HeroAdvance.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}
	return false;
}


void CHeroPromote::levelSkillUp(CHero* hero,const Hero* pHero)
{
	m_hero = hero;
	m_reshero.readData(*pHero);

	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroPromote::levelSkillUp Lost Image = %d",hero->thumb);
	}
	itembg1->removeChildByTag(1);
	head->setScale(82.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);

	CImageView *itembg2 = (CImageView *)(m_ui->findWidgetById("itembg2"));
	head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroPromote::levelSkillUp Lost Image = %d",hero->thumb);
	}
	itembg2->removeChildByTag(1);
	head->setScale(82.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg2->getContentSize().width/2, itembg2->getContentSize().height/2));
	itembg2->addChild(head);

	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("hero_mask1"));
	mask1->setTexture(setItemQualityTexture(hero->iColor));
	CImageView *mask2 = (CImageView *)(m_ui->findWidgetById("hero_mask2"));
	if (hero->quality<hero->maxQuality)
	{
		mask2->setTexture(setItemQualityTexture(hero->iColor+1));
	}
	else
	{
		mask2->setTexture(setItemQualityTexture(hero->iColor));
	}

	int count = 0;
	if (m_hero->skill2.skillId>0)
	{
		count++;
	}
	if (m_hero->skill4.skillId>0)
	{
		count++;
	}
	m_tableView->setCountOfCell(count);
	m_tableView->reloadData();

// 	CLabel *hp = (CLabel*)(m_ui->findWidgetById("hp"));
// 	hp->setString(ToString(hero->hp));
// 
// 	CLabel *attack = (CLabel*)(m_ui->findWidgetById("atk"));
// 	attack->setString(ToString(hero->atk));
// 
// 	CLabel *deffend = (CLabel*)(m_ui->findWidgetById("def"));
// 	deffend->setString(ToString(hero->def));
// 
// 	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
// 	level->setString(CCString::createWithFormat("%d/%d",hero->level,hero->maxLevel)->getCString());
// 
// 	CLabel *skillLev = (CLabel*)(m_ui->findWidgetById("skillLev"));
// 	skillLev->setString(CCString::createWithFormat("%d/%d",hero->skill3.level,hero->skill3.maxLevel)->getCString());
// 
// 	CLabel *mskillLev = (CLabel*)(m_ui->findWidgetById("mskillLev"));
// 	mskillLev->setString(CCString::createWithFormat("%d/%d",hero->skill6.level,hero->skill6.maxLevel)->getCString());
// 
// 	CLabel *addhp = (CLabel*)(m_ui->findWidgetById("addhp"));
// 
// 	CLabel *addatk = (CLabel*)(m_ui->findWidgetById("addatk"));
// 
// 	CLabel *adddef = (CLabel*)(m_ui->findWidgetById("adddef"));
// 
// 	CLabel *addlevel = (CLabel*)(m_ui->findWidgetById("addlevel"));
// 	CLabel *addskillLev = (CLabel*)(m_ui->findWidgetById("addskillLev"));
// 	CLabel *addmskillLev = (CLabel*)(m_ui->findWidgetById("addmskillLev"));
// 
// 	addhp->setString(ToString(pHero->hp()));
// 	adddef->setString(ToString(pHero->def()));
// 	addatk->setString(ToString(pHero->atk()));
// 	addlevel->setString(CCString::createWithFormat("%d/%d",pHero->level(),pHero->maxlevel())->getCString());
// 	addskillLev->setString(CCString::createWithFormat("%d/%d",pHero->skill3().level(),pHero->skill3().maxlevel())->getCString());
// 
// 	if (pHero->hp()==hero->hp)
// 	{
// 		addhp->setColor(ccWHITE);
// 	}
// 	if (pHero->def()==hero->def)
// 	{
// 		adddef->setColor(ccWHITE);
// 	}
// 	if (pHero->atk()==hero->atk)
// 	{
// 		addatk->setColor(ccWHITE);
// 	}
// 	if (pHero->level()==hero->level)
// 	{
// 		addlevel->setColor(ccWHITE);
// 	}
// 	if (pHero->skill3().level()==hero->skill3.level)
// 	{
// 		addskillLev->setColor(ccWHITE);
// 	}
} 

void CHeroPromote::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout *)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

	m_skillLay = (CLayout *)(m_ui->findWidgetById("skill2"));
	m_skillLay->retain();
	m_skillLay->removeFromParent();

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroPromote::tableviewDataSource));
	m_tableView->reloadData();
}

void CHeroPromote::onExit()
{
	BaseLayer::onExit();
}

CCObject* CHeroPromote::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CHeroPromote::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{		
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i = 1; i <=12; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
	}
	CLabel *name1 = (CLabel*)pCell->findWidgetById("name1");
	CLabel *level1 = (CLabel*)pCell->findWidgetById("level1");
	CLabel *desc1 = (CLabel*)pCell->findWidgetById("desc1");
	CLabel *name2 = (CLabel*)pCell->findWidgetById("name2");
	CLabel *level2 = (CLabel*)pCell->findWidgetById("level2");		
	CLabel *desc2= (CLabel*)pCell->findWidgetById("desc2");

	if (uIdx==0)
	{
		name1->setString(m_hero->skill2.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill2.level)->getCString());
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill2.skillId);
		if (cfg1)
		{
			desc1->setString(cfg1->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(4);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc1->setString(GETLANGSTR(202));
		}
		
		name2->setString(m_reshero.skill2.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill2.level)->getCString());
		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_reshero.skill2.skillId);
		if (cfg2)
		{
			desc2->setString(cfg2->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(5);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc2->setString(GETLANGSTR(202));
		}

		if (m_hero->skill3.id>0)
		{
			addSkill(uIdx,pCell);
		}
	}
	else if (uIdx==1)
	{
		name1->setString(m_hero->skill4.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill4.level)->getCString());
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill4.skillId);
		if (cfg1)
		{
			desc1->setString(cfg1->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(4);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc1->setString(GETLANGSTR(202));
		}
		name2->setString(m_reshero.skill4.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill4.level)->getCString());
		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_reshero.skill4.skillId);
		if (cfg2)
		{
			desc2->setString(cfg2->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = pCell->getChildByTag(5);
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc2->setString(GETLANGSTR(202));
		}
		if (m_hero->skill5.id>0)
		{
			addSkill(uIdx,pCell);
		}
	}
}

void CHeroPromote::addSkill(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_skillLay);
	for (int i = 0; i < m_skillLay->getChildrenCount(); i++)
	{
		CCNode *child = (CCNode*)lay->getChildren()->objectAtIndex(i);
		lay->removeChild(child);
		pCell->addChild(child);
	}
	CLabel *name1 = (CLabel*)pCell->findWidgetById("name3");
	CLabel *level1 = (CLabel*)pCell->findWidgetById("level3");
	CLabel *desc1 = (CLabel*)pCell->findWidgetById("desc3");
	CLabel *name2 = (CLabel*)pCell->findWidgetById("name4");
	CLabel *level2 = (CLabel*)pCell->findWidgetById("level4");		
	CLabel *desc2= (CLabel*)pCell->findWidgetById("desc4");

	if (uIdx==0)
	{
		name1->setString(m_hero->skill3.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill3.level)->getCString());
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill3.skillId);
		if (cfg1)
		{
			desc1->setString(cfg1->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = (CCNode*)pCell->findWidgetById("mask3");
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc1->setString(GETLANGSTR(202));
		}
		name2->setString(m_reshero.skill3.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill3.level)->getCString());
		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_reshero.skill3.skillId);
		if (cfg2)
		{
			desc2->setString(cfg2->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = (CCNode*)pCell->findWidgetById("mask4");
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc2->setString(GETLANGSTR(202));
		}
	}
	else if (uIdx==1)
	{
		name1->setString(m_hero->skill5.name.c_str());
		level1->setString(CCString::createWithFormat("Lv.%d",m_hero->skill5.level)->getCString());
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill5.skillId);
		if (cfg1)
		{
			desc1->setString(cfg1->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = (CCNode*)pCell->findWidgetById("mask3");
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc1->setString(GETLANGSTR(202));
		}
		name2->setString(m_reshero.skill5.name.c_str());
		level2->setString(CCString::createWithFormat("Lv.%d",m_reshero.skill5.level)->getCString());
		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_reshero.skill5.skillId);
		if (cfg2)
		{
			desc2->setString(cfg2->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
			if (spr)
			{	
				CCNode *parent = (CCNode*)pCell->findWidgetById("mask4");
				spr->setPosition(ccp(parent->getContentSize().width/2,parent->getContentSize().height/2));
				parent->addChild(spr);
			}
		}
		else
		{
			desc2->setString(GETLANGSTR(202));
		}
	}
}

bool CHeroPromote::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
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

CHeroPromote::~CHeroPromote()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_skillLay);
}
