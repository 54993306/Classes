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
#include "Battle/AnimationManager.h"
#include "bag/bagData.h"
#include "common/ShaderDataHelper.h"
#include "SkillUpdate.h"
#include "tollgate/ItemInfo.h"
using namespace BattleSpace;

CHeroSkill::CHeroSkill():m_skillIndex(0),m_selectItem(nullptr),m_offsetX1(0),m_offsetX2(0),m_pItemInfo(nullptr),m_ui_special(nullptr)
{

}

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
	CCNode *node = (CCNode*)pSender;

	if(node->isVisible())
	{
		PlayEffectSound(SFX_Button);
	}
	
	CSkill *skill= (CSkill*)node->getUserData();
	CCNode *cost = m_ui->findWidgetById("cost");

	if (skill->skillId == m_hero->skill2.skillId )
	{
		m_skillIndex = 2;
	}
	else if (skill->skillId == m_hero->skill3.skillId )
	{
		m_skillIndex = 3;
		addFireAndNum(cost, m_hero->skill3.cost);
	}
	else if (skill->skillId == m_hero->skill4.skillId )
	{
		m_skillIndex = 4;
		addFireAndNum(cost, m_hero->skill4.cost);
	}
	else if (skill->skillId == m_hero->skill5.skillId )
	{
		m_skillIndex = 5;
	}
	else if (skill->skillId==m_hero->skill6.skillId)
	{
		m_skillIndex = 6;
	}
	cost->setVisible(m_skillIndex==3||m_skillIndex==4);

	CLayout *item = (CLayout*)(m_ui->findWidgetById("item"));
	if (item->isVisible())
	{	
		CLayout *skillLay = (CLayout*)(m_ui->findWidgetById("skill"));
		float offset = 110;
		skillLay->setPositionY(skillLay->getPositionY() - offset);	
		item->setVisible(false);
	}
	
	CLabel *desc2 = (CLabel*)m_ui->findWidgetById("desc2");
	CLabel *name2 = (CLabel*)m_ui->findWidgetById("name2");
	CLabel *level = (CLabel*)m_ui->findWidgetById("level");
	showSkillType();


	if (m_skillIndex==3||m_skillIndex==4)
	{
		level->setString(CCString::createWithFormat("Lv.%d",skill->level)->getCString());
	}
	else
	{
		level->setString("");
	}

	const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(skill->skillId/*+skill->level*/);
	if(cfg1)
	{
		name2->setString(cfg1->name.c_str());
		desc2->setString(cfg1->desc.c_str());
	}

	m_selectImg->removeFromParent();
	m_selectImg->setPosition(node->getPosition());
	m_selectImg->setPositionY(node->getPositionY()-node->getContentSize().height/2);
	node->getParent()->addChild(m_selectImg,1);

	CCNode *levelup = m_ui->findWidgetById("levelup");
	levelup->setVisible((m_skillIndex==3||m_skillIndex==4));
	CCNode *wrong_tip = m_ui->findWidgetById("wrong_tip");
	wrong_tip->setVisible(m_skillIndex==3||m_skillIndex==4);

	skillLevel();

	//是否加锁
	if(m_hero->quality < 2 && m_hero->skill2.skillId == skill->skillId)
	{
		desc2->setString(CCString::createWithFormat(GETLANGSTR(2012),2)->getCString());
	}
	else if (m_hero->quality<4 &&m_hero->skill6.skillId == skill->skillId)
	{
		desc2->setString(CCString::createWithFormat(GETLANGSTR(2012),4)->getCString());
	}
// 	CSkillUpdate *su = CSkillUpdate::create();
// 	LayerManager::instance()->push(su);
// 	su->heroSkill(m_hero,&m_hero->skill3);
}

void CHeroSkill::showSkill(CHero* hero, int type)
{
 	m_hero = hero;

	//如果是五星英雄
	showSpeciaBtn(m_hero->iColor==5);

	if (this->isVisible())
	{
		GetTcpNet->registerMsgHandler(RoleBag,this,CMsgHandler_selector(CHeroSkill::processMsg));
	}

	CLayout *star1 = (CLayout*)(m_ui->findWidgetById("star1"));
	CLayout *star2 = (CLayout*)(m_ui->findWidgetById("star2"));
	CLayout *star3 = (CLayout*)(m_ui->findWidgetById("star3"));
	CLayout *levtip = (CLayout*)(m_ui->findWidgetById("levtip"));
	star1->setVisible(hero->iColor>=1);
	star2->setVisible(hero->iColor>=2);
    star3->setVisible(hero->iColor>=4);
	removeSkillIcon();

	if (m_offsetX1!=0)
	{
		nodeOffset(star1,-m_offsetX1);
		nodeOffset(levtip,-m_offsetX1);
	}
	nodeOffset(star2,-m_offsetX2);

	switch (hero->iColor)
	{
	case 1:
		{
			m_offsetX1 = 120;
			m_offsetX2 = 0;
			nodeOffset(star1,m_offsetX1);
			nodeOffset(levtip,m_offsetX1);
			showOneStarSkill(star1);
		}
		break;
	case 2:
	case 3:
		{
			m_offsetX1 = 70;
			m_offsetX2 = 70;
			nodeOffset(star1,m_offsetX1);
			nodeOffset(levtip,m_offsetX1);
			nodeOffset(star2,m_offsetX2);
			showOneStarSkill(star1);
			CImageView *mask4 = (CImageView*)star2->findWidgetById("mask_4");
			showSkillIcon(&m_hero->skill2, mask4);
		}
		break;
	case 4:
	case 5:
		{
			m_offsetX1 = 0;
			m_offsetX2 = 0;
			showOneStarSkill(star1);
			CImageView *mask4 = (CImageView*)star2->findWidgetById("mask_4");
			showSkillIcon(&m_hero->skill2, mask4);
			CImageView *mask5 = (CImageView*)star3->findWidgetById("mask_5");
			//写死80000
			m_hero->skill6.skillId = 80000;
			showSkillIcon(&m_hero->skill6, mask5);
		}
		break;
	default:
		break;
	}
	
}

void CHeroSkill::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout *)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

	CButton *levelup =(CButton*)m_ui->findWidgetById("levelup");
	levelup->setOnClickListener(this,ccw_click_selector(CHeroSkill::onLvUp));

	m_selectImg = (CCSprite*)(m_ui->findWidgetById("selectImg"));
	m_selectImg->retain();

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionHorizontal);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroSkill::tableviewDataSource));
	m_tableView->reloadData();
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this,GameMsghandler_selector(CHeroSkill::updateHeroInfo));
	GetTcpNet->registerMsgHandler(RoleBag,this,CMsgHandler_selector(CHeroSkill::processMsg));
	CCNode *light = (CCNode *)(m_ui->findWidgetById("light"));
    m_point = light->getPosition();
}

void CHeroSkill::processMsg(int type, google::protobuf::Message *msg)
{
	WareHouseResponse *res = (WareHouseResponse*)msg;
	m_bagData.itemList.clear();
	m_bagData.read(*res);
	m_tableView->setCountOfCell(m_bagData.itemList.size());
	m_tableView->reloadData();
	if (m_bagData.itemList.size()>0)
	{
		CLayout *item = (CLayout*)(m_ui->findWidgetById("item"));
		CLayout *skill = (CLayout*)(m_ui->findWidgetById("skill"));
		if (!item->isVisible())
		{
			skill->setPositionY(skill->getPositionY() + 110);	
			item->setVisible(true);
		}
	}
	else
	{
		ShowPopTextTip(GETLANGSTR(2013));
	}
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
	CItem *item = &m_bagData.itemList.at(uIdx);
	while (lay->getChildrenCount()>0)
	{
		CCNode * node = (CCNode*)lay->getChildren()->objectAtIndex(0);
		lay->removeChild(node);
		pCell->addChild(node);

		switch (node->getTag())
		{
		case 1:
			{
			}
			break;
		case 2:
			{
				CImageView *mask = (CImageView*)node;
				mask->setTexture(setItemQualityTexture(item->quality));
				mask->setUserData(item);
				mask->setTouchEnabled(true);
				mask->setOnClickListener(this,ccw_click_selector(CHeroSkill::onSkillItem));
				mask->setOnPressListener(this,ccw_press_selector(CHeroSkill::onPress));
				CImageView *prop = getItemSprite(item);
				prop->setScale(0.85f);
				prop->setPosition(ccp(mask->getContentSize().width/2,mask->getContentSize().height/2));
				mask->addChild(prop);
			}
			break;
		case 3:
			{
				CLabel *haveNum = (CLabel*)node;
				haveNum->setString(ToString(item->itemNum));
			}
			break;
		default:
			break;
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
	GetTcpNet->unRegisterAllMsgHandler(this);
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

void CHeroSkill::levelUp(bool isSuccess)
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
				CSkillUpdate *su = CSkillUpdate::create();
				LayerManager::instance()->push(su);
				su->heroSkill(m_hero,&m_hero->skill3);
				m_hero->skill3.level++;
				CLabel *level = (CLabel*)m_ui->findWidgetById("level");
				level->setString(CCString::createWithFormat("Lv.%d",m_hero->skill3.level)->getCString());
				CLabel *desc = (CLabel*)m_ui->findWidgetById("desc2");
				const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill3.skillId/*+m_hero->skill3.level*/);
				if(cfg1)
				{
					desc->setString(cfg1->desc.c_str());
				}
			}
			break;
		case 4:
			{
				CSkillUpdate *su = CSkillUpdate::create();
				LayerManager::instance()->push(su);
				su->heroSkill(m_hero,&m_hero->skill4);
				m_hero->skill4.level++;
				CLabel *level = (CLabel*)m_ui->findWidgetById("level");
				level->setString(CCString::createWithFormat("Lv.%d",m_hero->skill4.level)->getCString());
				CLabel *desc = (CLabel*)m_ui->findWidgetById("desc2");
				const SkillCfg *cfg1 = DataCenter::sharedData()->getSkill()->getCfg(m_hero->skill4.skillId/*+m_hero->skill4.level*/);
				if(cfg1)
				{
					desc->setString(cfg1->desc.c_str());
				}
			}
			break;
		default:
			break;
		}
		skillLevel();
	}
	else
	{
		this->scheduleOnce(schedule_selector(CHeroSkill::failAction),0.5f);
	}
}

void CHeroSkill::failAction(float dt)
{
	CCSprite *spr = CCSprite::create("hero/upgrade_lost.png");
	spr->setPositionX(400);
	spr->setPositionY(190);
// 	spr->setScale(2.0f);
	m_ui->addChild(spr,10);
	spr->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(1.2f,ccp(0,270))),CCRemoveSelf::create()));
}

void CHeroSkill::showSkillIcon(CSkill *skill, CImageView * mask1)
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
	spr->setOnClickListener(this,ccw_click_selector(CHeroSkill::onClickSkill));
	spr->setUserData(skill);
	spr->setTouchEnabled(true);
	spr->setScale(0.81f);
	spr->setId("skillIcon");
	m_ui->addChild(spr);

	if (skill->skillId==m_hero->skill3.skillId)
	{
		spr->setVisible(false);
		onClickSkill(spr);
		spr->setVisible(true);
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

void CHeroSkill::showOneStarSkill(CLayout * star1)
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

void CHeroSkill::onLvUp(CCObject* pSender)
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

void CHeroSkill::onSkillItem(CCObject* pSender)
{
	CImageView *itemspr = (CImageView*)pSender;
	m_selectItem =(CItem*)itemspr->getUserData();
	CPlayerControl::getInstance().sendSkillLvUp(m_hero->id,m_skillIndex,m_selectItem->id);
	CCNode *light = (CCNode *)(m_ui->findWidgetById("light"));
// 	light->setScale(1.1f);
	light->setPosition(m_point);
	light->runAction(CCSequence::create(CCShow::create(),CCMoveBy::create(0.45f,ccp(225,0)),CCHide::create(),nullptr));
}

void CHeroSkill::removeSkillIcon()
{
	CCNode *skillIcon = m_ui->findWidgetById("skillIcon");
	while (skillIcon)
	{
		skillIcon->removeFromParent();
		skillIcon = m_ui->findWidgetById("skillIcon");
	}
}

void CHeroSkill::nodeOffset(CCNode * parent, float offset)
{
	for (int i = 0; i < parent->getChildrenCount(); i++)
	{
		CCNode *child = (CCNode*)parent->getChildren()->objectAtIndex(i);
		child->setPositionX(child->getPositionX()+offset);
	}
}

void CHeroSkill::skillLevel()
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

void CHeroSkill::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CItem* item = (CItem*)pImage->getUserData();

	//奖励类型(1 道具，2 英雄，3 铜钱，4 元宝，5 粮食, 6体力)
	if(m_pItemInfo==nullptr)
	{
		m_pItemInfo = CItemInfo::create();
		this->addChild(m_pItemInfo);
	}

	switch (iState)
	{
	case CTouchPressOn:
		{
			PlayEffectSound(SFX_429);

			m_pItemInfo->setInfo(item);
			m_pItemInfo->bindPos(pImage);
			m_pItemInfo->changeForSign(item);
			m_pItemInfo->show();
			CCLOG("show");
		}
		break;
	case CTouchPressOff:
		{
			m_pItemInfo->hide();
			CCLOG("hide");
		}
		break;
	default:
		break;
	}
	
}

void CHeroSkill::showSkillType()
{
	CLabel *type = (CLabel*)m_ui->findWidgetById("type");
	switch (m_skillIndex)
	{
	case 3:
		type->setString(GETLANGSTR(2019));
		break;
	case 4:
		type->setString(GETLANGSTR(2020));
		break;
	case 5:
		type->setString(GETLANGSTR(2021));
		break;
	case 2:
		type->setString(GETLANGSTR(2022));
		break;
	case 6:
		type->setString(GETLANGSTR(2023));
		break;
	default:
		break;
	}
}

void CHeroSkill::showSpeciaBtn( bool bVisiable )
{
	CImageView *pBtn = (CImageView *)m_ui->findWidgetById("special_btn");
	if (bVisiable)
	{
		pBtn->setVisible(true);
		pBtn->setTouchEnabled(true);
		pBtn->setOnClickListener(this, ccw_click_selector(CHeroSkill::onSpecialBtnClick));
	}
	else
	{
		pBtn->setVisible(false);
	}

	showSpeciaPage(false);
}


void CHeroSkill::onSpecialBtnClick( CCObject *pObj )
{
	if (m_ui->isVisible())
	{
		showSpeciaPage(true);
	}
	else
	{
		showSpeciaPage(false);
	}
}

void CHeroSkill::showSpeciaPage( bool bVisiable )
{
	//切换到进阶界面
	if (!m_ui)
	{
		m_ui = LoadComponent("Heroskill.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
	}
	m_ui->setVisible(!bVisiable);

	if (!m_ui_special)
	{
		m_ui_special = LoadComponent("HeroskillEvolve.xaml");  //
		m_ui_special->setPosition(VCENTER);
		this->addChild(m_ui_special);

		//切回按钮
		CImageView *pBtn = (CImageView *)m_ui_special->findWidgetById("special_btn");
		pBtn->setTouchEnabled(true);
		pBtn->setOnClickListener(this, ccw_click_selector(CHeroSkill::onSpecialBtnClick));

		//五个技能图标事件
		for (int i=0; i<5; i++)
		{
			CImageView *pImageView = (CImageView *)m_ui_special->findWidgetById(CCString::createWithFormat("itembg_%d", i+1)->getCString());
			pImageView->setTexture(CCTextureCache::sharedTextureCache()->addImage("skillIcon/123456.png"));
			pImageView->setTouchEnabled(true);
			pImageView->setTag(i+1);
			pImageView->setOnClickListener(this, ccw_click_selector(CHeroSkill::showSpecialInfo));
			pImageView->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
		}


		//默认选择第一个
		showSpecialInfo((CImageView *)m_ui_special->findWidgetById("itembg_1"));

	}
	m_ui_special->setVisible(bVisiable);

	NOTIFICATION->postNotification("CHeroAttribute::setHeroSkillBgVisiable", CCBool::create(bVisiable));
}

void CHeroSkill::showSpecialInfo( CCObject *pSender )
{
	//TODO
	CCNode *pNode = (CCNode *)pSender;
	int iTag = pNode->getTag();

	const SkillCfg *pCfg = DataCenter::sharedData()->getSkill()->getCfg(10+iTag);
	if (pCfg)
	{
		CLabel *pName = (CLabel *)m_ui_special->findWidgetById("name");
		pName->setString(pCfg->name.c_str());
		CLabel *pDesc = (CLabel *)m_ui_special->findWidgetById("desc");
		pDesc->setString(pCfg->desc.c_str());
	}

	//光标
	CImageView* pSelectRect = (CImageView *)m_ui_special->findWidgetById("selectImg");
	pSelectRect->setPosition(pNode->getPosition()+ccp(38, -10));
}

