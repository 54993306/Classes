#include "HeroEvolve.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "model/DataCenter.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "HeroControl.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "tollgate/TollgatePreview.h"
#include "tollgate/SelectArmy.h"
#include "common/CommonFunction.h"
#include "mainCity/CCompaseLayer.h"
#include "Global.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/UITools.h"
#include "hero/HeroCall.h"
#include "mainCity/CNewHero.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "hero/HeroEvolveDataPanel.h"
#include "hero/HeroEvolveEffectLayer.h"
#include "tools/CCShake.h"

CHeroEvolve::CHeroEvolve()
	:m_ui(nullptr), m_pNewHero(nullptr), m_pHeroArmature(nullptr), m_pCircleBar(nullptr),m_bAskNextQualityHero(false)
	,m_fBallRadius(0.0f),m_fCircleBarRadius(0.0f),m_fAngleGap(0.0f), m_pCurrentRankHeroData(nullptr), m_iCurrentTouchBallIndex(-1)
	,m_pEffectLayer(nullptr)
{
	for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	{
		m_pBall[i] = nullptr;
	}
	resetHeroData();
}

bool CHeroEvolve::init()
{
	if (BaseLayer::init())
	{
		this->setIsShowBlack(false);
		this->setVisible(true);
		
		m_ui = LoadComponent("HeroEvolve.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//进阶按钮
		CButton *pEvolve =(CButton*)(m_ui->findWidgetById("evolve"));
		pEvolve->setOnClickListener(this,ccw_click_selector(CHeroEvolve::onEvolveBtn));

		//添加魂石按钮
		CButton *pAddStone =(CButton*)(m_ui->findWidgetById("add"));
		pAddStone->setOnClickListener(this,ccw_click_selector(CHeroEvolve::heroCall));


		//数据面板
		m_pDataPanel = CHeroEvolveDataPanel::create();
		//m_pDataPanel->setPosition(VCENTER);
		this->addChild(m_pDataPanel, 2);

		//初始化进度条相关
		initProgressAndBall();

		return true;
	}
	return false;
}


void CHeroEvolve::initProgressAndBall()
{
	//拿到太极层
	CLayout *pTaiJiLayer = (CLayout*)m_ui->findWidgetById("taiji");

	//根据标记精灵初始化圆形进度条
	CImageView *pPosRead = (CImageView*)pTaiJiLayer->findWidgetById("taiji_center");
	//中心位置
	CCPoint pCenterPos = pPosRead->getPosition();

	CCSprite* pSpriteBar = CCSprite::create("heroEvolve/tri_ring.png");
	m_pCircleBar = CCProgressTimer::create(pSpriteBar);
	m_pCircleBar->setAnchorPoint(CCPointCenter);
	m_pCircleBar->setPosition(pCenterPos);
	m_pCircleBar->setType(kCCProgressTimerTypeRadial);
	pPosRead->getParent()->addChild(m_pCircleBar, pPosRead->getZOrder()+1);

	//拿到半径长度
	m_fCircleBarRadius = pSpriteBar->getContentSize().width*pSpriteBar->getScale()/2;

	//圆形平均分割为五个圆心角,//计算五个球的位置，并初始化
	for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	{
		float fAngle = CC_DEGREES_TO_RADIANS(HERO_EVOLVE_ANGLE_GAP*i);

		//计算 x,y 偏移坐标（x = sinX, y = cosX）
		float fOffX = m_fCircleBarRadius*sin(fAngle);
		float fOffY = m_fCircleBarRadius*cos(fAngle); 
		CCPoint pRealPos = pCenterPos+ccp(fOffX, fOffY);

		//初始化球
		CImageView* pBall = CImageView::create("heroEvolve/starup_2_lock.png");
		pBall->setAnchorPoint(CCPointCenter);
		pBall->setPosition(pRealPos);
		pBall->setScale(HERO_EVOLVE_BALL_SCALE);
		pBall->setTag(i);
		pBall->setTouchEnabled(true);
		//pBall->setOpacity(40);
		pBall->setOnPressListener(this, ccw_press_selector(CHeroEvolve::onPressBallData));
		m_pCircleBar->getParent()->addChild(pBall, m_pCircleBar->getZOrder()+1);
		m_pBall[i] = pBall;

		if(m_fBallRadius==0.0f)
		{
			m_fBallRadius = pBall->getContentSize().width*HERO_EVOLVE_BALL_SCALE/2-10;
		}
	}


	//计算进度条的基础参数
	m_fAngleGap = CC_RADIANS_TO_DEGREES(asin(m_fBallRadius/m_fCircleBarRadius));

	//文字转动
	CImageView *pImageWords = (CImageView*)pTaiJiLayer->findWidgetById("taiji_words");
	pImageWords->runAction(CCRepeatForever::create(CCRotateBy::create(8.0f, 360)));
	pImageWords->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.55f, 130), CCFadeTo::create(0.55f, 255))));

}


void CHeroEvolve::updateProgressAndBalll()
{
	//当前进阶等级
	int iRank = m_pCurrentRankHeroData->quality;

	//当前魂石收集比例
	float fProgressStone = 0;
	if(m_pCurrentRankHeroData->itemNum2>0)
	{
		fProgressStone = (float)m_pCurrentRankHeroData->itemNum1/(float)m_pCurrentRankHeroData->itemNum2;
	}	

	//小于5%的angle约等于5%,前提是最少有一个魂石
	if(m_pCurrentRankHeroData->itemNum1>=1)
	{
		fProgressStone = fProgressStone<0.05f?0.05f:fProgressStone;
	}
	fProgressStone = fProgressStone>1.0f?1.0f:fProgressStone;

	//判断进度条位置
	float fAngleStart = (iRank-1)*HERO_EVOLVE_ANGLE_GAP+m_fAngleGap;
	float fAngleRange = HERO_EVOLVE_ANGLE_GAP-m_fAngleGap*2;

	float fAngleReal = fAngleStart + fAngleRange*fProgressStone;

	m_pCircleBar->setPercentage(100.0f*fAngleReal/360.f);

	//关于球-已升/到了未升/未升
	for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	{
		m_pBall[i]->removeAllChildrenWithCleanup(true);
		m_pBall[i]->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("heroEvolve/starup_%d_lock.png", i+1)->getCString()));

		//已升
		if(iRank>i)
		{
			CCTexture2D* pTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("heroEvolve/starup_%d_on.png", i+1)->getCString());
			m_pBall[i]->setTexture(pTexture);
		}
		//未升
		else
		{
			//收集中
			if(iRank == i)
			{
				//收集够,闪烁
				if(fProgressStone>=1.0f)
				{
					m_pBall[i]->removeAllChildrenWithCleanup(true);
					CCSprite* pBlinkiMage = CCSprite::create(CCString::createWithFormat("heroEvolve/starup_%d_off.png", i+1)->getCString());
					pBlinkiMage->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCFadeTo::create(0.3f, 50), CCFadeTo::create(0.3f, 255))));
					m_pBall[i]->addChild(pBlinkiMage);
					NodeFillParent(pBlinkiMage);
				}
				//未收集够
				else
				{
					//不处理
				}
			}
			//未到
			else
			{
				//不处理
			}
		}
	}
}


void CHeroEvolve::onEvolveBtn(CCObject* pSender)
{
	if(checkHeroEvolveOpen(m_pCurrentRankHeroData))
	{
		((CHeroAttribute*)(this->getParent()))->getDelegate()->onEvolveHero();
		CCLOG("ERROR  CHeroEvolve::onEvolveBtn");
	}
	//evolveSuccess( &m_pHero[3]);
}

void CHeroEvolve::onEnter()
{
	BaseLayer::onEnter();

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO, this, GameMsghandler_selector(CHeroEvolve::updateHeroInfo));	

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroEvolve::compaseSuccess), COMPOSE_SUCCESS, nullptr);

}


void CHeroEvolve::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
	NOTIFICATION->removeAllObservers(this);

	//CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("heroEvolve/jinjie.ExportJson");
	//for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	//{
	//	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(
	//		CCString::createWithFormat("heroEvolve/r%d.plist", i+1)->getCString()
	//		);
	//}
	//CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CHeroEvolve::updateHeroInfo(const TMessage& tMsg)
{
	CHero *pHero = (CHero*)tMsg.lParam;
	
	//第一次初始化, 
	//除了魂石数量增加（有其他回调）和第一次传数据以外
	//没有其他非当前界面操作可以影响英雄数据，没必要接受更新
	if(tMsg.nMsg == 1)
	{
		updateEvolve(pHero);
	}
	else
	{
		//nMsg == 0
	}
}


void CHeroEvolve::updateEvolve( CHero *pHero )
{
	//TODO
	//重置英雄数据
	resetHeroData();
	saveHeroData(pHero);

	//更新头像
	updateHead(pHero);
	
	//更新石头数量
	updateStoneNum(pHero);

	//更新进阶按钮状态
	updateEvolveButtonStatus();

	//更新进度条和球
	updateProgressAndBalll();

}



void CHeroEvolve::updateHead(CHero *pHero)
{
	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", pHero->thumb)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroEvolve::showHeroHead Lost Image = %d",pHero->thumb);
	}
	itembg1->removeChildByTag(1);
	head->setScale(90.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);
}

void CHeroEvolve::updateStoneNum(CHero * pHero)
{
	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	if (pHero->itemNum2>0)
	{	
		val->setString(CCString::createWithFormat("%d/%d",pHero->itemNum1,pHero->itemNum2)->getCString());
	}
	else
	{
		val->setString(CCString::createWithFormat("%d/?",pHero->itemNum1)->getCString());
	}
}

bool CHeroEvolve::checkHeroEvolveOpen(CHero * pHero)
{
	//检查英雄进阶条件
	///*等级是否足够*/，碎片是否足够
	bool isLevelOk = /*pHero->level>=pHero->maxLevel*/true;
	bool isShardsOk = pHero->itemNum1>=pHero->itemNum2;
	if(!isLevelOk && !isShardsOk)
	{
		ShowPopTextTip(GETLANGSTR(1019));
		return false;
	}
	else if(!isLevelOk)
	{
		ShowPopTextTip(GETLANGSTR(1018));
		return false;
	}
	else if(!isShardsOk)
	{
		ShowPopTextTip(GETLANGSTR(1020));
		return false;
	}
	else
	{
		return true;
	}
}

void CHeroEvolve::evolveSuccess( CHero *pHero)
{
	//TODO
	//考虑数据怎么接入
	
	CHero *pOldHero = m_pCurrentRankHeroData;
	CHero *pNewHero = pHero;

	//存储数据
	saveHeroData(pHero, true);

	//更新魂石数量
	updateStoneNum(pHero);

	//更新进度条和球-回调更新
	//updateProgressAndBalll();

	//更新按钮状态
	updateEvolveButtonStatus();

	//播放特效
	showEvoleEffect(pOldHero, pNewHero);
}



void CHeroEvolve::compaseSuccess( CCObject* pObj )
{
	//更新数量
	m_pCurrentRankHeroData->itemNum1++;
	updateStoneNum(m_pCurrentRankHeroData);

	//更新显示状态
	updateProgressAndBalll();
}

void CHeroEvolve::heroCall( CCObject* pSender )
{
	CHeroCall *evolveLayer = CHeroCall::create();
	evolveLayer->setNeedNum(m_pCurrentRankHeroData->itemNum2);
	LayerManager::instance()->push(evolveLayer);
	CPlayerControl::getInstance().sendItemInfo(m_pCurrentRankHeroData->itemId);
}


void CHeroEvolve::onPressBallData( CCObject* pSender, CTouchPressState iState )
{
	CCNode* pNode = (CCNode*)pSender;
	int iTag = pNode->getTag();
	
	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			if(m_iCurrentTouchBallIndex != -1)
			{
				return;
			}

			m_iCurrentTouchBallIndex = iTag;

			//如果已经有数据
			if(m_pHero[m_iCurrentTouchBallIndex].id != -1)
			{
				m_pDataPanel->show(m_pCurrentRankHeroData, &m_pHero[m_iCurrentTouchBallIndex]);
			}
			//没有数据
			else
			{
				//请求数据, 这里还不能显示面板(+1因为qulity从1开始)
				askForRankData(m_iCurrentTouchBallIndex+1);
			}
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			m_iCurrentTouchBallIndex = -1;
			m_pDataPanel->hide();
		}
		break;
	default:
		break;
	}
}

void CHeroEvolve::showEvoleEffect(CHero *pOldHero, CHero *pNewHero)
{
	//特效层加载
	if(m_pEffectLayer == nullptr)
	{
		m_pEffectLayer = CHeroEvolveEffectLayer::create();
		this->addChild(m_pEffectLayer);
	}
	//绑定回调-是否需要？
	//TODO

	//执行特效
	int iAimRank = pNewHero->quality;
	m_pEffectLayer->bindBallChangeCallBack(this, evolveEffectSelector(CHeroEvolve::ballChangeBegin));
	m_pEffectLayer->showEffect(
		pOldHero,
		pNewHero,
		m_pBall[iAimRank-1]->getPosition(),
		m_pDataPanel->createSkillPanel(pOldHero, pNewHero, m_pEffectLayer->getSkillPanel()),
		m_pDataPanel->getUnLockTypeWithRank(iAimRank)
		);
}

void CHeroEvolve::showEvoleEffectCallBack()
{
	//TODO

}

void CHeroEvolve::askForRankData( int iRank )
{
	setAskNextQualityHero(true);
	CPlayerControl::getInstance().sendGetHeroInfo(m_pCurrentRankHeroData->id, 0, 0, iRank);
}

void CHeroEvolve::askForRankDataCallBack( CHero* pHero )
{
	setAskNextQualityHero(false);

	//更新英雄阶段信息
	saveHeroData(pHero);

	//是当前的触摸球,就更新显示， 不判断可能会有问题，quality如果能跟请求一样就好了
	if(m_iCurrentTouchBallIndex+1 == pHero->quality)
	{
		m_pDataPanel->show(m_pCurrentRankHeroData, pHero, true);
	}
}

void CHeroEvolve::saveHeroData( CHero * pHero, bool bResetCurrentHero/*=false*/ )
{
	int iRank = pHero->quality;
	if(iRank>HERO_EVOLVE_RANK_MAX)
	{
		CCLOG("CHeroEvolve::saveHeroData---RANK-DATA-ERROR");
		return;
	}
	m_pHero[iRank-1] = *pHero;

	setCurrentHeroData(iRank, bResetCurrentHero);
}

void CHeroEvolve::setCurrentHeroData( int iRank, bool bForce/*=false*/ )
{
	//更新当前的英雄数据指向
	if(m_pCurrentRankHeroData==nullptr || m_pCurrentRankHeroData->id==-1  || bForce)
	{
		m_pCurrentRankHeroData = &m_pHero[iRank-1];

		uiChangeWhenHeroRankChange();
	}
}

void CHeroEvolve::uiChangeWhenHeroRankChange()
{
	//是否到达最高等级
	if(m_pCurrentRankHeroData->quality == HERO_EVOLVE_RANK_MAX)
	{
		//太极转动
		CLayout *pData = (CLayout *)m_ui->findWidgetById("taiji");
		CImageView *pImage = (CImageView*)pData->findWidgetById("taiji_center");
		pImage->runAction(CCRepeatForever::create(CCRotateBy::create(10.0f, -360)));
	}
	else
	{
		//太极不动
		CLayout *pData = (CLayout *)m_ui->findWidgetById("taiji");
		CImageView *pImage = (CImageView*)pData->findWidgetById("taiji_center");
		pImage->stopAllActions();
	}
}

void CHeroEvolve::resetHeroData()
{
	for(int i=0; i<HERO_EVOLVE_RANK_MAX; i++)
	{
		m_pHero[i].id = -1;
	}
}

void CHeroEvolve::updateEvolveButtonStatus()
{
	//按钮那边的变化，满级以后不再显示按钮
	CButton* evolve = (CButton*)(m_ui->findWidgetById("evolve"));
	CLabel* pEvolveLabel = (CLabel*)(m_ui->findWidgetById("evolve_label"));
	CLabel* pEvolveTips = (CLabel*)(m_ui->findWidgetById("wrong_tip"));

	if(m_pCurrentRankHeroData->quality == 5)
	{
		evolve->setVisible(false);
		pEvolveLabel->setVisible(false);
		pEvolveTips->setVisible(true);
	}
	else
	{
		evolve->setVisible(true);
		pEvolveLabel->setVisible(true);
		pEvolveTips->setVisible(false);
	}
}

void CHeroEvolve::ballChangeBegin()
{
	//延时0.1s执行
	runAction(CCSequence::create(
		CCDelayTime::create(0.1f),
		CCCallFunc::create(this, callfunc_selector(CHeroEvolve::updateProgressAndBalll)),
		nullptr
		));
	//立即震动
	CCDirector::sharedDirector()->getRunningScene()->setScale(1.05f);
	CCDirector::sharedDirector()->getRunningScene()->runAction(CCSequence::create(
		CCShake::create(0.3f, 8),
		CCScaleTo::create(0.0f, 1.0f),
		CCMoveTo::create(0.0f, CCPointZero),
		nullptr
		));
}

