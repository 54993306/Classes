#include "VipLayer.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "tools/UICloneMgr.h"

#include "mainCity/MainCityControl.h"
#include "scene/CPopTip.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "CVipPay.h"

CVipLayer::CVipLayer():m_power(nullptr),m_optionType(0),m_cell(nullptr),m_bArchive(false)
{

}


bool CVipLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("Vip");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("Vip.xaml");  //  SelectSkill
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);	

		setCoverScreen(true);

		return true;
	}
	return false;

}

void CVipLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CVipLayer::onToggle(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	if (!m_power)
	{
		m_power = CVipPower::create();
		m_power->setTouchPriority(this->getTouchPriority()-1);
		this->addChild(m_power);
		if (m_vipInfo.level==0)
		{
			CPlayerControl::getInstance().sendVipInfoByLevel(1);
		}
		else
		{
			m_power->updateVipPriv(&m_vipInfo);
		}
	}
// 	CCSprite *bg= (CCSprite *)m_ui->findWidgetById("scroll");
// 	bg->setVisible(!bg->isVisible());
	
	m_gridView->setVisible(!m_gridView->isVisible());
	m_power->setVisible(!m_gridView->isVisible());

	CCSprite *power= (CCSprite *)m_ui->findWidgetById("power");
	power->setVisible(m_gridView->isVisible());
	CCSprite *recharge= (CCSprite *)m_ui->findWidgetById("recharge");
	recharge->setVisible(!m_gridView->isVisible());
}

void CVipLayer::onEnter()
{
	BaseLayer::onEnter();
	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
    m_ui->removeChild(m_cell);

	m_gridView = (CGridView*)m_ui->findWidgetById("scroll");
	m_gridView->setDirection(eScrollViewDirectionVertical);	
	m_gridView->setCountOfCell(0);
	m_gridView->setSizeOfCell(m_cell->getContentSize());
	m_gridView->setAnchorPoint(ccp(0,0));
	m_gridView->setColumns(2);
	m_gridView->setAutoRelocate(true);
	m_gridView->setDeaccelerateable(false);
	m_gridView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CVipLayer::gridviewDataSource));
	m_gridView->reloadData();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CVipLayer::onClose));
	this->addChild(pClose, 999);

	CButton* btn= (CButton *)m_ui->findWidgetById("toggle");
	btn->setOnClickListener(this,ccw_click_selector(CVipLayer::onToggle));
	GetTcpNet->registerMsgHandler(VipInfoMsg,this, CMsgHandler_selector(CVipLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(VipShopMsg,this, CMsgHandler_selector(CVipLayer::resVipShop));
	GetTcpNet->registerMsgHandler(BuyCardMsg,this, CMsgHandler_selector(CVipLayer::buyCardRes));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_VIPINFO,this,GameMsghandler_selector(CVipLayer::updateBuyInfo));
}

CCObject* CVipLayer::gridviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{ 
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	/*if (!pCell&&uIdx>=0&&uIdx<m_itemList.size())*/
	{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		addGridCell(uIdx,pCell);
	}
	return pCell;
}


void CVipLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
    CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_VIPINFO,this);
	BaseLayer::onExit();

	if(m_bArchive)
	{
		//显示被遮盖的层，渲染
		LayerManager::instance()->showLayerUnBeCovered(this);
		NOTIFICATION->postNotification(HIDE_TOP_LAYER);
		NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	}
} 

void CVipLayer::addGridCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	const CGoldCard &gcard = m_goldCardList.at(uIdx);
	for (int i = 1; i <=8; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
		switch (i)
		{
		case 1:
			{
				CImageViewScale9 *imgscale = (CImageViewScale9*)child;
				imgscale->setTouchEnabled(true);
				imgscale->setUserData(&m_goldCardList.at(uIdx));
				imgscale->setOnClickListener(this,ccw_click_selector(CVipLayer::onBuy));
			}
			break;
		case 8:
			{
				CLabel *label = (CLabel*)child;
				label->setString(CCString::createWithFormat(GETLANGSTR(162),gcard.price)->getCString());
			}
			break;
		case 5:
			{
				child->setVisible(gcard.recommend);
			}
			break;
		case 6:
			{
				CLabel *label = (CLabel*)child;
				label->setString(gcard.name.c_str());
			}
			break;
		case 7:
			{
				CLabel *label = (CLabel*)child;
				label->setString(gcard.note.c_str());
			}
			break;

		case 4:
			{
				if (gcard.id!=1)
				{
					int iIndex = uIdx+1;
					iIndex = iIndex>6?6:iIndex;
					CImageView *img = (CImageView*)child;
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("vip/gouyu%d.png", iIndex)->getCString()));
				}
			}
			break;
		default:
			break;
		}
	}
}

CVipLayer::~CVipLayer()
{
	CC_SAFE_RELEASE(m_cell);
}

void CVipLayer::onBuy(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
	ShowPopTextTip(GETLANGSTR(191));
	return;
#endif

	CImageViewScale9* btn = (CImageViewScale9*)pSender;
	CGoldCard *card = (CGoldCard*)(btn->getUserData());
	m_card = card;
	if (!card->canBy)
	{
		ShowPopTextTip(GETLANGSTR(1029));
	}
	else
	{
		CCString *str = CCString::createWithFormat(GETLANGSTR(1030),card->price,card->name.c_str());
		ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(CVipLayer::onConfirmBuy));
	}
}

void CVipLayer::onConfirmBuy(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		int iCardId = m_card->id;

		((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
		
		CVipPay* pPay = CVipPay::create();
		pPay->setCardId(m_card->id);
		LayerManager::instance()->push(pPay);

		return;
	}
	
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CVipLayer::ProcessMsg(int type, google::protobuf::Message *msg)
{
	if(!isVisible())
	{
		this->setVisible(true);

		m_bArchive = true;

		//隐藏被遮盖的层，不渲染
		LayerManager::instance()->hideLayerBeCovered(this);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		NOTIFICATION->postNotification(SHOW_TOP_LAYER);
	}


	VipInfoRes *res = (VipInfoRes*)msg;
	m_vipInfo.read(*res);

	CLabel *vipVal = (CLabel*)m_ui->findWidgetById("value");
	vipVal->setString(CCString::createWithFormat("%d/%d",m_vipInfo.exp,m_vipInfo.nextExp)->getCString());

	CLabelAtlas *viplev = (CLabelAtlas*)m_ui->findWidgetById("level_vip");
	viplev->setString(ToString(m_vipInfo.level));

	CLabelAtlas *vipnextlev = (CLabelAtlas*)m_ui->findWidgetById("level_vip1");
	vipnextlev->setString(ToString(m_vipInfo.level+1));

	CLabel *gold = (CLabel*)m_ui->findWidgetById("gold");
	gold->setString(ToString(m_vipInfo.nextExp - m_vipInfo.exp));

	CProgressBar *process = (CProgressBar*)m_ui->findWidgetById("progress");
	process->setMaxValue(m_vipInfo.nextExp);
	process->setValue(m_vipInfo.exp);

	if( m_vipInfo.nextExp == m_vipInfo.exp )
	{
		CLayout *pLayAttr = (CLayout*)m_ui->findWidgetById("lay_attr");
		pLayAttr->setVisible(false);
		CLabel *pMaxVip = (CLabel*)m_ui->findWidgetById("max_vip");
		pMaxVip->setVisible(true);
	}
}

void CVipLayer::resVipShop(int type, google::protobuf::Message *msg)
{
	this->setVisible(true);

	VipShopRes *res = (VipShopRes *)msg;
	for (int i = 0; i < res->carlist_size(); i++)
	{
		CGoldCard gc;
		gc.read(res->carlist(i));
		m_goldCardList.push_back(gc);
	}
	m_gridView->setCountOfCell(m_goldCardList.size());
	m_gridView->reloadData();
	if (m_optionType==1)
	{
		onToggle(nullptr);
	}
}

void CVipLayer::setOptionType(int type)
{
	m_optionType = type;
}


void CVipLayer::buyCardRes(int type, google::protobuf::Message *msg)
{
	BuyCardRes *res = (BuyCardRes*)msg;

	//获取购买弹框
	CVipPay* pPay = dynamic_cast<CVipPay*>(LayerManager::instance()->getLayer("CVipPay"));

	if(pPay)
	{
		pPay->enableSmsCodeBtn();
	}

	//成功
	if (res->result() == 1)
	{
		PlayEffectSound(SFX_415);

		int iSetp = res->viplevel()==0?1:2;

		//当前步骤
		if(pPay)
		{
			//第一步验证
			if(iSetp ==1)
			{
				pPay->setStep1Success(true);
				pPay->showTimeClock();
				return;
			}
		}

		if(iSetp == 2)
		{
			pPay->onClose(nullptr);

			m_vipInfo.level = res->viplevel();
			m_vipInfo.exp = res->vipexp();
			m_vipInfo.nextExp = res->nextexp();

			CLabel *vipVal = (CLabel*)m_ui->findWidgetById("value");
			vipVal->setString(CCString::createWithFormat("%d/%d",m_vipInfo.exp,m_vipInfo.nextExp)->getCString());

			CLabelAtlas *viplev = (CLabelAtlas*)m_ui->findWidgetById("level_vip");
			viplev->setString(ToString(m_vipInfo.level));

			CLabelAtlas *vipnextlev = (CLabelAtlas*)m_ui->findWidgetById("level_vip1");
			vipnextlev->setString(ToString(m_vipInfo.level+1));

			CLabel *gold = (CLabel*)m_ui->findWidgetById("gold");
			gold->setString(ToString(m_vipInfo.nextExp - m_vipInfo.exp));

			CProgressBar *process = (CProgressBar*)m_ui->findWidgetById("progress");
			process->setMaxValue(m_vipInfo.nextExp);
			process->setValue(m_vipInfo.exp);

			if( m_vipInfo.nextExp == m_vipInfo.exp )
			{
				CLayout *pLayAttr = (CLayout*)m_ui->findWidgetById("lay_attr");
				pLayAttr->setVisible(false);
				CLabel *pMaxVip = (CLabel*)m_ui->findWidgetById("max_vip");
				pMaxVip->setVisible(true);
			}
		}
	}
	else if(res->result() == 2)
	{
		ShowPopTextTip(GETLANGSTR(1171));
	}
	else if(res->result() == 3)
	{
		ShowPopTextTip(GETLANGSTR(1172));
		if(pPay)
		{
			pPay->showErrorCode(res->errorcode());
		}
	}
	else if(res->result() == 4)
	{
		ShowPopTextTip(GETLANGSTR(1173));
		if(pPay)
		{
			pPay->showErrorCode(res->errorcode());
		}
	}
	else if(res->result() == 5)
	{
		ShowPopTextTip(GETLANGSTR(170));
	}
}


void CVipLayer::updateBuyInfo(const TMessage& tMsg)
{
	int viplev = tMsg.nMsg;
	int gold = tMsg.intParam;
    if (viplev>m_vipInfo.level+1)
    {
		CLabelAtlas *vipnextlev = (CLabelAtlas*)m_ui->findWidgetById("level_vip1");
		vipnextlev->setString(ToString(viplev));

		CLabel *goldlab = (CLabel*)m_ui->findWidgetById("gold");
		goldlab->setString(ToString(gold-m_vipInfo.exp));
    }
	else if (viplev == m_vipInfo.level+1)
	{
		CLabelAtlas *vipnextlev = (CLabelAtlas*)m_ui->findWidgetById("level_vip1");
		vipnextlev->setString(ToString(viplev));

		CLabel *goldlab = (CLabel*)m_ui->findWidgetById("gold");
		goldlab->setString(ToString(m_vipInfo.nextExp - m_vipInfo.exp));
	}

	if( m_vipInfo.nextExp == m_vipInfo.exp )
	{
		CLayout *pLayAttr = (CLayout*)m_ui->findWidgetById("lay_attr");
		pLayAttr->setVisible(false);
		CLabel *pMaxVip = (CLabel*)m_ui->findWidgetById("max_vip");
		pMaxVip->setVisible(true);
	}
}