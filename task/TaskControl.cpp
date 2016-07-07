#include "TaskControl.h"
#include "net/CNetClient.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "tools/ShowTexttip.h"
#include "CTaskLayer.h"
#include "scene/layer/LayerManager.h"
#include "mainCity/mainScene.h"

CGameTips::CGameTips()
	:mailTips(false),taskTips(false)
	,composeTips(false),lotteryTips(false)
	,heroTips(false),farmTips(false)
	,strenTips(false)
	,actTips(false)
	,shopTips(false)
	,bountyTips(false)
	,risingTips(false)
{

}

void CGameTips::read(const GameTips &gt)
{
	this->mailTips = gt.mailtips();
	this->taskTips = gt.tasktips();
	this->lotteryTips = gt.lotterytips();
	this->heroTips= gt.herotips();
	this->farmTips = gt.farmtips();
	this->strenTips = gt.strentips();	
	this->actTips = gt.acttips();
	this->shopTips = gt.shoptips();
	this->bountyTips = gt.bountytips();
	this->risingTips = gt.risingtips();
}


CTaskControl* CTaskControl::instance = nullptr;

CTaskControl* CTaskControl::getInstance()
{
	if (instance==nullptr)
	{
		instance = new CTaskControl;
	}
	return instance;
}

CTaskControl::CTaskControl()
{
	GetTcpNet->registerMsgHandler(TaskNoticeMsg,this,CMsgHandler_selector(CTaskControl::recvTaskNotice));
	GetTcpNet->registerMsgHandler(TaskPrizeMsg,this,CMsgHandler_selector(CTaskControl::recvTaskPrize));
	GetTcpNet->registerMsgHandler(MailNoticeMsg,this,CMsgHandler_selector(CTaskControl::recvMailNotice));
	GetTcpNet->registerMsgHandler(GameTipMsg,this,CMsgHandler_selector(CTaskControl::recvGameTips));

}

CTaskControl::~CTaskControl()
{
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CTaskControl::sendGetTaskPrize(int taskId)
{
	PrizeRequest *req = new PrizeRequest;
	req->set_taskid(taskId);
	GetTcpNet->sendData(req,TaskPrizeMsg);
	delete req;
}

void CTaskControl::recvTaskNotice(int type, google::protobuf::Message *msg)
{
	if (type==TaskNoticeMsg)
	{
		TaskNotice* res = (TaskNotice*)msg;
		CTaskLayer * taskLayer = dynamic_cast<CTaskLayer*>(LayerManager::instance()->getLayer("CTaskLayer"));
		if (taskLayer)
		{		
			sendTaskList(taskLayer->getTaskType());
			taskLayer->showReadAll(true);
		}
		else
		{
			CSceneManager::sharedSceneManager()->PostMessageA(TASK_NOTICE,res->param(),nullptr,nullptr);
		}
	}
}

void CTaskControl::recvMailNotice(int type, google::protobuf::Message *msg)
{
	if (type==MailNoticeMsg)
	{
		MailNotice* res = (MailNotice*)msg;
		CSceneManager::sharedSceneManager()->PostMessageA(MAIL_NOTICE,res->newmail(),nullptr,nullptr);
	}
}

void CTaskControl::recvTaskPrize(int type, google::protobuf::Message *msg)
{
	PrizeResponse *res = (PrizeResponse*)msg;
	int result = res->result();

	// 1 领取成功，2 任务未完成，3 已领取过奖励，4 背包没有空位，5 数据错误

	switch (result)
	{
	case 1:
		{
			CTaskLayer *taskLayer = (CTaskLayer*)LayerManager::instance()->getLayer("CTaskLayer");
			if (taskLayer)
			{
				taskLayer->popItemPrize(res);
			}
			CSceneManager::sharedSceneManager()->PostMessageA(DELETE_TASK,0,nullptr,nullptr);
		}
		break;
	case 2:
		ShowPopTextTip(GETLANGSTR(239));		
		break;
	case 3:
		ShowPopTextTip(GETLANGSTR(222));		
		break;
	case 4:
		ShowPopTextTip(GETLANGSTR(223));		
		break;
	case 5:
		ShowPopTextTip(GETLANGSTR(170));
		break;

	default:
		break;
	}

	if (res->has_role())
	{
		UserData *roleData = DataCenter::sharedData()->getUser()->getUserData();
		roleData->read(res->role());
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
	}
}

void CTaskControl::sendTaskList(int type)
{
	TaskListRequest *req = new TaskListRequest;
	req->set_type(type);
	GetTcpNet->sendData(req,TaskListMsg,true);
	delete req;
}

void CTaskControl::recvGameTips(int type, google::protobuf::Message *msg)
{
	GameTips *res = (GameTips*)msg;
	m_gameTips.read(*res);
	CMainScene * mainScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if (mainScene)
	{
		CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_GAMETIP,0,nullptr,nullptr);
	}
}

CGameTips * CTaskControl::getGameTips()
{
	return &m_gameTips;
}

void CTaskControl::resetGameTips()
{
	memset(&m_gameTips,0,sizeof(GameTips));
}


