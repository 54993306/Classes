#include "ActivityControl.h"
#include "net/CNetClient.h"
#include "protos/act_protocol.pb.h"

CActivityControl* CActivityControl::m_instance = nullptr;

CActivityControl* CActivityControl::getInstance()
{
	if(m_instance==nullptr)
	{
		m_instance = new CActivityControl;
	}
	return m_instance;
}

CActivityControl::CActivityControl()
{
	////绑定获取活动列表回调
	//GetTcpNet->registerMsgHandler(ActListMsg, this, CMsgHandler_selector(CActivityControl::recvActList));
	//绑定购买礼包回调
	//GetTcpNet->registerMsgHandler(BuyGiftMsg, this, CMsgHandler_selector(CActivityControl::recvBuyGiftMsg));

}

CActivityControl::~CActivityControl()
{
	//解绑回调
	GetTcpNet->unRegisterAllMsgHandler(this);
}

void CActivityControl::askForActList( int iType )
{
	ActListReq *req = new ActListReq;
	req->set_tab(iType);
	GetTcpNet->sendData(req, ActListMsg,true);
	delete req;
}

void CActivityControl::askForBuyGift( int iId, int iExId/*=0*/ )
{
	BuyGiftReq *req = new BuyGiftReq;
	req->set_actid(iId);
	if(iExId>0)
	{
		req->set_exid(iExId);
	}
	GetTcpNet->sendData(req, BuyGiftMsg);
	delete req;
}

void CActivityControl::recvActList( int type, google::protobuf::Message *msg )
{
	if(type==ActListMsg)
	{
		//ActListRes *res = (ActListRes*)msg;

		//int iRequestActivityType = res->tab();
		////请求另一个
		//askForBuyGift();
	}
}

void CActivityControl::recvBuyGiftMsg( int type, google::protobuf::Message *msg )
{
	//if(type==BuyGiftMsg)
	//{
	//	//MailNotice* res = (MailNotice*)msg;
	//	////场景数据更新
	//	//CSceneManager::sharedSceneManager()->PostMessageA(MAIL_NOTICE, res->newmail(),nullptr,nullptr);
	//}
}

