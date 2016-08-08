#include "PvpRuleLayer.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "common/CCMixLabelAction.h"
#include "rank/RankLayer.h"
#include "netcontrol/CPlayerControl.h"

bool WRuleSortPvpPrize(const PvpPrize& data1, const PvpPrize& data2)
{
	return data1.rank()<data2.rank();
}

bool WRuleSortPrize(const Prize& data1, const Prize& data2)
{
	return data1.type()<data2.type();
}

CPvpRuleLayer::CPvpRuleLayer():
m_ui(nullptr), m_pLayer(nullptr), m_pScroll(nullptr), m_pCell(nullptr)
{

}

CPvpRuleLayer::~CPvpRuleLayer()
{
	CC_SAFE_RELEASE(m_pCell);
}

bool CPvpRuleLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("pvp_rule.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		//保存Cell
		m_pCell = (CLayout*)findWidgetById("cell");
		m_pCell->retain();
		m_pCell->removeFromParent();

		//展示区图片
		m_pLayer = CLayout::create();

		m_pScroll = (CScrollView*)m_ui->findWidgetById("scroll_info");
		m_pScroll->setDirection(eScrollViewDirectionVertical);
		m_pScroll->setBounceable(false);
		m_pScroll->getContainer()->addChild(m_pLayer);
		updateShowInfoScroll(CCSize(852, 1270));

		//嫁接内容
		CLayout* pLayout = (CLayout*)findWidgetById("layer_info");
		pLayout->retain();
		pLayout->removeFromParent();
		m_pLayer->addChild(pLayout);
		pLayout->release();
		pLayout->setPosition(ccp(434, 1080));

		setVisible(true);
		
		setIsShowBlack(false);

		return true;
	}

	return false;
}

void CPvpRuleLayer::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(PvpPrizeListMsg, this, CMsgHandler_selector(CPvpRuleLayer::processNetMsg));
}

void CPvpRuleLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);


	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);

}

void CPvpRuleLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	
	if(!isVisible())
	{
		this->setVisible(true);
	}
	
	switch (type)
	{
	case PvpPrizeListMsg:
		{
			PvpPrizeList* pPrizes = (PvpPrizeList*)msg;
			//initRank
			initRank(pPrizes);
		}
		break;
	default:
		break;
	}
}

void CPvpRuleLayer::updateShowInfoScroll(CCSize size)
{
	m_pLayer->setContentSize(size);
	m_pScroll->setContainerSize(size);
	m_pLayer->setPosition(ccp(m_pScroll->getContainerSize().width*0.5f, m_pScroll->getContainerSize().height*0.5f));
	m_pScroll->setContentOffsetToTop();
}

void CPvpRuleLayer::initRank(PvpPrizeList* pPrizes)
{
	CCNode* pNode = (CCNode*)m_pLayer->findWidgetById("title_tip2");

	CCPoint pPos = pNode->getPosition()+ccp(215, 167);

	vector<PvpPrize> vecPvpPrize;
	for(unsigned int b=0; b<pPrizes->prize_list().size(); b++)
	{
		vecPvpPrize.push_back(pPrizes->prize_list().Get(b));
	}
	std::sort(vecPvpPrize.begin(), vecPvpPrize.end(), WRuleSortPvpPrize);

	for(int i=0; i<vecPvpPrize.size(); i++)
	{
		//最多10个
		if(i==10) break;

		const PvpPrize& pvpPrize = vecPvpPrize[i];

		CLayout* pCell = UICloneMgr::cloneLayout(m_pCell);
		pCell->setPosition(pPos+ccp(0, -87*i));
		pNode->getParent()->addChild(pCell);

		//排名
		int iRank = pvpPrize.rank();
		CCSprite* pRankIcon = (CCSprite*)(pCell->findWidgetById("rank_icon"));
		if(iRank<=3)
		{
			CCString* pStr = CCString::createWithFormat("worldBoss/rank_%d.png", iRank);
			std::string strFullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pStr->getCString());
			if(CCFileUtils::sharedFileUtils()->isFileExist(strFullPath))
			{
				pRankIcon->setTexture(CCTextureCache::sharedTextureCache()->addImage(pStr->getCString()));
			}
		}
		else
		{
			pRankIcon->setVisible(false);
		}
		//label
		CLabelAtlas* pAtlas = CLabelAtlas::create("", "worldBoss/no_04.png", 26, 43, 48);
		pAtlas->setString(CCString::createWithFormat("%d", iRank)->getCString());
		pAtlas->setPosition(pRankIcon->getPosition()+ccp(0, 25));
		pRankIcon->getParent()->addChild(pAtlas, pRankIcon->getZOrder()+1);

		
		CImageView* rank_bg = (CImageView*)(pCell->findWidgetById("rank_bg"));
		rank_bg->setTouchEnabled(true);
		rank_bg->setOnClickListener(this,ccw_click_selector(CPvpRuleLayer::goRank));
		//数量
		//奖励
		
		vector<Prize> vecWarPrize;
		for(unsigned int b=0; b<pvpPrize.prizes().size();b++)
		{
			vecWarPrize.push_back(pvpPrize.prizes().Get(b));
		}
		std::sort(vecWarPrize.begin(), vecWarPrize.end(), WRuleSortPrize);

		for(unsigned int j=0; j<pvpPrize.prizes().size(); j++)
		{
	//		if(i==2)break;

			const Prize& prize = vecWarPrize[j];
			
			CLabel* pPrize = (CLabel*)pCell->findWidgetById(CCString::createWithFormat("text_%d", j+1)->getCString());
			pPrize->setString(CCString::createWithFormat("%d", prize.num())->getCString());
			
			CCSprite* pIcon = (CCSprite *)pCell->findWidgetById(CCString::createWithFormat("icon_%d", j+1)->getCString());
			pIcon->setOpacity(0);
			CImageView *head = CImageView::create(GetImageName(prize.type(), prize.color(), prize.thumb()).c_str());
			if (!head)
			{
				head = CImageView::create("prop/0.png");
				CCLOG("[ ERROR ] CPvpRuleLayer::initRank Lost Image");
			}
			head->setPosition(ccp(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
			pIcon->addChild(head);

		}
	}
}

void CPvpRuleLayer::goRank(CCObject* pSender)
{
	//CRankLayer *rank = CRankLayer::create();
	//LayerManager::instance()->push(rank);
	//CPlayerControl::getInstance().sendRank(1);
}