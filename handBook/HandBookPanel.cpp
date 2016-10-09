/******************************************************
*文件名称:	HandBookPanel.cpp
*编写日期:	2016-8-23-16:01
*编写作者:	YPF
*功能描述:	图鉴外框
*******************************************************/

#include "HandBookPanel.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "HandBookHero.h"
#include "HandBookMonster.h"
#include "HandBookEquip.h"


CHandBookPanel::CHandBookPanel():
m_ui(nullptr),m_pHeroLayer(nullptr), m_pMonsterLayer(nullptr),m_pEquipLayer(nullptr),m_iType(HandBookTabNull)
{

}

CHandBookPanel::~CHandBookPanel()
{
	
}

bool CHandBookPanel::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("HandBookMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("handBook.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		setVisible(false);

		return true;
	}

	return false;
}

void CHandBookPanel::onEnter()
{
	BaseLayer::onEnter();

	//选项卡
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->findWidgetById("radio");
	for (unsigned int i=0; i<HandBookTabMax; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CHandBookPanel::onSwitchBtn));
	}

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CHandBookPanel::onClose));
	this->addChild(pClose, 999);

	//默认选择一个
	selectTabPanel(HandBookTabHero);
}

void CHandBookPanel::onExit()
{
	BaseLayer::onExit();

	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
}

void CHandBookPanel::onSwitchBtn( CCObject *pSender, bool bChecked )
{
	CRadioButton* pBtn = (CRadioButton*)pSender;
	int iSelectIndex = pBtn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		selectTabPanel((HandBookTabType)iSelectIndex);
	}
}

void CHandBookPanel::selectTabPanel( HandBookTabType iType, bool bForce )
{
	if(m_iType==iType && !bForce)
	{
		return;
	}

	m_iType = iType;

	//更新展示区域
	updateShowArea();
}

void CHandBookPanel::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CHandBookPanel::updateShowArea()
{
	CCNode* pLayer = m_ui;

	if(m_pHeroLayer) m_pHeroLayer->setVisible(false);
	if(m_pMonsterLayer) m_pMonsterLayer->setVisible(false);
	if(m_pEquipLayer)m_pEquipLayer->setVisible(false);

	switch (m_iType)
	{
	case HandBookTabHero:
		{
			if(!m_pHeroLayer)
			{
				m_pHeroLayer = CHandBookHeroLayer::create();
				m_pHeroLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pHeroLayer);
				m_pHeroLayer->setPosition(pLayer->convertToNodeSpace(m_pHeroLayer->getPosition()));
				AskForHandBook(m_iType);
			}
			else
			{
				m_pHeroLayer->setVisible(true);
			}
		}
		break;
	case HandBookTabMonster:
		{
			if(!m_pMonsterLayer)
			{
				m_pMonsterLayer = CHandBookMonsterLayer::create();
				m_pMonsterLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pMonsterLayer);
				m_pMonsterLayer->setPosition(pLayer->convertToNodeSpace(m_pMonsterLayer->getPosition()));
				AskForHandBook(m_iType);
			}
			m_pMonsterLayer->setVisible(true);
		}
		break;
	case HandBookTabEquip:
		{
			if(!m_pEquipLayer)
			{
				m_pEquipLayer = CHandBookEquipLayer::create();
				m_pEquipLayer->setTouchPriority(LayerManager::instance()->getPriority()-1);
				pLayer->addChild(m_pEquipLayer);
				m_pEquipLayer->setPosition(pLayer->convertToNodeSpace(m_pEquipLayer->getPosition()));
				AskForHandBook(m_iType);
			}
			m_pEquipLayer->setVisible(true);
		}
		break;
	case HandBookTabMax:
		break;
	default:
		break;
	}
}
