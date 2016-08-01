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
#include "Battle/BattleMacro.h"
#include "common/ShaderDataHelper.h"
#include "Battle/ConstNum.h"

CSelectDefense::~CSelectDefense()
{
	CC_SAFE_RELEASE(m_cell);
}

CSelectDefense::CSelectDefense()
	:m_iStrategyIndex(0),m_pTouch(nullptr),m_bHold(false),m_pDragHeroData(nullptr),m_bBaseTouch(true)
	,m_pBatchGreen(nullptr), m_Captain(nullptr),mIconNode(nullptr),mMoveIcon(nullptr),mStrategyIndex(1)
	,mgetBattleInfo(false),mgetHeroList(false)
{}

bool CSelectDefense::init()
{
	if (BaseLayer::init())
	{
		m_ui = LoadComponent("selectDefense.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setIsShowBlack(false);
		if (!initGreen())
			return false;

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

	//重置
	CButton *pReset = (CButton *)m_ui->findWidgetById("reset");
	pReset->setOnClickListener(this, ccw_click_selector(CSelectDefense::reset));

	GetTcpNet->registerMsgHandler(HeroListMsg,this,CMsgHandler_selector(CSelectDefense::processMessage));
	GetTcpNet->registerMsgHandler(AskPvpTeamDataMsg,this,CMsgHandler_selector(CSelectDefense::processMessage));
}

void CSelectDefense::onExit()
{
	saveTeamData();
	BaseLayer::onExit();
	mHeroIcons.clear();
	mIconPoint.clear();
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	NOTIFICATION->removeAllObservers(this);

}


void CSelectDefense::reset( CCObject *pSender )
{
	updateCaptainSkill(nullptr);
	changeFloor(false);
	clearIcon();
	m_gridView->reloadData();
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
	CRadioButton *btn = (CRadioButton*)pSender;

	int iIndex = btn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		updateStrategy( iIndex );
	}
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
			pHead->setTag(1);
			pHead->setUserData(hero);
			pBg->addChild(pHead);
			NodeFillParent(pHead);

			//变黑处理
			setSelectHeroEffect( pHead,  isHeroSelected(hero->id));

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


bool CSelectDefense::isHeroSelected( int iId )
{
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;

		CHero* tHero = (CHero*)tIcon->getUserData();
		int tGrid = getGridByPoint(tIcon->getPosition());
		if( tHero->id == iId )
		{
			return true;
		}
	}
	return false;
}


void CSelectDefense::setSelectHeroEffect( CImageView *pImage, bool bBlack )
{
	if(pImage == nullptr)
	{
		return;
	}
	if ( bBlack )
	{
		pImage->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	}
	else
	{
		pImage->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
	}
}


CImageView * CSelectDefense::getHeroImageFromScrollViewById( int iId )
{
	CCArray *pChildren = m_gridView->getContainer()->getChildren();
	CCObject *pObj = nullptr;
	CCARRAY_FOREACH(pChildren, pObj)
	{
		CCNode *pCell = (CCNode *)pObj;
		CImageView *pHeadView = dynamic_cast<CImageView *>(pCell->getChildByTag(1)->getChildByTag(1));
		if(pHeadView)
		{
			CHero *pHero = (CHero *)pHeadView->getUserData();
			if(pHero)
			{
				if( pHero->id == iId )
				{
					return pHeadView;
				}
			}
		}
	}
	return nullptr;
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
			mgetHeroList = true;
			if (mgetBattleInfo)
				initHeroIconByMsg();
		}break;
	case AskPvpTeamDataMsg:										//TODO
		{
			mPvPData = *((PvpTeamData*)msg);
			mgetBattleInfo = true;								//两条服务器消息并不是同步到达，两条同时达到后才能初始化数据
			if (mgetHeroList)
				initHeroIconByMsg();
		}break;
	}
}
//根据服务器信息初始化界面
void CSelectDefense::initHeroIconByMsg()
{
	CRadioBtnGroup *pStrategyGroup = (CRadioBtnGroup*)m_ui->findWidgetById("grid_strategy");
	for(int i=0; i<3; i++)
	{
		pStrategyGroup->getItmeByIndex(i)->setChecked((i+1 == mPvPData.status()));
		if(mPvPData.status() == i+1)
		{
			updateStrategy(mPvPData.status());
		}
	}
	bool tHasCaptain = false;
	for ( int tIndex = 0;tIndex < mPvPData.team_size();tIndex++ )
	{
		if (mPvPData.team(tIndex).captin())
		{
			tHasCaptain = true;
			changeFloor(true);
			break;
		}
	}
	if (!tHasCaptain)
	{
		CCLOG("[ *ERROR ] CSelectDefense::initHeroIconByMsg Un Has Captain");
		return;
	}
	for ( int tIndex = 0;tIndex < mPvPData.team_size();tIndex++ )
	{
		//CCLOG("----------------1  %d",mPvPData.team(tIndex).hero_id());
		for (auto &tHero : m_heroList)
		{
			if (tHero.id == mPvPData.team(tIndex).hero_id())
			{
				for (auto tIcon : mHeroIcons)
				{
					if (tIcon->isVisible())
						continue;
					tIcon->setVisible(true);
					tIcon->setUserData(&tHero);
					char path[60] = {0};
					sprintf(path,"headImg/%d.png",tHero.thumb);
					if (!tIcon->initWithFile(path))
					{
						tIcon->initWithFile("headImg/10003.png");
						CCLOG("[ *ERROR ] CSelectDefense::initHeroIconByMsg %d",tHero.thumb);
					}
					CCLOG("------------%d,%d",mPvPData.team(tIndex).pos_x(),mPvPData.team(tIndex).pos_y());
					if (mPvPData.team(tIndex).captin())
					{
						updateCaptainSkill(&tHero);
						tIcon->setPosition(getPointByGrid(C_OtherCaptain));
					}else{
						int tGrid = mPvPData.team(tIndex).pos_x()*4+mPvPData.team(tIndex).pos_y();
						tIcon->setPosition(getPointByGrid(tGrid));
					}
					break;
				}
				break;
			}
		}
	}

	m_gridView->reloadData();
	updateSelectHeroNum();
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
	//mTouchBegin = pTouch;
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
		for(unsigned int i=0; i<pChildren->count(); i++)
		{
			CCNode* pNode = (CCNode*)pChildren->objectAtIndex(i);
			if(pNode==nullptr)
			{
				break;
			}
			else
			{
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
			//执行拖动武将
			//CCLOG("%f--%f", pTouch->getLocation().x, pTouch->getLocation().y);
			dragHero( pTouch );
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
			//松手放置武将
			putDownHero( pTouch );
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

	//生成被拖动武将
	createDragHero();
}

void CSelectDefense::updateStrategy( int iIndex )
{
	//更新策略
	CLabel *pStrategyText = (CLabel *)m_ui->findWidgetById("strategy_text");
	pStrategyText->setString(GETLANGSTR(2038+iIndex));
	mStrategyIndex = iIndex;
	m_iStrategyIndex = iIndex;
}

void CSelectDefense::updateCaptainSkill( CHero *pHero )
{
	CCNode *pBg = (CCNode*)m_ui->findWidgetById("desc_bg");
	CCNode *pTitle = (CCNode*)m_ui->findWidgetById("desc_title");
	CLabel *pDesc = (CLabel*)(m_ui->findWidgetById("desc"));

	if( pHero==nullptr )
	{
		pBg->runAction(CCFadeOut::create(0.2f));
		pTitle->runAction(CCFadeOut::create(0.2f));
		pDesc->runAction(CCFadeOut::create(0.2f));
		return;
	}

	if ( pHero->id!=0 && pHero->skill5.skillId>0 )
	{
		const SkillCfg *cfg = DataCenter::sharedData()->getSkill()->getCfg(pHero->skill5.skillId);
		if (cfg)
		{ 
			pDesc->setString(cfg->desc.c_str());
		}
		else
		{
			pDesc->setString("");
		}
	}
	else
	{
		pDesc->setString("");
	}

	pBg->runAction(CCFadeIn::create(0.2f));
	pTitle->runAction(CCFadeIn::create(0.2f));
	pDesc->runAction(CCFadeIn::create(0.2f));
}
/*******************************************************************************************/

int CSelectDefense::getGridByPoint( CCPoint pPoint )
{
	for(map<int,CCPoint>::iterator iter = mIconPoint.begin(); iter != mIconPoint.end(); ++iter)
	{
		CCRect rect(iter->second.x - mGridSize.x / 2,iter->second.y - mGridSize.y / 2,mGridSize.x,mGridSize.y);
		if(rect.containsPoint(pPoint))
		{
			//CCLOG("CSelectDefense::getIndexByTouch %d",iter->first); 
			return iter->first;
		}
	}
	return INVALID_GRID;
}

CCPoint CSelectDefense::getPointByGrid(int pGrid)
{
	map<int,CCPoint>::iterator iter = mIconPoint.find(pGrid);
	if (iter != mIconPoint.end())
		return iter->second;
	CCLOG("[ *ERROR ] CSelectDefense::getPointByGrid %d",pGrid);
	return CCPoint(0,0);
}

int CSelectDefense::getIndexByTouch( CCTouch* pTouch )
{
	CCPoint p = mIconNode->convertToNodeSpace(pTouch->getLocation());
	return getGridByPoint(p);
}

cocos2d::CCPoint CSelectDefense::getPointByTouch( CCTouch* pTouch )
{
	CCPoint p = mIconNode->convertToNodeSpace(pTouch->getLocation());
	for(map<int,CCPoint>::iterator iter = mIconPoint.begin(); iter != mIconPoint.end(); ++iter)
	{
		CCRect rect(iter->second.x - mGridSize.x / 2,iter->second.y - mGridSize.y / 2,mGridSize.x,mGridSize.y);
		if(rect.containsPoint(p))
		{
			CCLOG("CSelectDefense::getPointByTouch %d",iter->first);
			return iter->second;
		}
	}
	return CCPoint(0,0);
}

CImageView* CSelectDefense::hasIconByTouch( CCTouch* pTouch )
{
	CCPoint p = mIconNode->convertToNodeSpace(pTouch->getLocation());
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;
		if (tIcon->boundingBox().containsPoint(p))
			return tIcon;
	}
	return nullptr;
}

bool CSelectDefense::initGreen()
{
	mIconNode = CLayout::create();
	mIconNode->setPosition(350,340);
	m_ui->addChild(mIconNode);

	m_pBatchGreen = CCSpriteBatchNode::create("pvp/pvp_box_blue.png");
	m_pBatchGreen->setPosition(ccp(100,35));
	mIconNode->addChild(m_pBatchGreen);

	//队长格子
	m_Captain = (CImageView*)m_ui->findWidgetById("captain");
	m_Captain->removeFromParent();
	m_Captain->setPosition(ccp(38,120));
	mIconNode->addChild(m_Captain);
	mIconPoint[C_OtherCaptain] = m_Captain->getPosition();

	//拿到绿色格子
	m_ui->findWidgetById("green_start")->setVisible(false);
	//4X4
	mGridSize = ccp(62,58);
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			CCSprite* pGreenTile = CCSprite::create("pvp/pvp_box_blue.png");
			pGreenTile->setPosition(ccp(mGridSize.x*i, mGridSize.y*j));
			m_pBatchGreen->addChild(pGreenTile);
			CCPoint p = m_pBatchGreen->convertToWorldSpace(ccp(mGridSize.x*i, mGridSize.y*j));
			mIconPoint[(i+C_PVPCol)*4+abs(j-3)] = mIconNode->convertToNodeSpace(p);

			CCLabelTTF* lab = CCLabelTTF::create(ToString((i+C_PVPCol)*4+abs(j-3)),"arial",20);
			lab->setPosition(mIconNode->convertToNodeSpace(p));
#if BATTLE_TEST
			mIconNode->addChild(lab);
#endif
		}
	}
	createHeroIcon();
	changeFloor(false);
	return true;
}

void CSelectDefense::changeFloor( bool pShow )
{
	CCArray* tChildren = m_pBatchGreen->getChildren();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(tChildren,obj)
	{
		CCNode* tChild = (CCNode*)obj;
		tChild->setVisible(pShow);
	}
}

void CSelectDefense::clearIcon()
{
	for (auto tIcon : mHeroIcons)
	{
		tIcon->setUserData(nullptr);
		tIcon->setPosition(CCPointZero);
		tIcon->setVisible(false);
	}
}

void CSelectDefense::createHeroIcon()
{
	mMoveIcon = CImageView::create("headImg/10005.png");
	mMoveIcon->setScaleX(62.0f*0.85f/92.0f);
	mMoveIcon->setScaleY(58.0f*0.85f/92.0f);
#if BATTLE_TEST
	mMoveIcon->setVisible(true);
#else
	mMoveIcon->setVisible(false);
#endif
	mIconNode->addChild(mMoveIcon);
	for (int i=0;i<5;i++)
	{
		CImageView* tHeroIcon = CImageView::create();
		tHeroIcon->setScaleX(62.0f*0.85f/92.0f);
		tHeroIcon->setScaleY(58.0f*0.85f/92.0f);
		tHeroIcon->setVisible(false);
		tHeroIcon->setTouchEnabled(true);
		tHeroIcon->setOnClickListener(this,ccw_click_selector(CSelectDefense::onTouchIcon));
		mIconNode->addChild(tHeroIcon);
		mHeroIcons.push_back(tHeroIcon);
	}
}

void CSelectDefense::onTouchIcon( CCObject *pSender )
{
	CImageView* tImageView = (CImageView*) pSender;
	CHero *pHero = (CHero *)tImageView->getUserData();
	if(pHero)
	{
		setSelectHeroEffect(getHeroImageFromScrollViewById(pHero->id), false);
	}
	if (getGridByPoint(tImageView->getPosition()) == C_OtherCaptain)
	{
		updateCaptainSkill(nullptr);
		changeFloor(false);
		clearIcon();
		m_gridView->reloadData();
	}else{
		tImageView->setVisible(false);
		tImageView->setUserObject(nullptr);
	}

	updateSelectHeroNum();
}

bool CSelectDefense::hasInBattle()
{
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;
		if ( tIcon->getUserData() == m_pDragHeroData)
			return true;
	}
	return false;
}

void CSelectDefense::createDragHero( )
{
	if (hasInBattle())
	{
		//提示该武将已经布上阵中
		return;
	}
	char path[60] = {0};
	sprintf(path,"headImg/%d.png",m_pDragHeroData->thumb);
	if (!mMoveIcon->initWithFile(path))
	{
		mMoveIcon->initWithFile("headImg/10003.png");
		CCLOG("[ *ERROR ] CSelectDefense::createDragHero %d",m_pDragHeroData->thumb);
	}
	mMoveIcon->setPosition(mIconNode->convertToNodeSpace(m_pTouch->getLocation()));
	mMoveIcon->setVisible(true);
}

void CSelectDefense::dragHero(  CCTouch* pTouch )
{
	if (mMoveIcon->isVisible())
	{
		CCPoint p = mIconNode->convertToNodeSpace(pTouch->getLocation());
		mMoveIcon->setPosition(p);
	}
}

void CSelectDefense::putDownHero( CCTouch* pTouch )
{
	if (!mMoveIcon->isVisible())
		return;
	mMoveIcon->setVisible(false);								//有可能当前位置上已经有图标了，执行替换处理
	if (!captainJudge(pTouch))
		return;
	initIconByTouch(pTouch);
}

bool CSelectDefense::captainJudge(CCTouch* pTouch)
{
	if ( !getIndexByTouch(pTouch) )						//落点不再格子区域内
		return false;
	if (getIndexByTouch(pTouch) != C_OtherCaptain)		//落点不再主帅位置
	{
		CCPoint p = getPointByGrid(C_OtherCaptain);
		for (auto tIcon : mHeroIcons)
		{
			if (!tIcon->isVisible())
				continue;
			if (tIcon->boundingBox().containsPoint(p))
				return true;
		}
		return false;
	}else{
		changeFloor(true);
	}
	return true;
}

void CSelectDefense::initIconByTouch( CCTouch* pTouch )
{
	char path[60] = {0};
	sprintf(path,"headImg/%d.png",m_pDragHeroData->thumb);
	CCPoint p = getPointByTouch(pTouch);
	CImageView* tIcon = hasIconByTouch(pTouch);
	if (tIcon)
	{
		if (!tIcon->initWithFile(path))
			tIcon->initWithFile("headImg/10008.png");
		tIcon->setPosition(p);
		tIcon->setUserData(m_pDragHeroData);
		setSelectHeroEffect(getHeroImageFromScrollViewById(m_pDragHeroData->id), true);
		int tGrid = getGridByPoint(tIcon->getPosition());
		if ( tGrid == C_OtherCaptain ){
			updateCaptainSkill(m_pDragHeroData);
		}
	}else{
		for (auto t2Icon : mHeroIcons)
		{
			if (t2Icon->isVisible())
				continue;
			t2Icon->setPosition(p);
			t2Icon->setVisible(true);
			if (!t2Icon->initWithFile(path))
				t2Icon->initWithFile("headImg/10008.png");
			t2Icon->setUserData(m_pDragHeroData);
			setSelectHeroEffect(getHeroImageFromScrollViewById(m_pDragHeroData->id), true);
			int tGrid = getGridByPoint(t2Icon->getPosition());
			if ( tGrid == C_OtherCaptain ){
				updateCaptainSkill(m_pDragHeroData);
			}
			break;
		}
	}
	updateSelectHeroNum();
}

void CSelectDefense::saveTeamData()
{
	PvpTeamData *pPvpTeamData = new PvpTeamData;
	pPvpTeamData->set_status(m_iStrategyIndex);
	bool tSendState = false;
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;
		Member* pMember = new Member;
		CHero* tHero = (CHero*)tIcon->getUserData();
		int tGrid = getGridByPoint(tIcon->getPosition());
		pMember->set_captin(false);
		pMember->set_hero_id(tHero->id);
		if ( tGrid == C_OtherCaptain )
		{
			pMember->set_captin(true);
			tSendState = true;
		}else{
			pMember->set_pos_x(tGrid/C_GRID_ROW);
			pMember->set_pos_y(tGrid%C_GRID_ROW);
		}
		Member* pNewMember = pPvpTeamData->add_team();
		pNewMember->CopyFrom(*pMember);
		delete pMember;
	}
	if (tSendState)
	{
		GetTcpNet->sendData(pPvpTeamData, SendPvpTeamDataMsg);
		GetTcpNet->closeWait(0);
	}
		
	delete pPvpTeamData;
}

void CSelectDefense::updateSelectHeroNum()
{
	CLabel *pNum = (CLabel *)m_ui->findWidgetById("num");
	int iNum = 0;
	for (auto t2Icon : mHeroIcons)
	{
		if (t2Icon->isVisible())
		{
			iNum++;
		}
	}
	pNum->setString(CCString::createWithFormat("%d/%d", iNum, 5)->getCString());
}

