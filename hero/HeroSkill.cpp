#include "HeroSkill.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "SkillDesc.h"
#include "tools/ShowTexttip.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "scene/AnimationManager.h"
using namespace BattleSpace;
bool CHeroSkill::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		m_ui = LoadComponent("Heroskill.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CHeroSkill::onClickSkill(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CCNode *node = (CCNode*)pSender;
    CSkill *skill = (CSkill*)node->getUserData();

	//是否加锁
	if(m_hero->quality < 3 && &m_hero->skill2 == skill)
	{
		ShowPopTextTip(GETLANGSTR(1157));
		return;
	}

	//bool isPasssive = ((CCBool*)node->getUserObject())->getValue();	
	//int idx = ((CTableViewCell*)node->getParent())->getIdx();

	//CSkillDesc *skillDesc= CSkillDesc::create();
	//LayerManager::instance()->push(skillDesc);
	//skillDesc->updateSkillInfo(skill,isPasssive, idx);

}

void CHeroSkill::showSkill(CHero* hero, int type)
{
	m_hero = hero;
	m_type = type;
	m_tableView->setCountOfCell(2);
	m_tableView->reloadData();
}

void CHeroSkill::onEnter()
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
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroSkill::tableviewDataSource));
	m_tableView->reloadData();
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroSkill::updateHeroInfo));
}

CCObject* CHeroSkill::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CHeroSkill::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{		
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	while (lay->getChildrenCount()>0)
	{
		CCNode * node = (CCNode*)lay->getChildren()->objectAtIndex(0);
		lay->removeChild(node);
		pCell->addChild(node);
	}
	CCNode *cost1 = (CCNode*)pCell->findWidgetById("cost");
	CLabel *desc1 = (CLabel*)pCell->findWidgetById("desc1");
	CLabel *name1 = (CLabel*)pCell->findWidgetById("name1");
	CLabel *level1 = (CLabel*)pCell->findWidgetById("level1");

	CImageViewScale9 *bg1 = (CImageViewScale9*)pCell->findWidgetById("bg1");
	CImageViewScale9 *bg2 = (CImageViewScale9*)pCell->findWidgetById("bg2");
	bg1->setTouchEnabled(true);
	bg1->setOnClickListener(this,ccw_click_selector(CHeroSkill::onClickSkill));	
	bg2->setTouchEnabled(true);
	bg2->setOnClickListener(this,ccw_click_selector(CHeroSkill::onClickSkill));

	CLabel *name2 = (CLabel*)pCell->findWidgetById("name2");

	CCSprite *mask1= (CCSprite*)pCell->findWidgetById("mask1");
	CCSprite *mask2= (CCSprite*)pCell->findWidgetById("mask2");
	CLabel *desc2 = (CLabel*)pCell->findWidgetById("desc2");
	CImageView *lock = (CImageView*)pCell->findWidgetById("_lock");


	if (uIdx==1)
	{
		level1->setString(CCString::createWithFormat("%d/%d",m_hero->skill3.level,m_hero->skill3.maxLevel)->getCString());
		bg1->setUserData(&m_hero->skill3);
		bg1->setUserObject(CCBool::create(true));
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill3.skillId);
		if(cfg1)
			name1->setString(cfg1->name.c_str());
		//cost
		addFireAndNum(cost1, m_hero->skill3.cost);

		//描述
		if(cfg1)
			desc1->setString(cfg1->desc.c_str());

		bg2->setUserData(&m_hero->skill2);
		bg2->setUserObject(CCBool::create(false));

		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill2.skillId);
		CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
		if(!spr)
		{
			CCLOG("ERROR  CHeroSkill::addTableCell");
			spr  = CImageView::create("skillIcon/100000.png");
		}
		spr->setPosition(ccp(mask1->getContentSize().width/2,mask1->getContentSize().height/2));
		spr->setScale(0.9f);
		mask1->addChild(spr);
		if(cfg2)
			name2->setString(cfg2->name.c_str());

		spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
		if(!spr)
		{
			CCLOG("ERROR  CHeroSkill::addTableCell");
			spr  = CImageView::create("skillIcon/100000.png");
		}
		spr->setPosition(ccp(mask2->getContentSize().width/2,mask2->getContentSize().height/2));
		spr->setScale(0.9f);
		mask2->addChild(spr);

		//描述
		if(cfg2)
			desc2->setString(cfg2->desc.c_str());

		//是否加锁
		if(m_hero->quality < 3)
		{
			lock->setVisible(true);
			desc2->setString(GETLANGSTR(1157));
		}
	}
	else if (uIdx==0)
	{
		CLabel *skillType = (CLabel*)(pCell->findWidgetById("type"));
		skillType->setString(GETLANGSTR(273));
		
		level1->setString(CCString::createWithFormat("%d/%d",m_hero->skill4.level,m_hero->skill4.maxLevel)->getCString());
		bg1->setUserData(&m_hero->skill4);
		bg1->setUserObject(CCBool::create(true));
		const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill4.skillId);
		if (cfg1)
		{
// 			desc1->setString(cfg1->desc.c_str());
			CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
			if(!spr)
			{
				CCLOG("ERROR  CHeroSkill::addTableCell");
				spr  = CImageView::create("skillIcon/100000.png");
			}	
			spr->setScale(0.9f);
			spr->setPosition(ccp(mask1->getContentSize().width/2,mask1->getContentSize().height/2));
			mask1->addChild(spr);
			name1->setString(cfg1->name.c_str());
		}
		else
		{
// 			desc1->setString(GETLANGSTR(202));
		}

		//cost
		addFireAndNum(cost1, m_hero->skill4.cost);

		//描述
		if(cfg1)
			desc1->setString(cfg1->desc.c_str());
		
		bg2->setUserData(&m_hero->skill5);
		bg2->setUserObject(CCBool::create(false));

		const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill5.skillId);
		if (cfg2)
		{
// 			desc2->setString(cfg2->desc.c_str());
			CCSprite* spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg2->icon)->getCString());
			if(!spr)
			{
				CCLOG("ERROR  CHeroSkill::addTableCell");
				spr  = CImageView::create("skillIcon/100000.png");
			}	
			spr->setPosition(ccp(mask2->getContentSize().width/2,mask2->getContentSize().height/2));
			spr->setScale(0.9f);
			mask2->addChild(spr);
			name2->setString(cfg2->name.c_str());

			//描述
			if(cfg2)
				desc2->setString(cfg2->desc.c_str());
		}
		else
		{
// 			desc2->setString(GETLANGSTR(202));
		}

	}
}

void CHeroSkill::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	showSkill(hero,m_type);
}

void CHeroSkill::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
}

void CHeroSkill::addFireAndNum( CCNode* pNode, int iNum )
{
	//cost消耗
	CCNode* pCost = pNode;
	//添加火和数字
	if(pCost->getChildByTag(1)==nullptr)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9051.plist");//将plist文件加载进入缓存
		AnimationManager::sharedAction()->ParseAnimation("9051");
		CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9051");
		culAnim->setDelayPerUnit(0.1f);
		CCAnimate* pAnimate = CCAnimate::create(culAnim);
		CCSprite* pFire  = CCSprite::create();
		pFire->setPosition(ccp(pCost->getContentSize().width/2, pCost->getContentSize().height/2+5));
		pCost->addChild(pFire, 1, 1);
		pFire->runAction(CCRepeatForever::create(pAnimate));
	}
	pCost->removeChildByTag(2);
	CCLabelAtlas* pCostAtlas = CCLabelAtlas::create(ToString(iNum), "label/number2.png", 26, 32, 46);
	pCostAtlas->setAnchorPoint(ccp(0.5f, 0.5f));
	pCostAtlas->setScale(0.9f);
	pCostAtlas->setPosition(ccp(pCost->getContentSize().width/2, pCost->getContentSize().height/2));
	pCost->addChild(pCostAtlas, 2, 2);
}

