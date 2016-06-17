#include "SelectFriend.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "Battle/AnimationManager.h"
#include "common/color.h"

#include "tools/StringUtil.h"
#include "SelectFriend.h"
#include "SelectArmy.h"

#include "common/CGameSound.h"
#include "Resources.h"

CSelectFirend::~CSelectFirend()
{
	CC_SAFE_RELEASE(m_cell);
}


bool CSelectFirend::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("SelectFriendmask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("SelectFriend.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		m_selectType = 1;
		return true;
	}
	return false;
}

void CSelectFirend::onEnter()
{ 
	BaseLayer::onEnter();

	CButton *btn= (CButton *)m_ui->findWidgetById("close");
	btn->setOnClickListener(this,ccw_click_selector(CSelectFirend::onClose));

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_friendTable = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_friendTable->setDirection(eScrollViewDirectionVertical);
	m_friendTable->setSizeOfCell(m_cell->getContentSize());
	m_friendTable->setCountOfCell(0);
	m_friendTable->setBounceable(false);
	m_friendTable->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CSelectFirend::friendDataSource));
	m_friendTable->reloadData();
	GetTcpNet->registerMsgHandler(FriendListMsg,this,CMsgHandler_selector(CSelectFirend::friendListRes));
	GetTcpNet->registerMsgHandler(UnionRes,this,CMsgHandler_selector(CSelectFirend::processNetMsg));
}


CCObject* CSelectFirend::friendDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}


void CSelectFirend::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CSelectFirend::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver("updateHero",this);
	BaseLayer::onExit();
}


void CSelectFirend::onAddFriend(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	m_selectFriend = (CFriend*)btn->getUserData();

	if (m_unionList.size()>0)
	{
		CSelectArmy *selArmy = CSelectArmy::create();
		LayerManager::instance()->push(selArmy);
		CPlayerControl::getInstance().sendHeroList(0,0,m_selectFriend->friendId);
		selArmy->setUnionList(m_unionList);
	}
	else
	{
		GetTcpNet->sendDataType(UnionRes);
	}
	
}

void CSelectFirend::processNetMsg(int type, google::protobuf::Message *msg)
{
	UnionResponse *res = (UnionResponse*)msg;
	
	CSelectArmy *selArmy = CSelectArmy::create();
	LayerManager::instance()->push(selArmy);
	CPlayerControl::getInstance().sendHeroList(0,0,m_selectFriend->friendId);
	selArmy->setUnionList(m_unionList);
// 	selArmy->setStageFriendId(m_stagId,m_selectFriend->friendId);
}

int CSelectFirend::getCombatLevel(int combat)
{
	int level = 0;
	if (combat>12000)
	{
		level = 1;
	}
	else if (combat>10000)
	{
		level = 2;
	}
	else if (combat>8000)
	{
		level = 3;
	}
	else if (combat>6000)
	{
		level = 4;
	}
	else if (combat>5000)
	{
		level = 5;
	}
	else if (combat>4000)
	{
		level = 6;
	}
	else if (combat>3000)
	{
		level = 7;
	}
	else if (combat>2000)
	{
		level = 8;
	}
	else 
	{
		level = 9;
	}
	return level;
}

void CSelectFirend::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	CFriend *frd = &m_friendList.at(uIdx);
	for (int i=1;i<=11;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		//好友头像，名称，等级，友情点
		switch (i)
		{
		case 1:
			break;
		case 2:
			{
				CCSprite *spr = CCSprite::create(CCString::createWithFormat("headImg/%d.png", frd->thumb)->getCString());
				if (!spr)
				{
					spr = CCSprite::create("headImg/101.png");
					CCLOG("[ ERROR ] CSelectFirend::addTableCell Lost Image = %d",frd->thumb);
				}
				if (spr)
				{
					child->addChild(spr);
					spr->setScale(0.8f);
					spr->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
				}
			}
			break;
		case 3:
			{
				((CLabel*)child)->setString(frd->friendName.c_str());
			}
			break;
		case 4:
			{
				((CLabel*)child)->setString(CCString::createWithFormat("Lv.%d",frd->level)->getCString());
			}
			break;
		case 6:
			{

			}
			break;
		case 7:
			{
				((CLabel*)child)->setString(ToString(frd->fcoin));
			}
			break;
		case 8:
			{
				CButton *btn = (CButton*)child;
				btn->setUserData(&m_friendList.at(uIdx));	
				btn->setOnClickListener(this,ccw_click_selector(CSelectFirend::onAddFriend));
			}
			break;
		//战斗力
		case 9:
			{
				int batlev = getCombatLevel(frd->combat);
				CImageView *img = (CImageView*)child;
				if (batlev<=3)
				{
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage("warPreview/combat_1.png"));
				}
				else
				{
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warPreview/combat_%d.png",batlev)->getCString()));
				}
			}
			break;
		case 10:
			{
				int batlev = getCombatLevel(frd->combat);
				CImageView *img = (CImageView*)child;
				if (batlev<=3)
				{
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage("warPreview/level_1.png"));
					if (batlev==1)
					{
						CImageView *clone = UICloneMgr::cloneImageView(img);
						clone->setPosition(ccp(img->getPositionX()+img->getContentSize().width*0.8,img->getPositionY()));
						CImageView *clone1 = UICloneMgr::cloneImageView(img);
						clone1->setPosition(ccp(img->getPositionX()+img->getContentSize().width*1.6,img->getPositionY()));
					    pCell->addChild(clone);
						pCell->addChild(clone1);
					}
				}
				else
				{
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("warPreview/level_%d.png",batlev)->getCString()));
				}
			}
			break;
		default:
			break;
		}
	}
}

void CSelectFirend::friendListRes(int type, google::protobuf::Message *msg)
{
	FriendListResponse *res = (FriendListResponse*)msg;
	for (int i = 0; i < res->friends_size(); i++)
	{
		CFriend frd;
		frd.read(res->friends(i));
		m_friendList.push_back(frd);
	}
	m_friendTable->setCountOfCell(m_friendList.size());
	m_friendTable->reloadData();
}

void CSelectFirend::setStagId(int stageId)
{
	m_stagId = stageId;
}
