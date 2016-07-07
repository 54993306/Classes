#include "VipPower.h"
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
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"

#include "common/CommonFunction.h"

#include "common/CGameSound.h"
#include "Resources.h"

CVipPower::CVipPower():m_vipLev(0)
,m_pMonsterInfo(nullptr)
,m_pItemInfo(nullptr)
{

}


bool CVipPower::init()
{
	if (BaseLayer::init())
	{
// 		MaskLayer* lay = MaskLayer::create("VipPower");
// 		lay->setContentSize(CCSizeMake(2824,640));
// 		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("VipDesc.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		this->setOpacity(0);

		return true;
	}
	return false;

}

void CVipPower::onClose(CCObject* pSender)
{
	removeFromParentAndCleanup(true);
	//LayerManager::instance()->pop();
	//LayerManager::instance()->pop();
}

void CVipPower::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);
	
	m_privlcell = (CLayout*)(m_ui->findWidgetById("pCell"));
	m_privlcell->retain();
	m_ui->removeChild(m_privlcell);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(2);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CVipPower::gridviewDataSource));
	m_gridView->reloadData();

	m_tableView = (CTableView*)m_ui->findWidgetById("privlScroll");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_privlcell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CVipPower::tableviewDataSource));
	m_tableView->reloadData();


	CButton *left= (CButton *)m_ui->findWidgetById("left");
	left->setOnClickListener(this,ccw_click_selector(CVipPower::onLeft));

	CButton *right= (CButton *)m_ui->findWidgetById("right");
	right->setOnClickListener(this,ccw_click_selector(CVipPower::onRight));

	GetTcpNet->registerMsgHandler(VipPrivMsg,this,CMsgHandler_selector(CVipPower::ProcessMsg));
}

void CVipPower::onLeft(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (m_vipLev>1)
	{
		CPlayerControl::getInstance().sendVipInfoByLevel(--m_vipLev);
		btn->setEnabled(false);
		btn->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f),CCCallFuncN::create(this,callfuncN_selector(CVipPower::enableBtn))));
	}
	
}

void CVipPower::enableBtn(CCNode *pNode)
{
	CButton *btn = (CButton*)pNode;
	btn->setEnabled(true);
}

void CVipPower::onRight(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (m_vipLev<15)
	{
		CPlayerControl::getInstance().sendVipInfoByLevel(++m_vipLev);
		btn->setEnabled(false);
		btn->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f),CCCallFuncN::create(this,callfuncN_selector(CVipPower::enableBtn))));
	}
}

CCObject* CVipPower::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	/*if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())*/
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addGridCell(uIdx,pCell);
	}
	return pCell;
}


CCObject* CVipPower::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
	/*if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())*/
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addTableCell(uIdx,pCell);
	}
	return pCell;
}

void CVipPower::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	BaseLayer::onExit();
}


void CVipPower::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	const CPrize &prize = m_vip.prizeList.at(uIdx);
	for (int i = 1; i <=3; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		switch (i)
		{
		case 1:
			{
				CImageView *head = CImageView::create(GetImageName(prize.type, prize.color, prize.thumb).c_str());
				if (!head)
				{
					head = CImageView::create("headImg/101.png");
					CCLOG("[ ERROR ] CVipPower::addGridCell Lost Image = %d",prize.thumb);
				}
				if (head)
				{
					//head->setScale(0.82f);
					head->setPosition(ccp(child->getContentSize().width/2, child->getContentSize().height/2));
					child->addChild(head);
				}
				CImageView* pPress = (CImageView*)child;
				pPress->setTag(uIdx);
				pPress->setTouchEnabled(true);
				pPress->setOnPressListener(this, ccw_press_selector(CVipPower::onPress));

				//添加星星
				if(prize.quality >0)
				{
					CLayout* pStarLayout = SmartGetStarLayout(&prize);
					child->addChild(pStarLayout);
				}
			}
			break;
		case 2:
			{
				SmartSetRectPrizeColor(((CImageView*)child), &prize);
			}
			break;
		case 3:
			{
				CLabel *label = (CLabel*)child;
				label->setAnchorPoint(ccp(1,0.5));
				if(prize.num>1)
				{
					label->setString(ToString(prize.num));
				}
				else
				{
					label->setString("");
				}
			}
			break;
		default:
			break;
		}
	}
}

void CVipPower::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_privlcell);
	const VipPriv &priv = m_vipDesc.getPrivByLevel(m_vip.level).at(uIdx);
	for (int i = 1; i <=4; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		
		switch (i)
		{
		case 2:
			{
				CLabel *label = (CLabel*)child;
				label->setString(priv.priv1.c_str());
			}
			break;
		case 3:
			{
				CLabel *label = (CLabel*)child;
				label->setString(priv.priv2.c_str());
				label->setPositionX(pCell->getChildByTag(2)->getPositionX()+pCell->getChildByTag(2)->getContentSize().width);
			}
			break;
		case 4:
			{
				CLabel *label = (CLabel*)child;
				label->setString(priv.priv3.c_str());
				label->setPositionX(pCell->getChildByTag(3)->getPositionX()+pCell->getChildByTag(3)->getContentSize().width);
			}
			break;
		default:
			break;
		}
	}
}

CVipPower::~CVipPower()
{
 	CC_SAFE_RELEASE(m_cell);
	CC_SAFE_RELEASE(m_privlcell);
}

void CVipPower::updateVipPriv(CVipInfoRes *pVipInfo)
{
	m_vip = pVipInfo->data;
	m_vipLev = pVipInfo->level;
	CLabelAtlas *viplevel = (CLabelAtlas *)m_ui->findWidgetById("level");
	viplevel->setString(ToString(m_vipLev));

	m_gridView->setCountOfCell(pVipInfo->data.prizeList.size());
	m_gridView->reloadData();
	m_tableView->setCountOfCell(m_vipDesc.getPrivByLevel(m_vip.level).size());
	m_tableView->reloadData();

	CButton *left= (CButton *)m_ui->findWidgetById("left");
	left->setVisible(m_vipLev!=1);
	CButton *right= (CButton *)m_ui->findWidgetById("right");
	right->setVisible(m_vipLev!=15);
}

void CVipPower::ProcessMsg(int type, google::protobuf::Message *msg)
{
	VipData *res = (VipData*)msg;
	m_vip.prizeList.clear();
	m_vip.read(*res);  
	m_vipLev = m_vip.level;

	CLabelAtlas *viplevel = (CLabelAtlas *)m_ui->findWidgetById("level");
	viplevel->setString(ToString(m_vipLev));

	m_gridView->setCountOfCell(m_vip.prizeList.size());
	m_gridView->reloadData();
	m_tableView->setCountOfCell(m_vipDesc.getPrivByLevel(m_vip.level).size());
	m_tableView->reloadData();

	CButton *left= (CButton *)m_ui->findWidgetById("left");
	left->setVisible(m_vipLev!=1);
	CButton *right= (CButton *)m_ui->findWidgetById("right");
	right->setVisible(m_vipLev!=15);

	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_VIPINFO,m_vipLev,nullptr,nullptr,m_vip.target);
}

void CVipPower::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CPrize* prize = (CPrize*)(&m_vip.prizeList.at(pImage->getTag()));
	int iType = prize->type;

	//1 道具，2 英雄，3 铜钱，4 元宝，5 粮食
	switch (iType)
	{
	case 3:
	case 4:
	case 5:
	case 1:
		{
			if(m_pItemInfo==nullptr)
			{
				m_pItemInfo = CItemInfo::create();
				this->addChild(m_pItemInfo);
			}

			switch (iState)
			{
			case CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					m_pItemInfo->setInfo(prize);
					m_pItemInfo->bindPos(pImage);
					m_pItemInfo->changeForSign(prize);
					m_pItemInfo->show();
					CCLOG("show");
				}
				break;
			case CTouchPressOff:
				{
					m_pItemInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	case 2:
		{
			if(m_pMonsterInfo==nullptr)
			{
				m_pMonsterInfo = CMonsterInfo::create();
				this->addChild(m_pMonsterInfo);
			}

			switch (iState)
			{
			case CTouchPressOn:
				{
					PlayEffectSound(SFX_429);

					CHero pHero;
					pHero.heroid = prize->id;
					pHero.thumb = prize->thumb;
					pHero.quality = prize->quality;
					pHero.iColor = prize->color;
					pHero.level = 1;
					m_pMonsterInfo->setHero(&pHero);
					m_pMonsterInfo->bindPos(pImage);
					m_pMonsterInfo->changeForSign(&pHero);
					m_pMonsterInfo->show();
					CCLOG("show");
				}
				break;
			case CTouchPressOff:
				{
					m_pMonsterInfo->hide();
					CCLOG("hide");
				}
				break;
			default:
				break;
			}

		}break;
	default:
		break;
	}

}


