/******************************************************
*文件名称:	HandBookHero.cpp
*编写日期:	2016-8-23-16:03
*编写作者:	YPF
*功能描述:	英雄图鉴
*******************************************************/

#include "HandBookEffectTip.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "tools/UICloneMgr.h"

CHandBookEffectTip::CHandBookEffectTip():
m_ui(nullptr), m_pTableView(nullptr), m_pCell(nullptr)
{

}

CHandBookEffectTip::~CHandBookEffectTip()
{
	CC_SAFE_RELEASE(m_pCell);
}

bool CHandBookEffectTip::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CHandBookEffectTipMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("handBookEffectTip.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);


		//保存Cell
		m_pCell = (CLayout*)findWidgetById("Cell");
		m_pCell->retain();
		m_pCell->removeFromParent();


		//获取空的滑动列表Scroll
		m_pTableView = (CTableView*)m_ui->findWidgetById("scroll");
		m_pTableView->setDirection(eScrollViewDirectionVertical);
		m_pTableView->setSizeOfCell(m_pCell->getContentSize());
		m_pTableView->setCountOfCell(0);
		m_pTableView->setBounceable(true);
		m_pTableView->setDataSourceAdapter(this, ccw_datasource_adapter_selector(CHandBookEffectTip::tableviewDataSource));


		//退出
		CButton* pClose = CButton::create("common/back.png", "common/back.png");
		pClose->getSelectedImage()->setScale(1.1f);
		pClose->setPosition(VLEFT+50, VTOP-50);
		pClose->setOnClickListener(this,ccw_click_selector(CHandBookEffectTip::onClose));
		this->addChild(pClose, 999);


		setIsShowBlack(false);
		setVisible(true);

		m_pTableView->setCountOfCell(10);
		m_pTableView->reloadData();

		return true;
	}

	return false;
}

void CHandBookEffectTip::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(BossPrizesMsg, this, CMsgHandler_selector(CHandBookEffectTip::processNetMsg));
}

void CHandBookEffectTip::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CHandBookEffectTip::processNetMsg( int type, google::protobuf::Message *msg )
{
	if(!isVisible())
	{
		this->setVisible(true);
	}

	updateUI();
	
}

void CHandBookEffectTip::updateUI()
{
	m_pTableView->setCountOfCell(5);
	m_pTableView->reloadData();
}

CCObject* CHandBookEffectTip::tableviewDataSource( CCObject* pConvertCell, unsigned int uIdx )
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

void CHandBookEffectTip::addTableCell( unsigned int uIdx, CTableViewCell * pCell )
{
	//先克隆外框条
	CLayout *pLay = UICloneMgr::cloneLayout( m_pCell );

	for (unsigned int i=1; i<=5; i++)
	{
		CCNode *pChild = pLay->getChildByTag(i);
		pLay->removeChild(pChild);
		pChild->setTag(i);
		pCell->addChild(pChild);
		pCell->setTag(uIdx);
	}
}


void CHandBookEffectTip::onClickBlank( CCObject *pSender )
{
	//TODO

}

void CHandBookEffectTip::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();

}
