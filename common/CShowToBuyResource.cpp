#include "CShowToBuyResource.h"
#include "scene/CPopTip.h"
#include "common/MultLanguage.h"
#include "mainCity/BuyResource.h"
#include "netcontrol/CPlayerControl.h"
#include "vip/VipLayer.h"

CShowToBuyResource::CShowToBuyResource()
	:m_pPopTip(nullptr)
	,m_iCurrentType(ShowBuyResourceAction)
{

}

CShowToBuyResource::~CShowToBuyResource()
{

}

void CShowToBuyResource::onEnter()
{
	BaseLayer::onEnter();
	this->setVisible(true);
}

void CShowToBuyResource::onExit()
{
	BaseLayer::onExit();
}

void CShowToBuyResource::showToBuyResourceByType( ShowBuyResourceType type )
{
	m_iCurrentType = type;

	CPopTip *pop = CPopTip::create();
	pop->setVisible(true);
	pop->addContentTip(getInfoText().c_str());
	pop->setTouchEnabled(true);
	pop->setTouchPriority(-100);
	pop->setButtonLisener(this, ccw_click_selector(CShowToBuyResource::onClick));
	this->addChild(pop);

	LayerManager::instance()->push(this);
}

void CShowToBuyResource::onClick( CCObject* pSender )
{
	CButton *btn = (CButton*)pSender;

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();

	if (btn->getTag()==PopTipConfirm)
	{
		//确定
		doConfirmAction();
	}
	else
	{
		//取消
	}
}

std::string CShowToBuyResource::getInfoText()
{
	switch (m_iCurrentType)
	{
	case ShowBuyResourceAction:
		return GETLANGSTR(1104);
		break;
	case ShowBuyResourceGold:
		return GETLANGSTR(1106);
		break;
	case ShowBuyResourceCoin:
		return GETLANGSTR(1105);
		break;
	case ShowBuyResourceActionLimit:
		return GETLANGSTR(1107);
		break;
	default:
		break;
	}
	CCLOG("ERROR CShowToBuyResource::getInfoText");
	return "";
}

void CShowToBuyResource::doConfirmAction()
{
	switch (m_iCurrentType)
	{
	case ShowBuyResourceAction:
		{
			BuyResource* bufLayer = BuyResource::create();
			bufLayer->setExchangeType(BUY_FOOD);
			LayerManager::instance()->push(bufLayer);
			CPlayerControl::getInstance().sendConvert(2);
		}
		break;
	case ShowBuyResourceGold:
		{
			CVipLayer *vipLayer= CVipLayer::create();
			if(LayerManager::instance()->push(vipLayer))
			{
				GetTcpNet->sendDataType(VipInfoMsg,true);
				GetTcpNet->sendDataType(VipShopMsg,true);
			}
		}
		break;
	case ShowBuyResourceCoin:
		{
			BuyResource* bufLayer = BuyResource::create();
			bufLayer->setExchangeType(BUY_COIN);
			LayerManager::instance()->push(bufLayer);
			CPlayerControl::getInstance().sendConvert(1);
		}
		break;
	case ShowBuyResourceActionLimit:
		{
			CVipLayer *vipLayer= CVipLayer::create();
			if(LayerManager::instance()->push(vipLayer))
			{
				GetTcpNet->sendDataType(VipInfoMsg,true);
				GetTcpNet->sendDataType(VipShopMsg,true);
			}
		}
		break;
	default:
		break;
	}
}
