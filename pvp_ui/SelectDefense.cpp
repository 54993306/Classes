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
	,m_pGreenLay(nullptr), m_Captain(nullptr),mIconNode(nullptr),mMoveIcon(nullptr),mStrategyIndex(1)
	,mgetBattleInfo(false),mgetHeroList(false),m_bAllSelected(false),m_pEffectLayer(nullptr)
{
	for(int i=0; i<3; i++)
	{
		m_pStrategyEffect[i] = nullptr;
	}
}

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


void CSelectDefense::initEffect()
{
	m_pEffectLayer = MaskLayer::create("CSelectDefenseEffectMask");
	m_pEffectLayer->setContentSize(CCSizeMake(DESIGN_WIDTH, VIRTUAL_FIXED_HEIGHT));
	m_pEffectLayer->setIsShowBlack(false);
	m_pEffectLayer->setTouchEnabled(false);
	m_pEffectLayer->setVisible(false);
	m_pEffectLayer->setTouchPriority(LayerManager::instance()->getLayerManagerMinPriority()-2);
	m_ui->addChild(m_pEffectLayer, 99);

	for(int i=0; i<3; i++)
	{
		m_pStrategyEffect[i] = SkeletonAnimation::createWithFile(
			CCString::createWithFormat("pvp/strategy/strategy%d.json", i+1)->getCString(), 
			CCString::createWithFormat("pvp/strategy/strategy%d.atlas", i+1)->getCString(),
			1
			);
		MaskLayer *pMask = m_pEffectLayer;
		m_pStrategyEffect[i]->completeListener = std::bind(
			[pMask](int iData1, int iDat2)
		{
			pMask->setTouchEnabled(false);
			pMask->setVisible(false);
		}, 
		std::placeholders::_1, std::placeholders::_2);
		m_pStrategyEffect[i]->setPosition(ccp(469, 333));
		m_pEffectLayer->addChild(m_pStrategyEffect[i]);
	}
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

	//初始化特效
	initEffect();

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
	if(!isCaptainSelected())
	{
		return;
	}

	updateCaptainSkill(nullptr);
	changeFloor(false);
	clearIcon();
	m_gridView->reloadData();

	updateSelectHeroNum();
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

		playStrategyEffect(iIndex);
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
	CHero *pHero = m_currHeroList->at(uIdx);
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(pHero->thumb);

	for (int i = 1; i <=7; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView *pBg = (CImageView*)child;
			pBg->setUserData(pHero);
			pBg->setTouchEnabled(true);
			pBg->setOnClickListener(this,ccw_click_selector(CSelectDefense::onClickHero));

			CImageView *pHead = CImageView::create(CCString::createWithFormat("headImg/%d.png", pHero->thumb)->getCString());
			if (!pHead)
			{
				pHead = CImageView::create("headImg/101.png");
				CCLOG("[ ERROR ] CSelectDefense::addGridCell Lost Image = %d",pHero->thumb);
			}
			pHead->setTag(1);
			pHead->setUserData(pHero);
			pBg->addChild(pHead);
			NodeFillParent(pHead);
		}
		else if (i==3)
		{
			CCSprite *mask = (CCSprite*)child;
			mask->setTexture(SetRectColor(pHero->roletype));

			//添加星级
			CLayout* pStarLayout = getStarLayout(pHero->iColor);
			mask->addChild(pStarLayout);
		}	
		else if (i==4)
		{
			CLabel *name = (CLabel *)child;
			name->setString(ToString(pHero->level));
		}
		//对勾黑底
		else if (i==5)
		{
			CCNodeRGBA *pBlack = (CCNodeRGBA *)child;
			pBlack->setOpacity(160);
		}
		//边框
		else if(i==7)
		{
			CImageView *pRight = (CImageView *)child;
			pRight->setUserData(pHero);
			pRight->setVisible(false);
			pRight->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(
				CCFadeTo::create(0.3, 150),
				CCFadeTo::create(0.3, 255)
				)));
			if(m_pDragHeroData!=nullptr)
			{
				pRight->setVisible(pHero->id==m_pDragHeroData->id);
			}
		}
	}

	//上阵状态处理
	bool bPushDown = isHeroSelected(pHero->id);
	setSelectHeroEffect( pCell,  bPushDown);
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


void CSelectDefense::setSelectHeroRect( int iId )
{
	m_pDragHeroData = nullptr;
	setFloorEffect(false);
	CCArray *pChildren = m_gridView->getContainer()->getChildren();
	CCObject *pObj = nullptr;
	CCARRAY_FOREACH(pChildren, pObj)
	{
		CGridViewCell *pCell = (CGridViewCell *)pObj;
		CImageView *pView = dynamic_cast<CImageView *>(pCell->getChildByTag(7));
		if(pView)
		{
			pView->setVisible(false);
			CHero *pHero = (CHero *)pView->getUserData();
			if(pHero)
			{
				if( pHero->id == iId )
				{
					pView->setVisible(true);
					m_pDragHeroData = pHero;
					setFloorEffect(true);
				}
			}
		}
	}
}


void CSelectDefense::setSelectHeroEffect( CGridViewCell *pCell, bool bSelected )
{
	if(pCell == nullptr)
	{
		return;
	}

	//头像
	CImageView *pHead = ( CImageView *)pCell->getChildByTag(1)->getChildByTag(1);
	if ( bSelected )
	{
		pHead->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	}
	else
	{
		pHead->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
	}

	//对勾
	CImageView *pBlack = (CImageView *)pCell->getChildByTag(5);
	pBlack->setVisible(bSelected);
	CImageView *pRight = (CImageView *)pCell->getChildByTag(6);
	pRight->setVisible(bSelected);

	//框
	if ( bSelected )
	{
		CImageView *pRect = (CImageView *)pCell->getChildByTag(7);
		pRect->setVisible(false);
		setFloorEffect(false);
	}
}


CGridViewCell * CSelectDefense::getHeroCellFromScrollViewById( int iId )
{
	CCArray *pChildren = m_gridView->getContainer()->getChildren();
	CCObject *pObj = nullptr;
	CCARRAY_FOREACH(pChildren, pObj)
	{
		CGridViewCell *pCell = (CGridViewCell *)pObj;
		CImageView *pHeadView = dynamic_cast<CImageView *>(pCell->getChildByTag(1)->getChildByTag(1));
		if(pHeadView)
		{
			CHero *pHero = (CHero *)pHeadView->getUserData();
			if(pHero)
			{
				if( pHero->id == iId )
				{
					return pCell;
				}
			}
		}
	}
	return nullptr;
}


void CSelectDefense::onClickHero(CCObject *pSender)
{
	//TODO
	CCNode *pNode = (CCNode *)pSender;
	CHero *pHero = (CHero *)pNode->getUserData();
	
	//上阵状态取消掉
	if(isHeroSelected(pHero->id))
	{
		//根据id找icon
		CImageView *pIcon = getHeroIconById(pHero->id);
		if(pIcon)
		{
			//待移动对象
			CImageView *pWaitMoveIcon = getWaitMoveIcon();
			if (pWaitMoveIcon == pIcon)
			{
				//移除当前选择头像
				setWaitMoveState(pWaitMoveIcon, false);
			}
			removeIcon(pIcon);
		}

		updateSelectHeroNum();
	}
	//没上阵选中
	else
	{
		//相同英雄不重复选
		if(m_pDragHeroData == pHero) return;
		//有待移动对象，不可选
		if(getWaitMoveIcon()) return;

		//更新选中框状态
		setSelectHeroRect(pHero->id);
	}
}


void CSelectDefense::onClickTile( CCObject *pSender )
{
	//TODO
	CCNode *pNode = (CCNode *)pSender;


	//没有选中英雄
	if(m_pDragHeroData==nullptr) return;
	//格子上有人，不执行
	if(checkIsHeroStandOnTile(pNode)) return;
	

	CImageView *pWaitMoveIcon = getWaitMoveIcon();
	if(!pWaitMoveIcon)
	{
		//没有待移动格子，正常执行添加操作
		initIconByPos(pNode->getPosition());
	}
	else
	{
		bool isCaptain = ( getGridByPoint(pWaitMoveIcon->getPosition()) == C_OtherCaptain);
		if(!isCaptain)
		{
			//有待移动格子，执行位移
			pWaitMoveIcon->setPosition(pNode->getPosition());
			setWaitMoveState(pWaitMoveIcon, false);
		}
	}
}

void CSelectDefense::onClickCaptainTile( CCObject *pSender )
{
	//TODO
	//没有选中英雄
	if(m_pDragHeroData==nullptr) return;

	CCNode *pNode = (CCNode *)pSender;

	initIconByPos(pNode->getPosition());
}


void CSelectDefense::onClickIcon( CCObject *pSender )
{
	CImageView *pHead = (CImageView *)pSender;

	//当前有选择框出现,或者待移动icon
	if(m_pDragHeroData != nullptr )
	{
		//待移动对象点击了自己，移除掉
		CImageView *pWaitMoveIcon = getWaitMoveIcon();
		if (pWaitMoveIcon == pHead)
		{
			//移除当前选择头像
			removeIcon(pHead);
			setWaitMoveState(pHead, false);
		}
		else
		{
			//替换头像
			changeIconByPos(pHead, pHead->getPosition());
		}
		
	}
	//没框, 没待移动对象
	else
	{
		//当前头像为可移动
		setWaitMoveState(pHead, true);
	}
	
	updateSelectHeroNum();
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
	CImageView *pBg = (CImageView*)m_ui->findWidgetById("desc_bg");
	CCNode *pTitle = (CCNode*)m_ui->findWidgetById("desc_title");
	CLabel *pDesc = (CLabel*)(m_ui->findWidgetById("desc"));

	if( pHero==nullptr && pBg->getOpacity()!=0)
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

bool CSelectDefense::initGreen()
{
	mIconNode = CLayout::create();
	mIconNode->setPosition(ccp(1138/2+204, 640/2+192));
	mIconNode->setContentSize(CCSizeMake(1138, 640));
	m_ui->addChild(mIconNode);

	m_pGreenLay = CLayout::create();
	m_pGreenLay->setContentSize(CCSizeMake(1138, 640));
	m_pGreenLay->setAnchorPoint(CCPointZero);
	m_pGreenLay->setPosition(CCPointZero);
	m_pGreenLay->setCascadeOpacityEnabled(true);
	mIconNode->addChild(m_pGreenLay);

	//队长格子
	m_Captain = (CImageView*)m_ui->findWidgetById("captain");
	m_Captain->removeFromParent();
	m_Captain->setPosition(ccp(55, 143));
	mIconNode->addChild(m_Captain);
	m_Captain->setTouchEnabled(true);
	m_Captain->setOnClickListener(this, ccw_click_selector(CSelectDefense::onClickCaptainTile));
	mIconPoint[C_OtherCaptain] = m_Captain->getPosition();

	//拿到绿色格子
	m_ui->findWidgetById("green_start")->setVisible(false);
	//4X4
	mGridSize = ccp(76, 69);
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			CImageView* pGreenTile = CImageView::create("pvp/pvp_box_blue.png");
			pGreenTile->setPosition(ccp(mGridSize.x*i, mGridSize.y*j) + ccp(140, 40));
			pGreenTile->setTouchEnabled(true);
			pGreenTile->setOnClickListener(this, ccw_click_selector(CSelectDefense::onClickTile));
			m_pGreenLay->addChild(pGreenTile);
			mIconPoint[(i+C_PVPCol)*4+abs(j-3)] = pGreenTile->getPosition();

			CCLabelTTF* lab = CCLabelTTF::create(ToString((i+C_PVPCol)*4+abs(j-3)),"arial",20);
			lab->setPosition(pGreenTile->getPosition());
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
	CCArray* tChildren = m_pGreenLay->getChildren();
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
		setWaitMoveState(tIcon, false);
	}
}

void CSelectDefense::createHeroIcon()
{
	mMoveIcon = CImageView::create("headImg/10005.png");
	mMoveIcon->setScaleX(76.0f*0.85f/92.0f);
	mMoveIcon->setScaleY(69.0f*0.85f/92.0f);
#if BATTLE_TEST
	mMoveIcon->setVisible(true);
#else
	mMoveIcon->setVisible(false);
#endif
	mIconNode->addChild(mMoveIcon);
	for (int i=0;i<5;i++)
	{
		CImageView* tHeroIcon = CImageView::create();
		tHeroIcon->setScaleX(76.0f*0.85f/92.0f);
		tHeroIcon->setScaleY(69.0f*0.85f/92.0f);
		tHeroIcon->setVisible(false);
		tHeroIcon->setTouchEnabled(true);
		tHeroIcon->setOnClickListener(this,ccw_click_selector(CSelectDefense::onClickIcon));
		mIconNode->addChild(tHeroIcon);
		mHeroIcons.push_back(tHeroIcon);
	}
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
	//if (!mMoveIcon->isVisible())
	//	return;
	//mMoveIcon->setVisible(false);								//有可能当前位置上已经有图标了，执行替换处理
	//if (!captainJudge(pTouch))
	//	return;
	//initIconByPos(pTouch);
}

bool CSelectDefense::isCaptainSelected()
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
}

void CSelectDefense::initIconByPos( CCPoint pos )
{
	//人数已满不可选
	if(m_bAllSelected) return;

	char path[60] = {0};
	sprintf(path,"headImg/%d.png",m_pDragHeroData->thumb);
	for (auto t2Icon : mHeroIcons)
	{
		if (t2Icon->isVisible())
			continue;

		t2Icon->setPosition(pos);
		t2Icon->setVisible(true);
		if (!t2Icon->initWithFile(path))
			t2Icon->initWithFile("headImg/10008.png");
		t2Icon->setUserData(m_pDragHeroData);
		int tGrid = getGridByPoint(t2Icon->getPosition());
		if ( tGrid == C_OtherCaptain ){
			updateCaptainSkill(m_pDragHeroData);
			changeFloor(true);
		}
		setSelectHeroEffect(getHeroCellFromScrollViewById(m_pDragHeroData->id), true);
		m_pDragHeroData = nullptr;
		break;
	}
	updateSelectHeroNum();
}

void CSelectDefense::changeIconByPos( CImageView *pIcon, CCPoint pos )
{
	CHero *pHeroOld = (CHero *)pIcon->getUserData();

	CImageView *pWaitMoveIcon = getWaitMoveIcon();
	//没有待移动格子，正常替换，移除旧的，换上新的
	if(pWaitMoveIcon == nullptr)
	{
		//清除旧的
		setSelectHeroEffect(getHeroCellFromScrollViewById(pHeroOld->id), false);
		//换上新的
		setSelectHeroEffect(getHeroCellFromScrollViewById(m_pDragHeroData->id), true);

		char path[60] = {0};
		sprintf(path,"headImg/%d.png",m_pDragHeroData->thumb);
		if (!pIcon->initWithFile(path))
			pIcon->initWithFile("headImg/10008.png");
		pIcon->setPosition(pos);
		pIcon->setUserData(m_pDragHeroData);
		int tGrid = getGridByPoint(pIcon->getPosition());
		if ( tGrid == C_OtherCaptain ){
			updateCaptainSkill(m_pDragHeroData);
		}

		m_pDragHeroData = nullptr;
	}
	//有待移动格子，交换
	else
	{
		CCPoint pPosFlag = pIcon->getPosition();
		pIcon->setPosition(pWaitMoveIcon->getPosition());
		pWaitMoveIcon->setPosition(pPosFlag);

		if ( getGridByPoint(pWaitMoveIcon->getPosition()) == C_OtherCaptain )
		{
			updateCaptainSkill((CHero *)pWaitMoveIcon->getUserData());
			changeFloor(true);
		}
		else if( getGridByPoint(pIcon->getPosition()) == C_OtherCaptain )
		{
			updateCaptainSkill((CHero *)pIcon->getUserData());
			changeFloor(true);
		}
		
		setWaitMoveState(pWaitMoveIcon, false);

	}

	updateSelectHeroNum();
}


void CSelectDefense::removeIcon( CImageView *pIcon )
{
	CHero *pHero = (CHero *)pIcon->getUserData();

	if(pHero)
	{
		setSelectHeroEffect(getHeroCellFromScrollViewById(pHero->id), false);
	}

	if (getGridByPoint(pIcon->getPosition()) == C_OtherCaptain)
	{
		updateCaptainSkill(nullptr);
		changeFloor(false);
		clearIcon();
		m_gridView->reloadData();
	}
	else
	{
		pIcon->setVisible(false);
		pIcon->setUserObject(nullptr);
	}
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
		if(checkModify(*pPvpTeamData))
		{
			GetTcpNet->sendData(pPvpTeamData, SendPvpTeamDataMsg);
			GetTcpNet->closeWait(0);
		}
	}
		
	delete pPvpTeamData;
}

bool CSelectDefense::checkModify( const PvpTeamData& newData )
{
	int iSize = mPvPData.team_size();

	if(iSize != newData.team_size())
	{
		return true;
	}

	for(int i=0; i<iSize; i++)
	{
		const Member& oldMember = mPvPData.team().Get(i);
		const Member& newMember = newData.team().Get(i);
		if( oldMember.hero_id() != newMember.hero_id() 
			|| oldMember.captin() != newMember.captin() 
			|| oldMember.pos_x() != newMember.pos_x()
			|| oldMember.pos_y() != newMember.pos_y()
			)
		{
			return true;
		}
	}

	return false;
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

	m_bAllSelected = (iNum==5);
}

void CSelectDefense::setFloorEffect( bool bShow )
{
	if( bShow )
	{
		CCRepeatForever *pRep = CCRepeatForever::create(CCSequence::createWithTwoActions(
			CCFadeTo::create(0.2f, 150),
			CCFadeTo::create(0.2f, 255)
			));
		pRep->setTag(1);


		if(isCaptainSelected())
		{
			CCArray* tChildren = m_pGreenLay->getChildren();
			CCObject* obj = nullptr;
			CCARRAY_FOREACH(tChildren,obj)
			{
				CCNode* tChild = (CCNode*)obj;
				tChild->setVisible(true);
				if(m_Captain->getActionByTag(1)==nullptr)
				{
					//人满了, 非移动，则只能替换现有的
					CImageView *pWaitMoveIcon = getWaitMoveIcon();
					bool isCaptain = false;
					if(pWaitMoveIcon)
					{
						isCaptain = ( getGridByPoint(pWaitMoveIcon->getPosition()) == C_OtherCaptain);
					}

					if( (m_bAllSelected && !pWaitMoveIcon)  || (pWaitMoveIcon && isCaptain) )
					{
						if(checkIsHeroStandOnTile(tChild))
						{
							tChild->runAction((CCAction *)(pRep->copy()));
						}
					}
					else
					{
						tChild->runAction((CCAction *)(pRep->copy()));
					}
				}
			}
		}
		
		if(m_Captain->getActionByTag(1)==nullptr)
		{
			m_Captain->runAction(pRep);
		}

	}
	else
	{
		{
			CCArray* tChildren = m_pGreenLay->getChildren();
			CCObject* obj = nullptr;
			CCARRAY_FOREACH(tChildren,obj)
			{
				CCNode* tChild = (CCNode*)obj;
				tChild->stopAllActions();
			}
		}

		m_Captain->stopAllActions();
	}
}

bool CSelectDefense::checkIsHeroStandOnTile( CCNode *pTile )
{
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;
		if (ccpDistance( tIcon->getPosition(), pTile->getPosition()) == 0)
		{
			return true;
		}
	}
	return false;
}

void CSelectDefense::setWaitMoveState( CImageView *pIcon, bool bFlag )
{
	if(bFlag)
	{
		//进入
		if(pIcon->getActionByTag(SelectDefenseHeadWaitMoveAction) == nullptr)
		{
			CCRepeatForever *pRep = CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.2f, 180), CCFadeTo::create(0.2f, 255)));
			pRep->setTag(SelectDefenseHeadWaitMoveAction);
			pIcon->runAction(pRep);
			setFloorEffect(true);
			m_pDragHeroData = (CHero *)pIcon->getUserData();
		}

	}else
	{
		//出去
		pIcon->stopAllActions();
		pIcon->setOpacity(255);
		setFloorEffect(false);
		m_pDragHeroData = nullptr;
	}
}


CImageView * CSelectDefense::getWaitMoveIcon()
{
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;
		if (tIcon->getActionByTag(SelectDefenseHeadWaitMoveAction)!=nullptr)
		{
			return tIcon;
		}
	}
	return nullptr;
}

CImageView * CSelectDefense::getHeroIconById( int iId )
{
	for (auto tIcon : mHeroIcons)
	{
		if (!tIcon->isVisible())
			continue;

		CHero *pHero = (CHero *)tIcon->getUserData();
		if(pHero)
		{
			if(pHero->id == iId)
			{
				return tIcon;
			}
		}
	}
	return nullptr;
}

void CSelectDefense::playStrategyEffect( int iIndex )
{
	m_pEffectLayer->setTouchEnabled(true);
	m_pEffectLayer->setVisible(true);
	m_pStrategyEffect[iIndex-1]->setAnimation(0, "stand", false);
}

