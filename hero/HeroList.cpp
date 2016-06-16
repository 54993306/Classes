#include "HeroList.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "HeroEvolve.h"
#include "common/CommonFunction.h"
#include "common/CShowToBuyResource.h"
#include "scene/CPopTip.h"
#include "mainCity/CNewHero.h"
#include "hero/HeroCall.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/ShaderDataHelper.h"

CHeroList::CHeroList():m_cell(nullptr),m_cell1(nullptr),m_selectHero(nullptr),m_pSelectCallHero(nullptr)
{
	for(unsigned int i=0; i<4; i++)
	{
		m_iEquipStock[i] = -1;
	}
}


bool CHeroList::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("herolistMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("HeroList.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		m_subLay = (CLayout*)m_ui->findWidgetById("sublay");

		m_pNewHeroEffect = CNewHero::create();
		m_pNewHeroEffect->setVisible(false);
		this->addChild(m_pNewHeroEffect, 20);

		return true;
	}
	return false;

}

void CHeroList::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CHeroList::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	m_cell1 = (CLayout*)(m_ui->findWidgetById("Cell1"));
	m_cell1->retain();
	m_ui->removeChild(m_cell1);

	m_emptyLay = (CLayout *)(m_ui->findWidgetById("Cell_Empty"));
	m_emptyLay->retain();
	m_ui->removeChild(m_emptyLay);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(2);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	
// 	m_pageView->setBounceable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroList::gridviewDataSource));
	m_gridView->reloadData();


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("onClose");
	pClose->setOnClickListener(this,ccw_click_selector(CHeroList::onClose));
	m_ui->addChild(pClose, 999);

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_subLay->getChildByTag(10);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CHeroList::onSwitchBtn));
	}

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroList::updateEquipStockCount), UPDATE_EQUIP_STOCK_COUNT, nullptr);

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroList::compaseSuccess), COMPOSE_SUCCESS, nullptr);

	//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
}

CCObject* CHeroList::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	/*if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())*/
	CHero *hero = m_currHeroList->at(uIdx);
	if (!pCell)
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
	}
	else
	{
		pCell->removeAllChildren();
	}
	if (hero->own)
	{	
		addGridCell(uIdx,pCell);
	}
	else
	{
		addCallCell(uIdx,pCell);
	}
	return pCell;
}

void CHeroList::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;

	int selIndex = 2*btn->getTag();
	
	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		m_currHeroList = &m_heroMap[btn->getTag()];

		m_gridView->setCountOfCell(m_currHeroList->size());
		m_gridView->reloadData();
	}
}

void CHeroList::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	BaseLayer::onExit();
	//NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
	NOTIFICATION->removeAllObservers(this);
}

void CHeroList::runOpenAnim()
{
	m_subLay->setPositionY(m_subLay->getPositionY()-400);
	m_subLay->runAction(CCSequence::create(CCDelayTime::create(0.1f),CCEaseBounceInOut::create(CCMoveBy::create(0.2f,ccp(0,400))),nullptr));
}

void CHeroList::runStoneAnim(CCNode *pNode)
{
	
}

void CHeroList::updateHeroList(vector<CHero>& heroList, int insertIndex)
{
	bool isFirst = false;
	if(m_heroList.size()<=0)
	{
		isFirst = true;
	}

	m_heroList.clear();
	m_heroMap.clear();
	m_heroList = heroList;
	for (int i = 0; i < heroList.size(); i++)
	{
		(m_heroMap[heroList.at(i).roletype]).push_back(&m_heroList.at(i));  //&heroList.at(i);
		(m_heroMap[4]).push_back(&m_heroList.at(i));
	}	

	for (int j = 1; j <=3; j++)
	{
		for (int i = 0; i < m_heroMap[j].size(); i++)
		{
			if (!m_heroMap[j].at(i)->own&&m_heroMap[j].at(i)->itemNum1<m_heroMap[j].at(i)->itemNum2)
			{
				if (i%2==0)
				{
					vector<CHero*> *vec =&m_heroMap[j];
					vec->insert(vec->begin()+i,&m_heroList.at(insertIndex+1));
					vec->insert(vec->begin()+i+1,&m_heroList.at(insertIndex));
				}
				else
				{
					vector<CHero*> *vec =&m_heroMap[j];
					vec->insert(vec->begin()+i,&m_heroList.at(insertIndex));
					vec->insert(vec->begin()+i+1,&m_heroList.at(insertIndex+1));
					vec->insert(vec->begin()+i+2,&m_heroList.at(insertIndex));
				}
				break;
			}
		}
	}
	
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_subLay->getChildByTag(10);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		if (radioBtn->isChecked())
		{
			m_currHeroList = &m_heroMap[radioBtn->getTag()];
			m_gridView->setCountOfCell(m_currHeroList->size());
			ReloadGridViewWithSameOffset(m_gridView);
			if(isFirst)
			{
				m_gridView->setContentOffsetToTop();
			}
			break;
		}
	}
}

void CHeroList::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CHero *hero = m_currHeroList->at(uIdx);
	
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);

	for (int i = 1; i <=9; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			((CImageView*)child)->setUserData(hero);
			((CImageView*)child)->setTouchEnabled(true);
			((CImageView*)child)->setOnClickListener(this,ccw_click_selector(CHeroList::onSelectHero));
		}
		else if (i==2)
		{
			
		}
		else if (i==3)
		{
			CCSprite *mask = (CCSprite*)child;
			mask->setTexture(setItemQualityTexture(hero->iColor));
			CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
			if (!head)
			{
				head = CCSprite::create("headImg/101.png");
				CCLOG("[ ERROR ] CHeroList::addGridCell Lost Image = %d",hero->thumb);
			}
			head->setPosition(child->getPosition());
			head->setScale(0.95f);
			pCell->addChild(head);

			//head加底
			CCSprite* pBoard = CCSprite::create("common/cell.png");
			head->addChild(pBoard, -1);
			NodeFillParent(pBoard);

			//添加星星
			CLayout* pStarLayout = getStarLayout(hero->quality);
			mask->addChild(pStarLayout);
		}
		else if (i==5)
		{
			CLabel *name = (CLabel *)child;
			if(data && data->heroName.size() > 0)
				name->setString(data->heroName.c_str());
		}
		else if (i==4)
		{
			if(data)
			{
				CCSprite *heroType = (CCSprite*)child;
				heroType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", hero->roletype, data->iType2)->getCString()));
			}
		}
		else if (i==7)
		{
			CLabel *name = (CLabel *)child;
			name->setString(ToString(hero->level));
		}
		else if (i==9)
		{
			child->setVisible(hero->evol);
		}
	}
// 
// 	for (int i=5; i>=1&&i>hero->star;i--)
// 	{
// 		CCSprite *star = (CCSprite*)pCell->findWidgetById(CCString::createWithFormat("star%d",i)->getCString());
// 		star->setVisible(false);
// 		
// 	}
// 	if (hero->star==2||hero->star==4)
// 	{
// 		for (int i = 1; i <=hero->star; i++)
// 		{
// 			CCSprite *star = (CCSprite*)pCell->findWidgetById(CCString::createWithFormat("star%d",i)->getCString());
// 			star->setPositionX(star->getPositionX()+star->boundingBox().size.width/2-4);
// 		}	
// 	}
     CLayout* pLayout = (CLayout*)lay->findWidgetById("armor");
	 addArmorForCell(pCell, pLayout, hero);
}

void CHeroList::addArmorForCell( CGridViewCell* pCell, CLayout* pLayout, CHero* pHero )
{
	pLayout->removeFromParent();
	pCell->addChild(pLayout);

	for(unsigned int i=0; i<4; i++)
	{
		CItem* pItem;
		if(i==0) pItem = &pHero->armor1;
		if(i==1) pItem = &pHero->armor2;
		if(i==2) pItem = &pHero->armor3;
		if(i==3) pItem = &pHero->armor4;

		//100.锁住 200.解锁了没装备 300.解锁了有装备 400.库里有可用装备
		int iParam = pItem->itparam;
		//卸下装备后，库里就有库存了，这里处理一下
		if(m_iEquipStock[i]>0 && iParam==200)
		{
			iParam = 400;
		}
		else if(m_iEquipStock[i]==0 && iParam==400)
		{
			iParam = 200;
		}

		//添加
		CCSprite* pAdd = (CCSprite*)(pLayout->findWidgetById(CCString::createWithFormat("add%d", i+1)->getCString()));

		//锁
		CImageView* pLock = (CImageView*)(pLayout->findWidgetById(CCString::createWithFormat("_lock%d", i+1)->getCString()));

		//边框
		CImageView* pMask = (CImageView*)(pLayout->findWidgetById(CCString::createWithFormat("mask%d", i+1)->getCString()));
		pMask->removeChildByTag(1);
		pMask->removeChildByTag(2);

		//未装备字样
		pAdd->removeAllChildren();
		CCSprite* pUnEquip = CCSprite::create("common/word_unequipped.png");
		pUnEquip->setPosition(ccp(pAdd->getContentSize().width/2, pUnEquip->getContentSize().height/2));
		pAdd->addChild(pUnEquip);

		//开放
		if(iParam != 100)
		{
			pLock->setVisible(false);
			pLock->setTouchEnabled(false);

			pMask->setVisible(true);
			//pMask->setTouchEnabled(true);
			//pMask->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onSelectItem));
			//pMask->setTag(i+1);

			//是否装备
			if(iParam == 300)
			{
				CCSprite *item = CCSprite::create(GetImageName(1, pItem->quality, pItem->iconId).c_str());
				if (!item)
				{
					item = CCSprite::create("prop/32003.png");
					CCLOG("[ ERROR ] CHeroAttribute::showArmor Lost Image = %d", pItem->iconId);
				}
				item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
				item->setTag(1);
				pMask->addChild(item, -1);

				pMask->setTexture(setItemQualityTexture(pItem->quality));
				pAdd->setVisible(false);
			}
			//没有装备
			else if(iParam == 200)
			{
				pAdd->setVisible(true);
				pMask->setTexture(setItemQualityTexture(1));//默认白色
			}
			//400.库里有可用装备
			else if (iParam==400)
			{
				pUnEquip->setVisible(false);
				pAdd->setVisible(true);
				pMask->setTexture(setItemQualityTexture(1));//默认白色

				CCSprite *item = CCSprite::create("common/icon_add1.png");
				item->setPosition(ccp(pMask->getContentSize().width/2, pMask->getContentSize().height/2));
				item->setTag(2);
				pMask->addChild(item);
			}

		}
		//没开放
		else
		{
			pAdd->setVisible(false);

			pMask->setTouchEnabled(false);
			pMask->setTexture(setItemQualityTexture(1));

			pLock->setVisible(true);
			//pLock->setTouchEnabled(true);
			//pLock->setOnClickListener(this, ccw_click_selector(CHeroAttribute::onTipsForUnlockArmor));
			//pLock->setTag(i);
		}
	}
}

void CHeroList::addCallCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CHero *hero = m_currHeroList->at(uIdx);

	if (hero->heroid<=0&&uIdx%2==0)
	{
		CLayout *lay = UICloneMgr::cloneLayout(m_emptyLay);
		for (int i = 1; i <=3; i++)
		{
			CCNode *child = lay->getChildByTag(i);
			lay->removeChild(child);
			pCell->addChild(child);
		}
	}
	else if (hero->heroid>0)
	{
		CLayout *lay = UICloneMgr::cloneLayout(m_cell1);
		
		const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);

		for (int i = 1; i <=10; i++)
		{
			CCNode *child = lay->getChildByTag(i);
			lay->removeChild(child);
			pCell->addChild(child);

			if (i==1)
			{
				((CImageView*)child)->setUserData(hero);
				((CImageView*)child)->setTouchEnabled(true);
				((CImageView*)child)->setOnClickListener(this,ccw_click_selector(CHeroList::onGetItem));
				child->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
			else if (i==2)
			{

			}
			else if (i==3)
			{
				CCSprite *mask = (CCSprite*)child;
				mask->setTexture(setItemQualityTexture(hero->iColor));
				CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
				if (!head)
				{
					head = CCSprite::create("headImg/101.png");
					CCLOG("[ ERROR ] CHeroList::addCallCell Lost Image = %d",hero->thumb);
				}
				head->setPosition(child->getPosition());
				head->setScale(0.95f);
				pCell->addChild(head);

				//head加底
				CCSprite* pBoard = CCSprite::create("common/cell.png");
				head->addChild(pBoard, -1);
				NodeFillParent(pBoard);
				head->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
			else if (i==5)
			{
				CLabel *name = (CLabel *)child;
				if(data && data->heroName.size() > 0)
					name->setString(data->heroName.c_str());
			}
			else if (i==4)
			{
				CCSprite *heroType = (CCSprite*)child;
				if (data && hero->roletype>0)
				{	
					heroType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", hero->roletype, data->iType2)->getCString()));
				}
				child->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
			}
			else if (i==7)
			{
				CLabel *name = (CLabel *)child;
				if (hero->itemNum1<hero->itemNum2)
				{			
					name->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());
				}
				else
				{
					name->setString(GETLANGSTR(1014));
				}
			}
			else if (i==9)
			{
				CProgressBar *progress = (CProgressBar*)child;
				progress->setMaxValue(hero->itemNum2);
				progress->setValue(hero->itemNum1);
			}
		}
	}
}

void CHeroList::onSelectHero(CCObject *pSender)
{
	PlayEffectSound(SFX_Button);

	CHero *hero = (CHero*)((CButton*)pSender)->getUserData();
	//showNewHeroEffect(hero);
	CPlayerControl::getInstance().sendGetHeroInfo(hero->id,0);
}

void CHeroList::onGetItem(CCObject *pSender)
{
	PlayEffectSound(SFX_Button);

	CHero *hero = (CHero*)((CButton*)pSender)->getUserData();
	if (hero->itemNum1>=hero->itemNum2)
	{
		m_selectHero = hero;
		UserData *user = DataCenter::sharedData()->getUser()->getUserData();
		if (user->getCoin()<1000)
		{
			//ShowPopTextTip(GETLANGSTR(205));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceCoin);
			return;
		}
		ShowConfirmTextTip(GETLANGSTR(1015), this,ccw_click_selector(CHeroList::onRecruitConfirm));
		//CPlayerControl::getInstance().sendRecruitHero(hero->heroid);
	}
	else
	{
		m_pSelectCallHero = hero;
		CHeroCall *evolveLayer = CHeroCall::create();
		evolveLayer->setNeedNum(hero->itemNum2);
		LayerManager::instance()->push(evolveLayer);
		CPlayerControl::getInstance().sendItemInfo(hero->itemId);
	}
}

void CHeroList::onRecruitConfirm(CCObject *pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		CPlayerControl::getInstance().sendRecruitHero(m_selectHero->heroid);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

CHeroList::~CHeroList()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_cell1);
	CC_SAFE_RELEASE(m_emptyLay);
}


void CHeroList::showNewHeroEffect(CHero *hero)
{
	//获得新英雄出场
	HeroLotteryData data;
	data.heroType = hero->roletype;
	data.thumb = hero->thumb;
	data.quality = hero->quality;
	m_pNewHeroEffect->showNewHeroEffect(&data);
}

void CHeroList::recoverPosition(CCNode *pNode)
{
	pNode->setPositionY(pNode->getPositionY()-100);
	((CCSprite*)pNode)->setOpacity(255);
}

void CHeroList::updateEquipStockCount( CCObject* pObj )
{
	EquipStockData* pData = (EquipStockData*)pObj;
	m_iEquipStock[pData->m_iIndex] = pData->m_iCount;
}

void CHeroList::compaseSuccess( CCObject* pObj )
{
	m_pSelectCallHero->itemNum1++;
	ReloadGridViewWithSameOffset(m_gridView);
}
