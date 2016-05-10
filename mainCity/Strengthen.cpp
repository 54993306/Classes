#include "Strengthen.h"
#include "net/CNetClient.h"
#include "mainCity/LevelUpItem.h"
#include "mainCity/SmeltArmor.h"
#include "mainCity/StarUpItem.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "task/TaskControl.h"

CStrengthen::CStrengthen():m_pUi(nullptr),
	m_pLevelUpLay(nullptr),
	m_pSmeltLay(nullptr),
	m_pStarUpLay(nullptr),
	m_iType(StrengthenLevelUp)
{

}

CStrengthen::~CStrengthen()
{

}

bool CStrengthen::init()
{
	if (BaseLayer::init())
	{
		m_pUi = LoadComponent("Strengthen.xaml");
		m_pUi->setPosition(VCENTER);
		this->addChild(m_pUi, 10);

		//升级层 
		m_pLevelUpLay = CLevelUpItem::create();
		int iPriority = LayerManager::instance()->getCurrLayer()->getTouchPriority()-1;
		m_pLevelUpLay->setTouchPriority(iPriority);
		this->addChild(m_pLevelUpLay);

		//洗练层


		//升星层


		setCoverScreen(true);

		setVisible(true);

		return true;
	}
	return false;
}



void CStrengthen::onEnter()
{ 
	BaseLayer::onEnter();
	
	PlayEffectSound(SFX_427);

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_pUi->getChildByTag(10);
	for (int i = 0; i < 3; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this, ccw_check_selector(CStrengthen::onSwitchBtn));
	}

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_pUi->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("close");
	pClose->setOnClickListener(this,ccw_click_selector(CStrengthen::onClose));
	m_pUi->addChild(pClose, 999);
	NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
}


void CStrengthen::onExit()
{
	BaseLayer::onExit();

	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);

	//显示被遮盖的层，渲染
	LayerManager::instance()->showLayerUnBeCovered(this);

	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CStrengthen::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;
	int selIndex = 2*btn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		if(m_pLevelUpLay)	m_pLevelUpLay->setVisible(false);
		if(m_pSmeltLay)		m_pSmeltLay->setVisible(false);
		if(m_pStarUpLay)		m_pStarUpLay->setVisible(false);

		int iTag = btn->getTag();
		m_iType = (StrengthenType)iTag;
		switch (m_iType)
		{
			//强化
		case StrengthenLevelUp:
			{
				if(!m_pLevelUpLay)
				{
					m_pLevelUpLay = CLevelUpItem::create();
					int iPriority = LayerManager::instance()->getCurrLayer()->getTouchPriority()-1;
					m_pLevelUpLay->setTouchPriority(iPriority);
					this->addChild(m_pLevelUpLay);
				}
				m_pLevelUpLay->setVisible(true);

			}break;
			//洗练
		case StrengthenSmelt:
			{
				PlayEffectSound(SFX_428);

				if (!m_pSmeltLay)
				{
					m_pSmeltLay = CSmeltArmor::create();
					int iPriority = LayerManager::instance()->getCurrLayer()->getTouchPriority()-1;
					m_pSmeltLay->setTouchPriority(iPriority);
					this->addChild(m_pSmeltLay);
				}
				m_pSmeltLay->setVisible(true);

			}break;
			//升星
		case StrengthenStarUp:
			{
				if (!m_pStarUpLay)
				{
					m_pStarUpLay = CStarUpItem::create();
					int iPriority = LayerManager::instance()->getCurrLayer()->getTouchPriority()-1;
					m_pStarUpLay->setTouchPriority(iPriority);
					this->addChild(m_pStarUpLay);
				}
				m_pStarUpLay->setVisible(true);

			}break;
		default:
			break;
		}
	}
}


void CStrengthen::onClose(CCObject *pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CStrengthen::showRedTip()
{
	CCNode *redPoint = (CCNode*)(m_pUi->findWidgetById("redPoint"));
	redPoint->setVisible(CTaskControl::getInstance()->getGameTips()->strenTips);
	CCNode *redPoint1 = (CCNode*)(m_pUi->findWidgetById("redPoint1"));
	redPoint1->setVisible(CTaskControl::getInstance()->getGameTips()->risingTips);
}


void CStrengthen::changeToStrengthenWithType( StrengthenType iType )
{
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_pUi->getChildByTag(10);

	m_iType = iType;

	for(unsigned int i=0; i<3; i++)
	{
		CRadioButton* pRadioButton = (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioGroup->selectButton(pRadioButton, i==iType);
	}
}
