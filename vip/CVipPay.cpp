#include "CVipPay.h"
#include "Global.h"
#include "common/CGameSound.h"
#include "common/CursorTextField.h"
#include "UserDefaultData.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "jni/CJniHelper.h"
#include "netcontrol/CPlayerControl.h"
#include "common/ShaderDataHelper.h"

CVipPay::CVipPay():m_ui(nullptr),m_pTelInput(nullptr),m_pCodeInput(nullptr),m_iCardId(0),m_bStep1Success(false),m_iTimeRemain(0)
{

}

CVipPay::~CVipPay()
{

}

bool CVipPay::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CVipPaymask");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer, true);

		m_ui = LoadComponent("PayBox.xaml");
		m_ui->setTag(1);
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui, 2);

		this->setVisible(true);

		return true;
	}
	return false;
}

void CVipPay::onEnter()
{
	BaseLayer::onEnter();


	//确定
	CButton* pConfirm = (CButton*)m_ui->findWidgetById("confirm");
	pConfirm->setOnClickListener(this, ccw_click_selector(CVipPay::onConfirm));

	//取消
	CButton* pCancel = (CButton*)m_ui->findWidgetById("cancel");
	pCancel->setOnClickListener(this, ccw_click_selector(CVipPay::onCancel));

	//获取验证码
	CButton* pGetSercurityCode = (CButton*)m_ui->findWidgetById("get_code");
	pGetSercurityCode->setOnClickListener(this, ccw_click_selector(CVipPay::onGetSecurityCode));

	//读取上一次的机器ID
	//获取不到就从安卓端读取
	//安卓端也读取不到就报错吧
	m_sDeviceId = CCUserDefault::sharedUserDefault()->getStringForKey(DEVICE_ID, "");
	if(m_sDeviceId.compare("")==0)
	{
		m_sDeviceId = CJniHelper::getInstance()->getDeviceId();
		if(m_sDeviceId.compare("")==0)
		{
			//onClose(nullptr);
			//ShowPopTextTip(GETLANGSTR(1174));
			//return;
		}
	}
	//存下机器码
	CCUserDefault::sharedUserDefault()->setStringForKey(DEVICE_ID, m_sDeviceId);
	CCUserDefault::sharedUserDefault()->flush();

	//读取上一次支付成功的手机号
	string sTel = CCUserDefault::sharedUserDefault()->getStringForKey(TELEPHONE, "");

	CImageViewScale9* pRect1 = (CImageViewScale9*)m_ui->findWidgetById("rect_1");
	m_pTelInput = CursorTextField::textFieldWithPlaceHolder(sTel.c_str(), FONT_NAME, 18, CCSize(220, 40), ccWHITE);
	m_pTelInput->setPriority(-20);
	m_pTelInput->setTextInput(sTel);
	m_pTelInput->setLimitNum(17);
	m_pTelInput->setAnchorPoint(ccp(0, 0.5f));
	m_pTelInput->setPosition( ccp(pRect1->getPositionX()-90, pRect1->getPositionY()));
	m_ui->addChild(m_pTelInput);


	//验证码
	CImageViewScale9* pRect2 = (CImageViewScale9*)m_ui->findWidgetById("rect_2");
	m_pCodeInput = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 18, CCSize(120, 40), ccWHITE);
	m_pCodeInput->setPriority(-20);
	m_pCodeInput->setTextInput("");
	m_pCodeInput->setPosition(ccp(pRect2->getPositionX()-46, pRect2->getPositionY()));
	m_pCodeInput->setLimitNum(10);
	m_pCodeInput->setAnchorPoint(ccp(0, 0.5f));
	m_ui->addChild(m_pCodeInput);
}

void CVipPay::onExit()
{
	BaseLayer::onExit();	
}


void CVipPay::onConfirm( CCObject * pSender )
{
	//手机不能为空
	if(strcmp(m_pTelInput->getString(),"")==0)
	{
		ShowPopTextTip(GETLANGSTR(1175));
		return;
	}

	//验证码不能为空
	if(strcmp(m_pCodeInput->getString(),"")==0)
	{
		ShowPopTextTip(GETLANGSTR(1176));
		return;
	}

	//第一步验证是否成功
	if(!m_bStep1Success)
	{
		ShowPopTextTip(GETLANGSTR(170));
		return;
	}

	hideErrorCode();

	CCLOG("sms%s", m_pCodeInput->getString());
	//请求支付
	CPlayerControl::getInstance().sendBuyCardStep2(m_pCodeInput->getString());
}

void CVipPay::onCancel( CCObject * pSender )
{
	onClose(nullptr);
}

void CVipPay::onGetSecurityCode( CCObject * pSender )
{
	hideErrorCode();
	m_sTelNumber = m_pTelInput->getString();
	if(m_sTelNumber.compare("")==0)
	{
		ShowPopTextTip(GETLANGSTR(1175));
		return;
	}
	CCLOG("CARD:%d", m_iCardId);
	CCLOG("DEVICE:%s", m_sDeviceId.c_str());
	CCLOG("TELEPHONE:%s", m_sTelNumber.c_str());
	CPlayerControl::getInstance().sendBuyCardStep1(m_iCardId, m_sDeviceId, m_sTelNumber);
	disEnableSmsCodeBtn();
}

void CVipPay::onClose( CCObject * pSender )
{
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CVipPay::showErrorCode( string sErrorCode )
{
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("error_code");
	pLabel->setVisible(true);
	pLabel->setString(CCString::createWithFormat("Error Code: %s", sErrorCode.c_str())->getCString());
}

void CVipPay::setCardId( int iCardId )
{
	m_iCardId = iCardId;
}

void CVipPay::hideErrorCode()
{
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("error_code");
	pLabel->setVisible(false);
}

void CVipPay::setStep1Success( bool bSuccess )
{
	m_bStep1Success = bSuccess;
	if(bSuccess)
	{
		CCUserDefault::sharedUserDefault()->setStringForKey(TELEPHONE, m_sTelNumber);
		CCUserDefault::sharedUserDefault()->flush();
	}
}
void CVipPay::showTimeClock()
{
	CCNode* pBtn = (CCNode*)m_ui->findWidgetById("get_code");
	pBtn->setVisible(false);
	CCNode* pLabel = (CCNode*)m_ui->findWidgetById("time_delay");
	pLabel->setVisible(true);
	m_iTimeRemain = 60;
	schedule(schedule_selector(CVipPay::updateClock), 1.0f);
}

bool CVipPay::getStep1Success()
{
	return m_bStep1Success;
}

void CVipPay::updateClock( float dt )
{
	m_iTimeRemain--;
	if(m_iTimeRemain<0)
	{
		hideTimeClock();
	}
	else
	{
		CLabel* pLabel = (CLabel*)m_ui->findWidgetById("time_delay");
		pLabel->setString(ToString(m_iTimeRemain));
	}
}

void CVipPay::hideTimeClock()
{
	CCNode* pBtn = (CCNode*)m_ui->findWidgetById("get_code");
	pBtn->setVisible(true);
	CCNode* pLabel = (CCNode*)m_ui->findWidgetById("time_delay");
	pLabel->setVisible(false);
	unschedule(schedule_selector(CVipPay::updateClock));
}

void CVipPay::enableSmsCodeBtn()
{
	CButton* pBtn = (CButton*)m_ui->findWidgetById("get_code");
	pBtn->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderDefault));
	pBtn->setEnabled(true);
}

void CVipPay::disEnableSmsCodeBtn()
{
	CButton* pBtn = (CButton*)m_ui->findWidgetById("get_code");
	pBtn->getNormalImage()->setShaderProgram(ShaderDataMgr->getShaderByType(ShaderStone));
	pBtn->setEnabled(false);
}

