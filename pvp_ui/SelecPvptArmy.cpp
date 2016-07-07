#include "SelectPvpArmy.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "common/CommonFunction.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/color.h"

#include "tools/StringUtil.h"
#include "hero/HeroAttribute.h"
#include "hero/HeroControl.h"
#include "guide/GuideManager.h"
#include "tollgate/TollgatePreview.h"

#include "common/CGameSound.h"
#include "Resources.h"

#include "model/DataCenter.h"
#include "model/WarManager.h"

using namespace BattleSpace;
CSelectPvpArmy::~CSelectPvpArmy()
{
	CC_SAFE_RELEASE(m_cell);
}

CSelectPvpArmy::CSelectPvpArmy()
	:m_selectType(0),m_currPage(0),m_selectFriend(NULL),m_frdId(0),
	m_friendId(0), m_stageId(0), m_questId(0), m_pActiveLay(nullptr),
	m_bCaptainShowAction(false),m_WorldBoss(false){}

bool CSelectPvpArmy::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("SelectArmymask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("selectPvpHero.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		m_selectType = 1;

		//设置遮挡屏幕
		setCoverScreen(true);

		return true;
	}
	return false;
}

void CSelectPvpArmy::onEnter()
{ 
	BaseLayer::onEnter();
	CPlayerControl::getInstance().sendHeroList(0);

	//旋转3球
	CCSprite* pCircle = (CCSprite*)m_ui->findWidgetById("circle_3");
	//pCircle->runAction(CCRepeatForever::create(CCRotateBy::create(10,360)));
	pCircle->setOpacity(0);
	for(unsigned int i=0; i<3; i++)
	{
		CCSprite* pCircle1 = (CCSprite*)m_ui->findWidgetById(CCString::createWithFormat("circle_%d", i)->getCString());
		//pCircle1->setPosition(pCircle->convertToNodeSpace(m_ui->convertToWorldSpace(pCircle1->getPosition())));
		//pCircle1->retain();
		pCircle1->removeFromParentAndCleanup(true);
		//pCircle->addChild(pCircle1);
		//pCircle1->runAction(CCRepeatForever::create(CCRotateBy::create(10,-360)));
		//pCircle1->release();
	}
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("skill/9051.plist");//将plist文件加载进入缓存
	AnimationManager::sharedAction()->ParseAnimation("9051");
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9051");
	culAnim->setDelayPerUnit(0.1f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CCSprite* pFire  = CCSprite::create();
	pFire->setPosition(ccp(pCircle->getContentSize().width/2, pCircle->getContentSize().height/2));
	pCircle->addChild(pFire, -1, 1);
	pFire->runAction(CCRepeatForever::create(pAnimate));
	pFire->setOpacity(0);

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CSelectPvpArmy::onClose));
	this->addChild(pClose, 999);


	CButton *combat= (CButton *)m_ui->findWidgetById("combat");
	combat->setOnClickListener(this,ccw_click_selector(CSelectPvpArmy::onCombat));
	combat->getSelectedImage()->setScale(1.1f);
	combat->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.1f), CCScaleTo::create(0.3f, 1.0f))));
	combat->setAnchorPoint(ccp(0.5f, 0.5f));
	combat->setPosition(ccpAdd(combat->getPosition(), ccp(combat->getContentSize().width/2, combat->getContentSize().height/2)));

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);
	m_cell1 = (CLayout*)(m_ui->findWidgetById("Cell1"));
	m_cell1->retain();
	m_ui->removeChild(m_cell1);

	m_armyView = (CPageView *)(m_ui->findWidgetById("scroll1"));
	m_armyView->setDirection(eScrollViewDirectionHorizontal);
	m_armyView->setSizeOfCell(CCSizeMake(m_cell1->getContentSize().width*5,m_cell1->getContentSize().height));
	m_armyView->setCountOfCell(0);
	m_armyView->setBounceable(false);
	m_armyView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSelectPvpArmy::armyDataSource));
	m_armyView->setDragable(false);
	m_armyView->reloadData();

	m_selectLay = (CLayout*)(m_ui->findWidgetById("select"));
	m_selectLay->retain();

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(2);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	// 	m_pageView->setBounceable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSelectPvpArmy::gridviewDataSource));
	m_gridView->reloadData();
	m_gridView->setContentOffsetToTop();

	m_oppTeamView = (CGridView*)m_ui->findWidgetById("scroll2");
	m_oppTeamView->setDirection(eScrollViewDirectionVertical);	
	m_oppTeamView->setCountOfCell(0);
	m_oppTeamView->setSizeOfCell(m_cell1->getContentSize());
	m_oppTeamView->setAnchorPoint(ccp(0,0));
	m_oppTeamView->setColumns(5);
	m_oppTeamView->setAutoRelocate(true);
	m_oppTeamView->setDeaccelerateable(false);
	m_oppTeamView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSelectPvpArmy::gridviewDataSource));
	m_oppTeamView->reloadData();
	m_oppTeamView->setContentOffsetToTop();

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CSelectPvpArmy::onSwitchBtn));
	}

//     CRadioButton *myBat = (CRadioButton *)m_ui->findWidgetById("myBat");
// 	myBat->setOnCheckListener(this,ccw_check_selector(CSelectPvpArmy::onMyBat));
// 	CRadioButton *friendBat = (CRadioButton *)m_ui->findWidgetById("friendBat");
// 	
// 	//有世界BOSS
// 	if(m_WorldBoss)
// 	{
// 		CCSprite* pIcon1 = (CCSprite*)m_ui->findWidgetById("switch9");
// 		pIcon1->setVisible(false);
// 		CCSprite* pIcon2 = (CCSprite*)m_ui->findWidgetById("switch11");
// 		pIcon2->setVisible(false);
// 
// 		myBat->setVisible(false);
// 		friendBat->setVisible(false);
// 	}
// 	else
// 	{
// 		friendBat->setOnCheckListener(this, ccw_check_selector(CSelectPvpArmy::onFriendBat));
// 	}
	GetTcpNet->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CSelectPvpArmy::processMessage));
	GetTcpNet->registerMsgHandler(UnionRes,this,CMsgHandler_selector(CSelectPvpArmy::processMessage));
	GetTcpNet->registerMsgHandler(PvpOppTeamMsg,this,CMsgHandler_selector(CSelectPvpArmy::processMessage));

	NOTIFICATION->addObserver(this, callfuncO_selector(CSelectPvpArmy::willToBatthle), RECEIVE_BATTLE_INFO, nullptr);

	//先转太极到裁切层去
	CLayout* taijiLay = (CLayout*)m_ui->findWidgetById("taiji_lay");
	CCSprite*stencil = (CCSprite*)taijiLay->findWidgetById("circle");
	stencil->retain();
	stencil->removeFromParentAndCleanup(true);
	//绑定裁切区域
	CCClippingNode* pClippingNode = CCClippingNode::create();
	pClippingNode->setPosition(ccp(0, 0));
	pClippingNode->setStencil(stencil);
	pClippingNode->setAlphaThreshold(0);
	stencil->release();
	taijiLay->addChild(pClippingNode);

	CCSprite*taiji1 = (CCSprite*)taijiLay->findWidgetById("tj_1");
	taiji1->retain();
	taiji1->removeFromParentAndCleanup(false);
	pClippingNode->addChild(taiji1);
	taiji1->release();

	CCSprite*taiji2 = (CCSprite*)taijiLay->findWidgetById("tj_2");
	taiji2->retain();
	taiji2->removeFromParentAndCleanup(false);
	pClippingNode->addChild(taiji2);
	taiji2->release();

	m_pTaiJi1 = taiji1;
	m_pTaiJi2 = taiji2;
}

CCObject* CSelectPvpArmy::armyDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CPageViewCell *pCell = (CPageViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CPageViewCell;
		pCell->autorelease();
		addHeroCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addHeroCell(uIdx, pCell);
	}
	return pCell;
}

void CSelectPvpArmy::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSelectPvpArmy::onClearHero(CCObject* pSender)
{

	PlayEffectSound(SFX_Button);

	CImageView *heromask = (CImageView*)pSender;
	CHero hero;
	int iIndex = ((CCInteger*)(heromask->getUserObject()))->getValue();

	//动画限制点击
	if(m_bCaptainShowAction && iIndex==0)
	{
		return;
	}
	if (m_union.heroList.at(iIndex).id == m_friendId)
	{
		m_frdId = 0;
		m_friendId =0;
	}
    m_union.heroList.at(((CCInteger*)(heromask->getUserObject()))->getValue()) = hero;
	m_armyView->reloadData();
	updateCost();

	ReloadGridViewWithSameOffset(m_gridView);

	updateMainSkillDesc();

	//清除队长UI
	if(iIndex==0)
	{
		showRemoveCaptain();
	}
}

void CSelectPvpArmy::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);

	NOTIFICATION->removeAllObservers(this);

	if(isVisible())
	{
		//还原被遮盖的层，渲染
		LayerManager::instance()->showLayerUnBeCovered(this);
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
}

void CSelectPvpArmy::processNetMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	HeroListResponse *res = (HeroListResponse*)msg;
	m_friendList.clear();
	for (int i=0; i<res->herolist_size(); i++)
	{
		Hero hero = res->herolist(i);
		CHero obj;
		obj.readData(hero);
		m_friendList.push_back(obj);
	}
	updateHeroList();
}

void CSelectPvpArmy::setUnionList(vector<CUnion> unionList)
{
	m_unionList = unionList;
	
}

void CSelectPvpArmy::addHeroCell(unsigned int uIdx, CPageViewCell * pCell)
{
	CUnion &nion = m_union;
	for (int j = 0; j < 5&&j<nion.heroList.size(); j++)
	{	
		CLayout *lay = UICloneMgr::cloneLayout(m_cell1);

		if (nion.heroList[j].id<=0)
		{
			lay->setVisible(false);
			continue;
		}
		CHero &hero = nion.heroList[j];
		for (int i=1; i<=5&&i<=hero.star;i++)
		{
			CCSprite *star = (CCSprite*)lay->findWidgetById(CCString::createWithFormat("star%d",i)->getCString());
			star->setVisible(true);
		}
		if (hero.star==2||hero.star==4)
		{
			for (int i = 1; i <=hero.star; i++)
			{
				CCSprite *star = (CCSprite*)lay->findWidgetById(CCString::createWithFormat("star%d",i)->getCString());
				star->setPositionX(star->getPositionX()+star->boundingBox().size.width/2-4);
			}	
		}

		for (int i=1;i<=4;++i)
		{
			CCNode *child = lay->getChildByTag(i);
			if (!child)continue;
			if (j)child->setPositionX(child->getPositionX()+m_cell1->getContentSize().width*j);
			lay->removeChild(child);
			pCell->addChild(child);
			switch (i)
			{
			case 1:
				{
					if (hero.thumb>0)
					{
						CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero.thumb)->getCString());
						if (!head)
						{
							head = CCSprite::create("headImg/101.png");
							CCLOG("[ ERROR ] CSelectPvpArmy::addHeroCell Lost Image = %d",hero.thumb);
						}
					//	head->setScale(0.92f);
						head->setPosition(ccp(child->getContentSize().width/2, child->getContentSize().height/2));
						child->addChild(head);
					}
				}break;
			case 2:
				{
					CImageView *mask =(CImageView*)child;
					mask->setTexture(SetRectColor(hero.roletype));
					mask->setTouchEnabled(true);
					mask->setOnClickListener(this,ccw_click_selector(CSelectPvpArmy::onClearHero));
					mask->setUserObject(CCInteger::create(j));
					if (m_questId>0&&j==0&&hero.id>0)
						mask->setTouchEnabled(false);

					//添加星级
					CLayout* pStarLayout = getStarLayout(hero.iColor);
					mask->addChild(pStarLayout);

				}break;
			case 3:
				{
					if (hero.coin>=0)
						child->setVisible(true);
				}break;
			case 4:
				{
					const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero.thumb);
					if(data)
					{
						CCSprite *heroType = (CCSprite*)child;
						heroType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", hero.roletype, data->iType2)->getCString()));
					}
					
				}break;
			default:break;
			}
		}
	}
}

// bool CSelectPvpArmy::onlongClickFriend(CCObject* pSender, CCTouch* pTouch)
// {
// 	CImageViewScale9 *selectImg = (CImageViewScale9*)pSender;
// 	CHero *hero = (CHero*)selectImg->getUserData();
// 	CHeroControl *heroctl = CHeroControl::create();
// 	heroctl->setIsHideOperation(true);
// 	CSceneManager::sharedSceneManager()->getRunningScene()->removeChildByTag(1900);
// 	CSceneManager::sharedSceneManager()->getRunningScene()->addChild(heroctl,0,1900);
// 	CPlayerControl::getInstance().sendGetHeroInfo(hero->id,0,m_friendId);
// 	return true;
// }

void CSelectPvpArmy::updateMainSkillDesc()
{
	if(m_union.heroList.size()==0) return;

	const CHero &hero = m_union.heroList.at(0);
	CLabel *mainSkillLab = (CLabel*)(m_ui->findWidgetById("desc"));
	if (hero.id!=0&&hero.skill5.skillId>0)
	{
		const SkillCfg *cfg = DataCenter::sharedData()->getSkill()->getCfg(hero.skill5.skillId);
		if (cfg)
		{ 
			mainSkillLab->setString(cfg->desc.c_str());
		}
		else
		{
			mainSkillLab->setString("");
		}
	}
	else
	{
		mainSkillLab->setString("");
	}
}

void CSelectPvpArmy::onCombat(CCObject* pSender)
{
	CCLOG("CSelectPvpArmy::onCombat");
	if (!m_union.heroList.size())
		return;
	if (m_union.heroList.at(0).id==0)
	{
		ShowPopTextTip(GETLANGSTR(253));
		return;
	}
	{
		PlayEffectSound(SFX_430);

		CButton *combat= (CButton *)pSender;
		combat->setEnabled(false);

		//有世界BOSS
		if(m_WorldBoss)
		{
			CPlayerControl::getInstance().sendEnterStageForBoss(
				m_union.heroList.at(0).id,
				m_union.heroList.at(1).id,
				m_union.heroList.at(2).id,
				m_union.heroList.at(3).id,
				m_union.heroList.at(4).id);
			DataCenter::sharedData()->getWar()->setWorldBoss(true);
		}else{
			CPlayerControl::getInstance().sendEnterStage(
				m_stageId,
				m_union.heroList.at(0).id,
				m_union.heroList.at(1).id,
				m_union.heroList.at(2).id,
				m_union.heroList.at(3).id,
				m_union.heroList.at(4).id,
				m_frdId,m_questId);
		}
	}

}

void CSelectPvpArmy::onMyBat(CCObject *pSender, bool bChecked)
{
	CCNode * node1= (CCNode*)m_ui->findWidgetById("switch9");
	CCNode * node2= (CCNode*)m_ui->findWidgetById("switch10");
	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		CRadioButton *friendBat = (CRadioButton *)m_ui->findWidgetById("friendBat");
		friendBat->setChecked(false);
		m_selectType = 1;
		updateCurrHeroList();
		node1->setVisible(true);
		node2->setVisible(false);
	}
	else
	{
		node1->setVisible(false);
		node2->setVisible(true);
	}
}

void CSelectPvpArmy::onFriendBat(CCObject *pSender, bool bChecked)
{
	CCNode * node1= (CCNode*)m_ui->findWidgetById("switch11");
	CCNode * node2= (CCNode*)m_ui->findWidgetById("switch12");
	if (bChecked)
	{	
		PlayEffectSound(SFX_Button);

		CRadioButton *myBat = (CRadioButton *)m_ui->findWidgetById("myBat");
		m_selectType = 2;
		myBat->setChecked(false);
		if (m_friendList.empty())
		{
			GetTcpNet->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CSelectPvpArmy::processNetMsg));
			CPlayerControl::getInstance().sendHeroList(100);
		}
		else
		{
			updateCurrHeroList();
		}
		node1->setVisible(true);
		node2->setVisible(false);
	}
	else
	{
		node1->setVisible(false);
		node2->setVisible(true);
	}
}

CCObject* CSelectPvpArmy::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	/*if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())*/
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addHeroSelectCell(uIdx,pCell);
	}
	return pCell;
}

void CSelectPvpArmy::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;

	int selIndex = 2*btn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		showSelectRadioImg(selIndex);

		if (m_selectType==1)
		{
			m_currHeroList = &m_heroMap[btn->getTag()];
		}
		else
		{
			m_currHeroList = &m_friendMap[btn->getTag()];
		}
		m_gridView->setCountOfCell(m_currHeroList->size());
		//ReloadGridViewWithSameOffset(m_gridView);
		m_gridView->reloadData();
	}
}

void CSelectPvpArmy::updateHeroList()
{
	if (m_selectType==1)
	{
		for (int i = 0; i < m_heroList.size(); i++)
		{
			CHero& hero = m_heroList.at(i);
			hero.coin = -1;
			(m_heroMap[m_heroList.at(i).roletype]).push_back(&hero);  //&heroList.at(i);
			(m_heroMap[4]).push_back(&hero);
		}
	}
	else
	{
		for (int i = 0; i < m_friendList.size(); i++)
		{
			(m_friendMap[m_friendList.at(i).roletype]).push_back(&m_friendList.at(i));  //&heroList.at(i);
			(m_friendMap[4]).push_back(&m_friendList.at(i));
		}
	}
	updateCurrHeroList();
}

void CSelectPvpArmy::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CHero *hero = m_currHeroList->at(uIdx);
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i = 1; i <=22; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			((CImageView*)child)->setUserData(hero);
			((CImageView*)child)->setTouchEnabled(true);
			((CImageView*)child)->setOnClickListener(this,ccw_click_selector(CSelectPvpArmy::onSelectHero));
		}
		else if (i==3)
		{
			CCSprite *mask = (CCSprite*)child;
			mask->setTexture(SetRectColor(hero->roletype));
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CSelectPvpArmy::addGridCell Lost Image = %d",hero->thumb);
			}	
			head->setPosition(child->getPosition());
			pCell->addChild(head);
		}
		else if (i==5)
		{
			CLabel *name = (CLabel *)child;
			if (m_selectType == 1) {
				const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);
				if(c_data && c_data->heroName.size() > 0)
					name->setString(c_data->heroName.c_str());
			} else if(m_selectType == 2) {
				name->setString(hero->name.c_str());
			}
		}
		else if (i==4)
		{
			CCSprite *heroType = (CCSprite*)child;
			heroType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("public/HeroType_%d.png",hero->roletype)->getCString()));
		}
		else if (i==9)
		{
			CLabel *name = (CLabel *)child;
			name->setString(CCString::createWithFormat("Lv.%d",hero->level)->getCString());
		}
		else if (i==19)
		{
			child->setVisible(m_selectType==2);
		}
		else if (i==20)
		{
			child->setVisible(m_selectType==2);
			if (m_selectType==2)
			{
				CLabel *coin =(CLabel*)child;
				coin->setString(ToString(hero->coin));
			}
		}
		else if (i==21||i==22)
		{
			bool isSel = findIsSelect(hero);
			child->setVisible(isSel);
		}
	}

	for (int i=5; i>=1&&i>hero->star;i--)
	{
		CCSprite *star = (CCSprite*)pCell->findWidgetById(CCString::createWithFormat("star%d",i+5)->getCString());
		star->setVisible(false);

	}
	if (hero->star==2||hero->star==4)
	{
		for (int i = 1; i <=hero->star; i++)
		{
			CCSprite *star = (CCSprite*)pCell->findWidgetById(CCString::createWithFormat("star%d",i+5)->getCString());
			star->setPositionX(star->getPositionX()+star->boundingBox().size.width/2-4);
		}	
	}
	if (hero->armor1.id>0&&hero->armorNum>0)
	{
		CCSprite *img = (CCSprite*)(pCell->getChildByTag(15));  //道具底框
		CCSprite *item = CCSprite::create(CCString::createWithFormat("prop/%d.png",hero->armor1.iconId)->getCString());
		if (!item)
		{
			item = CCSprite::create("prop/32003.png");
			CCLOG("[ ERROR ] CSelectPvpArmy::addGridCell Lost Image = %d",hero->armor1.iconId);
		}
		item->setPosition(ccp(img->getContentSize().width/2,img->getContentSize().height/2));
		img->addChild(item);

		CCSprite *fuwen = (CCSprite*)(pCell->getChildByTag(17));  //符文
		fuwen->setVisible(false);

		CCSprite *mask = (CCSprite*)(pCell->getChildByTag(7));
		mask->setTexture(setItemQualityTexture(hero->armor1.quality));
	}
	else if (hero->armor1.id==0&&hero->armorNum>0)
	{
		CCSprite *fuwen = (CCSprite*)(pCell->getChildByTag(17));  
		fuwen->setVisible(false);
	}

	if (hero->armor2.id>0&&hero->armorNum>1)
	{
		CCSprite *img = (CCSprite*)(pCell->getChildByTag(16));    //道具底框
		CCSprite *item = CCSprite::create(CCString::createWithFormat("prop/%d.png",hero->armor2.iconId)->getCString());
		if (!item)
		{
			item = CCSprite::create("prop/32003.png");
			CCLOG("[ ERROR ] CSelectPvpArmy::addGridCell Lost Image = %d",hero->armor2.iconId);
		}
		item->setPosition(ccp(img->getContentSize().width/2,img->getContentSize().height/2));
		img->addChild(item);

		CCSprite *fuwen = (CCSprite*)(pCell->getChildByTag(18));  //符文
		fuwen->setVisible(false);

		CCSprite *mask = (CCSprite*)(pCell->getChildByTag(8));
		mask->setTexture(setItemQualityTexture(hero->armor2.quality));
	}
	else if (hero->armor2.id==0&&hero->armorNum>1)
	{
		CCSprite *fuwen = (CCSprite*)(pCell->getChildByTag(18));  
		fuwen->setVisible(false);
	}
}

void CSelectPvpArmy::addHeroSelectCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CHero *hero = m_currHeroList->at(uIdx);
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);

	for (int i = 1; i <=11; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			((CImageView*)child)->setUserData(hero);
			((CImageView*)child)->setTouchEnabled(true);
			((CImageView*)child)->setOnClickListener(this,ccw_click_selector(CSelectPvpArmy::onSelectHero));
		}
		else if (i==3)
		{
			CCSprite *mask = (CCSprite*)child;
			mask->setTexture(SetRectColor(hero->roletype));
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CSelectPvpArmy::addGridCell Lost Image = %d",hero->thumb);
			}	
			head->setPosition(ccpAdd(child->getPosition(), ccp(child->getContentSize().width/2, child->getContentSize().height/2)));
			head->setScale(0.9f);
			pCell->addChild(head);

			//添加星级
			CLayout* pStarLayout = getStarLayout(hero->iColor);
			mask->addChild(pStarLayout);
		}
		else if (i==5)
		{
			CLabel *name = (CLabel *)child;
			if (m_selectType == 1) {
				if(c_data && c_data->heroName.size() > 0)
					name->setString(c_data->heroName.c_str());
			} else if(m_selectType == 2) {
				name->setString(hero->name.c_str());
			}
		}
		else if (i==4)
		{
			if(c_data)
			{
				CCSprite *heroType = (CCSprite*)child;
				heroType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png",hero->roletype, c_data->iType2)->getCString()));
			}
		}
		else if (i==7)
		{
			CLabel *name = (CLabel *)child;
			name->setString(ToString(hero->level));
		}
		else if (i==9)
		{
			child->setVisible(m_selectType==2);
			child->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.25f, 1.1f), CCScaleTo::create(0.25f, 1.0f))));
		}
		else if (i==10)
		{
			child->setVisible(m_selectType==2);
			if (m_selectType==2)
			{
				CLabel *coin =(CLabel*)child;
				coin->setString(ToString(hero->coin));
			}
		}
		else if (i==11||i==2)
		{
			bool isSel = findIsSelect(hero);
			child->setVisible(isSel);
		}
	}
}

void CSelectPvpArmy::showSelectRadioImg(int selIndex)
{
	
}

void CSelectPvpArmy::onSelectHero(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	CHero *hero = (CHero*)((CButton*)pSender)->getUserData();

	PlayEffectSound(SFX_Button);

	int i = 0;
	//在队伍中查找是否已经存在
	for (; i < m_union.heroList.size(); i++)
	{
		CHero *hr = &m_union.heroList.at(i);

		//已经存在队伍中
		if (hr->id==hero->id&&!CGuideManager::getInstance()->getIsRunGuide())
		{
			//清除队长UI
			if(i==0)
			{
				if(!showRemoveCaptain())
				{
					return;
				}
			}

			//从队伍中移除
			CImageView *heromask = (CImageView*)pSender;
			CHero newHero;
			if (hr->id == m_friendId)
			{
				m_frdId = 0;
				m_friendId =0;
			}
			m_union.heroList.at(i) = newHero;
			m_armyView->reloadData();
			updateCost();

			ReloadGridViewWithSameOffset(m_gridView);

			updateMainSkillDesc();

			return;
		}
		//好友英雄，直接切换
		else if (m_selectType==2&&hr->coin>0)
		{
			m_friendId = hero->id;
			m_frdId = hero->heroid;
			m_union.heroList.at(i) = *hero;
			m_armyView->reloadData();
			updateCost();
			
			ReloadGridViewWithSameOffset(m_gridView);

			updateMainSkillDesc();
			//ShowPopTextTip("cann't select two friend hero!");
			return;
		}
	}

	//找空位直接补上
	for (i=0; i < m_union.heroList.size(); i++)
	{
		CHero *hr = &m_union.heroList.at(i);
		if (hr->id==0)
		{
			//展现队长UI
			if(i==0)
			{
				if(!showAddCaptain())
				{
					return;
				}
			}

			*hr = *hero;
			CCNode *selSpr = btn->getParent()->getChildByTag(11);
			selSpr->setVisible(true);
			CCSprite *selSpr1 = (CCSprite*)btn->getParent()->getChildByTag(2);
			selSpr1->setVisible(true);
			if (m_selectType==2)
			{
				m_friendId = hr->id;
				m_frdId = hr->heroid;
			}

			break;
		}
	}
	m_armyView->reloadData();
	updateCost();
	updateMainSkillDesc();
}

void CSelectPvpArmy::processMessage(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);
		//隐藏被遮盖的层，不渲染
		LayerManager::instance()->hideLayerBeCovered(this);
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
		NOTIFICATION->postNotification(WARPREVIEW_EXIT);
	}

	switch (type)
	{
	case HeroListMsg:
		{
			HeroListResponse *res = (HeroListResponse*)msg;
			m_heroList.clear();
			
			if (CGuideManager::getInstance()->getIsRunGuide())
			{
				CHero hr;
				for (int i=0; i<res->herolist_size(); i++)
				{
					Hero hero = res->herolist(i);
					CHero obj;
					obj.readData(hero);
					if (obj.heroid==102)
					{
						hr = obj;
					}
					else
					{
						m_heroList.push_back(obj);
					}
				}
				if (hr.id>0)
				{
					m_heroList.push_back(hr);
				}
			}
			else
			{
				for (int i=0; i<res->herolist_size(); i++)
				{
					Hero hero = res->herolist(i);
					CHero obj;
					obj.readData(hero);
					m_heroList.push_back(obj);
				}
			}
			this->updateHeroList();
		}
		break;

	case UnionRes:
		{
			UnionResponse *res = (UnionResponse*)msg;
		    m_union.read(*res);

			if(CGuideManager::getInstance()->getIsRunGuide()&&CGuideManager::getInstance()->getCurrTask()==2)
			{
				for(unsigned int i=0; i<m_union.heroList.size(); i++)
				{
					m_union.heroList.at(i).id = 0;
					m_union.heroList.at(i).heroid = 0;
				}
			}

			//coin设置为-1，都是自己英雄里的人，不是好友的
			for(unsigned int i=0; i<m_union.heroList.size(); i++)
			{
				m_union.heroList.at(i).coin = -1;
			}

			m_armyView->setCountOfCell(1);
		    m_armyView->reloadData();
			//Mr.yan
			if(!CGuideManager::getInstance()->getIsRunGuide())
			{
				ReloadGridViewWithSameOffset(m_gridView);
				m_gridView->setContentOffsetToTop();
			}
			updateCost();
			updateMainSkillDesc();
// 			selArmy->setStageFriendId(m_stagId,m_selectFriend->friendId);

			//动作初始化
			initAction();

		}
		break;
	case PvpOppTeamMsg:
		{
			OppTeamRes *res = (OppTeamRes*)msg;

		}
	}

}

void CSelectPvpArmy::updateCurrHeroList()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		if (radioBtn->isChecked())
		{
			if (m_selectType==1)
			{
				m_currHeroList = &m_heroMap[radioBtn->getTag()];
				m_gridView->setCountOfCell(m_currHeroList->size());
			}
			else
			{
				m_currHeroList = &m_friendMap[radioBtn->getTag()];
				m_gridView->setCountOfCell(m_currHeroList->size());
			}

// 			ReloadGridViewWithSameOffset(m_gridView);
			m_gridView->reloadData();
			break;
		}
	}
}

void CSelectPvpArmy::setStagId(int stageId, int questId/*=0*/)
{
	m_stageId = stageId;
	m_questId = questId;
}

void CSelectPvpArmy::updateCost()
{
	int initCost =0,totalCost = 0;
	for (int i = 0; i < m_union.heroList.size(); i++)
	{
		CHero &hero = m_union.heroList.at(i);
		if(i==0)
		{
			initCost+= hero.initCost;
		}
		if (hero.id>0)
		{
			totalCost += hero.maxCost;
		}
	}
	CLabel *cost = (CLabel*)(m_ui->findWidgetById("cost"));
	cost->setString(CCString::createWithFormat("%d/%d",initCost,totalCost)->getCString());
}

bool CSelectPvpArmy::findIsSelect(CHero * hero)
{
	bool isfind =false;
	for (int i = 0; i < m_union.heroList.size(); i++)
	{
		if (hero->id==m_union.heroList.at(i).id)
		{
			isfind = true;
			break;
		}
	}
	return isfind;
}


void CSelectPvpArmy::initAction()
{
	//拿到活动层
	m_pActiveLay = (CLayout*)m_ui->findWidgetById("active_lay");
	m_pActiveLay->setPositionY(m_pActiveLay->getPositionY()-150);

	showActionForArmy();
}


void CSelectPvpArmy::showActionForArmy(float dt)
{
	m_pActiveLay->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(0.25f, ccp(0, 150)), CCCallFunc::create(this, callfunc_selector(CSelectPvpArmy::showActionForArmyCallBack))));
	//链条1.链条2
// 	{
// 		CCNode* pLink = (CCNode*)m_ui->findWidgetById("link_1");
// 		pLink->runAction(CCMoveBy::create(0.25f, ccp(0, 30)));
// 	}
// 	{
// 		CCNode* pLink = (CCNode*)m_ui->findWidgetById("link_2");
// 		pLink->runAction(CCMoveBy::create(0.25f, ccp(0, 30)));
// 	}
}


void CSelectPvpArmy::showActionForArmyReverse(float dt)
{
	m_pActiveLay->runAction(CCMoveBy::create(0.25f, ccp(0, -150)));
	//链条1.链条2
	{
		CCNode* pLink = (CCNode*)m_ui->findWidgetById("link_1");
		pLink->runAction(CCMoveBy::create(0.25f, ccp(0, -30)));
	}
	{
		CCNode* pLink = (CCNode*)m_ui->findWidgetById("link_2");
		pLink->runAction(CCMoveBy::create(0.25f, ccp(0, -30)));
	}
}


void CSelectPvpArmy::showActionForArmyCallBack()
{
	//是否有队长
	if(checkShowCaptain())
	{
		showAddCaptain();
	}
}

bool CSelectPvpArmy::showAddCaptain()
{
	if(m_bCaptainShowAction)
	{
		return false;
	}
	m_bCaptainShowAction = true;

	//太极开，数字飞出，木板提上来
	{
		//太极
		CCNode* pTJ1 = m_pTaiJi1;
		pTJ1->runAction(CCSequence::createWithTwoActions(CCRotateBy::create(0.15f, 360), CCMoveBy::create(0.2f, ccp(35, 35))));
		CCNode* pTJ2 = m_pTaiJi2;
		pTJ2->runAction(CCSequence::createWithTwoActions(CCRotateBy::create(0.15f, 360), CCMoveBy::create(0.2f, ccp(-35, -35))));
		
		CCSprite* pFire = (CCSprite*)((CCSprite*)m_ui->findWidgetById("circle_3"))->getChildByTag(1);
		pFire->runAction(CCFadeIn::create(0.3f));
		
		//数字
		CCSprite* pText = (CCSprite*)m_ui->findWidgetById("cost");
		pText->setVisible(true);
		pText->setOpacity(0);
		pText->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCSpawn::createWithTwoActions(CCFadeIn::create(0.15f), CCEaseBackOut::create(CCMoveBy::create(0.2f, ccp(90, 0)))),nullptr));
		//木板
		{
			CCNode* pPanel = (CCNode*)m_ui->findWidgetById("bank_2");
			pPanel->runAction(CCMoveBy::create(0.2f, ccp(0, 102)));
			CCNode* pTitle = (CCNode*)m_ui->findWidgetById("desc_title");
			pTitle->runAction(CCMoveBy::create(0.2f, ccp(0, 102)));
			CCNode* pDesc = (CCNode*)m_ui->findWidgetById("desc");
			pDesc->runAction(CCMoveBy::create(0.2f, ccp(0, 102)));
		}
	}
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.4f), CCCallFunc::create(this, callfunc_selector(CSelectPvpArmy::showCaptainActionCallBack))));

	return true;
}

bool CSelectPvpArmy::showRemoveCaptain()
{
	if(m_bCaptainShowAction)
	{
		return false;
	}
	m_bCaptainShowAction = true;

	//太极关，数字飞回，木板收回去
	{
		//太极
		CCNode* pTJ1 = m_pTaiJi1;
		pTJ1->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(0.2f, ccp(-35, -35)), CCRotateBy::create(0.15f, 360)));
		CCNode* pTJ2 = m_pTaiJi2;
		pTJ2->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(0.2f, ccp(35, 35)), CCRotateBy::create(0.15f, 360)));

		CCSprite* pFire = (CCSprite*)((CCSprite*)m_ui->findWidgetById("circle_3"))->getChildByTag(1);
		pFire->runAction(CCFadeOut::create(0.3f));

		//数字
		CCNode* pText = (CCNode*)m_ui->findWidgetById("cost");
		pText->setVisible(true);
		pText->runAction(CCSpawn::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(0.2f, ccp(-90, 0))), CCFadeOut::create(0.07f)));

		//木板
		{
			CCNode* pPanel = (CCNode*)m_ui->findWidgetById("bank_2");
			pPanel->runAction(CCMoveBy::create(0.2f, ccp(0, -102)));
			CCNode* pTitle = (CCNode*)m_ui->findWidgetById("desc_title");
			pTitle->runAction(CCMoveBy::create(0.2f, ccp(0, -102)));
			CCNode* pDesc = (CCNode*)m_ui->findWidgetById("desc");
			pDesc->runAction(CCMoveBy::create(0.2f, ccp(0, -102)));
		}
	}
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.4f), CCCallFunc::create(this, callfunc_selector(CSelectPvpArmy::showCaptainActionCallBack))));

	return true;
}

void CSelectPvpArmy::showCaptainActionCallBack()
{
	m_bCaptainShowAction = false;
}

bool CSelectPvpArmy::checkShowCaptain()
{
	const CHero &hero = m_union.heroList.at(0);
	if (hero.id!=0&&hero.skill5.skillId>0)
	{
		return true;
	}
	return false;
}

void CSelectPvpArmy::willToBatthle( CCObject* pObj )
{
	CButton *combat= (CButton *)m_ui->findWidgetById("combat");
	combat->stopAllActions();
	//停止所有触摸事件
	CCDirector::sharedDirector()->getTouchDispatcher()->removeAllDelegates();
}

void CSelectPvpArmy::addOppTeamCell(unsigned int uIdx, CPageViewCell * pCell)
{
	    CLayout *lay = UICloneMgr::cloneLayout(m_cell1);
		CHero &hero = m_oppTeamList.at(uIdx);
		for (int i=1;i<=4;++i)
		{
			CCNode *child = lay->getChildByTag(i);
			if (!child)continue;
			lay->removeChild(child);
			pCell->addChild(child);
			switch (i)
			{
			case 1:
				{
					if (hero.thumb>0)
					{
						CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero.thumb)->getCString());
						if (!head)
						{
							head = CCSprite::create("headImg/101.png");
							CCLOG("[ ERROR ] CSelectPvpArmy::addHeroCell Lost Image = %d",hero.thumb);
						}
						//	head->setScale(0.92f);
						head->setPosition(ccp(child->getContentSize().width/2, child->getContentSize().height/2));
						child->addChild(head);
					}
				}break;
			case 2:
				{
					CImageView *mask =(CImageView*)child;
					mask->setTexture(setItemQualityTexture(hero.iColor));
				
					//添加星级
					CLayout* pStarLayout = getStarLayout(hero.iColor);
					mask->addChild(pStarLayout);
				}break;
			case 3:
			case 4:
				{
					child->setVisible(false);
				}break;
			default:break;
			}
		}
}
