#include "PropGetInfo.h"
#include "scene/layer/LayerManager.h"
#include "net/CNetClient.h"
#include "scene/layer/LayerManager.h"
#include "model/DataCenter.h"
#include "tools/UICloneMgr.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "tollgate/TollgatePreview.h"
#include "tollgate/SelectArmy.h"
#include "common/CommonFunction.h"
#include "mainCity/CCompaseLayer.h"

#include "common/CGameSound.h"
#include "Resources.h"

PropGetInfo::PropGetInfo()
	:m_ui(nullptr), m_cell(nullptr), m_tableView(nullptr)
{

}

bool PropGetInfo::init()
{
	if (BaseLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//黑底
		MaskLayer* pMaskLayer = MaskLayer::create("PropGetInfoMaskLayer");
		pMaskLayer->setContentSize(winSize);
		LayerManager::instance()->push(pMaskLayer);

		m_ui = LoadComponent("propGetInfo.xaml");
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);

		this->setVisible(true);

		return true;
	}
	return false;
}


void PropGetInfo::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void PropGetInfo::onEnter()
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
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(PropGetInfo::tableviewDataSource));
	m_tableView->reloadData();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(PropGetInfo::onClose));
	this->addChild(pClose, 999);

	GetTcpNet->registerMsgHandler(ItemInfoMsg, this, CMsgHandler_selector(PropGetInfo::processMessage));

}


void PropGetInfo::onExit()
{
	BaseLayer::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
}


CCObject* PropGetInfo::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void PropGetInfo::evolveTask(const vector<CDropStage>& stageList )
{
	CButton *evolve =(CButton*)(m_ui->findWidgetById("evolve"));
	if (evolve)
	{	
		evolve->setEnabled(true);
	}
	m_stageList = stageList;
	m_tableView->setCountOfCell(stageList.size());
	m_tableView->reloadData();
}

void PropGetInfo::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	CLayout *lay = UICloneMgr::cloneLayout(m_cell);

	const CDropStage &evol = m_stageList.at(uIdx);
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
		    bgView->setUserData(&m_stageList.at(uIdx));
			bgView->setOnClickListener(this, ccw_click_selector(PropGetInfo::onRecvTask));
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


void PropGetInfo::onRecvTask(CCObject* pSender)
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
				tower->setSearchItem(m_item);
				LayerManager::instance()->push(tower);
				//tower->touchRadioButton(2);
				CPlayerControl::getInstance().sendComposeItem(m_item.itemType==1?2:1);
			}
			else
			{
				onClose(nullptr);
				CCompaseLayer * pCompaseLayer = (CCompaseLayer*)pLayer;
				pCompaseLayer->setSearchItem(m_item);
				CPlayerControl::getInstance().sendComposeItem(m_item.itemType==1?2:1);
			}
		}
		else
		{
			//等级开放提示
			ShowPopTextTip(CCString::createWithFormat(GETLANGSTR(1026), m_item.itparam)->getCString());
		}
	}
	//>100 关卡
	else if(evol->stageId > 100)
	{
		if (evol->open)
		{
			CSelectArmy *preview = CSelectArmy::create();
			preview->setStagId(evol->stageId);
			LayerManager::instance()->push(preview);
			CPlayerControl::getInstance().sendUnion(evol->stageId);
		}
		else if (!evol->open)
		{
			ShowPopTextTip(GETLANGSTR(183));
		}
	}
}

void PropGetInfo::processMessage( int type, google::protobuf::Message *msg )
{
	switch (type)
	{
	case ItemInfoMsg:
		{
			ItemInfoRes *res = (ItemInfoRes*)msg;
			m_item.read(res->item());
			updateItemInfo();
			evolveTask(m_item.stageList);
		}
		break;
	default:
		break;
	}
}

void PropGetInfo::updateItemInfo()
{
	const ItemData * itemData = DataCenter::sharedData()->getItemDesc()->getCfg(m_item.itemId);

	//头像
	CImageView *itembg1 = (CImageView *)(m_ui->findWidgetById("itembg1"));
	CCSprite *head = CCSprite::create(GetImageName(m_item.itemType, m_item.quality, m_item.iconId).c_str());
	if (!head)
	{
		head = CCSprite::create("prop/100.png");
		CCLOG("[ ERROR ] PropGetInfo::updateItemInfo");
	}
	itembg1->removeChildByTag(1);
	head->setTag(1);
	itembg1->addChild(head);
	NodeFillParent(head);

	//品阶
	CImageView *mask1 = (CImageView *)(m_ui->findWidgetById("mask1"));
	mask1->setTexture(setItemQualityTexture(m_item.quality));

	//名字
	CLabel* name = (CLabel *)(m_ui->findWidgetById("name"));
	if(itemData)
	{
		name->setString(itemData->itemName.c_str());
	}

	//数量
	CLabel* val = (CLabel *)(m_ui->findWidgetById("haveNum"));
	val->setString(CCString::createWithFormat("%d", m_item.itemNum)->getCString());
}

bool PropGetInfo::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	bool res = CWidgetWindow::ccTouchBegan(pTouch, pEvent);
	CCSprite *bgSpr = (CCSprite*)m_ui->findWidgetById("bg");

	CCPoint pTouchPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	if( !res && !bgSpr->boundingBox().containsPoint(pTouchPos))
	{
		res = true;
		onClose(nullptr);
	}
	return res;
}


