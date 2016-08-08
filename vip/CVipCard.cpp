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
		MaskLayer* pMaskLayer = MaskLayer::create("VipCardMask");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);
		pMaskLayer->setOpacity(255);
		pMaskLayer->setIsShowBlack(true);

		this->setVisible(true);

		m_ui = LoadComponent("VipCard.xaml");
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);


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
	m_cardText = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 29, CCSize(690, 110), ccBLACK);
	m_cardText->setPriority(this->getTouchPriority());
	m_cardText->setLimitNum(17);
	m_cardText->setAnchorPoint(ccp(0, 0.5f));
	m_cardText->setPosition( ccp(pRect1->getPositionX()-440, pRect1->getPositionY()));
	m_ui->addChild(m_cardText, 999);

	//展示区图片
	CLayout *pShowInfo = CLayout::create();

	CScrollView *pScroll = (CScrollView*)m_ui->findWidgetById("scroll_info");
	pScroll->setDirection(eScrollViewDirectionVertical);
	pScroll->setBounceable(false);
	pScroll->getContainer()->addChild(pShowInfo);

	CCSize size = CCSize(957, 471);
	pShowInfo->setContentSize(size);
	pScroll->setContainerSize(size);
	pShowInfo->setPosition(ccp(pScroll->getContainerSize().width*0.5f, pScroll->getContainerSize().height*0.5f));
	pScroll->setContentOffsetToTop();

	//嫁接内容
	CLayout* pLayout = (CLayout*)findWidgetById("layer_info");
	pLayout->retain();
	pLayout->removeFromParent();
	pShowInfo->addChild(pLayout);
	pLayout->release();
	pLayout->setPosition(ccp(size.width/2, size.height/2+10));

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

void CVipCard::setPayEnable(bool isEnable)
{
	CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
	pConfirm->setEnabled(isEnable);
}


