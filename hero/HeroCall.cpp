#include "HeroCall.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "scene/alive/ActObject.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "HeroControl.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "tollgate/TollgatePreview.h"
#include "tollgate/SelectArmy.h"
#include "common/CommonFunction.h"
#include "mainCity/CCompaseLayer.h"
#include "scene/AnimationManager.h"
#include "Global.h"
#include "mainCity/Recruit.h"
#include "vip/VipLayer.h"
#include "shop/ShopLayer.h"
#include "mainCity/MainCityControl.h"

#include "common/CGameSound.h"
#include "Resources.h"


CHeroCall::CHeroCall()
	: m_ui(nullptr), m_hero(nullptr), m_cell(nullptr),
	m_tableView(nullptr), m_item(),m_itemNeedNum(0)
{

}

bool CHeroCall::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("CHeroCallMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		m_ui = LoadComponent("HeroCall.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		this->setVisible(true);

		return true;
	}
	return false;
}

void CHeroCall::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CHeroCall::onEnter()
{
	BaseLayer::onEnter();

	m_cell = (CLayout *)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_cell->removeFromParent();

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CHeroCall::tableviewDataSource));
	m_tableView->reloadData();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CHeroCall::onClose));
	this->addChild(pClose, 999);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HEROINFO,this, GameMsghandler_selector(CHeroCall::updateHeroInfo));	

	CNetClient::getShareInstance()->registerMsgHandler(ItemInfoMsg,this, CMsgHandler_selector(CHeroCall::processMessage));

	NOTIFICATION->addObserver(this, callfuncO_selector(CHeroCall::compaseSuccess), COMPOSE_SUCCESS, nullptr);
}

void CHeroCall::updateHeroInfo(const TMessage& tMsg)
{
	CHero *hero = (CHero*)tMsg.lParam;
	if (tMsg.nMsg == 1)
	{
		showHeroHead(hero);
	}
	else
	{
		showHeroQuality(hero);
	}
}

CCObject* CHeroCall::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CTableViewCell *pCell = (CTableViewCell*)(pConvertCell);
	if (!pCell)
	{
		pCell = new CTableViewCell;
		pCell->autorelease();
		addTableCell(uIdx, pCell);
	}
	else
	{
		pCell->removeAllChildren();
		addTableCell(uIdx, pCell);
	}
	return pCell;
}

void CHeroCall::evolveTask( const CItem& item )
{
	CButton *evolve =(CButton*)(m_ui->findWidgetById("evolve"));
	if (evolve)
	{	
		evolve->setEnabled(true);
	}
	m_item = item;
	m_tableView->setCountOfCell(m_item.stageList.size());
	m_tableView->reloadData();
}

void CHeroCall::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const CDropStage &evol = m_item.stageList.at(uIdx);
	const TaskData *task = DataCenter::sharedData()->getTask(Evole_Task)->getCfg(evol.stageId);

	for (int i = 1; i <=5; i++)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);

		if (i==1)
		{
			CImageView *bgView= (CImageView*)child;
			bgView->setTouchEnabled(true);
		    bgView->setUserData(&m_item.stageList.at(uIdx));
			bgView->setOnClickListener(this, ccw_click_selector(CHeroCall::onRecvTask));
		}
		else if (i==2)
		{
			CLabel *taskName = (CLabel*)child;
			if(task)
			{
				taskName->setString(task->taskName.c_str());
			}
		}
		else if (i==3)
		{
			CLabel *taskDesc = (CLabel*)child;
			if(task)
			{
				taskDesc->setString(task->taskDesc.c_str());
			}
			
		}
		else if (i==4)
		{
			UserData *data = DataCenter::sharedData()->getUser()->getUserData();

			//1.调和塔    
			if(evol.stageId == 1)
			{
				if(data->getLevel() >= m_item.itparam)
				{
					child->setVisible(false);
				}
				else
				{
					child->setVisible(true);
				}
			}
			else if (evol.stageId<=100)
			{
				child->setVisible(false);
			}
			else
			{
				child->setVisible(!evol.open);
			}
			
		}
		else if (i==5)
		{
			CCSprite* pNode= (CCSprite*)child;
			if(task)
			{
				CCTexture2D* pTexture = CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("evol/%d.png", task->icon)->getCString());
				if(pTexture)
				{
					pNode->setTexture(pTexture);
				}
			}
		}
	}
}

void CHeroCall::onExit()
{
	BaseLayer::onExit();
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HEROINFO,this);
	NOTIFICATION->removeAllObservers(this);
	GetTcpNet->unRegisterAllMsgHandler(this);
}


void CHeroCall::onRecvTask(CCObject* pSender)
{
	CDropStage *evol = (CDropStage*)((CCNode*)pSender)->getUserData();

	UserData *data = DataCenter::sharedData()->getUser()->getUserData();

	//1.调和塔    
	if(evol->stageId == 1)
	{
		if(data->getLevel() >= m_item.itparam)
		{
			//getLayer
			BaseLayer* pLayer = LayerManager::instance()->getLayer("CCompaseLayer");
			if(!pLayer)
			{
				CCompaseLayer *tower = CCompaseLayer::create();
				CItem item;
				item.iconId =m_item.iconId;
				tower->setSearchItem(item);
				LayerManager::instance()->push(tower);
				tower->touchRadioButton(2);
			}
			else
			{
				CCompaseLayer * pCompaseLayer = (CCompaseLayer*)pLayer;
				CItem item;
				item.iconId = m_hero->thumb;
				pCompaseLayer->setSearchItem(item);
				pCompaseLayer->setSearchItem(item);
				pCompaseLayer->touchRadioButton(2);
				onClose(nullptr);
			}
		}
		else
		{
			//等级开放提示
			ShowPopTextTip(CCString::createWithFormat(GETLANGSTR(1026), m_item.itparam)->getCString());
		}

	}
	else if (evol->stageId == 2)
	{
		CRecruit *recr = CRecruit::create();
		LayerManager::instance()->push(recr);
	}
	else if (evol->stageId ==3)
	{
		CVipLayer *vipLayer= CVipLayer::create();
		if(LayerManager::instance()->push(vipLayer))
		{
			GetTcpNet->sendDataType(VipInfoMsg,true);
			GetTcpNet->sendDataType(VipShopMsg,true);
			return;
		}
	}
	else if (evol->stageId ==4)
	{
		CShopLayer *shpLayer = CShopLayer::create();
		shpLayer->setShopType(1);
		LayerManager::instance()->push(shpLayer);
		CMainCityControl::getInstance()->sendShopRequest(1);
	}
	//>100 关卡
	else if(evol->stageId > 100)
	{
		if (evol->open)
		{
			//CPlayerControl::getInstance().sendStageInfo(evol->id);
			CSelectArmy *preview = CSelectArmy::create();
			preview->setStagId(evol->stageId);
			LayerManager::instance()->push(preview);
			CPlayerControl::getInstance().sendUnion(evol->stageId);

			/*CPlayerControl::getInstance().sendEnterStage(evol->id,0,0,0);*/
		}
		else if (!evol->open)
		{
			ShowPopTextTip(GETLANGSTR(183));
		}
	}
}

void CHeroCall::showHeroHead(CHero *hero)
{
	m_hero = hero;
	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", hero->thumb)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroCall::showHeroHead Lost Image = %d",hero->thumb);
	}
	itembg1->removeChildByTag(1);
	head->setScale(90.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);
	showHeroQuality(hero);

	CLabel* name = (CLabel *)(m_ui->findWidgetById("name"));
	name->setString(hero->name.c_str());

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(hero->itemNum2);
	process->setValue(hero->itemNum1);
}

void CHeroCall::showHeroQuality(CHero * hero)
{
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(hero->iColor));

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",hero->itemNum1,hero->itemNum2)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(hero->itemNum2);
	process->setValue(hero->itemNum1);

}

void CHeroCall::showItem(CItem * item)
{
	m_item = *item;

	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(CCString::createWithFormat("headImg/%d.png", item->iconId)->getCString());
	if (!head)
	{
		head = CCSprite::create("headImg/101.png");
		CCLOG("[ ERROR ] CHeroCall::showHeroHead Lost Image = %d",item->iconId);
	}
	itembg1->removeChildByTag(1);
	head->setScale(90.0f/100.0f);
	head->setTag(1);
	head->setPosition(ccp(itembg1->getContentSize().width/2, itembg1->getContentSize().height/2));
	itembg1->addChild(head);

	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(item->itemId);
	if (itemData)
	{
		CLabel* name = (CLabel *)(m_ui->findWidgetById("name"));
		name->setString(itemData->itemName.c_str());
	}
	
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(item->quality));

	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d",item->itemNum,m_itemNeedNum)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(m_itemNeedNum);
	process->setValue(item->itemNum);
}

bool CHeroCall::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");
	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}


void CHeroCall::setNeedNum(int num)
{
	m_itemNeedNum = num;
}

void CHeroCall::compaseSuccess( CCObject* pObj )
{
	m_item.itemNum++;
	CLabel* val = (CLabel *)(m_ui->findWidgetById("expVal"));
	val->setString(CCString::createWithFormat("%d/%d", m_item.itemNum,m_itemNeedNum)->getCString());

	CProgressBar* process = (CProgressBar *)(m_ui->findWidgetById("exp_process"));
	process->setMaxValue(m_itemNeedNum);
	process->setValue(m_item.itemNum);
}

void CHeroCall::processMessage( int type, google::protobuf::Message *msg )
{
	switch (type)
	{
	case ItemInfoMsg:
		{
			ItemInfoRes *res = (ItemInfoRes*)msg;
			CItem item;
			item.read(res->item());
			showItem(&item);
			evolveTask(item);
		}
		break;
	default:
		break;
	}
}
