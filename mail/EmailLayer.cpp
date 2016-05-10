#include "EmailLayer.h"
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
#include "common/ShaderDataHelper.h"
#include "common/CheckMoney.h"
#include "common/CommonFunction.h"
#include "tollgate/ItemInfo.h"
#include "tollgate/MonsterInfo.h"
#include "common/CShowToBuyResource.h"
#include "tools/CCShake.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "mail/EmailControl.h"
#include "mainCity/PanelTips.h"

CMailLayer::CMailLayer():
	m_ui(nullptr)
	,m_mailCell(nullptr)
	,m_pItemInfo(nullptr)
	,m_mailLay(nullptr)
	,m_tableView(nullptr)
	,m_tableMail(nullptr)
	,m_mailList(nullptr)
	,m_delegate(nullptr)
	,m_mailContentLay(nullptr)
	,m_contentCell(nullptr)
	,m_pMonsterInfo(nullptr)
{
}


CMailLayer::~CMailLayer()
{
	CC_SAFE_RELEASE(m_mailCell);
	CC_SAFE_RELEASE(m_contentCell);
}

bool CMailLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("CMailLayermask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("mail.xaml"); 
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setVisible(false);

		return true;
	}
	return false;

}

void CMailLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CMailLayer::onEnter()
{
	BaseLayer::onEnter();

	m_mailCell = (CLayout*)(m_ui->findWidgetById("Cell_mail"));
	m_mailCell->retain();
	m_ui->removeChild(m_mailCell);

	m_mailContentLay = (CLayout*)(m_ui->findWidgetById("check"));
	m_mailLay = (CLayout*)(m_ui->findWidgetById("mail"));

	CButton *read = (CButton *)(m_mailLay->findWidgetById("readAll"));
	read->setOnClickListener(this,ccw_click_selector(CMailLayer::onReadAllMail));

	m_tableView = (CTableView*)m_ui->findWidgetById("scroll_mail");
	m_tableView->setDirection(eScrollViewDirectionVertical);	
	m_tableView->setCountOfCell(0);
	m_tableView->setSizeOfCell(m_mailCell->getContentSize());
	m_tableView->setAnchorPoint(ccp(0,0));
	m_tableView->setAutoRelocate(true);
	m_tableView->setDeaccelerateable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CMailLayer::tableDataSource));
	m_tableView->reloadData();
	
	m_contentCell = (CLayout*)(m_ui->findWidgetById("Cell_Content"));
	m_contentCell->retain();
	m_ui->removeChild(m_contentCell);

	m_tableMail = (CTableView*)m_ui->findWidgetById("scroll_content");
	m_tableMail->setDirection(eScrollViewDirectionHorizontal);	
	m_tableMail->setCountOfCell(0);
	m_tableMail->setSizeOfCell(m_contentCell->getContentSize());
	m_tableMail->setAnchorPoint(ccp(0,0));
	m_tableMail->setAutoRelocate(true);
	m_tableMail->setDeaccelerateable(false);
	m_tableMail->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CMailLayer::tableviewDataSource));
	m_tableMail->reloadData();


	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CMailLayer::onClose));
	this->addChild(pClose, 999);

	CButton *ok = (CButton*)(m_mailContentLay->findWidgetById("ok"));
	ok->setOnClickListener(this,ccw_click_selector(CMailLayer::onGetMail));

	m_delegate = CEmailControl::create();
	((CEmailControl*)m_delegate)->setEmailLayer(this);
	this->addChild((CEmailControl*)m_delegate);

}


void CMailLayer::onExit()
{
	BaseLayer::onExit();

	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
}

CCObject* CMailLayer::tableDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CTableViewCell *pCell = (CTableViewCell*)pConvertCell;
	if (!pCell)
	{
		pCell = new CTableViewCell();
		pCell->autorelease();
		addEmailCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addEmailCell(uIdx,pCell);
	}
	return pCell;
}

void CMailLayer::addEmailCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CMail &mail = m_mailList->at(uIdx);

	CLayout *lay = UICloneMgr::cloneLayout(m_mailCell);

	for (int i = 1; i <= 9; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		child->setUserData((void*)&m_mailList->at(uIdx));
		if (i==1)
		{	
			child->setUserData((void*)&m_mailList->at(uIdx));
			((CButton*)child)->setOnClickListener(this,ccw_click_selector(CMailLayer::onCheckMail));
		}
		else if (i==7)
		{
		}
		else if (i==8)
		{
		}
	}

	string imgStr;
	if (mail.icon==1)
	{
		imgStr="headImg/306.png";
	}
	else if (mail.icon==2)
	{
		imgStr="headImg/111.png";
	}
	else
	{
		imgStr="headImg/311.png";
	}
	CCSprite *headimg = CCSprite::create(imgStr.c_str());
	CButton* btn= (CButton*)(pCell->getChildByTag(2));
	headimg->setScale(0.8f);
	headimg->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
	btn->addChild(headimg);

	CLabel *name = (CLabel*)(pCell->findWidgetById("name"));
	name->setString(mail.title.c_str());

	CLabel *content = (CLabel*)(pCell->findWidgetById("content"));
//	content->setString(mail.content.c_str());

	CLabel *time = (CLabel*)(pCell->findWidgetById("time"));
	time->setString(mail.addTime.c_str());

	CLabel *sender = (CLabel*)(pCell->findWidgetById("sender"));
	sender->setString(mail.sender.c_str());
}

void CMailLayer::onCheckMail(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);
	CButton *btn =(CButton*)pSender;
	if (m_delegate)
	{
		m_delegate->checkEMailCallBack((CMail*)btn->getUserData());
	}
}

void CMailLayer::onReadAllMail(CCObject* pSender)
{
	if (m_delegate)
	{
		m_delegate->extractAllCallBack();
	}
}

void CMailLayer::reloadEmailTableView(vector<CMail>* pMailList)
{
	setVisible(true);

	if(pMailList->size()==0)
	{
		CPanelTips* pPanel = CPanelTips::create();
		pPanel->setString(GETLANGSTR(1235));
		this->addChild(pPanel);
	}

	m_mailList = pMailList;
	m_tableView->setCountOfCell(m_mailList->size());
	m_tableView->reloadData();

	CButton *read = (CButton *)(m_mailLay->findWidgetById("readAll"));
	CLabel *readtext = (CLabel *)(m_mailLay->findWidgetById("readtext"));
	read->setVisible(!pMailList->empty());
	readtext->setVisible(!pMailList->empty());
}

void CMailLayer::showMailContent(const CMail &mail)
{
 	m_checkMail = mail;
	m_prizeRes = mail.prize;
	m_tableView->setVisible(false);
	m_mailContentLay->setVisible(true);
	CLabel *time = (CLabel*)(m_mailContentLay->findWidgetById("title"));
	time->setString(mail.title.c_str());

	CLabel *text = (CLabel*)(m_mailContentLay->findWidgetById("context"));
	text->setString(mail.content.c_str());

	int cellCount = m_prizeRes.prizeList.size();
	m_tableMail->setCountOfCell(cellCount);
	m_tableMail->reloadData();
}


void CMailLayer::onGetMail(CCObject* pSender)
{
	if (m_delegate)
	{
		m_delegate->extractMailCallBack(m_checkMail.mailId);
		m_mailContentLay->setVisible(false);
		m_tableView->setVisible(true);
		m_mailLay->setVisible(true);
	}
}


CCObject* CMailLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CMailLayer::addTableCell(unsigned int uIdx, CTableViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_contentCell);		

	CPrize *prize = &m_prizeRes.prizeList.at(uIdx);

	string str = GetImageName(prize->type, prize->color, prize->thumb);
	
	for (int i = 1; i <= 3; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			{
				CImageView *mask= (CImageView *)child;
				CCSprite *itemSpr = CCSprite::create(str.c_str());
				if (!itemSpr)
				{
					itemSpr = CCSprite::create("prop/32003.png");
					CCLOG("[ ERROR ] CMailLayer::addTableCell");
				}
				itemSpr->setPosition(ccp(mask->getContentSize().width/2,mask->getContentSize().height/2));
				mask->addChild(itemSpr);

				mask->setUserData(prize);
				mask->setTouchEnabled(true);
				mask->setOnPressListener(this, ccw_press_selector(CMailLayer::onPressMailPrize));
			}
			break;
		case 2:
			{
				CCSprite *mask= (CCSprite *)child;
				mask->setTexture(setItemQualityTexture(prize->color));

				//添加星星
				if(prize->quality > 0)
				{
					CLayout* pStarLayout = getStarLayout(prize->quality);
					mask->addChild(pStarLayout);
				}
			}
			break;
		case 3:
			{
				int iNum = prize->num;
				if (iNum>1)
				{
					CLabel *num = (CLabel*)child;
					num->setVisible(false);
					pCell->removeChild(num);
					CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
					haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
					haveNum->setPosition(ccp(85, 5));
					pCell->addChild(haveNum, 99);
					haveNum->setString(ToString(iNum));
				}
				else
				{
					child->setVisible(false);
				}
			}
			break;
		}
	}
}

void CMailLayer::onPressMailPrize( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	CPrize* prize = (CPrize*)pImage->getUserData();
	int iType = prize->type;

	//奖励类型(1 道具，2 英雄，3 铜钱，4 元宝，5 粮食， 6 体力)
	switch (iType)
	{
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
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