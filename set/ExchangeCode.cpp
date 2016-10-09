#include "ExchangeCode.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "common/MultLanguage.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "SetLayer.h"
#include "bag/bagData.h"
#include "model/DataCenter.h"
#include "common/CursorTextField.h"

#include "model/RoleNameGM.h"

CExchangeCode::CExchangeCode()
{

}

bool CExchangeCode::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("ExchangecodeMask");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		lay->setVisible(false);
		
		this->setIsShowBlack(false);
		this->setVisible(true);


		return true;
	}
	return false;

}
void CExchangeCode::loadExchangeByType(int type)
{
	m_type = type;
	if (type==ExCode)
	{
		m_ui = LoadComponent("ExchangeCode.xaml");  
		m_ui->setPosition(VCENTER);
	}
	else
	{
		m_ui = LoadComponent("ExchangeReward.xaml");  
		m_ui->setPosition(VCENTER);
	}
	this->addChild(m_ui);
}

void CExchangeCode::onExchange(CCObject* pSender)
{
	if (m_type==ExCode)
	{
		if (strcmp(m_textFieldName->getTextInput()->c_str(),"")==0)
		{
			ShowPopTextTip(GETLANGSTR(1176));
			return;
		}
		else
		{  
			CPlayerControl::getInstance().sendExchangeCode(m_textFieldName->getTextInput()->c_str());
		} 
	}
	else if (m_type == CBCode)
	{
		if (m_textFieldName->getTextInput()&&strcmp(m_textFieldName->getTextInput()->c_str(),"")!=0
			&&m_textFieldPwd->getTextInput()&&strcmp(m_textFieldPwd->getTextInput()->c_str(),"")!=0)
		{
			CPlayerControl::getInstance().sendCbExchange(m_textFieldName->getTextInput()->c_str(),m_textFieldPwd->getTextInput()->c_str());
		}
		else
		{
			ShowPopTextTip(GETLANGSTR(1176));
			return;
		}
	}
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CExchangeCode::onEnter()
{
	BaseLayer::onEnter();
	m_textFieldName = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 20, CCSizeMake(300,35),ccWHITE); 
	CCNode *node = (CCNode *)m_ui->findWidgetById("input");
	m_textFieldName->setPosition(ccp(node->getPositionX()-106,node->getPositionY()));
	m_textFieldName->setLimitNum(20);
	m_textFieldName->setPriority(this->getTouchPriority());
	m_textFieldName->setAnchorPoint(ccp(0,0.5));
	m_ui->addChild(m_textFieldName);

	CButton *ok = (CButton*)(m_ui->findWidgetById("ok"));
	ok->setOnClickListener(this,ccw_click_selector(CExchangeCode::onExchange));
	//CButton *cancel = (CButton*)(m_ui->findWidgetById("cancel"));
	//cancel->setOnClickListener(this,ccw_click_selector(CExchangeCode::onClose));	


	////退出
	//CButton* pClose = CButton::create("common/back.png", "common/back.png");
	//pClose->getSelectedImage()->setScale(1.1f);
	//pClose->setPosition(VLEFT+50, VTOP-50);
	//pClose->setOnClickListener(this,ccw_click_selector(CExchangeCode::onClose));
	//this->addChild(pClose, 999);

	if (m_type==2)
	{
		m_textFieldPwd = CursorTextField::textFieldWithPlaceHolder("", FONT_NAME, 20, CCSizeMake(300,35),ccWHITE); 
		CCNode *node = (CCNode *)m_ui->findWidgetById("input1");
		m_textFieldPwd->setPosition(ccp(node->getPositionX()-106,node->getPositionY()));
		m_textFieldPwd->setLimitNum(20);
		m_textFieldPwd->setPriority(this->getTouchPriority());
		m_textFieldPwd->setAnchorPoint(ccp(0,0.5));
		m_ui->addChild(m_textFieldPwd);
	}
	
	NOTIFICATION->postNotification(SET_LAYER_HIDE);

}

void CExchangeCode::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CExchangeCode::onExit()
{
	BaseLayer::onExit();
	NOTIFICATION->postNotification(SET_LAYER_SHOW);
	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
}


CExchangeCode::~CExchangeCode()
{

}

bool CExchangeCode::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
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



