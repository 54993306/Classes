﻿#include "Battle/BattleLayer/BattleMapLayer.h"
#include "model/DataCenter.h"
#include "tools/ToolDefine.h"
#include "common/CommonFunction.h"
#include "Battle/ConstNum.h"
#include "Battle/RoleObject/RoleObject.h"
#include "tools/commonDef.h"
#include "Battle/BaseRole.h"
#include "Battle/SkillRange.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/MoveRule.h"
#include "Battle/EffectObject.h"
#include "Global.h"
#include "tools/CCShake.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "Battle/MapEffect.h"
#include "Battle/ParseFileData.h"
#include "tools/ShowTexttip.h"
#include "Battle/BattleMessage.h"
#include "Battle/GuardArea.h"
#include "Battle/BaseRoleData.h"
#include "Battle/skEffectData.h"
namespace BattleSpace{

#define AddMoveImg		"lv.png"
#define CutMoveImg		"huang.png"
#define AtksImg			"hong.png"
#define BgImg			"bai.png"
#define MaxBgImg		"maxbg.png"
#define jiaoImg			"wuse.png"
#define LanBg			"lan.png"
#define Blue			"blue.png"

	BattleMapLayer::BattleMapLayer()
		:m_MapLayer(nullptr),m_GridIndex(nullptr),mGuardArea(nullptr)
		,m_SkillRange(nullptr),m_Manage(nullptr),m_BackgroundManage(nullptr)
		,m_DisPlayArea(nullptr),m_BackArea(false),m_FrontArea(false)
	{}
	BattleMapLayer::~BattleMapLayer()
	{
		CC_SAFE_RELEASE(m_BackgroundManage);
		m_BackgroundManage = nullptr;
		CC_SAFE_RELEASE(m_SkillRange);
		m_SkillRange = nullptr;
		m_VecGridIndex.clear();
		CC_SAFE_RELEASE(m_DisPlayArea);
		m_DisPlayArea = nullptr;
		CC_SAFE_RELEASE(m_GridIndex);
		m_GridIndex = nullptr;
		CC_SAFE_RELEASE(mGuardArea);
		mGuardArea = nullptr;
		m_Manage = nullptr;
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("warScene/movebg.plist");
	}

	void BattleMapLayer::onExit()
	{
		CCNode::onExit();
		removeEvent();
	}
	bool BattleMapLayer::init()
	{
		bool res = CCNode::init();
		m_Manage = DataCenter::sharedData()->getWar();
		m_SkillRange = SkillRange::create(m_Manage);
		m_SkillRange->retain();
		mGuardArea = GuardArea::create(m_Manage);
		mGuardArea->retain();

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("warScene/movebg.plist");
		m_DisPlayArea = CCSpriteBatchNode::create("warScene/movebg.png");
		m_DisPlayArea->retain();

		m_GridIndex = CCNode::create();
		m_GridIndex->retain();

		m_BackgroundManage = MapBackgroundManage::create();
		if (m_BackgroundManage)
			m_BackgroundManage->retain();

		createBackImage();
		return res;
	}

	void BattleMapLayer::addEvent()
	{
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::touchAreaCancel),B_CancelCostArea,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::DrawMoveAtkArea),B_RoleAttackCostArea,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::DrawAtkEffect),B_SkilEffectInMap,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::CombatArea),B_DrawSkillArea,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::CancelCombatArea),B_CancelDrawAttackArea,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::SkillBtnDrawSkillArea),B_DrawDynamicSkillArea,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::MapShake),B_Shark,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::CaptainHit),B_CaptainHurt,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleMapLayer::CostAreaTips),B_CostArea,nullptr);
	}
	void BattleMapLayer::removeEvent() { NOTIFICATION->removeAllObservers(this); }

	void BattleMapLayer::touchAreaCancel(CCObject* ob)
	{
		for (auto i:m_VecGridIndex)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i);
			if (sp->isVisible()) sp->setVisible(false);
		}
		m_VecGridIndex.clear();
	}

	void BattleMapLayer::createBackImage()
	{
		WarMapData* map = DataCenter::sharedData()->getMap()->getCurrWarMap();
		if (!map) return;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		for(int i = 0; i < C_GRID_ROW * C_GRID_COL;++i)
#else
		for(int i = C_BEGINGRID; i < C_GRID_ROW * C_GRID_COL;++i)
#endif
		{
			const CCPoint p = map->getPoint(i);		//画战场格子
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32	
			CCLabelTTF* floorID = CCLabelTTF::create(ToString(i),"SimHei",50);
			floorID->setPosition(ccp(p.x,p.y));
			m_GridIndex->addChild(floorID);
			int rowID = i%C_GRID_ROW;
			int colID = i/C_GRID_ROW;
			CCLabelTTF* row = CCLabelTTF::create(CCString::createWithFormat("row=%d", rowID)->getCString(),"SimHei",15);
			row->setPosition(ccp(p.x-30,p.y+38));
			row->setColor(ccc3(0,0,255));
			m_GridIndex->addChild(row);
			CCLabelTTF* col = CCLabelTTF::create(CCString::createWithFormat("col=%d", colID)->getCString(),"SimHei",15);
			col->setPosition(ccp(p.x-30,p.y+20));
			col->setColor(ccc3(0,0,255));
			m_GridIndex->addChild(col);
#endif
			CCSprite* sp = CCSprite::createWithSpriteFrameName(CutMoveImg);
			sp->setTag(i+map_Bg);
			sp->setPosition(ccp(p.x,p.y));
			sp->setVisible(false);	
			m_DisPlayArea->addChild(sp);	
			if (i==C_CAPTAINGRID)
			{
				CCSprite* _sp = CCSprite::createWithSpriteFrameName(MaxBgImg);
				_sp->setPosition(ccp(p.x,p.y));
				_sp->setAnchorPoint(ccp(0.5,0.885f));
				_sp->setVisible(false);
				m_DisPlayArea->addChild(_sp,1,C_ENDGRID+C_CAPTAINGRID);
			}
		}
		CCSprite* guideGrid = CCSprite::createWithSpriteFrameName(LanBg);
		guideGrid->setVisible(false);
		guideGrid->setTag(map_guide);
		CCFadeIn* fin = CCFadeIn::create(0.35f);
		CCFadeOut* fout = CCFadeOut::create(0.5f);
		guideGrid->runAction(CCRepeatForever::create(CCSequence::create(fout,fin,NULL)));
		m_DisPlayArea->addChild(guideGrid);
	}

	void BattleMapLayer::OpenGuide( int gird )
	{
		m_DisPlayArea->getChildByTag(map_guide)->setVisible(true);
		WarMapData* map = DataCenter::sharedData()->getMap()->getCurrWarMap();
		m_DisPlayArea->getChildByTag(map_guide)->setPosition(map->getPoint(gird));
	}

	void BattleMapLayer::CancelGuide() { m_DisPlayArea->getChildByTag(map_guide)->setVisible(false); }

	void BattleMapLayer::DrawAtkArea(BaseRole* alive)
	{
		alive->setTouchState(true);
		vector<int>VecGrid;
		if (alive->getBaseData()->getAlertType() && !alive->getCriAtk())
		{
			mGuardArea->initAliveGuard(alive,VecGrid);
		}else{
			m_SkillRange->initSkillArea(alive,VecGrid);
		}
		for (int grid:VecGrid) 
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(grid+map_Bg);
			if (!sp)continue;
			sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AtksImg));
			sp->setVisible(true);
			m_VecGridIndex.push_back(grid+map_Bg);
			BaseRole* t_alive = m_Manage->getAliveByGrid(grid);
			if (t_alive&&t_alive->getEnemy() != alive->getEnemy())
			{
				if (alive->getOpposite())
					m_BackArea = true;
				else
					m_FrontArea = true;
			}
		}
		for (auto i : alive->mTouchGrids)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i+map_Bg);
			if (!sp)continue;
			sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(Blue));
			sp->setVisible(true);
			m_VecGridIndex.push_back(i+map_Bg);
		}
		alive->setTouchState(false);
	}

	void BattleMapLayer::DrawMoveArea(BaseRole* alive)
	{
		for (auto i : *m_Manage->getMoveVec())
		{
			if (i < C_GRID_ROW+C_BEGINGRID)
				continue;
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i+map_Bg);
			if (!sp)continue;
			if (m_Manage->inAddCostArea(i))
			{
				sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AddMoveImg));
			}else{
				sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CutMoveImg));
			}
			sp->setVisible(true);
			m_VecGridIndex.push_back(i+map_Bg); 
		}
	}

	void BattleMapLayer::DrawMoveAtkArea(CCObject* ob)
	{
		BaseRole* alive = dynamic_cast<BaseRole*>(ob);
		if (!alive)
		{
			CCLOG("[ *ERROR ] WarMapLayer::DrawMoveAtkArea");
			return ;
		}
		touchAreaCancel(nullptr);
		DrawMoveArea(alive);
		DrawAtkArea(alive);
		if (!m_BackArea)
		{
			m_BackArea = true;
			alive->setOpposite(true);
			bool ReverseArea = false;
			alive->setTouchState(true);
			vector<int> tVector;
			m_SkillRange->initSkillArea(alive,tVector);
			for (auto tGrid:tVector)
			{
				BaseRole* pAlive = m_Manage->getAliveByGrid(tGrid);
				if (pAlive&&pAlive->getEnemy() != alive->getEnemy())
					ReverseArea = true;
			}
			if (ReverseArea)
				DrawMoveAtkArea(alive);	
			alive->setOpposite(false);
			alive->setTouchState(false);
		}
		m_BackArea = false;
	}
	//in map draw skill effect
	void BattleMapLayer::DrawAtkEffect(CCObject* ob)
	{
		RoleObject* act = (RoleObject*)ob;
		BaseRole* alive = act->getAlive();
		for (auto i:alive->mSkillArea)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i+map_Bg);
			EffectObject* Effect = EffectObject::create(ToString(act->getPlayerEffect()));
			int x = sp->getPosition().x - sp->getContentSize().width/4;
			if (alive->getEnemy()) x = sp->getPosition().x + sp->getContentSize().width/4;
			int y = sp->getPosition().y - sp->getContentSize().height*1.5f;
			Effect->setEffAnchorPoint(0.5f,0);
			Effect->setPosition(ccp(x,y));
			this->addChild(Effect);
			Effect->setShaderEffect(act->getArmature()->getShaderProgram());
			Effect->setMusic(act->getEffectMusic());
			if (sp->isVisible()) sp->setVisible(false);
		}
		act->setEffectMusic(0);
		act->setPlayerEffect(0);
	}
	//现在攻击的情况已经不绘制攻击范围了
	void BattleMapLayer::CancelCombatArea(CCObject* ob)
	{
		BaseRole* alive = (BaseRole*)ob;
		for (auto tGrid : alive->mSkillArea)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(tGrid+map_Bg);
			sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AtksImg));
			sp->setVisible(false);
		}
	}

	void BattleMapLayer::CombatArea(CCObject* ob)
	{
		BaseRole* alive = (BaseRole*)ob;
		for (auto i:alive->mSkillArea)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i+map_Bg);
			sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AtksImg));
			sp->setVisible(true);
		}
	}

	void BattleMapLayer::SkillBtnDrawSkillArea(CCObject* ob)
	{
		touchAreaCancel(nullptr);
		m_FrontArea = false;
		m_BackArea = false;
		BaseRole* alive = (BaseRole*)ob;
		alive->setCriAtk(true);
		DrawAtkArea(alive);		//先判断前方是否有敌人
		if (m_FrontArea)
		{
			m_FrontArea = false;
			alive->setCriAtk(false);
			return;
		}
		touchAreaCancel(nullptr);
		alive->setOpposite(true);
		DrawAtkArea(alive);		//判断后方是否有敌人
		alive->setOpposite(false);
		if (m_BackArea)
		{
			m_BackArea = false;
			alive->setCriAtk(false);
			return;
		}
		touchAreaCancel(nullptr);
		DrawAtkArea(alive);		//都没有绘制前方攻击范围
		alive->setCriAtk(false);
	}

	void BattleMapLayer::MapShake(CCObject* ob)
	{
		this->stopAllActions();		//有可能的情况是,没有执行到回调CCPlace就停止掉动作了	
		this->setPosition(ccp(0,0));
		CCPoint p = this->getPosition();
		this->runAction(CCSequence::create(CCShake::create(0.4f,13.0f),CCPlace::create(p),NULL));
	}

	MapBackgroundManage* BattleMapLayer::getBackgroundManage()
	{
		if (m_BackgroundManage)
			return m_BackgroundManage;
		return nullptr;
	}

	void BattleMapLayer::setMaplayer(CLayout* var)
	{
		m_MapLayer = var;
		CLayout *Floor = dynamic_cast<CLayout*>(m_MapLayer->findWidgetById("jinjing"));
		if (Floor)
		{
			Floor->addChild(m_DisPlayArea);
			Floor->addChild(m_GridIndex);
		}else{ CCLOG("[ *ERROR ] WarMapLayer::setMaplayer"); }
		this->addChild(m_MapLayer,-2);
	}

	CLayout* BattleMapLayer::getMaplayer(){return m_MapLayer;}
	//敌方释放技能预警
	void BattleMapLayer::DrawWarningEffect( vector<int>& pVector )
	{
		for (auto tGrid:pVector)
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(tGrid + map_Bg);
			if(!sp)
				continue;
			CCSprite* pNewSp = CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AtksImg));
			pNewSp->setPosition(sp->getPosition());
			sp->getParent()->addChild(pNewSp, sp->getZOrder()+1);
			pNewSp->runAction(CCSequence::create(
				CCRepeat::create(CCSequence::createWithTwoActions(CCTintTo::create(0.15f, 255, 0, 0), CCTintTo::create(0.15f, 255, 255, 255)), 6),
				CCRemoveSelf::create(),
				nullptr
				));
		}
	}
	//显示脚底格子
	void BattleMapLayer::CaptainHit( CCObject* ob )
	{
		CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(C_ENDGRID+C_CAPTAINGRID);
		sp->stopAllActions();
		sp->setVisible(true);
		sp->setOpacity(255);
		sp->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCFadeOut::create(0.5f),CCHide::create(),NULL));
	}

	void BattleMapLayer::CostAreaTips( CCObject* ob )
	{
		int type = ((CCInteger*)ob)->getValue();
		enum BlinkArea
		{
			green = 1,
			yellow,
			cancel,
		};
		for (auto i : *m_Manage->getMoveVec())
		{
			CCSprite* sp = (CCSprite*)m_DisPlayArea->getChildByTag(i+map_Bg);
			if (!sp)continue;
			sp->stopAllActions();
			sp->setVisible(false);
			if (m_Manage->inAddCostArea(i))
			{
				sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(AddMoveImg));
				sp->setVisible(type == green);
			}else{
				sp->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CutMoveImg));
				sp->setVisible(type == yellow);
			}
			if (sp->isVisible())
				sp->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeOut::create(0.25f), CCFadeIn::create(0.25f))));
		}
	}
};