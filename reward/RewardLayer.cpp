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


int CRewardLayer::TableViewOffSet = 0;
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

	TableViewOffSet = m_pageView->getContentOffset().y;
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
		m_pageView->setCountOfCell(m_bountyList.size());
		m_pageView->reloadData();

		if(bFirst)
		{
			if(TableViewOffSet == 0)
			{
				TableViewOffSet = m_pageView->getContentOffset().y;
			}
		//	 showEffectStart();
		}

		if (m_bountyList.empty())
		{
			return;
		}
		CBounty *bounty = &m_bountyList.at(0);
		m_iCurrentChapterId = bounty->id;

		m_selectSpr->removeFromParent();
		m_selectSpr->setPosition(m_pageView->cellAtIndex(0)->getChildByTag(1)->getPosition());
		m_pageView->cellAtIndex(0)->addChild(m_selectSpr);

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

void CRewardLayer::callBackForShake()
{
	m_ui->setScale(1.02f);
	m_ui->runAction(CCSequence::createWithTwoActions(CCShake::create(0.08f, 8), CCCallFunc::create(this, callfunc_selector(CRewardLayer::showShakeCallBack))));
}

void CRewardLayer::showShakeCallBack()
{
	m_ui->setScale(1.0f);
}

void CRewardLayer::showEffectStart()
{
	//上板块移动
	CLayout* pUpbar = (CLayout*)m_ui->findWidgetById("up_bar");
	pUpbar->setPositionY(pUpbar->getPositionY()+300);
	pUpbar->runAction(CCMoveBy::create(0.15f, ccp(0, -300)));

	//光效
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("9014");
	culAnim->setDelayPerUnit(0.05f);
	CCAnimate* pAnimate = CCAnimate::create(culAnim);
	CCSprite* pSprite = CCSprite::create();
	pSprite->setPosition(ccp(578, 266));
	pSprite->setScale(2.0f);
	pSprite->setScaleY(2.1f);
	m_ui->addChild(pSprite, m_pageView->getZOrder()-1);
	pSprite->runAction(CCRepeatForever::create(pAnimate));
	m_pBgEffect = pSprite;
	m_pBgEffect->setOpacity(0);
	m_pBgEffect->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.15f), CCFadeIn::create(0.2f)));

	float fTimeGap = 0.15f;
	for(unsigned int i=0; i<m_pageView->getContainer()->getChildrenCount(); i++)
	{
		CCNode* pNode = (CCNode*)m_pageView->getContainer()->getChildren()->objectAtIndex(i);
		pNode->setVisible(false);
		pNode->runAction(CCSequence::create(
			CCMoveBy::create(0.0f, ccp(0, -400)),
			CCDelayTime::create(fTimeGap*(i+1)),
			CCShow::create(),
			CCMoveBy::create(fTimeGap, ccp(0, 400)),
			CCCallFunc::create(this, callfunc_selector(CRewardLayer::callBackForShake)),
			nullptr));
		if(i==4) break;
	}

	this->runAction(CCSequence::create(CCDelayTime::create(0.75f), CCCallFunc::create(this, callfunc_selector(CRewardLayer::showEffectCallBack)),nullptr));
}

void CRewardLayer::showEffectCallBack()
{
	m_pageView->setContentOffsetInDuration(ccp(0, TableViewOffSet), 0.5f);
}

void CRewardLayer::updateOverTime(CBounty * bounty)
{
	CLabel *timeLab = (CLabel*)(m_ui->findWidgetById("time"));
	tm tim;
	time_t  t = bounty->time/1000;
	tim = *localtime(&t);	

	tim.tm_hour = tim.tm_hour==0?24:tim.tm_hour;

	if (tim.tm_min>9)
	{					
		timeLab->setString(CCString::createWithFormat("%d:%d",tim.tm_hour,tim.tm_min)->getCString());
	}
	else
	{
		timeLab->setString(CCString::createWithFormat("%d:0%d",tim.tm_hour,tim.tm_min)->getCString());
	}
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




