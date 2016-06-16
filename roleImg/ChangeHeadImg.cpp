#include "ChangeHeadImg.h"
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
#include "common/CommonFunction.h"

#include "common/CGameSound.h"
#include "Resources.h"

CChangeHeadImg::CChangeHeadImg()
{

}

bool CChangeHeadImg::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("ChangeHeadImg");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		lay->setVisible(false);

		m_ui = LoadComponent("ChangeHeadImg.xaml");  
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CChangeHeadImg::onChangHeadImg(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CImageView *head = (CImageView *)pSender;
	int *icon = (int*)head->getUserData();
	CPlayerControl::getInstance().sendChangeHeadImg(*icon);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CChangeHeadImg::onEnter()
{
	BaseLayer::onEnter();

	////退出
	//CButton* pClose = CButton::create("common/back.png", "common/back.png");
	//pClose->getSelectedImage()->setScale(1.1f);
	//pClose->setPosition(VLEFT+50, VTOP-50);
	//pClose->setOnClickListener(this,ccw_click_selector(CChangeHeadImg::onClose));
	//this->addChild(pClose, 999);

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(5);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CChangeHeadImg::gridviewDataSource));
	m_gridView->reloadData();

	GetTcpNet->registerMsgHandler(HeadImgListMsg,this, CMsgHandler_selector(CChangeHeadImg::ProcessMsg));

	NOTIFICATION->postNotification(HIDE_ROLE_INFO);
}

void CChangeHeadImg::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}


CCObject* CChangeHeadImg::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	if (uIdx<m_headIconList.size())
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addGridCell(uIdx,pCell);
	}
	return pCell;
}

void CChangeHeadImg::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	for (int i=1; i<=2; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView* pHead = CImageView::create(CCString::createWithFormat("headImg/%d.png", m_headIconList.at(uIdx))->getCString());
			if(!pHead)
			{
				pHead = CImageView::create("headImg/null.png");
				CCLOG("ERROR_____CChangeHeadImg::addGridCell");
			}
			child->addChild(pHead);
			NodeFillParent(pHead);
		}
		else if(i==2)
		{
			CImageView* pBtn = (CImageView*)child;
			pBtn->setTouchEnabled(true);
			pBtn->setUserData(&m_headIconList.at(uIdx));
			pBtn->setOnClickListener(this,ccw_click_selector(CChangeHeadImg::onChangHeadImg));
		}
	}
}

void CChangeHeadImg::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->postNotification(SHOW_ROLE_INFO);
}

CChangeHeadImg::~CChangeHeadImg()
{

}


void CChangeHeadImg::ProcessMsg(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	RoleIcons *res = (RoleIcons*)msg;
	for (int i = 0; i < res->icons_size(); i++)
	{
		int icon = res->icons(i);
		m_headIconList.push_back(icon);
	}

    m_gridView->setCountOfCell(m_headIconList.size());
	m_gridView->reloadData();
}

bool CChangeHeadImg::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}



