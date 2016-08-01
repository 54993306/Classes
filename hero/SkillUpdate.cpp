#include "SkillUpdate.h"
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
#include "Battle/AnimationManager.h"
#include "bag/bagData.h"
#include "common/ShaderDataHelper.h"
using namespace BattleSpace;

CSkillUpdate::CSkillUpdate():m_skillIndex(0),m_selectItem(nullptr),m_offsetX1(0),m_offsetX2(0)
{

}

bool CSkillUpdate::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("skillMaskLayer");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		m_ui = LoadComponent("SkillUpdate.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;
}

void CSkillUpdate::heroSkill(CHero *hero, CSkill *skill)
{
	PlayEffectSound(SFX_Button);

	CCNode *cost = m_ui->findWidgetById("cost");
	CCNode *cost3 = m_ui->findWidgetById("cost3");

	addFireAndNum(cost, skill->cost);
	addFireAndNum(cost3, skill->cost);

	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("heroEvolve/evolveSkill.ExportJson");
	CCArmature* Armature = CCArmature::create("evolveSkill");
	if (Armature)
	{	
		m_ui->addChild(Armature);
		Armature->setPosition(VCENTER);
		Armature->setVisible(false);
		Armature->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f),CCCallFuncN::create(this,callfuncN_selector(CSkillUpdate::showArmature))));
	} 

	CLabel *desc1 = (CLabel*)m_ui->findWidgetById("desc1");
	CLabel *name1 = (CLabel*)m_ui->findWidgetById("name1");
	CLabel *level = (CLabel*)m_ui->findWidgetById("level");

	CLabel *desc2 = (CLabel*)m_ui->findWidgetById("desc2");
	CLabel *name2 = (CLabel*)m_ui->findWidgetById("name2");
	CLabel *level1 = (CLabel*)m_ui->findWidgetById("level1");

	level->setString(CCString::createWithFormat("Lv.%d",skill->level)->getCString());
	level1->setString(CCString::createWithFormat("Lv.%d",skill->level+1)->getCString());
	const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(skill->skillId/*+skill->level*/);
	if(cfg1)
	{
		name1->setString(cfg1->name.c_str());
		desc1->setString(cfg1->desc.c_str());
	}

	const SkillCfg *cfg2 = DataCenter::sharedData()->getSkill()->getCfg(skill->skillId/*+skill->level+1*/);
	if(cfg2)
	{
		name2->setString(cfg2->name.c_str());
		desc2->setString(cfg2->desc.c_str());
	}
	
	CCNode *mask1 = (CCNode*)(m_ui->findWidgetById("mask_1"));
	CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
	if(!spr)
	{
		CCLOG("ERROR  CHeroSkill::addTableCell");
		spr  = CImageView::create("skillIcon/100000.png");
	}
	spr->setPosition(ccp(mask1->getContentSize().width/2,mask1->getContentSize().height/2));
	spr->setScale(0.81f);
	mask1->addChild(spr);

	CCNode *mask2 = (CCNode*)(m_ui->findWidgetById("mask_2"));
	spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
	if(!spr)
	{
		CCLOG("ERROR  CHeroSkill::addTableCell");
		spr  = CImageView::create("skillIcon/100000.png");
	}
	spr->setPosition(ccp(mask2->getContentSize().width/2,mask2->getContentSize().height/2));
	spr->setScale(0.81f);
	mask2->addChild(spr);

	CLayout *skill1 = (CLayout*)(m_ui->findWidgetById("skill1"));
	skill1->setPositionY(skill1->getPositionY()-370);
	skill1->setScale(1.2f);
	CLayout *skill2 = (CLayout*)(m_ui->findWidgetById("skill2"));
	skill2->setPositionY(skill2->getPositionY()-370);
	skill2->setScale(1.2f);
	skill1->runAction(CCSequence::create(CCEaseBackIn::create(CCMoveBy::create(0.2f,ccp(0,370))),CCScaleTo::create(0.2f,1.0f),nullptr));
	skill2->runAction(CCSequence::create(
		CCEaseBackIn::create(
			CCSpawn::createWithTwoActions(
				CCMoveBy::create(0.2f,ccp(0,370)),
				CCScaleTo::create(0.2f,1.0f))),
		CCDelayTime::create(0.3f),
		CCEaseBackIn::create(CCMoveBy::create(0.2f,ccp(0,139))),
		nullptr));

	this->scheduleOnce(schedule_selector(CSkillUpdate::skillActionExit),3.0f);
}

void CSkillUpdate::skillActionExit(float dt)
{	
	this->unschedule(schedule_selector(CSkillUpdate::skillActionExit));
	this->stopAllActions();
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSkillUpdate::showSkill(CHero* hero, int type)
{
 	m_hero = hero;
}

void CSkillUpdate::showArmature(CCNode* node)
{
	CCArmature *arma = (CCArmature*)node;
	arma->setVisible(true);
	arma->getAnimation()->play("start",-1,-1,0);
}

void CSkillUpdate::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout *)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

}

void CSkillUpdate::processMsg(int type, google::protobuf::Message *msg)
{
	
}

void CSkillUpdate::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
}

void CSkillUpdate::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CSkillUpdate::addFireAndNum( CCNode* pNode, int iNum )
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

void CSkillUpdate::levelUp(bool isSuccess)
{
	m_selectItem->itemNum--;
	if (m_selectItem->itemNum>0)
	{
		m_tableView->reloadData();
	}
	else
	{
		for (int i = 0; i < m_bagData.itemList.size(); i++)
		{
			if (m_bagData.itemList.at(i).itemId==m_selectItem->itemId)
			{
				m_bagData.itemList.erase(m_bagData.itemList.begin()+i);
				break;
			}
		}
		m_tableView->setCountOfCell(m_bagData.itemList.size());
		m_tableView->reloadData();
	}

	if (isSuccess)
	{
		switch (m_skillIndex)
		{
		case 3:
			{
				m_hero->skill3.level++;
				CLabel *level = (CLabel*)m_ui->findWidgetById("level");
				level->setString(CCString::createWithFormat("%d/%d",m_hero->skill3.level,m_hero->skill3.maxLevel)->getCString());
			}
			break;
		case 4:
			{
				m_hero->skill4.level++;
				CLabel *level = (CLabel*)m_ui->findWidgetById("level");
				level->setString(CCString::createWithFormat("%d/%d",m_hero->skill4.level,m_hero->skill4.maxLevel)->getCString());
			}
			break;
		default:
			break;
		}
		skillLevel();
		ShowPopTextTip(GETLANGSTR(243));
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(2014));
	}
}

void CSkillUpdate::showSkillIcon(CSkill *skill, CImageView * mask1)
{
	if (skill->skillId<=0)
	{
		return;
	}
	const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(skill->skillId);
	CImageView *spr = CImageView::create(CCString::createWithFormat("skillIcon/%d.png",cfg1->icon)->getCString());
	if(!spr)
	{
		CCLOG("ERROR  CHeroSkill::addTableCell");
		spr  = CImageView::create("skillIcon/100000.png");
	}
	spr->setPosition(ccpAdd(m_ui->convertToNodeSpace(mask1->getPosition()),ccp(mask1->getContentSize().width/2-5,mask1->getContentSize().height/2-5)));
	spr->setUserData(skill);
	spr->setTouchEnabled(true);
	spr->setScale(0.81f);
	spr->setId("skillIcon");
	m_ui->addChild(spr);

	if (skill->skillId==m_hero->skill3.skillId)
	{
	}
	if (skill->skillId == m_hero->skill2.skillId&&m_hero->quality<2)
	{
		spr->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	}
	if (skill->skillId==m_hero->skill6.skillId&&m_hero->quality<4)
	{
		spr->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	}
}

void CSkillUpdate::showOneStarSkill(CLayout * star1)
{
	CImageView *mask1 = (CImageView*)star1->findWidgetById("mask_1");
	CImageView *mask2 = (CImageView*)star1->findWidgetById("mask_2");
	CImageView *mask3 = (CImageView*)star1->findWidgetById("mask_3");
	showSkillIcon(&m_hero->skill5, mask1);
	showSkillIcon(&m_hero->skill3, mask2);
	showSkillIcon(&m_hero->skill4, mask3);
	skillLevel();
	CCNode *itembg_1 = star1->findWidgetById("itembg_1");
	itembg_1->setVisible(m_hero->skill5.skillId>0);
	mask1->setVisible(m_hero->skill5.skillId>0);
}

void CSkillUpdate::onLvUp(CCObject* pSender)
{
	CLayout *item = (CLayout*)(m_ui->findWidgetById("item"));
	CLayout *skill = (CLayout*)(m_ui->findWidgetById("skill"));

	float offset = 110;
    if (!item->isVisible())
    {
// 		skill->setPositionY(skill->getPositionY() + offset);	
// 		item->setVisible(true);
		CPlayerControl::getInstance().sendRoleBag(3,false,m_hero->heroid);
    }
	else
	{
		skill->setPositionY(skill->getPositionY() - offset);
		item->setVisible(false);
	}
}

void CSkillUpdate::onSkillItem(CCObject* pSender)
{
	CImageView *itemspr = (CImageView*)pSender;
	m_selectItem =(CItem*)itemspr->getUserData();
	CPlayerControl::getInstance().sendSkillLvUp(m_hero->id,m_skillIndex,m_selectItem->id);
}

void CSkillUpdate::removeSkillIcon()
{
	CCNode *skillIcon = m_ui->findWidgetById("skillIcon");
	while (skillIcon)
	{
		skillIcon->removeFromParent();
		skillIcon = m_ui->findWidgetById("skillIcon");
	}
}

void CSkillUpdate::nodeOffset(CCNode * parent, float offset)
{
	for (int i = 0; i < parent->getChildrenCount(); i++)
	{
		CCNode *child = (CCNode*)parent->getChildren()->objectAtIndex(i);
		child->setPositionX(child->getPositionX()+offset);
	}
}

void CSkillUpdate::skillLevel()
{
	CLabel *level3 = (CLabel*)(m_ui->findWidgetById("lev3"));
	CLabel *level4 = (CLabel*)(m_ui->findWidgetById("lev4"));
	level3->setString(CCString::createWithFormat("%d/%d",m_hero->skill3.level,m_hero->skill3.maxLevel)->getCString());
	level4->setString(CCString::createWithFormat("%d/%d",m_hero->skill4.level,m_hero->skill4.maxLevel)->getCString());	
	CCNode *levelup = m_ui->findWidgetById("levelup");		
	CCNode *wrong_tip = m_ui->findWidgetById("wrong_tip");

	CLayout *item = (CLayout*)(m_ui->findWidgetById("item"));
	CLayout *skill = (CLayout*)(m_ui->findWidgetById("skill"));

	if (m_skillIndex==3)
	{
		levelup->setVisible(m_hero->skill3.level<m_hero->skill3.maxLevel);
		wrong_tip->setVisible(m_hero->skill3.level<m_hero->skill3.maxLevel);
		if (m_hero->skill3.level==m_hero->skill3.maxLevel&&item->isVisible())
		{
			skill->setPositionY(skill->getPositionY() - 110);
			item->setVisible(false);
		}
	}
	if (m_skillIndex==4)
	{
		levelup->setVisible(m_hero->skill4.level<m_hero->skill4.maxLevel);
		wrong_tip->setVisible(m_hero->skill4.level<m_hero->skill4.maxLevel);
		if (m_hero->skill4.level==m_hero->skill4.maxLevel&&item->isVisible())
		{
			skill->setPositionY(skill->getPositionY() - 110);
			item->setVisible(false);
		}
	}
}

