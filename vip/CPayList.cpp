#include "CPayList.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "common/CursorTextField.h"
#include "UserDefaultData.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "jni/CJniHelper.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "GMessage.h"
#include "model/DataDefine.h"
#include "CVipPay.h"
#include "CVipCard.h"
#include "tools/UICloneMgr.h"
#include "SDK/PayDefine.h"

CPayList::CPayList():m_ui(nullptr),m_payType(PayListTypeDefault),m_iIndexFlag(-1)
{

}

CPayList::~CPayList()
{

}

bool CPayList::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("PaySelect");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		//其他放到onEnter里去

		return true;
	}
	return false;
}

void CPayList::onEnter()
{
	BaseLayer::onEnter();
	initUI();
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);
}


void CPayList::initUI()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	string strXaml = (m_payType==PayListTypeGoogle?"GooglePayList.xaml":"PayList.xaml");

	m_ui = LoadComponent(strXaml.c_str());
	m_ui->setTag(1);
	m_ui->setPosition(VCENTER);
	this->addChild(m_ui, 2);

	CColorView *grad = CColorView::create(ccc4(53, 53, 53,255));
	grad->setPosition(VCENTER);
	grad->setContentSize(CCSize(1138,640));
	m_ui->addChild(grad,-1);

	CColorView *view = CColorView::create(ccc4(255, 255, 255,255));
	view->setPosition(VCENTER);
	view->setContentSize(CCSize(1138,400));
	m_ui->addChild(view,-1);

	this->setVisible(true);

	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CPayList::onClose));
	this->addChild(pClose, 999);


	m_cell = (CLayout*)(m_ui->findWidgetById("Cell1"));
	// 	m_cell->retain();
	// 	m_ui->removeChild(m_cell);

	m_priceVec.push_back(50);
	m_priceVec.push_back(100);
	m_priceVec.push_back(300);
	m_priceVec.push_back(500);
	m_priceVec.push_back(1000);

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	//m_tableView->setSizeOfCell(CCSizeMake(790,115));
	m_tableView->setCountOfCell(5);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CPayList::tableviewDataSource));	
	m_tableView->reloadData();

}


void CPayList::onExit()
{
	BaseLayer::onExit();	

	NOTIFICATION->postNotification(SHOW_TOP_LAYER);
}

CCObject* CPayList::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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


void CPayList::onPhonePay( CCObject * pSender )
{
	CButton *btn = (CButton*)pSender;
	CLabel *name = (CLabel *)btn->getParent()->getChildByTag(2);
	CLabel *desc = (CLabel *)btn->getParent()->getChildByTag(3);

	int *price = (int*)(btn->getUserData());

	switch (m_payType)
	{
	case PayListTypeDefault:
		{
			CVipPay *phone = CVipPay::create();
			LayerManager::instance()->push(phone);
			
			phone->setPayItem(name->getString(),desc->getString(),*price);
		}
		break;
	case PayListTypeGoogle:
		{
			if(m_iIndexFlag == -1)
			{
				int iIndex = btn->getParent()->getTag();
				CCLOG("[ warning ] : PayListTypeGoogle--iIndex--%d", iIndex);
				m_iIndexFlag = iIndex;
				scheduleOnce(schedule_selector(CPayList::updateForBuyForGoogle), 0.1f);
			}
			
		}
		break;
	default:
		break;
	}

}

void CPayList::onCardPay( CCObject * pSender )
{
// 	onClose(nullptr);
	CVipCard *card = CVipCard::create();
	LayerManager::instance()->push(card);
}

void CPayList::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPayList::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *cell = (CLayout*)m_ui->findWidgetById(CCString::createWithFormat("Cell%d",uIdx+1)->getCString());
// 	CLayout *lay = UICloneMgr::cloneLayout(cell);
	pCell->setTag(uIdx);

	for (int i = 0; i < 4; i++)
	{
		CCNode * node = (CCNode*)cell->getChildren()->objectAtIndex(i);

		if (node->getTag()==1)
		{
			CButton *btn = CButton::createWith9Sprite(node->getContentSize(),"vip/payment_box2.png","vip/ais_box_on.jpg");
			btn->setPosition(node->getPosition());
			btn->setAnchorPoint(ccp(0,0));
			btn->setTag(1);
			btn->setOnClickListener(this,ccw_click_selector(CPayList::onPhonePay));
			btn->setUserData(&m_priceVec.at(uIdx));
			pCell->addChild(btn);
		}
		else if (node->getTag()==2||node->getTag()==3)
		{
			CLabel *lab = UICloneMgr::cloneLable((CLabel*)node);
			pCell->addChild(lab);
		}
		else
		{
			CImageView *image = UICloneMgr::cloneImageView((CImageView*)node);
			pCell->addChild(image);
		}
	}
	cell->setVisible(false);
}

void CPayList::updateForBuyForGoogle( float dt )
{
	if(m_iIndexFlag != -1)
	{
		GooglePaySDK::getInstance()->pay(G_PAY, (GAME_PRODUCT)(m_iIndexFlag), m_iIndexFlag);
		m_iIndexFlag = -1;
	}
}
