#include "SelectChallengeLayer.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"
#include "GMessage.h"
#include "model/DataDefine.h"
#include "netcontrol/CPlayerControl.h"
#include "pvp_ui/PvpMenuLayer.h"
#include "common/CCMixLabelAction.h"
#include "common/CommonFunction.h"
#include "pvp_ui/SelectPvpArmy.h"
#include "tools/ShowTexttip.h"
#include "Global.h"

bool SelectChallengeSortPvpPrize(const Prize* data1, const  Prize* data2)
{
	return data1->type()<data2->type();
}

bool SelectChallengeSortPvpChallenge(const Opponent* data1, const  Opponent* data2)
{
	return data1->points()>data2->points();
}


CSelectChallengeLayer::CSelectChallengeLayer():
	m_ui(nullptr),m_pCell(nullptr),m_bFirstInit(true), m_iLockIndex(0),m_pRobot(nullptr)
	,m_bGetNewData(false),m_iTimesCombat(0)
{
	for(int i=0; i<3; i++)
	{
		m_pPanel[i] = nullptr;
		m_pMoveLight[i] = nullptr;
		m_pBaseLight[i] = nullptr;
	}
}

CSelectChallengeLayer::~CSelectChallengeLayer()
{
	CC_SAFE_RELEASE(m_pCell);
}

bool CSelectChallengeLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CSelectChallengeLayerMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("selectChallenge.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//背景
		SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/pve_beijing.json", "pvp/pve_beijing.atlas", 1);
		pSkeletonAnimation->setPosition(CCPointZero);
		pSkeletonAnimation->setAnimation(0, "stand", true);
		m_ui->addChild(pSkeletonAnimation, -2);
		//背景冒烟
		CCPoint posLight[3] = {ccp(327, 167), ccp(580, 179), ccp(820, 192) };
		float fScale[3] = {1.0f, 0.9f, 0.8f};
		for(int i=0; i<3; i++)
		{
			SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile(
				CCString::createWithFormat("pvp/pvp_fazhen_%d.json", i+1)->getCString(), 
				CCString::createWithFormat("pvp/pvp_fazhen_%d.atlas", i+1)->getCString(),
				fScale[i]);
			pSkeletonAnimation->setPosition(posLight[i]);
			pSkeletonAnimation->setAnimation(0, "stand", true);
			pSkeletonAnimation->setVisible(false);
			m_ui->addChild(pSkeletonAnimation, 10);
			m_pBaseLight[i] = pSkeletonAnimation;
		}

		m_pCell = (CLayout*)m_ui->findWidgetById("Cell");
		CC_SAFE_RETAIN(m_pCell);
		m_pCell->removeFromParentAndCleanup(false);

		for(int i=0; i<3; i++)
		{
			CCNode* pNodePos = (CCNode*)m_ui->findWidgetById(CCString::createWithFormat("pos_%d", i+1)->getCString());
			m_pPanel[i] = UICloneMgr::cloneLayout(m_pCell);
			m_pPanel[i]->setPosition(pNodePos->getPosition());
			m_pPanel[i]->setAnchorPoint(ccp(0.5f, 0.0f));
			m_pPanel[i]->setScale(fChallengePanelScale[i]);
			pNodePos->getParent()->addChild(m_pPanel[i], pNodePos->getZOrder());
			m_pPanel[i]->setVisible(false);

			//光效
			CCNode *pBg = m_pPanel[i]->findWidgetById("bg");
			CCString *pEffectStrJson = CCString::createWithFormat("pvp/panel_eff_%d.json", i+1);
			CCString *pEffectStrAtlas = CCString::createWithFormat("pvp/panel_eff_%d.atlas", i+1);
			m_pMoveLight[i] = SkeletonAnimation::createWithFile(pEffectStrJson->getCString(), pEffectStrAtlas->getCString(), 1);
			m_pMoveLight[i]->setPosition( pBg->getPosition()+ccp(pBg->getContentSize().width/2, 0) );
			m_pMoveLight[i]->setAnimation(0, "stand1", true);
			pBg->getParent()->addChild(m_pMoveLight[i], 100);

			SkeletonAnimation *pSkeleton = m_pMoveLight[i];
			m_pMoveLight[i]->completeListener = std::bind([ pSkeleton ]( int iData1, int iData2 ){ 
				pSkeleton->setAnimation(0, "stand1", true);
			}, std::placeholders::_1, std::placeholders::_2);
		}


		//机器人按钮
		CImageView* pRobot = (CImageView*)m_ui->findWidgetById("robot");
		pRobot->setTouchEnabled(true);
		pRobot->setOnClickListener(this, ccw_click_selector(CSelectChallengeLayer::onRobot));
		pRobot->setOpacity(0);
		//spine机器人
		SkeletonAnimation *pSpineRobot = SkeletonAnimation::createWithFile("pvp/pvp_robot.json", "pvp/pvp_robot.atlas", 1);
		pSpineRobot->setPosition( ccp(127, -10) );
		m_ui->addChild(pSpineRobot, 100);
		pSpineRobot->setAnimation(0, "stand1", true);
		m_pRobot = pSpineRobot;
		pSpineRobot->completeListener = std::bind([ pSpineRobot ]( int iData1, int iData2 ){ 
			if(iData1 == 1)
			{
				pSpineRobot->setToSetupPose();
			}
		}, std::placeholders::_1, std::placeholders::_2);

		//刷新
		CButton* pRefresh = (CButton*)m_ui->findWidgetById("refresh");
		pRefresh->setOnClickListener(this, ccw_click_selector(CSelectChallengeLayer::onRefresh));
		pRefresh->setCascadeOpacityEnabled(true);
		pRefresh->setOpacity(0);
		//((CCNodeRGBA *)(pRefresh->getNormalImage()))->setOpacity(0);
		SkeletonAnimation *pRefreshEffect = SkeletonAnimation::createWithFile("pvp/pvp_refresh.json", "pvp/pvp_refresh.atlas", 1);
		pRefreshEffect->setPosition( pRefresh->getPosition() );
		pRefreshEffect->setAnimation(0, "stand", true);
		pRefreshEffect->timeScale = 0.7f;
		pRefresh->getParent()->addChild(pRefreshEffect, pRobot->getZOrder()+1);

		//退出
		CButton* pClose = CButton::create("common/back.png", "common/back.png");
		pClose->getSelectedImage()->setScale(1.1f);
		pClose->setPosition(VLEFT+50, VTOP-50);
		pClose->setOnClickListener(this,ccw_click_selector(CSelectChallengeLayer::onClose));
		this->addChild(pClose, 99);

		setVisible(false);

		return true;
	}

	return false;
}

void CSelectChallengeLayer::onEnter()
{
	BaseLayer::onEnter();

	GetTcpNet->registerMsgHandler(PvpDataMsg, this, CMsgHandler_selector(CSelectChallengeLayer::processNetMsg));

	//showOpenEffect();
	HttpLoadImage::getInstance()->bindUiTarget(this);

	NOTIFICATION->addObserver(this, callfuncO_selector(CSelectChallengeLayer::showChallenge), "CSelectChallengeLayer::showChallenge", nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CSelectChallengeLayer::hideChanllenge), "CSelectChallengeLayer::hideChanllenge", nullptr);
}

void CSelectChallengeLayer::onExit()
{
	BaseLayer::onExit();
	HttpLoadImage::getInstance()->bindUiTarget(nullptr);

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);

	if(isVisible())
	{
		NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}



void CSelectChallengeLayer::onOpen()
{
	LayerManager::instance()->push(this);
}


void CSelectChallengeLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSelectChallengeLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
	}
	
	switch (type)
	{
	case PvpDataMsg:
		{
			PvpDataRes *res = (PvpDataRes*)msg;

			m_pPvpDataRes = *res;

			if(m_bFirstInit)
			{
				showOpenEffect();
				m_bFirstInit = false;
			}
			else
			{
				m_bGetNewData = true;
				panelActionReshow();
			}
		}
		break;
	default:
		break;
	}
}

void CSelectChallengeLayer::initUIWithData()
{
	//更新公共数据，剩余次数，排名，积分，奖励
	//剩余次数
	CLabel* pRemainTimes = (CLabel*)m_ui->findWidgetById("remain_times");
	pRemainTimes->setString(CCString::createWithFormat(GETLANGSTR(2004), m_pPvpDataRes.now_time(), m_pPvpDataRes.max_time())->getCString());
	m_iTimesCombat = m_pPvpDataRes.now_time();

	CLayout* pLayerDown = (CLayout*)m_ui->findWidgetById("layer_down");
	//排名
	int iRank = m_pPvpDataRes.prize().rank();
	CCSprite* pRankIcon = (CCSprite*)(pLayerDown->findWidgetById("rank"));
	CCSprite* pRankText= (CCSprite*)(pLayerDown->findWidgetById("rank_text"));
	CCSprite* pNoRank= (CCSprite*)(pLayerDown->findWidgetById("no_rank"));

	if(iRank>0)
	{
		if(iRank<=3)
		{
			CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
			std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
			if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
			{
				pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
			}
		}

		//label
		CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
		pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
		pAtlas->setPosition(pRankIcon->getPosition()+ccp(-2, -2));
		pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);
		pAtlas->runAction(MixLabelAction::create(0.5f, pAtlas));

		pRankIcon->setVisible(iRank<=3?true:false);
		pRankText->setVisible(true);
		pNoRank->setVisible(false);
	}
	//未上榜
	else
	{
		//隐藏排名和图标，显示未上榜
		pRankIcon->setVisible(false);
		pRankText->setVisible(false);
		pNoRank->setVisible(true);
	}

	//积分
	CLabel* pScore = (CLabel*)pLayerDown->findWidgetById("my_score");
	pScore->setString(CCString::createWithFormat("%d", m_pPvpDataRes.prize().points())->getCString());

	//奖励
	std::sort(m_pPvpDataRes.prize().prizes().pointer_begin(), m_pPvpDataRes.prize().prizes().pointer_end(), SelectChallengeSortPvpPrize);

	for(unsigned int j=0; j<m_pPvpDataRes.prize().prizes().size(); j++)
	{
		if(j==2)break;

		const Prize& prize = m_pPvpDataRes.prize().prizes().Get(j);

		CLabel* pPrize = (CLabel*)pLayerDown->findWidgetById(CCString::createWithFormat("icon_text_%d", j+1)->getCString());
		pPrize->setString(CCString::createWithFormat("%d", prize.num())->getCString());

		CCSprite* pIcon = (CCSprite *)pLayerDown->findWidgetById(CCString::createWithFormat("icon_%d", j+1)->getCString());
		pIcon->setOpacity(0);
		CImageView *head = CImageView::create(GetImageName(prize.type(), prize.color(), prize.thumb()).c_str());
		if (!head)
		{
			head = CImageView::create("prop/0.png");
			CCLOG("[ ERROR ] CSelectChallengeLayer::initUIWithData Lost Image");
		}
		head->setPosition(ccp(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
		pIcon->removeAllChildrenWithCleanup(true);
		pIcon->addChild(head);
	}

	//更新敌人阵容数据
	updatePanelUI();
}

void CSelectChallengeLayer::onRobot( CCObject* pSender )
{
	if(isUILock()) return;

	m_pRobot->addAnimation(1, "stand2", false);
	CPvpMenuLayer *pMenuLayer = CPvpMenuLayer::create();
	LayerManager::instance()->push(pMenuLayer);

	pMenuLayer->runAction(CCSequence::createWithTwoActions(
		CCDelayTime::create(0.1f),
		CCCallFunc::create(pMenuLayer, callfunc_selector(CPvpMenuLayer::showOpenEffect))
		));

}

void CSelectChallengeLayer::onRefresh( CCObject* pSender )
{
	if(isUILock()) return;

	PlayEffectSound(SFX_513);

	//刷新数据
	CPlayerControl::getInstance().askForPvpChallenge();
}


void CSelectChallengeLayer::onCombat( CCObject* pSender )
{
	//TODO
	if(isUILock()) return;

	if( m_iTimesCombat<=0 )
	{
		PlayEffectSound(SFX_413);
		ShowPopTextTip(GETLANGSTR(2026));
		return;
	}

	PlayEffectSound(SFX_429);

	CCNode  *pNode = (CCNode *)pSender;

	Opponent *pOpponent = (Opponent *)pNode->getUserData();

	//进入选人界面
	CSelectPvpArmy *pSelArmy = CSelectPvpArmy::create();
	pSelArmy->setRoleId(pOpponent->role_id());
	pSelArmy->setIsRobot(pOpponent->is_robot());
	pSelArmy->setEnemyRoleName(pOpponent->role_name());
	pSelArmy->setEnemyRoleHead(pOpponent->role_thumb());
	pSelArmy->setEnemyRoleFacebook(pOpponent->fb_id());
	LayerManager::instance()->push(pSelArmy);

	AskForGoToSelectPvpArmy(pOpponent->role_id(), pOpponent->is_robot());
	CPlayerControl::getInstance().sendUnion(0, 0);
}


void CSelectChallengeLayer::updatePanelUI()
{
	//排序
	std::sort(m_pPvpDataRes.opponents().pointer_begin(), m_pPvpDataRes.opponents().pointer_end(), SelectChallengeSortPvpChallenge);

	for(int i=0; i<3; i++)
	{
		updateOnePanel(m_pPanel[i], m_pPvpDataRes.opponents().Get(i), i);
		m_pBaseLight[i]->setVisible(true);
	}
}

void CSelectChallengeLayer::updateOnePanel( CLayout* pPanel, const Opponent& pOpponent, int iBgId )
{
	pPanel->setVisible(true);
	
	//name，等级，头像，立绘，积分，等级
	//name
	CLabel* pNameId = (CLabel*)pPanel->findWidgetById("name");
	pNameId->setString(pOpponent.role_name().c_str());

	//等级
	CLabel* pLevel = (CLabel*)pPanel->findWidgetById("level");
	pLevel->setString(CCString::createWithFormat("%d", pOpponent.role_lv())->getCString());

	//头像
	CCSprite* pHead = (CCSprite*)pPanel->findWidgetById("headImg");
	CCSprite* pHeadBg = (CCSprite*)pPanel->findWidgetById("headbg");
	CCString* pHeadStr = CCString::createWithFormat("headIcon/%d.png", pOpponent.role_thumb());
	if (pOpponent.role_thumb()>0)
	{
		std::string strFullPathHead = CCFileUtils::sharedFileUtils()->fullPathForFilename(pHeadStr->getCString());
		if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPathHead))
		{
			pHead->removeFromParent();

			CImageView *sp = CImageView::create(pHeadStr->getCString());
			sp->setPosition(pHeadBg->getPosition()+ccp(pHeadBg->getContentSize().width*0.5f*pHeadBg->getScaleX(), pHeadBg->getContentSize().height*0.5f*pHeadBg->getScaleY()));
			sp->setScale(0.4f);
			sp->setId("headImg");
			pPanel->addChild(sp);
		} 
	}
	else
	{
		//加载facebook头像
		string fbName = pOpponent.fb_id()+".jpg";
		string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
		bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);		
		CImageView* headbg = (CImageView*)pPanel->findWidgetById("headbg");
		if(isFileExist)
		{
			pHead->removeFromParent();

			CCSprite *sp = CCSprite::create(fbName.c_str());
			CImageView* spr = maskedSprite(sp);
			spr->setScale(0.53f);
			spr->setPosition(pHeadBg->getPosition()+ccp(headbg->getContentSize().width*0.5f*headbg->getScaleX(), headbg->getContentSize().height*0.5f*headbg->getScaleY()));			
			spr->setId("headImg");
			pPanel->addChild(spr);

		}
		else
		{
			CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,pOpponent.fb_id().c_str());
			HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),pOpponent.fb_id().c_str());
			//headbg->setId(pOpponent.fb_id().c_str());
			m_pMapHeadBg[ pOpponent.fb_id() ] = headbg;
		}
	}
	

	//立绘
	CCSprite* pBody = (CCSprite*)pPanel->findWidgetById("picture");
	CCString* pBodyStr = CCString::createWithFormat("pvp/hero/%d.png", pOpponent.hero_thumb());
	std::string strFullPathBody = CCFileUtils::sharedFileUtils()->fullPathForFilename(pBodyStr->getCString());
	if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPathBody))
	{
		pBody->setTexture(CCTextureCache::sharedTextureCache()->addImage(pBodyStr->getCString()));
	}

	//积分
	CLabel* pScore = (CLabel*)pPanel->findWidgetById("score");
	pScore->setString(CCString::createWithFormat("%d", pOpponent.points())->getCString());

	//战力
	CLabelAtlas* pFightPower = (CLabelAtlas*)pPanel->findWidgetById("fight_power");
	pFightPower->setString(CCString::createWithFormat("%d", pOpponent.combat())->getCString());
	//战力+xxx基于背景框对其
	//拿到背景框
	CImageView* pRect = (CImageView*)pPanel->findWidgetById("bg");
	CCSprite* pFightText = (CCSprite*)pPanel->findWidgetById("fight_text");
	pFightText->setAnchorPoint(ccp(0.0f, 0.5f));
	pFightPower->setAnchorPoint(ccp(0.0f, 0.5f));
	int iPosXOff = (pFightText->getContentSize().width+pFightPower->getContentSize().width) /2 ;
	pFightText->setPositionX(pRect->getPositionX()+pRect->getContentSize().width/2-iPosXOff);
	pFightPower->setPosition(ccp(pFightText->getPositionX()+pFightText->getContentSize().width+5, pFightText->getPositionY()));  //+5稍微错开点

	//背景框设置触摸进战斗选人
	pRect->setTouchEnabled(true);
	pRect->setUserData((void*)&pOpponent);
	pRect->setOnClickListener(this, ccw_click_selector(CSelectChallengeLayer::onCombat));

	//背景框颜色变化
	const char* bg[3] = {"pvp/pvp_boxred.png", "pvp/pvp_boxyellow.png", "pvp/pvp_boxblue.png"};
	pRect->setTexture(CCTextureCache::sharedTextureCache()->addImage(bg[iBgId]));
}


void CSelectChallengeLayer::panelActionShow()
{
	for(int i=0; i<3; i++)
	{
		m_iLockIndex++;
		m_pPanel[i]->setScaleX(0.0f);
		m_pPanel[i]->runAction(CCSequence::create(
			CCDelayTime::create(0.1f*i),
			CCScaleTo::create(0.2f, fChallengePanelScale[i], m_pPanel[i]->getScaleY()), 
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionShowCallBack)),
			nullptr
			));
	}
}


void CSelectChallengeLayer::panelActionShowCallBack()
{
	m_iLockIndex--;
}

bool CSelectChallengeLayer::isUILock()
{
	return m_iLockIndex>0;
}

void CSelectChallengeLayer::panelActionReshow()
{
	for(int i=0; i<3; i++)
	{
		m_iLockIndex++;

		m_pMoveLight[i]->stopAllActions();
		m_pMoveLight[i]->setAnimation(0, "stand2", false);
		m_pPanel[i]->setVisible(true);
		m_pPanel[i]->setScaleX(fChallengePanelScale[i]);
		m_pPanel[i]->runAction(CCSequence::create(
			CCDelayTime::create(0.25f+0.1f*i),
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowDataChangeCallBack)),
			CCDelayTime::create(0.8f),
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowCallBack)),
			nullptr
			));

		//m_pPanel[i]->setScaleX(fChallengePanelScale[i]);
		//m_pPanel[i]->runAction(CCSequence::create(
		//	CCDelayTime::create(0.1f*i),
		//	CCScaleTo::create(0.2f, 0, m_pPanel[i]->getScaleY()), 
		//	CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowDataChangeCallBack)),
		//	CCScaleTo::create(0.2f, fChallengePanelScale[i], m_pPanel[i]->getScaleY()), 
		//	CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowCallBack)),
		//	nullptr
		//	));
	}
}

void CSelectChallengeLayer::panelActionReshowDataChangeCallBack()
{
	if(m_bGetNewData)
	{
		updatePanelUI();
		m_bGetNewData = false;
	}
}


void CSelectChallengeLayer::panelActionReshowCallBack()
{
	m_iLockIndex--;
}

void CSelectChallengeLayer::showOpenEffect()
{
	SkeletonAnimation *pSkeletonAnimation = SkeletonAnimation::createWithFile("pvp/pve.json", "pvp/pve.atlas", 1);
	pSkeletonAnimation->setPosition( CCPointZero );
	pSkeletonAnimation->setAnimation(0, "stand", false);
	m_ui->addChild(pSkeletonAnimation, 20);

	pSkeletonAnimation->completeListener = std::bind(
		[ pSkeletonAnimation ]( int iData1, int iData2 )->void{ 
			pSkeletonAnimation->removeFromParentAndCleanup(true); 
		}, 
		std::placeholders::_1, std::placeholders::_2
		);

		//layer_up移走
		CLayout *pLayerUp = (CLayout *)findWidgetById("layer_up");
		pLayerUp->setPositionY(pLayerUp->getPositionY()+250);

		//layer_down移走
		CLayout *pLayerDown = (CLayout *)findWidgetById("layer_down");
		pLayerDown->setPositionY(pLayerDown->getPositionY()-250);

		m_ui->runAction(CCSequence::createWithTwoActions(
			CCDelayTime::create(0.8f),
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::callBackForShowOpenEffect))
			));
}

void CSelectChallengeLayer::callBackForShowOpenEffect()
{
	//layer_up移走
	CLayout *pLayerUp = (CLayout *)findWidgetById("layer_up");
	pLayerUp->runAction(
		CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CCMoveBy::create(0.3f, ccp(0, -250)))
		);

	//layer_down移走
	CLayout *pLayerDown = (CLayout *)findWidgetById("layer_down");
	pLayerDown->runAction(
		CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CCMoveBy::create(0.3f, ccp(0, +250)))
		);

	initUIWithData();
}

void CSelectChallengeLayer::showChallenge( CCObject *pSender )
{
	for(int i=0; i<3; i++)
	{
		m_pPanel[i]->setVisible(true);
		m_pBaseLight[i]->setVisible(true);
	}

	//layer_up移走
	CLayout *pLayerUp = (CLayout *)findWidgetById("layer_up");
	pLayerUp->runAction(CCMoveBy::create(0.5f, ccp(0, -250)));

	//layer_down移走
	CLayout *pLayerDown = (CLayout *)findWidgetById("layer_down");
	pLayerDown->runAction(CCMoveBy::create(0.5f, ccp(0, +250)));

}

void CSelectChallengeLayer::hideChanllenge( CCObject *pSender )
{
	for(int i=0; i<3; i++)
	{
		m_pPanel[i]->setVisible(false);
		m_pBaseLight[i]->setVisible(false);
	}

	//layer_up移走
	CLayout *pLayerUp = (CLayout *)findWidgetById("layer_up");
	pLayerUp->runAction(CCMoveBy::create(0.5f, ccp(0, +250)));

	//layer_down移走
	CLayout *pLayerDown = (CLayout *)findWidgetById("layer_down");
	pLayerDown->runAction(CCMoveBy::create(0.5f, ccp(0, -250)));

}

void CSelectChallengeLayer::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);
	//保存facebook 头像
	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	CImageView* pHeadBg = m_pMapHeadBg[sTag];
	if (pHeadBg)
	{
		m_pMapHeadBg.erase(sTag);
		CLayout *pParent = ( CLayout *)pHeadBg->getParent();
		CImageView *headbg = (CImageView *)pParent->findWidgetById("headbg");
		CImageView *headImg = (CImageView *)pParent->findWidgetById("headImg");
		headImg->removeFromParentAndCleanup(true);

		CImageView *headSpr = maskedSprite(CCSprite::createWithTexture(texture));
		headSpr->setScale(0.53f);
		headSpr->setPosition(pHeadBg->getPosition()+ccp(headbg->getContentSize().width*0.5f*headbg->getScaleX(), headbg->getContentSize().height*0.5f*headbg->getScaleY()));	
		headSpr->setId("headImg");
		pParent->addChild(headSpr);
		
	}
	img->release();
}

CImageView * CSelectChallengeLayer::maskedSprite(CCSprite *textureSprite)  
{  
	CCSprite * maskSprite = CCSprite::create("mainCity/tencil.png");  
	CCRenderTexture * renderTexture = CCRenderTexture::create(maskSprite->getContentSize().width, maskSprite->getContentSize().height);  

	maskSprite->setPosition(ccp(maskSprite->getContentSize().width / 2, maskSprite->getContentSize().height / 2));  
	textureSprite->setPosition(ccp(textureSprite->getContentSize().width / 2, textureSprite->getContentSize().height / 2));  

	ccBlendFunc bfun1;
	bfun1.src = GL_ONE;
	bfun1.dst = GL_ZERO;
	maskSprite->setBlendFunc(bfun1);  
	ccBlendFunc bfun2;
	bfun2.src = GL_DST_ALPHA;
	bfun2.dst = GL_ZERO;
	textureSprite->setBlendFunc(bfun2);  

	renderTexture->begin();  
	maskSprite->visit();  
	textureSprite->visit();  
	renderTexture->end();  

	CImageView * retval = CImageView::createWithTexture(renderTexture->getSprite()->getTexture());  
	retval->setFlipY(true);  

	return retval;  
}  
