#include "SelectDefense.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "common/CommonFunction.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"

#include "tools/StringUtil.h"
#include "hero/HeroAttribute.h"
#include "hero/HeroControl.h"

#include "common/CGameSound.h"
#include "Resources.h"


CSelectDefense::~CSelectDefense()
{
	CC_SAFE_RELEASE(m_cell);
}

CSelectDefense::CSelectDefense()
	:m_iStrategyIndex(0),m_pTouch(nullptr),m_bHold(false),m_pDragHeroData(nullptr),m_bBaseTouch(true)
	,m_pBatchGreen(nullptr), m_pCaptainTile(nullptr)
{

}

bool CSelectDefense::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("selectDefense.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setIsShowBlack(false);

		return true;
	}
	return false;
}

void CSelectDefense::onEnter()
{ 
	BaseLayer::onEnter();

	//请求英雄列表
	CPlayerControl::getInstance().sendHeroList(0);
	
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(2);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSelectDefense::gridviewDataSource));
	m_gridView->reloadData();
	m_gridView->setContentOffsetToTop();

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->findWidgetById("grid");
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CSelectDefense::onSwitchBtn));
	}

	CRadioBtnGroup *pStrategyGroup = (CRadioBtnGroup*)m_ui->findWidgetById("grid_strategy");
	for(int i=0 ;i<3; i++)
	{
		CRadioButton *pRadioBtn = (CRadioButton*)pStrategyGroup->getChildByTag(i+1);
		pRadioBtn->setOnCheckListener(this, ccw_check_selector(CSelectDefense::onSwitchStrategy));
	}

	GetTcpNet->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CSelectDefense::processMessage));
	GetTcpNet->registerMsgHandler(AskPvpTeamDataMsg,this,CMsgHandler_selector(CSelectDefense::processMessage));
}

void CSelectDefense::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	NOTIFICATION->removeAllObservers(this);
}



void CSelectDefense::updateMainSkillDesc()
{
	//if(m_union.heroList.size()==0) return;

	//const CHero &hero = m_union.heroList.at(0);
	//CLabel *mainSkillLab = (CLabel*)(m_ui->findWidgetById("desc"));
	//if (hero.id!=0&&hero.skill5.skillId>0)
	//{
	//	const SkillCfg *cfg = DataCenter::sharedData()->getSkill()->getCfg(hero.skill5.skillId);
	//	if (cfg)
	//	{ 
	//		mainSkillLab->setString(cfg->desc.c_str());
	//	}
	//	else
	//	{
	//		mainSkillLab->setString("");
	//	}
	//}
	//else
	//{
	//	mainSkillLab->setString("");
	//}
}



CCObject* CSelectDefense::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CSelectDefense::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;

	int selIndex = 2*btn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		m_currHeroList = &m_heroMap[btn->getTag()];
		m_gridView->setCountOfCell(m_currHeroList->size());
		//ReloadGridViewWithSameOffset(m_gridView);
		m_gridView->reloadData();
	}
}

void CSelectDefense::onSwitchStrategy( CCObject *pSender, bool bChecked )
{
	//TODO
}

void CSelectDefense::updateHeroList()
{
	for (int i = 0; i < m_heroList.size(); i++)
	{
		CHero& hero = m_heroList.at(i);
		hero.coin = -1;
		(m_heroMap[m_heroList.at(i).roletype]).push_back(&hero);  //&heroList.at(i);
		(m_heroMap[4]).push_back(&hero);
	}
	updateCurrHeroList();
}

void CSelectDefense::addHeroSelectCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CHero *hero = m_currHeroList->at(uIdx);
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(hero->thumb);

	for (int i = 1; i <=6; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView *pBg = (CImageView*)child;
			pBg->setUserData(hero);
			pBg->setTouchEnabled(true);
			pBg->setOnClickListener(this,ccw_click_selector(CSelectDefense::onSelectHero));

			CImageView *pHead = CImageView::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
			if (!pHead)
			{
				pHead = CImageView::create("headImg/101.png");
				CCLOG("[ ERROR ] CSelectDefense::addGridCell Lost Image = %d",hero->thumb);
			}	
			pBg->addChild(pHead);
			NodeFillParent(pHead);
		}
		else if (i==3)
		{
			CCSprite *mask = (CCSprite*)child;
			mask->setTexture(SetRectColor(hero->roletype));

			//添加星级
			CLayout* pStarLayout = getStarLayout(hero->iColor);
			mask->addChild(pStarLayout);
		}	
		else if (i==4)
		{
			CLabel *name = (CLabel *)child;
			name->setString(ToString(hero->level));
		}
		else if (i==5)
		{
			CCNodeRGBA *pBlack = (CCNodeRGBA *)child;
			pBlack->setOpacity(160);
		}
	}
}


void CSelectDefense::onSelectHero(CCObject *pSender)
{
	//TODO
}

void CSelectDefense::processMessage(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);
		LayerManager::instance()->getLayer("CPvpMenuLayer")->setVisible(true);

		//隐藏被遮盖的层，不渲染
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
	}

	switch (type)
	{
	case HeroListMsg:
		{
			HeroListResponse *res = (HeroListResponse*)msg;
			m_heroList.clear();
			for (int i=0; i<res->herolist_size(); i++)
			{
				Hero hero = res->herolist(i);
				CHero obj;
				obj.readData(hero);
				m_heroList.push_back(obj);
			}
			this->updateHeroList();
			
		}	
		break;
	case AskPvpTeamDataMsg:
		{


			PvpTeamData* pData = (PvpTeamData*)msg;

			//TODO

		}
		break;
	}
}

void CSelectDefense::updateCurrHeroList()
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		if (radioBtn->isChecked())
		{
			m_currHeroList = &m_heroMap[radioBtn->getTag()];
			m_gridView->setCountOfCell(m_currHeroList->size());
			m_gridView->reloadData();
			break;
		}
	}
}

bool CSelectDefense::ccTouchBegan( CCTouch* pTouch, CCEvent* pEvent )
{
	bool bTouched = false;

	m_bBaseTouch = true;

	//是否触摸到gridview
	CCPoint touchPointInView = m_gridView->getParent()->convertToNodeSpace(pTouch->getLocation());
	if(m_gridView->boundingBox().containsPoint(touchPointInView))
	{
		//触摸处理
		m_bBaseTouch = false;

		bTouched = true;
		
		m_bHold = false;

		m_pDragHeroData = nullptr;

		//判断是否点击到了头像
		CCArray* pChildren = m_gridView->getContainer()->getChildren();
		for(unsigned int i=0; i<m_gridView->getCountOfCell(); i++)
		{
			CCNode* pNode = (CCNode*)pChildren->objectAtIndex(i);
			CImageView* pImage = (CImageView*)pNode->getChildByTag(1);
			CCPoint touchPointInView = pImage->getParent()->convertToNodeSpace(pTouch->getLocation());
			if(pImage->boundingBox().containsPoint(touchPointInView))
			{
				//点击到了头像，记录当前英雄数据
				m_pDragHeroData = (CHero*)pImage->getUserData();

				//按住检测
				stopActionByTag(SelectDefenseLayerActionTouchHoldCheck);
				CCSequence* pSeq = CCSequence::createWithTwoActions(CCDelayTime::create(SelectDefenseHoldTime), CCCallFunc::create(this, callfunc_selector(CSelectDefense::callBackForTouchHold)));
				pSeq->setTag(SelectDefenseLayerActionTouchHoldCheck);
				this->runAction(pSeq);

				break;
			}
		}

		if(m_pDragHeroData == nullptr)
		{
			m_gridView->onTouchBegan(pTouch);
		}
	}
	else
	{
		//进入触摸控制
		bTouched = BaseLayer::ccTouchBegan(pTouch, pEvent);
	}

	if(bTouched && m_pTouch == nullptr)
	{
		m_pTouch = pTouch;
	}

	return bTouched;
}

void CSelectDefense::ccTouchMoved( CCTouch* pTouch, CCEvent* pEvent )
{
	if(m_pTouch == pTouch)
	{
		//检测时间内
		if(getActionByTag(SelectDefenseLayerActionTouchHoldCheck) != nullptr)
		{
			//检测变动范围
			//计算是否丢失事件，8像素以内，判断为hold
			CCPoint pStart = pTouch->getStartLocation();
			CCPoint pCurrent = pTouch->getLocation();
			float fDis = ccpDistance(pStart, pCurrent);
			if(fDis>8)
			{
				stopActionByTag(SelectDefenseLayerActionTouchHoldCheck);
			}
			return;
		}

		//如果为hold状态
		if(m_bHold)
		{
			//TODO
			//执行拖动
			CCLOG("%f--%f", pTouch->getLocation().x, pTouch->getLocation().y);
		}
		else
		{
			//执行scrollView
			m_gridView->onTouchMoved(pTouch, 0.1f);
		}

		if(m_bBaseTouch)
		{
			BaseLayer::ccTouchMoved(pTouch, pEvent);
		}
	}
}

void CSelectDefense::ccTouchEnded( CCTouch* pTouch, CCEvent* pEvent )
{
	if(m_pTouch == pTouch)
	{
		m_pTouch = nullptr;
		stopActionByTag(SelectDefenseLayerActionTouchHoldCheck);
		if(m_bHold)
		{
			//TODO
		}
		else
		{
			m_gridView->onTouchEnded(pTouch, 0.1f);
		}

		if(m_bBaseTouch)
		{
			BaseLayer::ccTouchEnded(pTouch, pEvent);
		}
	}
}

void CSelectDefense::ccTouchCancelled( CCTouch* pTouch, CCEvent* pEvent )
{
	ccTouchEnded(pTouch, pEvent);
}

void CSelectDefense::callBackForTouchHold()
{
	//按住成功
	m_bHold = true;
}

void CSelectDefense::initGreen()
{
	m_pBatchGreen = CCSpriteBatchNode::create("pvp/pvp_box_blue.png");
	m_ui->findWidgetById("war_map")->addChild(m_pBatchGreen);
	
	//队长格子
	m_pCaptainTile = (CImageView*)m_ui->findWidgetById("");

	//拿到绿色格子
	CCSprite* pGreen = (CCSprite*)m_ui->findWidgetById("green_start");
	//4X4
	CCPoint pStartPos = pGreen->getPosition();
	CCPoint pGap = ccp(-30, 27);
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			CCSprite* pGreenTile = CCSprite::create("pvp/pvp_box_blue.png");
			pGreenTile->setPosition(pStartPos+ccp(pGap.x*i, pGap.y*j));
			m_pBatchGreen->addChild(pGreenTile);
		}
	}
}


