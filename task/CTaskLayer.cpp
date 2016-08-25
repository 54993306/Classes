#include "CTaskLayer.h"
#include "net/CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "mainCity/QuickProduct.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "GMessage.h"

#include "common/color.h"
#include "tools/ShowTexttip.h"

#include "TaskControl.h"

#include "model/DataCenter.h"
#include "mainCity/Recruit.h"
#include "mainCity/Strengthen.h"
#include "hero/HeroControl.h"
#include "tollgate/TollgateLayer.h"
#include "sign/PopItem.h"
#include "reward/RewardLayer.h"
#include "mainCity/CultivateLayer.h"
#include "mainCity/MainCityControl.h"
#include "common/CommonFunction.h"
#include "mainCity/BuyResource.h"
#include "vip/VipLayer.h"

#include "common/CGameSound.h"
#include "Resources.h"
#include "mainCity/PanelTips.h"

bool SortTask(const CTask& task1, const CTask& task2)
{
	const TaskData *taskData1 =  DataCenter::sharedData()->getTask(Normal_Task)->getTaskCfg(task1.tindex);
	const TaskData *taskData2 =  DataCenter::sharedData()->getTask(Normal_Task)->getTaskCfg(task2.tindex);
	if(!taskData1 || !taskData2)
	{
		return false;
	}
	return task1.finish*10000+taskData1->mask > task2.finish*10000+taskData2->mask;
}

CTaskLayer::~CTaskLayer()
{
	CC_SAFE_RELEASE(m_cell);
}

bool CTaskLayer::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("TaskMaskLayer");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);
		
		m_ui = LoadComponent("EveryTask.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}

void CTaskLayer::onEnter()
{ 
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(VLEFT+50, VTOP-50);
	pClose->setOnClickListener(this,ccw_click_selector(CTaskLayer::onClose));
	this->addChild(pClose, 999);

	m_cell = (CLayout*)(m_ui->findWidgetById("Cell"));
	m_cell->retain();
	m_ui->removeChild(m_cell);

	m_tableView = (CTableView *)(m_ui->findWidgetById("scroll"));
	m_tableView->setDirection(eScrollViewDirectionVertical);
	m_tableView->setSizeOfCell(m_cell->getContentSize());
	//m_tableView->setSizeOfCell(CCSizeMake(790,115));
	m_tableView->setCountOfCell(0);
	m_tableView->setBounceable(false);
	m_tableView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(CTaskLayer::tableviewDataSource));
	m_tableView->reloadData();

	//m_ui->setScale(0.0f);
	//CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	//m_ui->runAction(big);

	CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	for (int i = 0; i < 2; i++)
	{
		CRadioButton *radioBtn= (CRadioButton*)(radioGroup->getChildByTag(i+1));
		radioBtn->setOnCheckListener(this,ccw_check_selector(CTaskLayer::onSwitchBtn));
	}

	GetTcpNet->registerMsgHandler(TaskListMsg,this,CMsgHandler_selector(CTaskLayer::processNetMsg));
	CSceneManager::sharedSceneManager()->addMsgObserver(DELETE_TASK,this,GameMsghandler_selector(CTaskLayer::deleteTaskMsg));

	//GetTcpNet->registerMsgHandler(ComposeItemMsg,this,CMsgHandler_selector(CTaskLayer::processNetMsg));
// 	CCPoint destPos = m_ui->getPosition();
// 	m_ui->setPositionY(destPos.y + VCENTER.y);
// 	CCMoveTo *move = CCMoveTo::create(0.5f,destPos);
// 	CCEaseBounceOut *out = CCEaseBounceOut::create(move);
// 	m_ui->runAction(out);

	CButton *read = (CButton *)(m_ui->findWidgetById("readAll"));
	read->setOnClickListener(this,ccw_click_selector(CTaskLayer::onReadAllMail));
}

void CTaskLayer::onReadAllMail(CCObject* pSender)
{
	CTaskControl::getInstance()->sendGetTaskPrize(0);
	m_selectTask = nullptr;
}

void CTaskLayer::onSwitchBtn(CCObject *pSender, bool bChecked)
{
	CRadioButton *btn = (CRadioButton*)pSender;

	if (bChecked)
	{
		PlayEffectSound(SFX_Button);
		m_taskType = btn->getTag();

		m_taskList.clear();

		if (btn->getTag()==1)
		{
			isFinishTask(false);
			CTaskControl::getInstance()->sendTaskList(1);
			CCNode *redPoint = m_ui->findWidgetById("redPoint");
			redPoint->setVisible(false);
		}
		else
		{
			isFinishTask(true);
			CTaskControl::getInstance()->sendTaskList(2);
			CCNode *redPoint1 = m_ui->findWidgetById("redPoint1");
			redPoint1->setVisible(false);
		}
		CCNode *bg1 = m_ui->findWidgetById("bg1");
		CCNode *bg2 = m_ui->findWidgetById("bg2");
		CCNode *task1 = m_ui->findWidgetById("task1");
		CCNode *task2 = m_ui->findWidgetById("task2");
		bg1->setVisible(btn->getTag()==1);
		bg2->setVisible(btn->getTag()==2);
		task1->setVisible(btn->getTag()==1);
		task2->setVisible(btn->getTag()==2);		
	}
}

void CTaskLayer::toDoTasking(float dt)
 {
	 CRadioBtnGroup *radioGroup = (CRadioBtnGroup *)m_ui->getChildByTag(10);
	 ((CRadioButton*)(radioGroup->getChildByTag(1)))->setChecked(true);
	 ((CRadioButton*)(radioGroup->getChildByTag(2)))->setChecked(false);
 }

CCObject* CTaskLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
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

void CTaskLayer::onClose(CCObject* pSender)
{
	PlayEffectSound(SFX_Ensure);

	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}

void CTaskLayer::onExit()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(DELETE_TASK,this);
	BaseLayer::onExit();
	//NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CSceneManager::sharedSceneManager()->PostMessageA(SHOW_HEAD,0,nullptr,nullptr);
}


void CTaskLayer::onGetTaskPrize(CCObject* pSender)
{
 	CImageView* prize = (CImageView*)pSender;
	m_selectTask =(CTask*) prize->getUserData();
	if (m_selectTask)
	{
		CTaskControl::getInstance()->sendGetTaskPrize(m_selectTask->taskId);
		m_selectTask->finish = false;
	}
}

void CTaskLayer::onGoTask(CCObject* pSender)
{
	PlayEffectSound(SFX_Button);

	CButton* go = (CButton*)pSender;
	TaskData *task =(TaskData*)go->getUserData();
	// csv/task.csv
	switch (task->type)
	{
		//领取勾玉
	case 1:
		{
			CVipLayer *vipLayer= CVipLayer::create();
			if(LayerManager::instance()->push(vipLayer))
			{
				GetTcpNet->sendDataType(VipInfoMsg,true);
				GetTcpNet->sendDataType(VipShopMsg,true);
			}
		}
		break;
		//购买铜钱
	case 2:
		{
			//LayerManager::instance()->pop();
			//LayerManager::instance()->pop();
			BuyResource* bufLayer = BuyResource::create();
			bufLayer->setExchangeType(BUY_COIN);
			LayerManager::instance()->push(bufLayer);
			CPlayerControl::getInstance().sendConvert(1);
			return;
		}
		break;
		//抽取武将
	//case 115:
	case 3:
		{
			CRecruit *recr = CRecruit::create();
			LayerManager::instance()->push(recr);
		}
		break;
		//普通副本
	case 4:
		{
			CTollgateLayer* pLayer = CTollgateLayer::create();
			LayerManager::instance()->push(pLayer);
			//pLayer->setStory(true);
			CPlayerControl::getInstance().sendChapterList(0);
		}
		break;
		//精英副本
	case 5:
		{ 
 			CTollgateLayer *tollgate = CTollgateLayer::create();
 			LayerManager::instance()->push(tollgate);
			tollgate->setExitWithNoHardChapter(true);
			tollgate->setStory(false);
 			CPlayerControl::getInstance().sendChapterList(1);
		}
		break;
		//升级武将
	case 6:
		{
			CHeroControl *heroctl = CHeroControl::create();
			LayerManager::instance()->push(heroctl);
			CPlayerControl::getInstance().sendHeroList(1);
		}
		break;
		//强化装备
	case 7:
		{
			if (!CMainCityControl::getInstance()->isCityOpen(4))
			{
				ShowPopTextTip(GETLANGSTR(202));
				return;
			}
			LayerManager::instance()->push(CStrengthen::create());
		}
		break;
		//洗练装备
	case 8:
		{
			if (!CMainCityControl::getInstance()->isCityOpen(4))
			{
				ShowPopTextTip(GETLANGSTR(202));
				return;
			}
			CStrengthen* pStrengthen = CStrengthen::create();
			LayerManager::instance()->push(pStrengthen);
			pStrengthen->changeToStrengthenWithType(StrengthenSmelt);
		}
		break;
		//悬赏府
	case 9:
		{
			if (!CMainCityControl::getInstance()->isCityOpen(15))
			{
				ShowPopTextTip(GETLANGSTR(202));
				return;
			}
			CRewardLayer *reward = CRewardLayer::create();
			LayerManager::instance()->push(reward);
			GetTcpNet->sendDataType(BountyDataMsg,true);
		}
		break;
		//培植区
	case 10:	
		{
			if (!CMainCityControl::getInstance()->isCityOpen(6))
			{
				ShowPopTextTip(GETLANGSTR(202));
				return;
			}
			CCultivateLayer *cultivateLayer = CCultivateLayer::create();
			LayerManager::instance()->push(cultivateLayer);
			CMainCityControl::getInstance()->sendCityInfo(6);
		}
		break;
		//进阶装备
	case 11:
		{
			if (!CMainCityControl::getInstance()->isCityOpen(4))
			{
				ShowPopTextTip(GETLANGSTR(202));
				return;
			}
			CStrengthen* pStrengthen = CStrengthen::create();
			LayerManager::instance()->push(pStrengthen);
			pStrengthen->changeToStrengthenWithType(StrengthenStarUp);
		}
		break;
	default:
		break;
	}
}

void CTaskLayer::showTaskView()
{
	//if(m_taskList.size()==0)
	//{
	//	m_tableView->setCountOfCell(0);
	//	m_tableView->reloadData();
	//	return;
	//}

	std::sort(m_taskList.begin(), m_taskList.end(), SortTask);
	m_tableView->setCountOfCell(m_taskList.size());
	m_tableView->reloadData();
}

void CTaskLayer::onSelectTask(CCObject* pSender)
{
// 	CButton *btn = (CButton*)pSender;
// 	m_selectCellImg->removeFromParent();
// 
// 	btn->addChild(m_selectCellImg);
// 	m_selectCellImg->setVisible(true);
// 	m_selectCellImg->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
// 
// 	m_selectTask = (CTask *)btn->getUserData();
//  	m_selectIndex = btn->getTag();
// 	updateTaskInfo(m_selectTask);

}

void CTaskLayer::processNetMsg(int type, google::protobuf::Message *msg)
{
	if(!isVisible() && m_bFirstShow)
	{
		this->setVisible(true);
		//NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
	}
	
	switch (type)
	{
	case TaskListMsg:
		{
			TaskListResponse *res = (TaskListResponse*)msg;
			m_taskList.clear();
			m_taskAllList.clear();

// 			if (m_bFirstShow)
			{		
				showTaskTip(res);
				m_bFirstShow = false;
			}


			for (int i = 0; i < res->tasklist_size(); i++)
			{
				CTask task;
				task.read(res->tasklist(i));
				m_taskAllList.push_back(task);
			}
// 			vector<int> finishVec;
			for (int i = 0; i < res->tasklist_size(); i++)
			{
				CTask &task = m_taskAllList.at(i);
				if (task.finish)
				{
					m_taskList.push_back(task);
				}
			}

			for (int i = 0; i < res->tasklist_size(); i++)
			{
				CTask &task = m_taskAllList.at(i);
				if (!task.finish)
				{
					m_taskList.push_back(task);
				}
			}

			//如果没有任务
// 			if(res->tasklist_size()<=0)
// 			{
// 				onClose(nullptr);			
// 				ShowPopTextTip(GETLANGSTR(1022));
// 				return;
// 			}
			if(res->tasklist_size()<=0)
			{
				CCNode *bg1 = m_ui->findWidgetById("bg1");
				showNoTaskTip(true, bg1->isVisible()?GETLANGSTR(1239):GETLANGSTR(1238));
			}
			else
			{
				showNoTaskTip(false, nullptr);
			}

			isFinishTask(false);
			showTaskView();
			int size = m_taskList.size();
			for (int i = 0; i <size&&i<4; i++)
			{
				CTableViewCell *pCell = m_tableView->cellAtIndex(i);
				pCell->setPositionY(pCell->getPositionY()-400);
				pCell->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.1*i),CCEaseElasticInOut::create(CCMoveBy::create(0.3f,ccp(0,400)))));
			}
			CCSprite *frontBg = (CCSprite*)(m_ui->findWidgetById("frontBg"));
			int times = size>4?4:size;
			frontBg->runAction(CCRepeat::create(CCSequence::create(CCMoveBy::create(0.05f,ccp(0,10)),CCMoveBy::create(0.05f,ccp(0,-10)),CCDelayTime::create(0.1f),nullptr),times));
		}
		break;

	default:
		break;
	}
}



void CTaskLayer::updateTaskInfo(CTask* data)
{

	const TaskData *taskData =  DataCenter::sharedData()->getTask(Normal_Task)->getTaskCfg(data->tindex);
	CLabel *content = (CLabel*)(m_ui->findWidgetById("content"));
	content->setString(taskData->taskDesc.c_str());

// 	CLabel *itemDesc = (CLabel*)(m_ui->findWidgetById("target"));
// 	itemDesc->setString(data->target.);

	CButton *btn = (CButton *)(m_ui->findWidgetById("strengthen"));
	btn->setDisabledImageColor();

	CLabel *process = (CLabel*)(m_ui->findWidgetById("process"));
	process->setString(CCString::createWithFormat("%d/%d",data->process,data->target)->getCString());

	CLabel *exp = (CLabel*)(m_ui->findWidgetById("exp"));
	exp->setString(ToString(data->exp));

	CLabel *coin = (CLabel*)(m_ui->findWidgetById("coin"));
	coin->setString(ToString(data->coin));

	CLabel *food = (CLabel*)(m_ui->findWidgetById("food"));
	food->setString(ToString(data->food));

	CCSprite *mask = (CCSprite*)(m_ui->findWidgetById("mask"));
	mask->removeChildByTag(1);

	for (int i=5; i>data->prizeList.size();i--)
	{
		CButton *btn = (CButton*)(m_ui->findWidgetById(CCString::createWithFormat("item%d",i)->getCString()));
		btn->removeAllChildren();
		btn->setVisible(false);
	}

	for (int i=1; i <=5&&i<=data->prizeList.size(); i++)
	{
		CPrize &prize = data->prizeList[i-1];
		CButton *btn = (CButton*)(m_ui->findWidgetById(CCString::createWithFormat("item%d",i)->getCString()));
		btn->setVisible(true);
		btn->removeAllChildren();

		CCSprite *mask = CCSprite::create();
		SmartSetRectPrizeColor(mask, &prize);
		mask->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
		btn->addChild(mask);

		CCSprite *prop = CCSprite::create(CCString::createWithFormat("prop/%d.png", prize.thumb)->getCString());
		if (prop)
		{
			prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
			btn->addChild(prop,0,1);
		}
		else 
		{
			prop = CImageView::create("prop/32003.png");
			prop->setPosition(ccp(btn->getContentSize().width/2,btn->getContentSize().height/2));
			btn->addChild(prop,0,1);
		}
	}

	CButton *get = (CButton*)(m_ui->findWidgetById("strengthen"));
	CCSprite *getimg = (CCSprite*)(m_ui->findWidgetById("getimg"));
	if (data->finish)
	{
		get->setVisible(true);
		getimg->setVisible(true);
	}else
	{
		get->setVisible(false);
		getimg->setVisible(false);
	}
}

void CTaskLayer::addTableCell(unsigned int uIdx, CTableViewCell * pCell)
{
	if(m_taskList.size()==0) return;

	CTask *data = &m_taskList.at(uIdx);
	const TaskData *taskData =  DataCenter::sharedData()->getTask(Normal_Task)->getTaskCfg(data->tindex);
	if (!taskData)
	{
		return;
	}

	CLayout *lay = UICloneMgr::cloneLayout(m_cell);
	for (int i=1;i<=13;++i)
	{
		CCNode *child = lay->getChildByTag(i);
		lay->removeChild(child);
		pCell->addChild(child);
			
		switch (i)
		{
		case 2:
			{
				CCSprite *item = CCSprite::create(CCString::createWithFormat("headImg/%d.png",taskData->icon)->getCString());
				if (item)
				{
					item->setPosition(ccp(child->getContentSize().width/2,child->getContentSize().height/2));
					child->addChild(item);
					NodeFillParent(item);
				}
			}
			break;
		case 3:
			{
				CLabel *name = (CLabel*)child;
				name->setString(taskData->taskName.c_str());
			}
			break;
		case 4:
			{
				CLabel *desc = (CLabel*)child;
				desc->setString(taskData->taskDesc.c_str());
			}
			break;
	
		case 6:
			{
				CLabel *exp = (CLabel*)child;
				exp->setString(U8(CCString::createWithFormat("×%d",data->exp)->getCString())); 
			}
			break;
		case 7:
			{
				if (data->coin==0&&data->gold>0)
				{
					CImageView *img = (CImageView*)child;
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage("common/gold.png"));
					img->setScaleX(0.6f);
				}
				else if (data->coin==0&&data->action>0)
				{
					CImageView *img = (CImageView*)child;
					img->setTexture(CCTextureCache::sharedTextureCache()->addImage("common/luo.png"));
				}
				else if (data->coin==0&&data->gold==0)
				{
					child->setVisible(false);
				}
			}
			break;
		case 8:
			{
				CLabel *coin = (CLabel*)child;
				if (data->coin>0)
				{			
					coin->setString(U8(CCString::createWithFormat("×%d",data->coin)->getCString())); 
				}
				else if (data->gold>0)
				{
					coin->setString(U8(CCString::createWithFormat("×%d",data->gold)->getCString())); 
				}
				else if (data->action>0)
				{
					coin->setString(U8(CCString::createWithFormat("×%d",data->action)->getCString())); 
				}
				else
				{
					child->setVisible(false);
				}
			}
			break;
		case 9:
			{
				CButton *go =(CButton*)child;
				//go->setUserData(&m_taskList.at(uIdx));
				go->setVisible(taskData->type!=0);
				go->setUserData((void*)taskData);
				go->setOnClickListener(this,ccw_click_selector(CTaskLayer::onGoTask));
			}
			break;
		case 10:
			{
				child->setVisible(taskData->type!=0);
			}
			break;
		case 12:
			{
				if (data->process==0&&data->target==0)
				{
					child->removeFromParent();
				}
			}
			break;
		case 13:
			{
				if (data->process==0&&data->target==0)
				{
					child->removeFromParent();
				}
				else
				{
					CLabel *target = (CLabel*)child;
					target->setString(CCString::createWithFormat("%d/%d",data->process,data->target)->getCString()); 
				}
			}
		    break;
		default:
			break;
		}
	}

	//额外主角经验标签
	CCNode *child = lay->getChildByTag(17);
	lay->removeChild(child);
	pCell->addChild(child);

	if (data->finish)
	{
		pCell->getChildByTag(9)->setVisible(false);
		pCell->getChildByTag(10)->setVisible(false);
		CImageView *getPrize = (CImageView*)(pCell->getChildByTag(11));
		getPrize->setVisible(true);
		getPrize->setUserData(&m_taskList.at(uIdx));
		getPrize->setTouchEnabled(true);
		getPrize->setOnClickListener(this, ccw_click_selector(CTaskLayer::onGetTaskPrize));
		getPrize->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.1f), CCScaleTo::create(0.2f, 1.0f))));

		CImageView *bgPrize = (CImageView *) pCell->getChildByTag(1);;
		bgPrize->setUserData(&m_taskList.at(uIdx));
		bgPrize->setTouchEnabled(true);
		bgPrize->setOnClickListener(this, ccw_click_selector(CTaskLayer::onGetTaskPrize));
	}

	if (data->getTime>0)
	{
		pCell->getChildByTag(9)->setVisible(false);
		pCell->getChildByTag(10)->setVisible(false);
// #ifdef _WIN32
// 			
// 		time_t tnow = time(nullptr);
// 		t = t - tnow;
// #else
// 		time_t t =  data->getTime;
// 		struct cc_timeval now;  
// 		CCTime::gettimeofdayCocos2d(&now, NULL);  
// 		time_t tnow = now.tv_sec*1000 + now.tv_usec/1000;
// 		t = (t - tnow)/1000;
// #endif
		time_t t  = data->getTime/1000;		
		int  hour = floor(t / 3600);
		int min = floor((t - hour * 3600) / 60);
		int sec = floor(t - hour * 3600 - min * 60);
		int day =  floor(hour/24);

		CCNode *dikuang = lay->getChildByTag(14);
		lay->removeChild(dikuang);
		pCell->addChild(dikuang);

		CLabel *timeLab = (CLabel*)(lay->getChildByTag(15));
		if (day>0)
		{
			hour =  hour - 24 * day; 
			timeLab->setString(CCString::createWithFormat(GETLANGSTR(232),day,hour)->getCString());
		}
		else
		{
			timeLab->setString(CCString::createWithFormat(GETLANGSTR(231),hour,min)->getCString());
		}
		lay->removeChild(timeLab);
		pCell->addChild(timeLab);
	}
	
	CCSprite *mask = (CCSprite*)lay->getChildByTag(16);
	CCSprite* pSpecialMask = nullptr;
	if (taskData->mask==2)
	{
		 pSpecialMask = CCSprite::create("common/mask_box.png");
		pSpecialMask->setAnchorPoint(CCPointZero);
		pSpecialMask->setTag(16);
		pSpecialMask->setPosition(ccp(mask->getPositionX()-19, mask->getPositionY()-7));
	}
	lay->removeChild(mask);
	if(pSpecialMask)
	{
		mask = pSpecialMask;
	}
	pCell->addChild(mask);
}

void CTaskLayer::isFinishTask(bool isFinish)
{
// 	for (int i = 0; i < m_taskAllList.size(); i++)
// 	{
// 		CTask &task = m_taskAllList[i];
// 		if (task.finish == isFinish)
// 		{
// 			m_taskList.push_back(&m_taskAllList[i]);
// 		}
// 	}
}

void CTaskLayer::deleteTaskMsg(const TMessage& tMsg)
{
	if (m_selectTask)
	{
		vector<CTask>::iterator iter = m_taskList.begin();

		for (; iter!=m_taskList.end(); iter++)
		{
			if (m_selectTask->taskId==iter->taskId)
			{
				m_taskList.erase(iter);
				break;
			}
		}
		showTaskView();
	}
	else
	{
		vector<CTask>::iterator iter = m_taskList.begin();
		for (; iter!=m_taskList.end(); )
		{
			if (iter->finish)
			{
				iter = m_taskList.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		showTaskView();
		showReadAll(false);
		CCNode *redPoint = m_ui->findWidgetById("redPoint");
		CCNode *redPoint1 = m_ui->findWidgetById("redPoint1");
		redPoint->setVisible(false);
		redPoint1->setVisible(false);
	}
}

void CTaskLayer::popItemPrize(PrizeResponse* res)
{
	//Mr.yan需要数据对接，暂时屏蔽
	CGetPrizeRes prizers;
	prizers.result =1;
	for (int i = 0; i < res->prizes_size(); i++)
	{  
		CPrize prize;
		prize.read(res->prizes(i));
		prizers.prizeList.push_back(prize);
	}

	//多加一个主角经验在上面
// 	CPrize prize;
// 	prize.id = 10;
// 	prize.num = m_selectTask->exp;
// 	prize.color = 0;
// 	prize.type = 1;
// 	prize.thumb = 10;
// 	prize.quality = 0;
// 	prizers.prizeList.push_back(prize);

	CPopItem *popItem = CPopItem::create();
	LayerManager::instance()->push(popItem);
	popItem->popPrizeRes(&prizers);

	PlayEffectSound(SFX_432);

	//收集货币, 上层直接调用了role去更新
	//collectMoneyFromPrize(prizers);
}

int CTaskLayer::getTaskType()
{
	return m_taskType;
}

void CTaskLayer::showTaskTip(TaskListResponse * res)
{
	if (res->has_dailytips())
	{
		CCNode *redPoint = m_ui->findWidgetById("redPoint1");
		CCObject *usobj = redPoint->getUserObject();
		if (usobj)
		{
			bool b = ((CCBool*)usobj)->getValue();
			if (b!=res->dailytips()&&!b)
			{
				redPoint->setVisible(true);
			}
			else
			{
				redPoint->setVisible(false);
			}
		}
		else redPoint->setVisible(res->dailytips());
		if (m_taskType==2)
		{
			redPoint->setUserObject(CCBool::create(res->dailytips()));
		}
	}
	if (res->has_maintips())
	{
		CCNode *redPoint = m_ui->findWidgetById("redPoint");
		CCObject *usobj = redPoint->getUserObject();
		if (usobj)
		{
			bool b = ((CCBool*)usobj)->getValue();
			if (b!=res->maintips()&&!b)
			{
				redPoint->setVisible(true);
			}
			else
			{
				redPoint->setVisible(false);
			}
		}
		else redPoint->setVisible(res->maintips());
		if (m_taskType==1)
		{
			redPoint->setUserObject(CCBool::create(res->maintips()));
		}
	}

	if (m_taskType==1)
	{	
		showReadAll(res->maintips()||res->dailytips());
	}
}

void CTaskLayer::showNoTaskTip( bool bShow, const char* sInfo )
{
	if(bShow)
	{
		if(m_pPanelTips==nullptr)
		{
			m_pPanelTips = CPanelTips::create();
			this->addChild(m_pPanelTips);
		}
		m_pPanelTips->setVisible(true);
		m_pPanelTips->setString(sInfo);
	}
	else
	{
		if(m_pPanelTips!=nullptr)
		{
			m_pPanelTips->setVisible(false);
		}
	}
}

void CTaskLayer::showReadAll(bool isVisible)
{
	CButton *readAll = (CButton *)(m_ui->findWidgetById("readAll"));
	CLabel *readtext = (CLabel *)(m_ui->findWidgetById("readtext"));
	readAll->setVisible(isVisible);
	readtext->setVisible(isVisible);
}
