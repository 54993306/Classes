#include "RewardLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/UICloneMgr.h"
#include "RewardHardLayer.h"
#include "common/CommonFunction.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "tools/CCShake.h"
#include "common/ShaderDataHelper.h"


int CRewardLayer::m_iFlagId = 0;
using namespace BattleSpace;
bool CRewardLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("RewardMask");
		lay->setContentSize(CCSizeMake(1138,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("rewardStage.xaml");  //  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;

}

CRewardLayer::CRewardLayer():m_pos(0),m_iCurrentChapterId(0)
{

}

void CRewardLayer::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_selectSpr =  (CImageView*)(m_ui->findWidgetById("select"));
	m_selectSpr->retain();
	
	m_pageView = (CPageView *)m_ui->findWidgetById("scroll");
	m_pageView->setDirection(eScrollViewDirectionHorizontal);	
	m_pageView->setCountOfCell(0);
	m_pageView->setSizeOfCell(m_cell->getContentSize());
	m_pageView->setAnchorPoint(ccp(0,0));
	m_pageView->setAutoRelocate(true);
	m_pageView->setDeaccelerateable(false);
	m_pageView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CRewardLayer::tableviewDataSource));
	m_pageView->reloadData();
	
	GetTcpNet->registerMsgHandler(BountyDataMsg,this,CMsgHandler_selector(CRewardLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(BountyHardMsg,this,CMsgHandler_selector(CRewardLayer::ProcessMsg));

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CRewardLayer::onClose));
	this->addChild(pClose, 999);

	CButton *btn = (CButton*)m_ui->findWidgetById("combat");
	btn->setTouchEnabled(true);
	btn->setOnClickListener(this,ccw_click_selector(CRewardLayer::onStage));
	btn->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.1f), CCScaleTo::create(0.3f, 1.0f))));
	btn->setAnchorPoint(ccp(0.5f, 0.5f));
	btn->setPosition(ccpAdd(btn->getPosition(), ccp(btn->getContentSize().width/2, btn->getContentSize().height/2)));

	CImageView *left = (CImageView*)m_ui->findWidgetById("left");
	left->setTouchEnabled(true);
	left->setOnClickListener(this,ccw_click_selector(CRewardLayer::onLeft));
	CImageView *right = (CImageView*)m_ui->findWidgetById("right");
	right->setTouchEnabled(true);
	right->setOnClickListener(this,ccw_click_selector(CRewardLayer::onRight));


	RewardDataGM *data = DataCenter::sharedData()->getRewardDataGM();
	int size = data ->getStoryLength();
	int m_pos = CCRANDOM_0_1()*(size-1);
	CLabel *story = (CLabel*)m_ui->findWidgetById("story");
	story->setString(data->getStoryStr(m_pos));

	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
	NOTIFICATION->addObserver(this, callfuncO_selector(CRewardLayer::timeInBackground), TIME_IN_BACKGROUND, nullptr);
}

CCObject* CRewardLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CRewardLayer::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);	
	CBounty *bounty = &m_bountyList.at(uIdx);
	const RewardData *reward = DataCenter::sharedData()->getRewardDataGM()->getRewardCfg(bounty->id);
	bool isOpen = DataCenter::sharedData()->getUser()->getUserData()->getLevel()>=bounty->openLevel;

	if (!reward)
	{
		return;
	}
	for (int i = 1; i <=5; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 2:
			{
				CImageView *spr =(CImageView*)child;
				spr->setTouchEnabled(true);
				spr->setUserData(bounty);
				spr->setOnClickListener(this,ccw_click_selector(CRewardLayer::onSelect));
				spr->setEnabled(isOpen);
				CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("reward/%s.png",reward->icon1.c_str())->getCString());
				if (texture) 
				{			
					spr->setTexture(texture);
				}
				if (!isOpen)
				{
					spr->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
				}

				if(m_iCurrentChapterId == bounty->id)
				{
					m_selectSpr->removeFromParent();
					m_selectSpr->setPosition(spr->getPosition());
					//m_selectSpr->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
					spr->getParent()->addChild(m_selectSpr, 100);
				}

		//		NodeFillNode(head, spr);
			}
			break;
		case 4:
			{
				child->setVisible(!isOpen);
			}
			break;
		case 5:
			{
				child->setVisible(!isOpen);
				CLabel *level = (CLabel*)child;
				level->setString(ToString(bounty->openLevel));
			}
			break;
		default:
			break;
		}
	}
}

void CRewardLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	NOTIFICATION->removeAllObservers(this);

	m_iFlagId = m_iCurrentChapterId;
}


void CRewardLayer::ProcessMsg(int type, google::protobuf::Message *msg)
{
	bool bFirst = false;
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		PlayEffectSound(SFX_430);
		bFirst = true;
	}

	if (type==BountyDataMsg)
	{
		BountyDataRes *res = (BountyDataRes*)msg;

		//如果没有任务
		if(res->blist_size()<=0)
		{
			ShowPopTextTip(GETLANGSTR(1023));
			onClose(nullptr);
			return;
		}

		for (int i = 0; i < res->blist_size(); i++)
		{
			CBounty bounty;
			bounty.id= res->blist(i).id();
			bounty.time = res->blist(i).endtime();
			bounty.openLevel = res->blist(i).openlevel();
			m_bountyList.push_back(bounty);
		}

		//排序
		std::sort(m_bountyList.begin(), m_bountyList.end(), 
			std::bind([](const CBounty& bounty1, const CBounty& bounty2){
				return bounty1.id  > bounty2.id;
		}, std::placeholders::_1, std::placeholders::_2));

		if (m_bountyList.empty())
		{
			return;
		}
		CBounty *bounty = &m_bountyList.at(0);
		m_iCurrentChapterId = bounty->id;

		m_pageView->setCountOfCell(m_bountyList.size());
		m_pageView->reloadData();


		const RewardData *reward = DataCenter::sharedData()->getRewardDataGM()->getRewardCfg(bounty->id);

		CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
		level->setString(reward->level.c_str());

		CImageView *bg = (CImageView*)(m_ui->findWidgetById("bg"));
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("reward/%s.jpg",reward->icon.c_str())->getCString());
		if (texture)
		{
			bg->setTexture(texture);
		}
		updateOverTime(bounty);
		this->schedule(schedule_selector(CRewardLayer::updateRewardTime),1.0,kCCRepeatForever,0);

		showEffectStart();

	}
	else if (type==BountyHardMsg)
	{
		HardModeRes *res = (HardModeRes*)msg;
		//(1 成功，2 悬赏任务未开启，3 数据错误)
		if (res->status()==1)
		{
			vector<CBountyStage> stageList;
			for (int i=0;i<res->stage_size();i++)
			{		
				CBountyStage bountStage;
				bountStage.isOpen = res->stage(i).isopen();
				bountStage.action = res->stage(i).action();
				bountStage.id = res->stage(i).id();
				bountStage.openLevel = res->stage(i).openlevel();
				bountStage.hardMode = res->stage(i).hardmode();
				stageList.push_back(bountStage);
			}

			//没数据
			if(stageList.size()<=0)
			{
				return;
			}

			CRewardHardLayer *rewardhard = CRewardHardLayer::create();
			rewardhard->setChapterId(m_iCurrentChapterId);
			rewardhard->setStageList(stageList);
			LayerManager::instance()->push(rewardhard);
		}
		else if (res->status()==2)
		{
		}
		else
		{

		}
	}
}

void CRewardLayer::updateRewardTime(float dt)
{
	for (int i = 0; i < m_bountyList.size(); i++)
	{
		m_bountyList.at(i).time -= 1;
		if (m_bountyList.at(i).id == m_iCurrentChapterId)
		{
			updateOverTime(&m_bountyList.at(i));
		}
	}
}

void CRewardLayer::onStage(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);


// 	CButton *btn = (CButton*)pSender;
// 	CBounty *bounty = (CBounty*)btn->getUserData();
// 	m_iCurrentChapterId = bounty->id;
	if (m_iCurrentChapterId>0)
	{
		CPlayerControl::getInstance().sendBountyHard(m_iCurrentChapterId);
	}
}

void CRewardLayer::onSelect(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CImageView *btn = (CImageView*)pSender;
	CBounty *bounty = (CBounty*)btn->getUserData();
	m_iCurrentChapterId = bounty->id;

	m_selectSpr->removeFromParent();
	m_selectSpr->setPosition(btn->getPosition());
	//m_selectSpr->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
	btn->getParent()->addChild(m_selectSpr);

	const RewardData *reward = DataCenter::sharedData()->getRewardDataGM()->getRewardCfg(bounty->id);

	CLabel *level = (CLabel*)(m_ui->findWidgetById("level"));
	level->setString(reward->level.c_str());

	CImageView *bg = (CImageView*)(m_ui->findWidgetById("bg"));
	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("reward/%s.jpg",reward->icon.c_str())->getCString());
	if (texture)
	{
		bg->setTexture(texture);
	}

	updateOverTime(bounty);

}

void CRewardLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CRewardLayer::onHero(CCObject* pSender)
{
	RewardDataGM *data = DataCenter::sharedData()->getRewardDataGM();
	int size = data ->getStoryLength();
	for (int i = 0; ; i++)
	{
		int pos = CCRANDOM_0_1()*(size-1);
		if (pos!=m_pos)
		{
			m_pos = pos;
			break;
		}
	}
	CLabel *story = (CLabel*)m_ui->findWidgetById("story");
	story->setString(data->getStoryStr(m_pos));
}

void CRewardLayer::showEffectStart()
{
	int iTimes = 0;
	for(int i=0; i<m_bountyList.size(); i++)
	{
		if(m_bountyList.at(i).id == m_iFlagId)
		{
			iTimes = i;
		}
	}

	for(int i=0; i<iTimes; i++)
	{
		onRight(nullptr);
	}
	onSelect(m_pageView->cellAtIndex(iTimes)->getChildByTag(2));
}

void CRewardLayer::updateOverTime(CBounty * bounty)
{
	CLabel *timeLab = (CLabel*)(m_ui->findWidgetById("time"));
 	time_t  t = bounty->time;

	if (t>=0)
	{
		int hour = floor(t / 3600);
		int min = floor((t - hour * 3600) / 60);
		int sec = floor(t - hour * 3600 - min * 60);				
		timeLab->setString(CCString::createWithFormat("%d:%d:%d",hour,min,sec)->getCString());
	}

	//timeLab->setString(CCString::createWithFormat("%ld:   %d-%d  %d:0%d",t,tim.tm_mon+1,tim.tm_mday,tim.tm_hour,tim.tm_min)->getCString());
}

void CRewardLayer::onLeft(CCObject* pSender)
{
	CCPoint pos = m_pageView->getContentOffset();
	pos.x +=  m_cell->getContentSize().width;
	if (pos.x > m_pageView->getMaxOffset().x)
	{
		pos.x  = m_pageView->getMaxOffset().x;
	}
	m_pageView->setContentOffset(pos);
}

void CRewardLayer::onRight(CCObject* pSender)
{
	CCPoint pos = m_pageView->getContentOffset();
	pos.x -=  m_cell->getContentSize().width;
	if (pos.x < m_pageView->getMinOffset().x)
	{
		pos.x  = m_pageView->getMinOffset().x;
	}
	m_pageView->setContentOffset(pos);
}

void CRewardLayer::timeInBackground( CCObject* pObj )
{
	CCInteger* pValue = dynamic_cast<CCInteger*>(pObj);
	int iValue = pValue->getValue()/1000;
	for (int i = 0; i < m_bountyList.size(); i++)
	{
		m_bountyList.at(i).time -= iValue;
		if (m_bountyList.at(i).id == m_iCurrentChapterId)
		{
			updateOverTime(&m_bountyList.at(i));
		}
	}
}


