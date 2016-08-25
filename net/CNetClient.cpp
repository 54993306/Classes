#include "CNetClient.h"
#include "netcontrol/CPlayerControl.h"
#include "CCTK/scenemanager.h"
#include "common/color.h"
#include "tools/ShowTexttip.h"
#include "model/DataCenter.h"
#include "scene/layer/LayerManager.h"
#include "guide/GuideManager.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "UserDefaultData.h"
#include "task/TaskControl.h"
#include "SDK/GamePlatfomDefine.h"
#include "scene/LoginScene.h"

//根据typeName反射对象
google::protobuf::Message* createMessage(const std::string& type_name)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor =google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
	if (descriptor)
	{
		const google::protobuf::Message* prototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}

int	readInt(const char *buffer)
{
	const char *m_Buffer = buffer;
	int addr = m_Buffer[3] & 0xff;
	addr |= ((m_Buffer[2] << 8) & 0xff00);
	addr |= ((m_Buffer[1] << 16) & 0xff0000);
	addr |= ((m_Buffer[0] << 24) & 0xff000000);
	return addr;
}

CNetClient* CNetClient::instance= nullptr;

CNetClient* CNetClient::getShareInstance()
{
	if (instance==nullptr)
	{
		instance = new CNetClient();
		instance->init();
		instance->onEnter();
	}
	return instance;
}


bool CNetClient::init()
{
	if (CCNode::init())
	{
		 CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this,0,false);
		 CPlayerControl::getInstance();
		 this->registerMsgHandler(Reconnect,this,CMsgHandler_selector(CNetClient::reconnectResult));
		 this->registerMsgHandler(DisconnectMsg,this,CMsgHandler_selector(CNetClient::disconnectResult));
	}
	return true;
}

void CNetClient::onCreate()
{
	CSceneManager::sharedSceneManager()->addLooper(this);
}

void CNetClient::onConnected()
{
	printf("net connected finish");
	CCLOG("connected\n");
	if (m_isReconnet&&m_reConnectType==ReconnEnd)
	{
		m_isReconnet = false;
		m_reConnectType = ReconnNull;
		
		int iRoleId = DataCenter::sharedData()->getUser()->getUserData()->getRoleID();
		CCLOG("roleId %d", iRoleId);

		LoginType iType = (LoginType)CCUserDefault::sharedUserDefault()->getIntegerForKey(LOGIN_TYPE, LoginTypeNull);

		switch (iType)
		{
		case LoginTypeNull:
		case LoginTypeTourist:
			{
				string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(USER_NAME);
				string sPsw = CCUserDefault::sharedUserDefault()->getStringForKey(PASSWORD);
				string sOpenId = CCUserDefault::sharedUserDefault()->getStringForKey(FACEBOOK_ID);
				this->sendLogin(sUserName, sPsw, "", true, iRoleId, sOpenId, G_PLATFORM_TARGET);
			}
			break;
		case LoginTypeFacebook:
			{
				string sOpenId = CCUserDefault::sharedUserDefault()->getStringForKey(FACEBOOK_ID);
				this->sendLogin("", "", "", true, iRoleId, sOpenId, G_PLATFORM_TARGET);
			}
			break;
		case LoginTypeGoogle:
			{
				string sUserName = CCUserDefault::sharedUserDefault()->getStringForKey(GOOGLE_USER_NAME);
				string sPsw = CCUserDefault::sharedUserDefault()->getStringForKey(GOOGLE_PASSWORD);
				this->sendLogin(sUserName, sPsw, "", true, iRoleId, "", G_PLATFORM_TARGET);
			}
			break;
		default:
			break;
		}

	}else{
		m_reConnectType = ReconnNull;
	}
	setIsCloseConn(false);
}

void CNetClient::onDisconnected()
{
	printf("ondisconnected\n");

	if (m_reConnectType!=ReconnNo&&!m_isCloseConn&&CSceneManager::sharedSceneManager()->getRunningScene()&&CSceneManager::sharedSceneManager()->getRunningScene()->getClassName()!="LoginScene")
	{
		m_isReconnet = true;
		m_reConnectType = ReconnBegin;
	}
	CSceneManager::sharedSceneManager()->closePopupBox("WaitLayer");
	/*connect();*/
// 	CSceneManager::sharedSceneManager()->closePopupBox("WaitLayer");
}

void CNetClient::onConnectError() { printf("on ConnectError\n"); }
///当连接超时时调用
void CNetClient::onConnectTimeout()
{
	printf("on connect time out\n");
	if (m_isReconnet&&m_reConnectType==ReconnEnd)
	{
		m_reConnectType = ReconnBegin;
	}
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
	ShowTexttip(GETLANGSTR(245),RGB_RED,0,VCENTER,0.0f,2.0f,0.0f,20.0f);
}

void CNetClient::loop() { this->drive(); }

void CNetClient::sendHeartData()
{
	Protocol *protocol = new Protocol();
	protocol->set_protocoltype(HeartMsg);
	CStream stream(protocol->SerializeAsString().c_str(),protocol->ByteSize());
	write(stream);
	printf("send heart");
	delete protocol;
}

//注册消息处理类
void CNetClient::registerMsgHandler(MessageHandler *handler)
{
	list<MessageHandler*>::iterator iter= m_HandlerList.begin();
	for ( ; iter!= m_HandlerList.end(); ++iter)
	{
		if ((*iter)==handler)
			break;
	}
	if (iter==m_HandlerList.end())
		m_HandlerList.push_back(handler);
}

void CNetClient::registerMsgHandler(int type, CCObject* obj, handlerFunc func)
{
	std::pair<CCObject*,handlerFunc> phandler(obj,func);
	m_msgHandlerMap[type] = phandler;
}

//销毁消息处理类
void CNetClient::unRegisterMsgHandler(MessageHandler *handler)
{
	list<MessageHandler*>::iterator iter = m_HandlerList.begin();
	for (; iter!= m_HandlerList.end(); ++iter)
	{
		if ((*iter)==handler)
			break;
	}
	if (iter!=m_HandlerList.end())
		m_HandlerList.erase(iter);
}

void CNetClient::unRegisterMsgHandler(int type,CCObject *handler)
{
	HandlerMap::iterator iter = m_msgHandlerMap.find(type);
	if (iter!=m_msgHandlerMap.end())
		m_msgHandlerMap.erase(iter);
}

void CNetClient::unRegisterAllMsgHandler(CCObject *handler)
{
	HandlerMap::iterator iter = m_msgHandlerMap.begin();
	while (iter!=m_msgHandlerMap.end())
	{
		if (iter->second.first==handler)
		{
			iter = m_msgHandlerMap.erase(iter);
		}else{
			++iter;
		}
	}
}

///当读到消息时调用
void CNetClient::onMessage(STREAM stream)
{
	if (stream.size()>=2)
	{
		Protocol *protocol = (Protocol*)createMessage("protos.Protocol");
		char *buffer = stream.flush();
		protocol->ParseFromArray(buffer,stream.size());	
		if (protocol->has_data())
		{
			int protoType = protocol->protocoltype();
			CSceneManager::sharedSceneManager()->closePopupBox("WaitLayer");
			this->unschedule(schedule_selector(CNetClient::closeWait));
			string typeName = "";
			int size = sizeof(protoDef)/sizeof(ProtoDefine);
			
			
			if (protoType<1000)
			{
				ProtoDefine pdf = protoDef[protoType];
				if (pdf.typeName!="")
				{
					typeName = pdf.typeName;
				}
			}
			else
			{
				for (int i=80;i<size;++i)
				{
					ProtoDefine pdf = protoDef[i];
					if (pdf.code==protoType&&pdf.typeName!="")
					{
						typeName = pdf.typeName;
						break;
					}
				}
			}
			if (typeName=="")
			{
				return;
			}
			google::protobuf::Message *message = createMessage(typeName);
			message->ParseFromString(protocol->data());
			//消息处理
			list<MessageHandler *>::iterator iter = m_HandlerList.begin();
			for (;iter!=m_HandlerList.end(); iter++)
			{
				if ((*iter)->ProcessMsg(protoType,message))
				{
					delete message;
					break;
				}
			}
			if (iter==m_HandlerList.end())
			{
				HandlerMap::iterator it = m_msgHandlerMap.find(protoType);
				if (it!=m_msgHandlerMap.end())
				{
					std::pair<CCObject*,handlerFunc> p = it->second;
					(p.first->*(p.second))(protoType,message);
				}else{
					printf("message type=%d doesn't process",protoType);
				}
				Role *role = dynamic_cast<Role*>(message);
				if (CGuideManager::getInstance()->getIsRunGuide()&&!role&&protoType!=Reconnect&&protoType!=TaskNoticeMsg&&protoType!=GameTipMsg&&protoType!=BulletMsg)
					CGuideManager::getInstance()->nextStep();

				saveRecvMsg(protoType, message, typeName);

				delete message;
				printf("recv data %d,%d:\n",protoType,stream.size());
			}
		}
		delete protocol;
	}
}

void CNetClient::update(float delta)
{
	m_interalTime += delta;
	m_deltTime += delta;
	if (m_interalTime>=30.0f)
	{
		Protocol *protocol = (Protocol*)createMessage("protos.Protocol");
		protocol->set_protocoltype(1);
		CStream stream(protocol->SerializeAsString().c_str(),protocol->ByteSize());
		write(stream);
		delete protocol;
		m_interalTime = 0.0;
	}
	if (m_isReconnet&&m_reConnectType==ReconnBegin)
	{
		m_reConnectType=ReconnEnd;
		connect();
	}
}

void CNetClient::sendData(google::protobuf::Message *msg,int type, bool isSave/*=false*/)
{
	if (m_msgType==HeroInfoMsg||m_msgType!=type||m_deltTime>0.55f)
	{
		Protocol *protocol = (Protocol*)createMessage("protos.Protocol");
		protocol->set_protocoltype(type);
		protocol->set_data(msg->SerializeAsString());
		CStream stream(protocol->SerializeAsString().c_str(),protocol->ByteSize());
		write(stream);
		delete protocol;

		CSceneManager::sharedSceneManager()->openPopupBox("WaitLayer",nullptr);
		this->scheduleOnce(schedule_selector(CNetClient::closeWait),5);
		m_deltTime = 0.0f;
		m_msgType = type;

		if (!this->isConnected()&&isSave)
		{
			Message *mesg = nullptr;
			if (type ==HeroInfoMsg)
			{
				mesg = getHeroInfoMsg(type,msg);
			}
			else
			{
				mesg = getSaveMsg(type);
			}
			if (mesg)
			{
				dispatchNetMessage(type,mesg);
			}
		}
	}
}

void CNetClient::sendDataType(int type,bool isSave)
{
	if (m_msgType!=type||m_deltTime>0.5f)
	{
		Protocol *protocol = (Protocol*)createMessage("protos.Protocol");
		protocol->set_protocoltype(type);
		CStream strem(protocol->SerializeAsString().c_str(),protocol->ByteSize());
		write(strem);
		delete protocol;

		if (type== RoleExitMsg)
		{
			m_reConnectType = ReconnNo;
		}

		CSceneManager::sharedSceneManager()->openPopupBox("WaitLayer",nullptr);
		this->scheduleOnce(schedule_selector(CNetClient::closeWait),5);
		m_deltTime = 0.0f;
		m_msgType = type;

		if (!this->isConnected()&&isSave)
		{
			Message *msg = getSaveMsg(type);
			if (msg)
			{
				dispatchNetMessage(type,msg);
			}
		}
	}
}

void CNetClient::closeWait(float delt)
{
	CSceneManager::sharedSceneManager()->closePopupBox("WaitLayer");
	//ShowTexttip("你的網絡環境不穩定，請稍後再嘗試",RGB_RED,0);
	//移除无效（无内容）的弹框
	LayerManager::instance()->removeInvalidPopLayer();
}                   

//登录
void CNetClient::sendLogin(string usrName, string usrPass, string access_code/*=""*/,bool isReconn /*= false*/, int roleId /*= 0*/, string openId/*="" */, int iPlatform/*=0*/)
{
	LoginRequest *msg = new LoginRequest();
	msg->set_username(usrName);
	msg->set_password(usrPass);
	if (access_code!="")
	{
		msg->set_access_code(access_code);
	}
	if (isReconn)
	{
		msg->set_reconnect(isReconn);
		msg->set_roleid(roleId);
	}
	if (openId!="")
	{
		msg->set_openid(openId);
	}
	msg->set_platform(iPlatform);
	sendData(msg,LoginResponseMsg);
	delete msg;
}

void CNetClient::sendInGameReq(int roleId)
{
	InGameRequest *inReq  = new InGameRequest();
	inReq->set_roleid(roleId);
	sendData(inReq,3);
	delete inReq;
}

//创建角色
void CNetClient::sendCreateRole(int roleNation, string roleName)
{
	CreateRequest *creq = new CreateRequest;
	creq->set_rolenation(roleNation);
	creq->set_rolename(roleName);
	sendData(creq,4);
	delete creq;
}

//角色移动
void CNetClient::sendRoleMove(int xPos, int yPos)
{
	MoveRequest *move = new MoveRequest;
	move->set_movex(xPos);
	move->set_movey(yPos);
	sendData(move, RoleMoveMsg);
	delete move;
}

//在线角色列表
void CNetClient::sendRoleList(int mapId)
{
	RoleListRequest *roleReq = new RoleListRequest;
	roleReq->set_mapid(mapId);
	sendData(roleReq,RoleListMsg);
	delete roleReq;
}

//关卡列表
void CNetClient::sendStageList(int chapter)
{
	StageListRequest *req = new StageListRequest;
	req->set_chapter(chapter);
	sendData(req,StageList);
	delete req;
}

void CNetClient::reconnectResult(int type, google::protobuf::Message *msg)
{
	if (type==Reconnect)
	{
		ReconnectResponse *res = (ReconnectResponse*)msg;
		if (!res->result())
		{
			m_reConnectType=ReconnNo;
			//重连失败原因(1 账号验证失败， 2 游戏角色错误， 3 重连超时)
			if (res->reason()==1)
			{
				CCLOG("1 账号验证失败");
			}
			else if (res->reason()==2)
			{
				CCLOG("2 游戏角色错误");
			}
			else if (res->reason()==3)
			{
				CCLOG("3 重连超时");
			}
			LayerManager::instance()->closeAll();
			string name = CSceneManager::sharedSceneManager()->getRunningScene()->getClassName();
			if (strcmp(name.c_str(),"BattleScene"))
				CCDirector::sharedDirector()->replaceScene(GETSCENE(LoginScene));
		}
	}
}

void CNetClient::saveRecvMsg(int protoType, google::protobuf::Message * message, string typeName)
{
	if (protoType == HeroInfoMsg)
	{
		Hero *hero = (Hero*)message;
		map<int,Message*>::iterator msgIter = m_heroInfoMap.find(hero->id());
		if (msgIter!=m_heroInfoMap.end())
		{
			msgIter->second->CopyFrom(*message);
		}
		else
		{
			Message *msg = createMessage(typeName);
			msg->CopyFrom(*message);
			m_heroInfoMap[hero->id()] = msg;
		}
	}
	else
	{
		map<int,Message*>::iterator msgIter = m_recvMsgMap.find(protoType);
		if (msgIter!=m_recvMsgMap.end())
		{
			msgIter->second->CopyFrom(*message);
		}
		else
		{
			Message *msg = createMessage(typeName);
			msg->CopyFrom(*message);
			m_recvMsgMap[protoType] = msg;
		}
	}
}

Message* CNetClient::getSaveMsg(int protoType)
{
	if (protoType!=HeroInfoMsg)
	{
		map<int,Message*>::iterator msgIter = m_recvMsgMap.find(protoType);
		if (msgIter!=m_recvMsgMap.end())
		{
			return msgIter->second;
		}
	}
	return nullptr;
}

Message* CNetClient::getHeroInfoMsg(int protoType, Message *msg)
{
	RequestHeroInfo *req = (RequestHeroInfo*)msg;
	map<int,Message*>::iterator msgIter = m_heroInfoMap.find(req->roleheroid());
	if (msgIter!=m_heroInfoMap.end())
	{
		return msgIter->second;
	}
	return nullptr;
}

void CNetClient::dispatchNetMessage(int protoType, google::protobuf::Message * message)
{
	list<MessageHandler *>::iterator iter = m_HandlerList.begin();
	for (;iter!=m_HandlerList.end(); iter++)
	{
		if ((*iter)->ProcessMsg(protoType,message))
		{
			delete message;
			break;
		}
	}
	if (iter!=m_HandlerList.end())
		return;
	HandlerMap::iterator it = m_msgHandlerMap.find(protoType);
	if (it!=m_msgHandlerMap.end())
	{
		std::pair<CCObject*,handlerFunc> p = it->second;
		(p.first->*(p.second))(protoType,message);
	}else{
		printf("message type=%d doesn't process",protoType);
	}
}

void CNetClient::setIsCloseConn(bool isClose)
{
	m_isCloseConn = isClose;
}

bool CNetClient::getIsCloseConn()
{
	return m_isCloseConn;
}


void CNetClient::disconnectResult(int type, google::protobuf::Message *msg)
{
	Disconnect *res = (Disconnect*)msg;
	//(0x00重复登录，被迫下线，0x01被管理员踢下线，0x02意外断线)
	m_reConnectType = ReconnNo;
	DataCenter::destroy();
	CTaskControl::getInstance()->resetGameTips();	
	CCDirector::sharedDirector()->replaceScene(GETSCENE(LoginScene));
}
