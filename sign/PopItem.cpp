#include "PopItem.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"
#include "common/CommonFunction.h"
#include "mainCity/CNewHero.h"

#include "common/CGameSound.h"
#include "battle/AnimationManager.h"

using namespace BattleSpace;
CPopItem::CPopItem()
	:m_pMonsterInfo(nullptr)
	,m_pItemInfo(nullptr)
	,m_pNewHeroEffect(nullptr)
	,m_iIndexForFindNewHero(0)
	,m_pTargetCallBack(nullptr)
	,m_pTargetFun(nullptr)
{

}


bool CPopItem::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("PopItem");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("PopItem.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

		m_pNewHeroEffect = CNewHero::create();
		m_pNewHeroEffect->setVisible(false);
		this->addChild(m_pNewHeroEffect, 20);

		return true;
	}
	return false;

}

void CPopItem::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPopItem::onEnter()
{
	BaseLayer::onEnter();
	CButton *btn= (CButton *)m_ui->findWidgetById("ok");
	btn->setOnClickListener(this,ccw_click_selector(CPopItem::onClose));
	this->setDescVisiable(false);
	
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionHorizontal);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CPopItem::tableviewDataSource));
	m_tableView->reloadData();
}


void CPopItem::onExit()
{
	BaseLayer::onExit();
	doCallBack();
}


CPopItem::~CPopItem()
{
	clearPrizeSave();
	CC_SAFE_RELEASE(m_pTargetCallBack);
}


CCObject* CPopItem::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addTableCell(uIdx,pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx,pCell);
	}
	return pCell;
}

void CPopItem::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);		

	CPrize& prize = m_prizeRes.prizeList.at(uIdx);

	string str = GetImageName(prize.type, prize.color, prize.thumb);

	for (int i = 1; i <= 3; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			{
				CImageView *mask= (CImageView *)child;
				CImageView *itemSpr = CImageView::create(str.c_str());
				if (!itemSpr)
				{
					itemSpr = CImageView::create("prop/32003.png");
					CCLOG("[ ERROR ] CPopItem::addTableCell Lost Image = %d");
				}
				itemSpr->setPosition(ccp(mask->getContentSize().width/2,mask->getContentSize().height/2));
				mask->addChild(itemSpr);
			}
			break;
		case 2:
			{
				CImageView *mask= (CImageView *)child;
				SmartSetRectPrizeColor(mask, &prize);
				mask->setUserData(&prize);
				mask->setTouchEnabled(true);
				mask->setOnPressListener(this, ccw_press_selector(CPopItem::onPress));

				//添加星星
				CLayout* pStarLayout = SmartGetStarLayout(&prize);
				mask->addChild(pStarLayout);
			}
			break;
		case 3:
			{
				int iPrizeNum = prize.num;

				if (iPrizeNum>1)
				{
					CLabel *num = (CLabel*)child;
					num->setVisible(false);
					pCell->removeChild(num);
					CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
					haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
					haveNum->setPosition(ccp(82, 2));
					pCell->addChild(haveNum, 99);
					haveNum->setString(ToString(iPrizeNum));
				}
				else
				{
					child->setVisible(false);
				}
			}
			break;
		}
	}
}

void CPopItem::signPrize(CPrize *prize)
{
	//滑动条隐藏
	CCNode *scroll= (CImageView *)m_ui->findWidgetById("scroll");
	scroll->setVisible(false);

	CImageView *bg= (CImageView *)m_ui->findWidgetById("itembg1");
	CImageView *mask= (CImageView *)m_ui->findWidgetById("mask1");
	

	CLabel *itemnum= (CLabel *)m_ui->findWidgetById("num1");
	itemnum->setVisible(false);
	CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
	haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
	haveNum->setPosition(ccp(90, 5));
	mask->addChild(haveNum, 99);
	haveNum->setString(ToString(prize->num));

	if(prize->num<=1)
	{
		haveNum->setVisible(false);
	}

	CCSprite *itemSpr = CCSprite::create(GetImageName(prize->type, prize->color, prize->thumb).c_str());
	if (!itemSpr)
	{
		itemSpr = CCSprite::create("prop/32003.png");
		CCLOG("[ ERROR ] CPopItem::getPriz Lost Image = %d",prize->thumb);
	}

	if (itemSpr)
	{	
		itemSpr->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
		bg->addChild(itemSpr);
	}

	SmartSetRectPrizeColor(mask, prize);

	clearPrizeSave();
	
	CPrize* pPrize = new CPrize;
	*pPrize = *prize;
	m_prizeSave.push_back(pPrize);
	mask->setUserData(pPrize);
	mask->setTag(pPrize->type);
	mask->setTouchEnabled(true);
	mask->setOnPressListener(this, ccw_press_selector(CPopItem::onPress));

	//添加星星
	if(pPrize->quality > 0)
	{
		CLayout* pStarLayout = SmartGetStarLayout(prize);
		mask->addChild(pStarLayout);
	}

	if (prize->type==2)
	{
		HeroLotteryData data;
		data.heroType = prize->type;
		data.thumb = prize->thumb;
		data.quality = prize->color;
		m_pNewHeroEffect->showNewHeroEffect(&data);
	}
	else
	{
		CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8042");
		CCSprite *light = CCSprite::create("skill/8042.png");
		light->setPosition(VCENTER);
		light->runAction(CCSequence::createWithTwoActions(CCAnimate::create(lightAnim),CCRemoveSelf::create()));
		light->setScale(1138/light->getContentSize().width);
		m_ui->addChild(light);

		{
			CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8050");
			lightAnim->setDelayPerUnit(0.05f);
			CCSprite *light = CCSprite::create("skill/8050.png");
			light->setPosition(VCENTER);
			light->setScale(2.2f);
			light->runAction(CCRepeatForever::create(CCAnimate::create(lightAnim)->reverse()));
			m_ui->addChild(light);
		}
	}
}

void CPopItem::popItemList(const vector<CItem>& itemList)
{
	m_tableView->setVisible(false);

	CLayout* pLay = (CLayout*)m_ui->findWidgetById("lay_item");
	int offX[3] = {0, -63, -120};
	int iSize = itemList.size();
	iSize = iSize>3?3:iSize;
	pLay->setPositionX(pLay->getPositionX()+offX[iSize-1]);

	clearPrizeSave();

	for (int i = 0; i < iSize; i++)
	{
		const CItem &item = itemList.at(i);
		CImageView *bg= (CImageView *)m_ui->findWidgetById(CCString::createWithFormat("itembg%d",i+1)->getCString());
		CImageView *mask= (CImageView *)m_ui->findWidgetById(CCString::createWithFormat("mask%d",i+1)->getCString());
		mask->setTexture(setItemQualityTexture(item.quality));

		CLabel *itemnum= (CLabel *)m_ui->findWidgetById(CCString::createWithFormat("num%d",i+1)->getCString());
		itemnum->setVisible(false);
		CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
		haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
		haveNum->setPosition(ccp(90, 5));
		mask->addChild(haveNum, 99);
		haveNum->setString(ToString(item.itemNum));

		if(item.itemNum<=1)
		{
			haveNum->setVisible(false);
		}

		CImageView *itemSpr = getItemSprite(&item);
		if (itemSpr)
		{	
			itemSpr->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
			bg->addChild(itemSpr);
			if(itemSpr->getContentSize().width>bg->getContentSize().width)
			{
				NodeFillParent(itemSpr);
			}
		}

		CPrize* prize = new CPrize;
		prize->type = 1;
		prize->num = item.itemNum;
		prize->color = item.quality;
		prize->id = item.itemId;
		prize->thumb = item.iconId;

		m_prizeSave.push_back(prize);
		mask->setTouchEnabled(true);
		mask->setUserData(prize);
		mask->setOnPressListener(this, ccw_press_selector(CPopItem::onPress));


		//添加星星
		if(prize->quality > 0)
		{
			CLayout* pStarLayout = SmartGetStarLayout(prize);
			mask->addChild(pStarLayout);
		}


		bg->setVisible(true);
		mask->setVisible(true);
	}
}

void CPopItem::setBtnHandler( CCObject *pSender, SEL_ClickHandler pHandler)
{
	CLayout *selop= (CLayout *)m_ui->findWidgetById("selop");
	selop->setVisible(true);
	CButton *ok = (CButton*)(selop->getChildByTag(1));
	ok->setOnClickListener(pSender,pHandler);
	CButton *cancel = (CButton*)(selop->getChildByTag(2));
	cancel->setOnClickListener(pSender,pHandler);

	CButton *okbtn= (CButton *)m_ui->findWidgetById("ok");
	okbtn->setVisible(false);
	CLabel *okfont= (CLabel *)m_ui->findWidgetById("confirm");
	okfont->setVisible(false);
}

void CPopItem::setDescVisiable(bool isVisible)
{
	CLabel *desc= (CLabel *)m_ui->findWidgetById("desc");
	desc->setVisible(isVisible);
}


void CPopItem::popPrizeRes(CGetPrizeRes *prizeRes)
{
	m_prizeRes = *prizeRes;

	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("itembg1");
	bg->setVisible(false);

	CCSprite *mask= (CCSprite *)m_ui->findWidgetById("mask1");
	mask->setVisible(false);

	CLabel *itemnum= (CLabel *)m_ui->findWidgetById("num1");
	itemnum->setVisible(false);

	int cellCount = prizeRes->prizeList.size(); 

	m_tableView->setCountOfCell(cellCount);
	m_tableView->reloadData();

	//新英雄动画
	m_iIndexForFindNewHero= 0;
	callBackForShowNewHero();


	switch (cellCount)
	{
	case 1:
		{
			m_tableView->setContentOffset(ccp(180, 0));
			m_tableView->setEnabled(true);
			m_tableView->setDragable(false);
		}break;
	case 2:
		{
			m_tableView->setContentOffset(ccp(125, 0));
			m_tableView->setEnabled(true);
			m_tableView->setDragable(false);
		}break;
	case 3:
		{
			m_tableView->setContentOffset(ccp(75, 0));
			m_tableView->setEnabled(true);
			m_tableView->setDragable(false);
		}break;
	case 4:
		{
			m_tableView->setContentOffset(ccp(25, 0));
			m_tableView->setEnabled(true);
			m_tableView->setDragable(false);
		}break;
	default:
		{
			m_tableView->setEnabled(true);
			m_tableView->setContentOffsetInDuration(m_tableView->getMinOffset(), (cellCount-4)*0.5f);
		}
		break;
	}

	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8042");
	CCSprite *light = CCSprite::create("skill/8042.png");
	light->setPosition(VCENTER);
	light->runAction(CCSequence::createWithTwoActions(CCAnimate::create(lightAnim),CCRemoveSelf::create()));
	light->setScale(1138/light->getContentSize().width);
	m_ui->addChild(light);

	{
		CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8050");
		lightAnim->setDelayPerUnit(0.05f);
		CCSprite *light = CCSprite::create("skill/8050.png");
		light->setPosition(VCENTER);
		light->setScale(2.2f);
		light->runAction(CCRepeatForever::create(CCAnimate::create(lightAnim)->reverse()));
		m_ui->addChild(light);
	}
} 

void CPopItem::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CPrize* pPrize = (CPrize*)pImage->getUserData();
	int iType = pPrize->type;

	//奖励类型(1 道具，2 英雄，3 铜钱，4 元宝，5 粮食, 6体力)
	switch (iType)
	{
	case 1:
	case 6:
	case 3:
	case 4:
	case 5:
		{
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

					m_pItemInfo->setInfo(pPrize);
					m_pItemInfo->bindPos(pImage);
					m_pItemInfo->changeForSign(pPrize);
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

		}break;
	case 2:
		{
			if(m_pMonsterInfo==nullptr)
			{
				m_pMonsterInfo = CMonsterInfo::create();
				this->addChild(m_pMonsterInfo);
			}

			switch (iState)
			{
			case CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CHero pHero;
					pHero.heroid = pPrize->id;
					pHero.thumb = pPrize->thumb;
					pHero.quality = pPrize->quality;
					pHero.iColor = pPrize->color;
					pHero.level = 1;
					m_pMonsterInfo->setHero(&pHero);
					m_pMonsterInfo->bindPos(pImage);
					m_pMonsterInfo->changeForSign(&pHero);
					m_pMonsterInfo->show();
					CCLOG("show");
				}
				break;
			case CTouchPressOff:
				{
					m_pMonsterInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	default:
		break;
	}
}

void CPopItem::callBackForShowNewHero( )
{
	for(; m_iIndexForFindNewHero<m_prizeRes.prizeList.size(); m_iIndexForFindNewHero++)
	{
		CPrize& prize = m_prizeRes.prizeList.at(m_iIndexForFindNewHero);

		//获得新英雄出场
		if(prize.type == 2)
		{
			HeroLotteryData data;
			data.heroType = prize.type;
			data.thumb = prize.thumb;
			data.quality = prize.color;
			m_pNewHeroEffect->showNewHeroEffect(&data);
			m_pNewHeroEffect->runAction(CCSequence::createWithTwoActions(
				CCDelayTime::create(3.0f), 
				CCCallFunc::create(this, callfunc_selector(CPopItem::callBackForShowNewHero))));
			m_iIndexForFindNewHero++;
			return;
		}
	}

	m_pNewHeroEffect->setVisible(false);
}

void CPopItem::clearPrizeSave()
{
	for(unsigned int i=0; i<m_prizeSave.size(); i++)
	{
		CC_SAFE_DELETE(m_prizeSave[i]);
	}
	m_prizeSave.clear();
}

void CPopItem::doCallBack()
{
	if(m_pTargetCallBack != nullptr && m_pTargetFun!=nullptr)
	{
		(m_pTargetCallBack->*m_pTargetFun)();
	}
	m_pTargetCallBack = nullptr;
	m_pTargetFun = nullptr;
}

void CPopItem::bindTargetCallBack( CCObject* pObj, PopItemCallBack pFun )
{
	CC_SAFE_RETAIN(pObj);
	CC_SAFE_RELEASE(m_pTargetCallBack);

	m_pTargetCallBack = pObj;
	m_pTargetFun = pFun;
}
