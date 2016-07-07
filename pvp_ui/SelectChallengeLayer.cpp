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

bool SelectChallengeSortPvpPrize(const Prize* data1, const  Prize* data2)
{
	return data1->type()<data2->type();
}

CSelectChallengeLayer::CSelectChallengeLayer():
	m_ui(nullptr),m_pCell(nullptr),m_bFirstInit(true), m_iLockIndex(0)
	,m_bGetNewData(false)
{
	for(int i=0; i<3; i++)
	{
		m_pPanel[i] = nullptr;
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
		}

		//机器人
		CImageView* pRobot = (CImageView*)m_ui->findWidgetById("robot");
		pRobot->setTouchEnabled(true);
		pRobot->setOnClickListener(this, ccw_click_selector(CSelectChallengeLayer::onRobot));

		//刷新
		CButton* pRefresh = (CButton*)m_ui->findWidgetById("refresh");
		pRefresh->setOnClickListener(this, ccw_click_selector(CSelectChallengeLayer::onRefresh));

		setVisible(false);

		return true;
	}

	return false;
}

void CSelectChallengeLayer::onEnter()
{
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CSelectChallengeLayer::onClose));
	this->addChild(pClose, 999);

	GetTcpNet->registerMsgHandler(PvpDataMsg, this, CMsgHandler_selector(CSelectChallengeLayer::processNetMsg));
}

void CSelectChallengeLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);

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
				initUIWithData();
				panelActionShow();
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

	//弹出综合性面板
	CPvpMenuLayer* pLayer = CPvpMenuLayer::create();
	LayerManager::instance()->push(pLayer);
}

void CSelectChallengeLayer::onRefresh( CCObject* pSender )
{
	if(isUILock()) return;

	//刷新数据
	CPlayerControl::getInstance().askForPvpChallenge();
}


void CSelectChallengeLayer::onCombat( CCObject* pSender )
{
	//TODO
	if(isUILock()) return;

	//进入选人界面

}


void CSelectChallengeLayer::updatePanelUI()
{
	for(int i=0; i<3; i++)
	{
		updateOnePanel(m_pPanel[i], m_pPvpDataRes.opponents().Get(i), i);
	}
}

void CSelectChallengeLayer::updateOnePanel( CLayout* pPanel, const Opponent& pOpponent, int iBgId )
{
	//name，等级，头像，立绘，积分，等级
	//name
	CLabel* pNameId = (CLabel*)pPanel->findWidgetById("name");
	pNameId->setString(pOpponent.role_name().c_str());

	//等级
	CLabel* pLevel = (CLabel*)pPanel->findWidgetById("level");
	pLevel->setString(CCString::createWithFormat("%d", pOpponent.role_lv())->getCString());

	//头像
	CCSprite* pHead = (CCSprite*)pPanel->findWidgetById("headImg");
	CCString* pHeadStr = CCString::createWithFormat("headIcon/%d.png", pOpponent.role_thumb());
	std::string strFullPathHead = CCFileUtils::sharedFileUtils()->fullPathForFilename(pHeadStr->getCString());
	if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPathHead))
	{
		pHead->setTexture(CCTextureCache::sharedTextureCache()->addImage(pHeadStr->getCString()));
	}

	//立绘
	CCSprite* pBody = (CCSprite*)pPanel->findWidgetById("picture");
	CCString* pBodyStr = CCString::createWithFormat("pvp/%d.png", pOpponent.hero_thumb());
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
		m_pPanel[i]->setScaleX(fChallengePanelScale[i]);
		m_pPanel[i]->runAction(CCSequence::create(
			CCDelayTime::create(0.1f*i),
			CCScaleTo::create(0.2f, 0, m_pPanel[i]->getScaleY()), 
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowDataChangeCallBack)),
			CCScaleTo::create(0.2f, fChallengePanelScale[i], m_pPanel[i]->getScaleY()), 
			CCCallFunc::create(this, callfunc_selector(CSelectChallengeLayer::panelActionReshowCallBack)),
			nullptr
			));
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
