#include "WarRelive.h"
#include "scene/CPopTip.h"
#include "model/DataCenter.h"
#include "netcontrol/CPlayerControl.h"
#include "model/DataCenter.h"
#include "model/WarManager.h"
#include "warscene/ComBatLogic.h"
#include "Scene/WarScene.h"
#include "common/CheckMoney.h"
#include "common/CShowToBuyResource.h"
#include "tools/ShowTexttip.h"

WarRelive::WarRelive()
{
	m_iMoney = 0;
}

bool WarRelive::init()
{
	bool res = BaseLayer::init();
	do{
		CC_BREAK_IF(!res);
		this->setTouchPriority(-12);

		setVisible(true);

		MaskLayer* lay = MaskLayer::create("WarReliveMask");
		lay->setContentSize(CCSizeMake(2824,640));
		this->addChild(lay);

	} while (0);
	return res;
}


void WarRelive::onEnter()
{
	BaseLayer::onEnter();

	//绑定活动列表回调
	GetTcpNet->registerMsgHandler(ContinueStageMsg, this, CMsgHandler_selector(WarRelive::processNetMsg));
}

void WarRelive::onExit()
{
	BaseLayer::onExit();

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);
}


void WarRelive::show(int iMoney)
{
	m_iMoney = iMoney;

	DataCenter::sharedData()->getWar()->getLogicObj()->onPause();

	CPopTip *tip = CPopTip::create();
	LayerManager::instance()->pop();//pop掉masklayer
	tip->setTouchEnabled(true);
	tip->setTouchPriority(-100);
	tip->addContentTip(CCString::createWithFormat(GETLANGSTR(1129), iMoney)->getCString());
	tip->setButtonLisener(this,ccw_click_selector(WarRelive::onTouch));
	this->addChild(tip, 999);
}

void WarRelive::hide()
{
	removeFromParentAndCleanup(true);
	DataCenter::sharedData()->getWar()->getLogicObj()->onResume();
}

void WarRelive::onTouch( CCObject* pSender )
{
	CButton* btn = (CButton*)pSender;
	int tag = btn->getTag();

	if (tag == PopTipCancel)
	{
		hide();
		DataCenter::sharedData()->getWar()->getLogicObj()->CombatResult(CCInteger::create(fail));
	}
	else
	{
		if(!CheckGold(m_iMoney))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}

		//复活
		GetTcpNet->sendDataType(ContinueStageMsg);
	}
}

void WarRelive::processNetMsg( int type, google::protobuf::Message *msg )
{
	if(type == ContinueStageMsg)
	{
		StageContinueRes *res = (StageContinueRes*)msg;
		switch (res->result())//续关结果(1 成功，2 元宝不足，3 数据错误)
		{
		case 1:
			{
				hide();
				DataCenter::sharedData()->getWar()->getLogicObj()->reliveSuccess();
			};break;
		case 2:
			{
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
			};break;
		case 3:
			{
				ShowPopTextTip(GETLANGSTR(170));
			};break;
		default:
			break;
		}
	}
}
