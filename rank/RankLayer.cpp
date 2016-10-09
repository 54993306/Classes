#include "RankLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "scene/CPopTip.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "common/CommonFunction.h"
#include "mainCity/MainCityControl.h"

#define  FACEBOOKIMG "http://graph.facebook.com/%s/picture?width=70&height=70"


CRankLayer::CRankLayer():m_cell(nullptr),m_bArchive(false),m_optionType(2)
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
	
	m_cell2 = (CLayout*)(m_ui->findWidgetById("Cell2"));
	m_cell2->retain();
	m_ui->removeChild(m_cell2);

	m_cell3 = (CLayout* )m_ui->findWidgetById("Cell3");
	m_cell3->retain();
	m_ui->removeChild(m_cell3);

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
	for (int i = 0; i < 4; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CRankLayer::onSwitchBtn));
		if (!CMainCityControl::getInstance()->isCityOpen(17)&&i==2)
		{
			radioBtn->setVisible(false);
		}
	}


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CRankLayer::onClose));
	this->addChild(pClose, 999);

	GetTcpNet->registerMsgHandler(RankReqMsg,this, CMsgHandler_selector(CRankLayer::ProcessMsg));
	HttpLoadImage::getInstance()->bindUiTarget(this);

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
		else if (m_optionType==1)
		{
			CPlayerControl::getInstance().sendRank(1);
		}
		else if (m_optionType==3)
		{
			CPlayerControl::getInstance().sendRank(3);
		}
		else if (m_optionType==4)
		{
			CPlayerControl::getInstance().sendRank(1);
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
	HttpLoadImage::getInstance()->bindUiTarget(nullptr);

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
	else if(m_optionType == 3)
	{
		addPvpRank(pCell, data);
	}
	else if(m_optionType == 4)
	{
		addTowerRank(pCell,data);
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
	else if (m_optionType==3)
	{
		m_ui->removeChildByTag(200);
		if (data->rank>0)
		{
			CLayout *rankLay = CLayout::create(m_cell2->getContentSize());
			rankLay->setTag(200);
			rankLay->setPosition(ccp(545+25,490));
			m_ui->addChild(rankLay);
			addPvpRank(rankLay, data);
		}
	}
	else if (m_optionType == 4)
	{
		//自己的排名
		m_ui->removeChildByTag(200);
		if (data->rank>0)
		{
			CLayout *rankLay = CLayout::create(m_cell->getContentSize());
			rankLay->setTag(200);
			rankLay->setPosition(ccp(545+25,490));
			m_ui->addChild(rankLay);
			addTowerRank(rankLay, data);
		}
	}
}

CRankLayer::~CRankLayer()
{
	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_cell1);
	CC_SAFE_RELEASE(m_cell2);

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
				if (data->roleData->getThumb()>0)
				{
					CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", data->roleData->getThumb())->getCString());
					if (head)
					{
						head->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
						child->addChild(head);
					} 
				}
				else
				{
					string fbName = data->roleData->getFbId()+".jpg";
					string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
					bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
					if(isFileExist)
					{
						CCSprite* spr =CCSprite::create(fullName.c_str());
						child->addChild(spr);
						NodeFillParent(spr);
					}
					else
					{
						CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,data->roleData->getFbId().c_str());
						HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),data->roleData->getFbId().c_str());
					}
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
				if (data->rank<=3&&data->rank>0)
				{
					CImageView *view = (CImageView*)child;
					view->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("worldBoss/rank_%d.png",data->rank)->getCString()));
				}
				child->setVisible(data->rank<=3&&data->rank>0);
			}
			break;
		case 9:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->rank));
				child->setVisible(data->rank>0);
			}
			break;
		case 11:
			{
				child->setVisible(data->rank==0);
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

		//for (int j = 0; j < team->color; j++)
		//{
		//	CCNode *star = (CCNode*)(teamLay->findWidgetById(CCString::createWithFormat("star%d",j+1)->getCString()));
		//	star->setVisible(true);
		//}
		CCSprite *bg = (CCSprite*)teamLay->findWidgetById("bg");
		CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", team->thumb)->getCString());
		if (head)
		{
			head->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
			bg->addChild(head);
		}
		CCSprite *mask = (CCSprite*)teamLay->findWidgetById("mask");
		const HeroInfoData *heroData = DataCenter::sharedData()->getHeroInfo()->getCfg(team->thumb);
		mask->setTexture(SetRectColor(heroData->iType1));
		while (teamLay->getChildrenCount()>0)
		{
			CCNode *node = (CCNode *) teamLay->getChildren()->objectAtIndex(0);
			node->removeFromParent();
			pCell->addChild(node);
			node->setPositionX(node->getPositionX()-55*(data->teamList.size()-1-i));
		}

		//添加星星
		CLayout* pStarLayout = getStarLayout(team->color);
		mask->addChild(pStarLayout);
	}
}

void CRankLayer::addPvpRank(CLayout* pCell, CRankData * data)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell2);
	for (int i=1;i<=13;++i)
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
				if (data->roleData->getThumb()>0)
				{
					CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", data->roleData->getThumb())->getCString());
					if (head)
					{
						head->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
						child->addChild(head);
					} 
				}
				else
				{
					string fbName = data->roleData->getFbId()+".jpg";
					string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
					bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
					if(isFileExist)
					{
						CCSprite* spr =CCSprite::create(fullName.c_str());
						child->addChild(spr);
						NodeFillParent(spr);
					}
					else
					{
						CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,data->roleData->getFbId().c_str());
						HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),data->roleData->getFbId().c_str());
					}
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
				lab->setString(ToString(data->roleData->getCombat()));
			}
			break;
		case 8:
			{
				if (data->rank<=3&&data->rank>0)
				{
					CImageView *view = (CImageView*)child;
					view->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("worldBoss/rank_%d.png",data->rank)->getCString()));
				}
				child->setVisible(data->rank<=3&&data->rank>0);
			}
			break;
		case 9:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->rank));
				child->setVisible(data->rank>0);
			}
			break;
		case 11:
			{
				child->setVisible(data->rank==0);
			}
			break;
		case 13:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setAnchorPoint(ccp(0,0.5));
				lab->setString(ToString(data->hurt));
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
		//for (int j = 0; j < team->quality; j++)
		//{
		//	CCNode *star = (CCNode*)(teamLay->findWidgetById(CCString::createWithFormat("star%d",j+1)->getCString()));
		//	star->setVisible(true);
		//}
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

		//添加星星
		CLayout* pStarLayout = getStarLayout(team->color);
		mask->addChild(pStarLayout);
	}
}

void CRankLayer::addCombatRank(CLayout* pCell, CRankData * data)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell1);
	for (int i=1;i<=12;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		if (!child)
		{
			continue;
		}
		child->retain();
		lay->removeChild(child);
		pCell->addChild(child);
		child->release();

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
				if (data->roleData->getThumb()>0)
				{
					CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", data->roleData->getThumb())->getCString());
					if (head)
					{
						head->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
						child->addChild(head);
					} 
				}
				else
				{
					string fbName = data->roleData->getFbId()+".jpg";
					string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
					bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fullName);
					if(isFileExist)
					{
						CCSprite* spr =CCSprite::create(fullName.c_str());
						if (spr)
						{
							child->addChild(spr);
							NodeFillParent(spr);
						}
					}
					else
					{
						CCString *imgUrl = CCString::createWithFormat(FACEBOOKIMG,data->roleData->getFbId().c_str());
						HttpLoadImage::getInstance()->requestUrlImage(imgUrl->getCString(),data->roleData->getFbId().c_str());
					}
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
			{
				if (data->rank<=3&&data->rank>0)
				{
					CImageView *view = (CImageView*)child;
					view->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("worldBoss/rank_%d.png",data->rank)->getCString()));
				}
				child->setVisible(data->rank<=3&&data->rank>0);
			}
			break;
		case 9:
			{
				CLabelAtlas *lab = (CLabelAtlas*)child;
				lab->setString(ToString(data->rank));
				lab->setVisible(data->rank>0);
			}
			break;
		case 12:
			{
				child->setVisible(data->rank==0);
			}
			break;
		default:
			break;
		}
	}
}

void CRankLayer::addTowerRank(CLayout* pCell, CRankData* data)
{
	CLayout* clone = UICloneMgr::cloneLayout(m_cell3);
	for (int i = 1;i<= 12;i++)
	{
		CCNode* _child = (CCNode* )clone->getChildByTag(i);
		clone->removeChild(_child);
		pCell->addChild(_child);

		switch (i)
		{
		case 1:
			{
				//大框
				CButton* select = (CButton* )_child;
				if(data->rank == m_rankMap.find(m_optionType)->second.myRank.rank)
				{
					select->setNormalImage("worldBoss/box_rank.png");
					select->setPositionY(select->getPositionY() - 16);
				}
				else if (data->rank > 3)
				{
					select->setNormalImage("worldBoss/box_rank3.png");
				}
				select->setOnClickListener(this,ccw_click_selector(CRankLayer::onSelect));
			}
			break;
		case 2:
			{
				//头像框
				if (data->roleData->getThumb() > 0)
				{
					CCSprite* _head = CCSprite::create(CCString::createWithFormat("headImg/%d.png",data->roleData->getThumb())->getCString());
					if (_head)
					{
						_head->setPosition(ccp(_child->getContentSize().width/2, _child->getContentSize().height/2));
						_child->addChild(_head);
					}
				}
				else
				{
					string fbName = data->roleData->getFbId() + ".jpg";
					string fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(fbName.c_str());
					bool isFileExist = CCFileUtils::sharedFileUtils()->isFileExist(fbName);
					if (isFileExist)
					{
						CCSprite* _sp = CCSprite::create(fullName.c_str());
						_child->addChild(_sp);
						NodeFillParent(_sp);
					}
					else
					{
						CCString *_imgUrl = CCString::createWithFormat(FACEBOOKIMG,data->roleData->getFbId().c_str());
						HttpLoadImage::getInstance()->requestUrlImage(_imgUrl->getCString(),data->roleData->getFbId().c_str());
					}
				}
			}
			break;
		case 3:
			{
				//名字
				CLabel* _lab = (CLabel*)_child;
				_lab->setString(data->roleData->getRoleName().c_str());
			}
			break;
		case 4:
			{
				//等级
				CLabel* _lab = (CLabel*)_child;
				_lab->setString(ToString(data->roleData->getLevel()));
			}
			break;
		case 5://头像框
			break;
		case 6://红色图片-上面的字
			break;
		case 7://红色图片
			break;
		case 8:
			{
				//排名前三图标显示
				if (data->rank <= 3 && data->rank > 0)
				{
					CImageView* _view = (CImageView* )_child;
					_view->setTexture(CCTextureCache::sharedTextureCache()
						->addImage(CCString::createWithFormat("worldBoss/rank_%d.png",data->rank)->getCString()));
				}
				_child->setVisible(data->rank <= 3 && data->rank > 0);
			}
			break;
		case 9:
			{
				//排名
				CLabelAtlas* _lab = (CLabelAtlas* )_child;
				_lab->setString(ToString(data->rank));
				_lab->setVisible(data->rank > 0);
			}
			break;
		case 10:
			{
				//层数显示
				CLabelAtlas* _lab =  (CLabelAtlas*)_child;
				_lab->setString(ToString(data->roleData->getCombat()));
				_lab->setAnchorPoint(ccp(0,0.5));
			}
			break;
		case 11://Lv.
			break;
		case 12:
			{
				//未上榜
				_child->setVisible(data->rank == 0);
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

void CRankLayer::imageLoadSuccessCallBack(string sTag, vector<char>* pBuffer)
{
	CCImage* img = new CCImage;
	img->initWithImageData((unsigned char*)pBuffer->data(), pBuffer->size());
	CCTexture2D* texture = new CCTexture2D();
	texture->initWithImage(img);

	string path = HttpLoadImage::getInstance()->getStoragePath("download/fbImg",sTag.c_str())+".jpg";
	string buff(pBuffer->begin(), pBuffer->end());
	CCLOG("path: %s", path.c_str());
	FILE *fp = fopen(path.c_str(), "wb+");
	fwrite(buff.c_str(), 1, pBuffer->size(),  fp);
	fclose(fp);

	map<int,CRankResponse>::iterator iter = m_rankMap.find(m_optionType);	
	CTableViewCell *pCell = nullptr;
	int nIdx =-1;
	if (iter!=m_rankMap.end())
	{
		for (int i = 0; i < iter->second.rankList.size(); i++)
		{
			if (iter->second.rankList.at(i).roleData->getFbId()==sTag)
			{
				nIdx = i;
				break;
			}
		}
	}
	if (nIdx!=-1)
	{
		pCell = m_tableView->cellAtIndex(nIdx);
	}
	CCSprite *headImg = CCSprite::createWithTexture(texture);
	if (pCell&&headImg)
	{
		CCNode *node = pCell->getChildByTag(2);
		node->addChild(headImg);
		NodeFillParent(headImg);
	}	
	img->release();
}