#include "CVipCard.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "common/CursorTextField.h"
#include "UserDefaultData.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "jni/CJniHelper.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"
#include "tools/UICloneMgr.h"

CVipCard::CVipCard():m_ui(nullptr),m_cardText(nullptr),m_pwdText(nullptr),m_iCardId(0),m_bStep1Success(false),m_iTimeRemain(0)
{

}

CVipCard::~CVipCard()
{

}

bool CVipCard::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("VipCard");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		m_ui = LoadComponent("VipCard.xaml");
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);

		this->setVisible(true);
		CColorView *grad = CColorView::create(ccc4(53, 53, 53,255));
		grad->setPosition(VCENTER);
		grad->setContentSize(CCSize(1138,640));
		m_ui->addChild(grad,-1);

		CColorView *view = CColorView::create(ccc4(255, 255, 255,255));
		view->setPosition(VCENTER);
		view->setContentSize(CCSize(1138,425));
		m_ui->addChild(view,-1);
		return true;
	}
	return false;
}

void CVipCard::onEnter()
{
	BaseLayer::onEnter();
	//确定
	CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
	pConfirm->setOnClickListener(this, ccw_click_selector(CVipCard::onConfirm));

	CButton* cancel = (CButton*)m_ui->findWidgetById("cancel");
	cancel->setOnClickListener(this, ccw_click_selector(CVipCard::onCancel));

	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CVipCard::onClose));
	this->addChild(pClose, 999);
	
	CImageViewScale9* pRect1 = (CImageViewScale9*)m_ui->findWidgetById("rect1");
	m_cardText = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 22, CCSize(690, 60), ccBLACK);
	m_cardText->setPriority(this->getTouchPriority());
	m_cardText->setLimitNum(17);
	m_cardText->setAnchorPoint(ccp(0, 0.5f));
	m_cardText->setPosition( ccp(pRect1->getPositionX()-370, pRect1->getPositionY()));
	m_ui->addChild(m_cardText);


	m_cell = (CLayout*)(m_ui->findWidgetById("Cell1"));

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	//m_tableView->setSizeOfCell(CCSizeMake(790,115));
	m_tableView->setCountOfCell(5);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CVipCard::tableviewDataSource));	
	m_tableView->reloadData();
}

void CVipCard::onExit()
{
	BaseLayer::onExit();	
}


void CVipCard::onConfirm( CCObject * pSender )
{
	//手机不能为空
	if(strcmp(m_cardText->getString(),"")==0)
	{
		ShowPopTextTip(GETLANGSTR(1253));
		return;
	}
	CPlayerControl::getInstance().sendBuyMoneyCard(*m_cardText->getTextInput());
    setPayEnable(false);
}

void CVipCard::onCancel( CCObject * pSender )
{
	onClose(nullptr);
}

void CVipCard::onGetSecurityCode( CCObject * pSender )
{

}

void CVipCard::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CVipCard::showErrorCode( string sErrorCode )
{
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("error_code");

}

void CVipCard::setCardId( int iCardId )
{
	m_iCardId = iCardId;
}

void CVipCard::hideErrorCode()
{
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("error_code");
	pLabel->setVisible(false);
}

void CVipCard::setStep1Success( bool bSuccess )
{
	m_bStep1Success = bSuccess;
	if(bSuccess)
	{
		CCUserDefault::sharedUserDefault()->setStringForKey(TELEPHONE, m_sTelNumber);
		CCUserDefault::sharedUserDefault()->flush();
	}
}
void CVipCard::showTimeClock()
{
	
}

bool CVipCard::getStep1Success()
{
	return m_bStep1Success;
}

void CVipCard::updateClock( float dt )
{

}

CCObject* CVipCard::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CVipCard::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *cell = (CLayout*)m_ui->findWidgetById(CCString::createWithFormat("Cell%d",uIdx+1)->getCString());
	// 	CLayout *lay = UICloneMgr::cloneLayout(cell);
	for (int i = 0; i < 4; i++)
	{
		CCNode * node = (CCNode*)cell->getChildren()->objectAtIndex(i);

		if (node->getTag()==1)
		{
			CButton *btn = CButton::createWith9Sprite(node->getContentSize(),"vip/payment_box2.png","vip/payment_box2.png");
			btn->setPosition(node->getPosition());
			btn->setAnchorPoint(ccp(0,0));
			btn->setTag(1);
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

void CVipCard::setPayEnable(bool isEnable)
{
	CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
	pConfirm->setEnabled(isEnable);
}


