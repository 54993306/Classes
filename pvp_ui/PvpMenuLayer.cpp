#include "PvpMenuLayer.h"
#include "model/DataDefine.h"
#include "GMessage.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "pvp_ui/SelectDefense.h"
#include "pvp_ui/PvpRuleLayer.h"
#include "reward/WShopLayer.h"
#include "pvp_ui/PvpRecord.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/MainCityControl.h"
#include "pvp_ui//TranstionEffectLayer.h"

CPvpMenuLayer::CPvpMenuLayer():
m_ui(nullptr),m_pSelectDefenseLayer(nullptr), m_pPvpRuleLayer(nullptr), m_pShopLayer(nullptr),m_pPvpRecordLayer(nullptr),
m_iType(PvpMenuLayerTabNull),m_pCurrentLayer(nullptr)
{

}

CPvpMenuLayer::~CPvpMenuLayer()
{
	
}

bool CPvpMenuLayer::init()
{
	if(BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		
		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("PvpMenuLayerMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		//内容
		m_ui = LoadComponent("pvp_menu_layer.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		return true;
	}

	return false;
}

void CPvpMenuLayer::onEnter()
{
	BaseLayer::onEnter();

	//选项卡
	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->findWidgetById("radio");
	for (unsigned int i=0; i<PvpMenuLayerTabMax; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CPvpMenuLayer::onSwitchBtn));
	}

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CPvpMenuLayer::onClose));
	this->addChild(pClose, 999);

	//默认选择一个
	selectTabPanel(PvpMenuLayerTabDefense);

	showOpenEffect();
}

void CPvpMenuLayer::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
	//解绑场景事件监听
	CSceneManager::sharedSceneManager()->removeMsgObserver("NULL", this);


	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD, 0, nullptr,nullptr);
	
}

void CPvpMenuLayer::onSwitchBtn( CCObject *pSender, bool bChecked )
{
	CRadioButton* pBtn = (CRadioButton*)pSender;
	int iSelectIndex = pBtn->getTag();

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);

		selectTabPanel((PvpMenuLayerTabType)iSelectIndex);
	}
}

void CPvpMenuLayer::selectTabPanel( PvpMenuLayerTabType iType, bool bForce )
{
	if(m_iType==iType && !bForce)
	{
		return;
	}

	m_iType = iType;

	//更新展示区域
	updateShowArea();
}

void CPvpMenuLayer::onClose( CCObject* pSender )
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CPvpMenuLayer::updateShowArea()
{
	CCNode* pLayer = (CCNode*)m_ui->findWidgetById("layer_in");

	//由shop切回
	if(m_iType != PvpMenuLayerTabShop && m_pShopLayer && m_pShopLayer->isVisible())
	{
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}
	//切回shop
	if(m_iType == PvpMenuLayerTabShop)
	{
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
	}

	if(m_pSelectDefenseLayer) m_pSelectDefenseLayer->setVisible(false);
	if(m_pPvpRuleLayer) m_pPvpRuleLayer->setVisible(false);
	if(m_pShopLayer)m_pShopLayer->setVisible(false);
	if(m_pPvpRecordLayer)m_pPvpRecordLayer->setVisible(false);

	switch (m_iType)
	{
	case PvpMenuLayerTabDefense:
		{
			if(!m_pSelectDefenseLayer)
			{
				m_pSelectDefenseLayer = CSelectDefense::create();
				m_pSelectDefenseLayer->setTouchPriority(LayerManager::instance()->getLayerManagerMinPriority()-1);
				pLayer->addChild(m_pSelectDefenseLayer);
				m_pSelectDefenseLayer->setPosition(pLayer->convertToNodeSpace(m_pSelectDefenseLayer->getPosition()));
				CPlayerControl::getInstance().askForPvpTeamData();
			}
			else
			{
				m_pSelectDefenseLayer->setVisible(true);
			}
			m_pCurrentLayer = m_pSelectDefenseLayer;
		}
		break;
	case PvpMenuLayerTabRule:
		{
			if(!m_pPvpRuleLayer)
			{
				m_pPvpRuleLayer = CPvpRuleLayer::create();
				m_pPvpRuleLayer->setTouchPriority(LayerManager::instance()->getLayerManagerMinPriority()-1);
				pLayer->addChild(m_pPvpRuleLayer);
				m_pPvpRuleLayer->setPosition(pLayer->convertToNodeSpace(m_pPvpRuleLayer->getPosition()));
				AskForPvpRule();
			}
			m_pPvpRuleLayer->setVisible(true);
			m_pCurrentLayer = m_pPvpRuleLayer;
		}
		break;
	case PvpMenuLayerTabShop:
		{
			if(!m_pShopLayer)
			{
				m_pShopLayer = CWShopLayer::create();
				m_pShopLayer->loadUiByType(4);
				m_pShopLayer->setTouchPriority(LayerManager::instance()->getLayerManagerMinPriority()-1);
				pLayer->addChild(m_pShopLayer);
				m_pShopLayer->setPosition(pLayer->convertToNodeSpace(m_pShopLayer->getPosition()));
			}			
			CMainCityControl::getInstance()->sendShopRequest(3);
			m_pShopLayer->setVisible(true);
			m_pCurrentLayer = m_pShopLayer;
		}
		break;
	case PvpMenuLayerTabRecord:
		{
			if(!m_pPvpRecordLayer)
			{
				m_pPvpRecordLayer = CPvpRecord::create();
				m_pPvpRecordLayer->setTouchPriority(LayerManager::instance()->getLayerManagerMinPriority()-1);
				pLayer->addChild(m_pPvpRecordLayer);
				m_pPvpRecordLayer->setPosition(pLayer->convertToNodeSpace(m_pPvpRecordLayer->getPosition()));
			}			
			m_pPvpRecordLayer->setVisible(true);
			m_pCurrentLayer = m_pPvpRecordLayer;
		}
		break;
	case PvpMenuLayerTabMax:
		break;
	default:
		break;
	}
}

void CPvpMenuLayer::showOpenEffect()
{
	//layout
	CLayout* pLayout = (CLayout*)m_ui->findWidgetById("panel");

	//拿到背景框
	CCSprite* pRect = (CCSprite*)m_ui->findWidgetById("bg_rect");

	//拿到位置标记
	CCSprite* pBoxLight = (CCSprite*)pLayout->findWidgetById("box");
	CCPoint pPos = pRect->convertToNodeSpace(pBoxLight->getParent()->convertToWorldSpace(pBoxLight->getPosition()));

	//过渡效果
	CTranstionEffectLayer* pEffect = CTranstionEffectLayer::create();
	pEffect->setStartPos(pPos);
	pEffect->setLineDir(TranstionEffectLineDirectionVertical);
	pEffect->setGrowDir(TranstionEffectGrowDirectionMinus);
	pEffect->setLineCount(11);
	pEffect->setLineMax(15);
	pEffect->setTimeMove(0.03f);
	pEffect->setTimeFade(0.45f);
	pEffect->setCellRotation(-90);
	pRect->addChild(pEffect);

	pEffect->bindCallback(this, transitionEffect_end_callback(CPvpMenuLayer::showOpenEffectCallback));
	pEffect->showEffect();

	//隐藏当前显示层
	pLayout->setVisible(false);
}

void CPvpMenuLayer::showOpenEffectCallback()
{
	//显示默认的层
	//layout
	CLayout* pLayout = (CLayout*)m_ui->findWidgetById("panel");
	pLayout->setVisible(true);
}
