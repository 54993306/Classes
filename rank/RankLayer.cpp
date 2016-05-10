#include "RankLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "scene/CPopTip.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CommonFunction.h"


CRankLayer::CRankLayer():m_cell(nullptr),m_bArchive(false),m_optionType(1)
{

}


bool CRankLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("rank");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("Rank.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);	

		setCoverScreen(true);

		return true;
	}
	return false;

}

void CRankLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CRankLayer::onToggle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

}

void CRankLayer::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	m_cell1 = (CLayout*)(m_ui->findWidgetById("Cell1"));
	m_cell1->retain();
	m_ui->removeChild(m_cell1);
	
	m_heroLay = (CLayout*)(m_ui->findWidgetById("hero"));
	m_heroLay->retain();
	m_ui->removeChild(m_heroLay);

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CRankLayer::tableviewDataSource));
	m_tableView->reloadData();
	m_tableSize = m_tableView->getContentSize();

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CRankLayer::onSwitchBtn));
	}


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CRankLayer::onClose));
	this->addChild(pClose, 999);

	GetTcpNet->registerMsgHandler(RankReqMsg,this, CMsgHandler_selector(CRankLayer::ProcessMsg));
}

CCObject* CRankLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CRankLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		m_optionType = btn->getTag();
	
		if (m_rankMap.find(m_optionType)!=m_rankMap.end())
		{
			if (m_rankMap[m_optionType].myRank.rank==0)
			{
				m_tableView->setContentSize(CCSizeMake(m_tableSize.width,m_tableSize.height+m_cell->getContentSize().height));
			}
			else
			{
				m_tableView->setContentSize(m_tableSize);
			}
			m_tableView->setCountOfCell(m_rankMap[m_optionType].rankList.size());
			m_tableView->reloadData();
			addMyRank(&m_rankMap[m_optionType].myRank);
		}
		else if (m_optionType==2)
		{
			CPlayerControl::getInstance().sendRank(2);
		}
	}
}

void CRankLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
    CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_VIPINFO,this);
	BaseLayer::onExit();

	if(m_bArchive)
	{
		//显示被遮盖的层，渲染
		LayerManager::instance()->showLayerUnBeCovered(this);
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
		NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	}
} 

void CRankLayer::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CRankData *data = &m_rankMap[m_optionType].rankList.at(uIdx);
	if (m_optionType==1)
	{
		addBossRank(pCell, data);
	}
	else if (m_optionType ==2)
	{ 
		addCombatRank(pCell, data);
	}
	else
	{

	}
}

void CRankLayer::addMyRank(CRankData *data)
{
	if (m_optionType==1)
	{
		m_ui->removeChildByTag(200);
		if (data->rank>0)
		{
			CLayout *rankLay = CLayout::create(m_cell->getContentSize());
			rankLay->setTag(200);
			rankLay->setPosition(ccp(545+25,490));
			m_ui->addChild(rankLay);
			addBossRank(rankLay, data);
		}
		
	}
	else if (m_optionType ==2)
	{ 
		m_ui->removeChildByTag(200);
		if (data->rank>0)
		{
			CLayout *rankLay = CLayout::create(m_cell1->getContentSize());
			rankLay->setTag(200);
			rankLay->setPosition(ccp(545+25,490));
			m_ui->addChild(rankLay);
			addCombatRank(rankLay, data);
		}
	}
}

CRankLayer::~CRankLayer()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_cell1);

	for (int i=1; i<=2;i++)
	{
		map<int,CRankResponse>::iterator it = m_rankMap.find(i);
		if (it!=m_rankMap.end())
		{	
			vector<CRankData>::iterator iter = it->second.rankList.begin();
			for (; iter!= it->second.rankList.end(); iter++)
			{
				CC_SAFE_DELETE(iter->roleData);
				iter->roleData = nullptr;
			}
		}
	}
}


void CRankLayer::ProcessMsg(int type, google::protobuf::Message *msg)
{
	//if(!isVisible())
	{
		this->setVisible(true);
		RankResponse *res = (RankResponse *)msg;
		CRankResponse rankRes;
		rankRes.read(*res);
		m_rankMap[m_optionType] = rankRes;
		m_tableView->setCountOfCell(res->ranklist_size());
		if (rankRes.myRank.rank==0)
		{
			m_tableView->setContentSize(CCSizeMake(m_tableSize.width,m_tableSize.height+m_cell->getContentSize().height));
		}
		else
		{
			m_tableView->setContentSize(m_tableSize);
		}
		m_tableView->reloadData();
		addMyRank(&rankRes.myRank);

		//隐藏被遮盖的层，不渲染
// 		LayerManager::instance()->hideLayerBeCovered(this);
// 		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
// 		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
}


void CRankLayer::setOptionType(int type)
{
	m_optionType = type;
}


void CRankLayer::updateBuyInfo(const TMessage& tMsg)
{
	
}

void CRankLayer::addBossRank(CLayout* pCell, CRankData * data)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i=1;i<=10;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		if (!child)
		{
			continue;
		}
		lay->removeChild(child);
		pCell->addChild(child);

		switch (i)
		{
		case 1:
			{
				CButton *select = (CButton*)child;
				if (data->rank==m_rankMap.find(m_optionType)->second.myRank.rank)
				{
					select->setNormalImage("worldBoss/box_rank.png");
					select->setPositionY(select->getPositionY()-16);
				}
				else if (data->rank>3)
				{
					select->setNormalImage("worldBoss/box_rank3.png");
				}

				select->setOnClickListener(this,ccw_click_selector(CRankLayer::onSelect));
			}
			break;
		case 2:
			{
				CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", data->roleData->getThumb())->getCString());
				if (head)
				{
					head->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
					child->addChild(head);
				}
			}
			break;
		case 3:
			{
				CLabel *lab = (CLabel*)child;
				lab->setString(data->roleData->getRoleName().c_str());
			}
			break;
		case 4:
			{
				CLabel *lab = (CLabel*)child;
				lab->setString(ToString(data->roleData->getLevel()));
			}
			break;
		case 7:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->hurt));
			}
			break;
		case 8:
			{
				if (data->rank<=3)
				{
					CImageView *view = (CImageView*)child;
					view->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("worldBoss/rank_%d.png",data->rank)->getCString()));
				}
				child->setVisible(data->rank<=3);
			}
			break;
		case 9:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->rank));
			}
			break;

		default:
			break;
		}
	}
	
	
	for (int i = 0; i < data->teamList.size(); i++)
	{
		CLayout *teamLay  = UICloneMgr::cloneLayout(m_heroLay);
		CTeams *team = &data->teamList.at(i);
		for (int j = 0; j < team->quality; j++)
		{
			CCNode *star = (CCNode*)(teamLay->findWidgetById(CCString::createWithFormat("star%d",j+1)->getCString()));
			star->setVisible(true);
		}
		CCSprite *bg = (CCSprite*)teamLay->findWidgetById("bg");
		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", team->thumb)->getCString());
		if (head)
		{
			head->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
			bg->addChild(head);
		}
		CCSprite *mask = (CCSprite*)teamLay->findWidgetById("mask");
		mask->setTexture(setItemQualityTexture(team->color));
		while (teamLay->getChildrenCount()>0)
		{
			CCNode *node = (CCNode *) teamLay->getChildren()->objectAtIndex(0);
			node->removeFromParent();
			pCell->addChild(node);
			node->setPositionX(node->getPositionX()-55*i);
		}
	}
}

void CRankLayer::addCombatRank(CLayout* pCell, CRankData * data)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell1);
	for (int i=1;i<=11;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		if (!child)
		{
			continue;
		}
		lay->removeChild(child);
		pCell->addChild(child);

		switch (i)
		{
		case 1:
			{
				CButton *select = (CButton*)child;
				if (data->rank==m_rankMap.find(m_optionType)->second.myRank.rank)
				{
					select->setNormalImage("worldBoss/box_rank.png");
					select->setPositionY(select->getPositionY()-16);
				}
				else if (data->rank>3)
				{
					select->setNormalImage("worldBoss/box_rank3.png");
				}
				select->setOnClickListener(this,ccw_click_selector(CRankLayer::onSelect));
			}
			break;
		case 2:
			{
				CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", data->roleData->getThumb())->getCString());
				if (head)
				{
					head->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
					child->addChild(head);
				}
			}
			break;
		case 3:
			{
				CLabel *lab = (CLabel*)child;
				lab->setString(data->roleData->getRoleName().c_str());
			}
			break;
		case 4:
			{
				CLabel *lab = (CLabel*)child;
				lab->setString(ToString(data->roleData->getLevel()));
			}
			break;
		case 10:
			{
				((CLabelAtlas*)child)->setString(ToString(data->roleData->getCombat()));
				child->setAnchorPoint(ccp(0,0.5));
			}
			break;
		case 8:
			child->setVisible(data->rank<=3);
			break;
		case 9:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->rank));
			}
			break;
		default:
			break;
		}
	}
}

void CRankLayer::onSelect(CCObject* pSender)
{

}