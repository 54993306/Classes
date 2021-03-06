﻿#include "SkillTips.h"
#include "Battle/WarControl.h"
#include "common/CommonFunction.h"
#include "Battle/BaseRole.h"
#include "Battle/BattleMessage.h"
#include "Battle/BaseRoleData.h"
#include "Battle/RoleSkill.h"
#include "model/DataCenter.h"
namespace BattleSpace{

	SkillTips::SkillTips()
		:m_ui(nullptr)
	{}

	SkillTips::~SkillTips()
	{}

	void SkillTips::onEnter()
	{
		BaseLayer::onEnter();
		NOTIFICATION->addObserver(this,callfuncO_selector(SkillTips::removeTips),B_RemoveSkillTipsLayer,nullptr);
	}

	void SkillTips::onExit()
	{
		BaseLayer::onExit();
		NOTIFICATION->removeAllObservers(this);
	}

	bool SkillTips::init()
	{
		if (BaseLayer::init())
		{
			MaskLayer* lay = MaskLayer::create("skillTip");
			lay->setIsShowBlack(false);
			lay->setContentSize(CCSizeMake(2824,640));
			LayerManager::instance()->push(lay);
			this->setIsShowBlack(false);
			m_ui = LoadComponent("skillTip.xaml"); 
			m_ui->setPosition(VCENTER);
			this->addChild(m_ui);
			return true;
		}
		return false;
	}

	bool SkillTips::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);
		CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
		if( !res && !bgSpr->boundingBox().containsPoint(pTouch->getLocation()))
		{
			res = true;
			LayerManager::instance()->pop();
			LayerManager::instance()->pop();
		}
		return res;
	}

	void SkillTips::setSkillInfo(CCObject* target,BaseRole* alive)
	{
		if (outPutERRORMsg("SkillTips::setSkillInfo",target))
			return;
		if (outPutERRORMsg("SkillTips::setSkillInfo",alive))
			return;
		CCNode* icon = (CCNode*)target;
		CCPoint point = icon->getPosition();
		CLabel *name = (CLabel*)(m_ui->findWidgetById("name"));
		CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
		CLabel *desc = (CLabel*)(m_ui->findWidgetById("desc"));
		CLabel* level2 = (CLabel*)((m_ui->findWidgetById("level2")));
		level2->setVisible(false);
		this->setVisible(true);
		this->setPosition(ccpAdd(point,ccp(-326,-85)));
		const RoleSkill* skill = alive->getBaseData()->getActiveSkill();
		if (level)
		{
			level->setString(ToString(skill->getSkillLevel()));
			level->setVisible(false);
		}
		const SkillCfg *cfg = DataCenter::sharedData()->getSkill()->getCfg(skill->getSkillID());
		if (cfg)
		{
			desc->setString(cfg->desc.c_str());
			name->setString(cfg->name.c_str());
		}
	}

	void SkillTips::removeTips(CCObject* ob)
	{
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}

};