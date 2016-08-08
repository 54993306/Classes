
#include "EmailControl.h"
#include "net/CNetClient.h"
#include "tools/ShowTexttip.h"
#include "model/DataCenter.h"
#include "GMessage.h"
#include "sign/PopItem.h"

CEmailControl::CEmailControl():m_opType(0)
{

}

CEmailControl::~CEmailControl()
{

}

bool CEmailControl::init()
{
	if (CCNode::init())
	{
// 		MaskLayer* lay = MaskLayer::create("MaskLayer2");
// 		lay->setContentSize(CCSizeMake(1138,640));
// 		LayerManager::instance()->push(lay);

// 		m_emailLayer = CEmailLayer::create();
// 		int priop = LayerManager::instance()->getCurrLayer()->getTouchPriority()-1;
// 		m_emailLayer->setTouchPriority(priop);
// 		this->addChild(m_emailLayer);
// 		m_emailLayer->setDelegate(this);
		return true;
	}
	return false;
}
void CEmailControl::onEnter()
{
	CCNode::onEnter();
	GetTcpNet->registerMsgHandler(MailListMsg,this,CMsgHandler_selector(CEmailControl::ProcessMsg));
	GetTcpNet->registerMsgHandler(MaillReqMsg,this,CMsgHandler_selector(CEmailControl::ProcessMsg));
	GetTcpNet->registerMsgHandler(SendMailMsg,this,CMsgHandler_selector(CEmailControl::ProcessMsg));
}

void CEmailControl::onExit()
{
	CCNode::onExit();
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CEmailControl::sendToMail(int friendId,char *title, char* content)
{
	SendMailRequest *req = new SendMailRequest;
	req->set_friendid(friendId);
	req->set_title(title);
	req->set_content(content);
	GetTcpNet->sendData(req,SendMailMsg);
	delete req;
}

void CEmailControl::sendMailReq(int mailId, int reqType)
{
	MaillRequest *req = new MaillRequest;
	req->set_mailid(mailId);
	req->set_ctrltype(reqType);
	GetTcpNet->sendData(req,MaillReqMsg);
	delete req;
}

void CEmailControl::sendMailList()
{
	GetTcpNet->sendDataType(MailListMsg,true);
}

void CEmailControl::ProcessMsg(int type, google::protobuf::Message *msg)
{
	if (type==MailListMsg)
	{
		MailListResponse *res = (MailListResponse*)msg;
		for (int i = 0; i < res->maillist_size(); i++)
		{
			CMail mail;
			mail.read(res->maillist(i));
			m_mailList.push_back(mail);
		}
		m_emailLayer->reloadEmailTableView(&m_mailList);
	}
	else if (type==MaillReqMsg)
	{
	    MailResponse *res = (MailResponse*)msg;

		//1操作成功
		if (res->result()==1)
		{
			if (res->has_mail()&&m_opType==3)
			{
				m_selectMail->read(res->mail());
				m_emailLayer->showMailContent(*m_selectMail);
				m_emailLayer->reloadEmailTableView(&m_mailList);
				m_selectMail->readed = true;
			}
			else if (m_opType==1||m_opType==2)
			{
				vector<CMail>::iterator iter = m_mailList.begin();

				for (; iter!= m_mailList.end(); iter++)
				{
					if (iter->mailId == m_selectMail->mailId)
					{
						//3 铜钱，4 元宝，5 粮食
						UserData *user = DataCenter::sharedData()->getUser()->getUserData();
						for (int i = 0; i <m_selectMail->prize.prizeList.size(); i++)
						{
							if (m_selectMail->prize.prizeList.at(i).type==3)
							{
								user->setCoin(user->getCoin()+m_selectMail->prize.prizeList.at(i).num);
							}
							else if (m_selectMail->prize.prizeList.at(i).type==4)
							{
								user->setRoleGold(user->getRoleGold()+m_selectMail->prize.prizeList.at(i).num);
							}
							else if (m_selectMail->prize.prizeList.at(i).type==5)
							{
								user->setRoleFood(user->getRoleFood()+m_selectMail->prize.prizeList.at(i).num);
							}
							else if (m_selectMail->prize.prizeList.at(i).type==6)
							{
								user->setRoleAction(user->getRoleAction()+m_selectMail->prize.prizeList.at(i).num);
							}
							else if (m_selectMail->prize.prizeList.at(i).type==7)
							{
								user->setRolePoints(user->getRolePoints()+m_selectMail->prize.prizeList.at(i).num);
							}
						}
						CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
						m_mailList.erase(iter);
						break;
					}
				}
				m_emailLayer->reloadEmailTableView(&m_mailList);
			}
			else if (m_opType==5)
			{
				vector<CMail>::iterator iter = m_mailList.begin();
	
				UserData *user = DataCenter::sharedData()->getUser()->getUserData();
				for (int i = 0; i < res->prizelist_size(); i++)
				{
					const Prize &prize = res->prizelist(i);
					if (prize.type()==3)
					{
						user->setCoin(user->getCoin()+prize.num());
					}
					else if (prize.type()==4)
					{
						user->setRoleGold(user->getRoleGold()+prize.num());
					}
					else if (prize.type()==5)
					{
						user->setRoleFood(user->getRoleFood()+prize.num());
					}
				}
				CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
				m_mailList.clear();
				m_emailLayer->reloadEmailTableView(&m_mailList);

				CGetPrizeRes prizers;
				prizers.result =1;
				for (int i = 0; i < res->prizelist_size(); i++)
				{  
					CPrize prize;
					prize.read(res->prizelist(i));
					prizers.prizeList.push_back(prize);
				}
				//弹框
				CPopItem *popItem = CPopItem::create();
				LayerManager::instance()->push(popItem);
				popItem->popPrizeRes(&prizers);
			}
			else if (m_opType==4)
			{
				m_mailList.clear();
				m_emailLayer->reloadEmailTableView(&m_mailList);
			}
		}

		//，2背包空间不足，3道具已被提取，4数据错误
		switch (res->result())
		{
		case 2:
			ShowPopTextTip(GETLANGSTR(178));
			break;
		case 3:
			ShowPopTextTip(GETLANGSTR(234));
			break;
		case 4:
			ShowPopTextTip(GETLANGSTR(170));
			break;
		default:
			break;
		}
		
	}
	else if (type==SendMailMsg)
	{

	}
}

void CEmailControl::checkEMailCallBack(CMail *mail)
{
	m_opType = 3;
	m_selectMail =mail;
	this->sendMailReq(mail->mailId,3);
}

void CEmailControl::delEMailCallBack(CMail *mail)
{
	m_opType = 1;
	m_selectMail =mail;
	this->sendMailReq(mail->mailId,1);
}

void CEmailControl::delAllEMailCallBack()
{
	m_opType = 4;
	this->sendMailReq(0,1);
}

void CEmailControl::extractAllCallBack()
{
	m_opType = 5;
	this->sendMailReq(0,2);
}

void CEmailControl::extractMailCallBack(int mailId)
{
	m_opType = 2;
	this->sendMailReq(mailId,2);
}

void CEmailControl::sendEMailCallBack(int friendId,char *title, char* content)
{

}

void CEmailControl::replyEMailCallBack(const char* name)
{

}

void CEmailControl::closeMailView()
{
	LayerManager::instance()->pop();
}

void CEmailControl::setEmailLayer(CMailLayer *layer)
{
	m_emailLayer = layer;
}


