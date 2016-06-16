#include "ChangeName.h"
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
#include "common/MultLanguage.h"
#include "common/CGameSound.h"
#include "Resources.h"

CChangeName::CChangeName()
{

}

bool CChangeName::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("ChangeName");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		lay->setVisible(false);

		m_ui = LoadComponent("ChangeName.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);
		return true;
	}
	return false;

}


void CChangeName::onChangName(CCObject* pSender)
{
	if (strcmp(m_textFieldName->getTextInput()->c_str(),""))
	{
		CPlayerControl::getInstance().sendChangeName(*(m_textFieldName->getTextInput()));
	}
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CChangeName::onEnter()
{
	BaseLayer::onEnter();

	CButton *changeName = (CButton*)(m_ui->findWidgetById("changeName"));
	changeName->setOnClickListener(this,ccw_click_selector(CChangeName::onChangName));

	m_textFieldName = CursorTextField::textFieldWithPlaceHolder(GETLANGSTR(118), FONT_NAME, 15,CCSizeMake(150,35),ccWHITE); 
	CCNode *node = (CCNode *)m_ui->findWidgetById("textBg");
	m_textFieldName->setPosition(ccp(node->getPositionX()-62,node->getPositionY()));
	m_textFieldName->setLimitNum(15);
	m_textFieldName->setPriority(this->getTouchPriority());
	m_textFieldName->setAnchorPoint(ccp(0,0.5));
	m_ui->addChild(m_textFieldName);

	CImageView *randowName = (CImageView*)(m_ui->findWidgetById("rand"));
	randowName->setTouchEnabled(true);
	randowName->setOnClickListener(this,ccw_click_selector(CChangeName::onRandowName));

	NOTIFICATION->postNotification(HIDE_ROLE_INFO);
}


void CChangeName::onExit()
{
	BaseLayer::onExit();

	NOTIFICATION->postNotification(SHOW_ROLE_INFO);
}


CChangeName::~CChangeName()
{

}

void CChangeName::onRandowName(CCObject* pSender)
{
	string name = m_nameGm.getRandowName();
	m_textFieldName->setTextInput(name);
}

bool CChangeName::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch,pEvent);

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		PlayEffectSound(SFX_Ensure);
		LayerManager::instance()->pop();
		LayerManager::instance()->pop();
	}
	return res;
}

// void CChangeName::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
// {
// 	CWidgetWindow::ccTouchEnded(pTouch,pEvent);
// 	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
// 	if( !res && bgSpr->boundingBox().containsPoint(pTouch->getLocation()))
// 	{
