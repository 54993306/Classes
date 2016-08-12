#include "WRuleLayer.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"
#include "common/CommonFunction.h"
#include "common/CCMixLabelAction.h"
#include "rank/RankLayer.h"
#include "netcontrol/CPlayerControl.h"

bool WRuleSortBossPrize(const BossPrize& data1, const BossPrize& data2)
{
	return data1.rank()<data2.rank();
}

bool WRuleSortWarPrize(const Prize& data1, const Prize& data2)
{
	return data1.type()<data2.type();
}

CWRuleLayer::CWRuleLayer():
m_ui(nullptr), m_pLayer(nullptr), m_pScroll(nullptr), m_pCell(nullptr)
{

}

CWRuleLayer::~CWRuleLayer()
{
	CC_SAFE_RELEASE(m_pCell);
}

bool CWRuleLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//内容
		m_ui = LoadComponent("wRule.xaml");
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

		return true;
	}

	return false;
}

void CWRuleLayer::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(BossPrizesMsg, this, CMsgHandler_selector(CWRuleLayer::processNetMsg));
}

void CWRuleLayer::onExit()
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

void CWRuleLayer::processNetMsg( int type, google::protobuf::Message *msg )
{
	
	if(!isVisible())
	{
		this->setVisible(true);
	}
	
	switch (type)
	{
	case BossPrizesMsg:
		{
			BossPrizes* pBossPrizes = (BossPrizes*)msg;
			if(pBossPrizes->result() == 1)
			{
				//initRank
				initRank(pBossPrizes);
			}
		}
		break;
	default:
		break;
	}
}

void CWRuleLayer::updateShowInfoScroll(CCSize size)
{
	m_pLayer->setContentSize(size);
	m_pScroll->setContainerSize(size);
	m_pLayer->setPosition(ccp(m_pScroll->getContainerSize().width*0.5f, m_pScroll->getContainerSize().height*0.5f));
	m_pScroll->setContentOffsetToTop();
}

void CWRuleLayer::initRank(BossPrizes* pPrizes)
{
	CCNode* pNode = (CCNode*)m_pLayer->findWidgetById("title_tip2");

	CCPoint pPos = pNode->getPosition()+ccp(215, 167);

	vector<BossPrize> vecBossPrize;
	for(unsigned int b=0; b<pPrizes->prizelist().size(); b++)
	{
		vecBossPrize.push_back(pPrizes->prizelist().Get(b));
	}
	std::sort(vecBossPrize.begin(), vecBossPrize.end(), WRuleSortBossPrize);

	for(int i=0; i<vecBossPrize.size(); i++)
	{
		//最多10个
		if(i==10) break;

		const BossPrize& bossPrize = vecBossPrize[i];

		CLayout* pCell = UICloneMgr::cloneLayout(m_pCell);
		pCell->setPosition(pPos+ccp(0, -87*i));
		pNode->getParent()->addChild(pCell);

		//排名
		int iRank = bossPrize.rank();
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
		rank_bg->setOnClickListener(this,ccw_click_selector(CWRuleLayer::goRank));
		//数量
		//奖励
		
		vector<Prize> vecWarPrize;
		for(unsigned int b=0; b<bossPrize.prizes().size();b++)
		{
			vecWarPrize.push_back(bossPrize.prizes().Get(b));
		}
		std::sort(vecWarPrize.begin(), vecWarPrize.end(), WRuleSortWarPrize);

		for(unsigned int j=0; j<bossPrize.prizes().size(); j++)
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
				CCLOG("[ ERROR ] CWRuleLayer::initRank Lost Image");
			}
			head->setPosition(ccp(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
			pIcon->addChild(head);

		}
	}
}

void CWRuleLayer::goRank(CCObject* pSender)
{
	//CRankLayer *rank = CRankLayer::create();
	//LayerManager::instance()->push(rank);
	//CPlayerControl::getInstance().sendRank(1);
}