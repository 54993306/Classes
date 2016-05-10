#ifndef _CNET_CLIENT_H_
#define _CNET_CLIENT_H_

#include "CCTK/netbean.h"
#include "cocos2d.h"
#include <google/protobuf/message.h>

#include "CCTK/base.h"

#include "protos/protocol.h"

#include "protos/ProtoDefine.h"
#include "MessageHandler.h"

USING_NS_CC;
using namespace protos;
using namespace google::protobuf;

google::protobuf::Message* createMessage(const std::string& type_name);
int	readInt(const char *buffer);

typedef void (CCObject::*handlerFunc)(int type, google::protobuf::Message *msg);

#define CMsgHandler_selector(_selector) (handlerFunc)(&_selector)

class CNetClient: public CCNode, public CNetBean, public CLooper
{
public:
	
	~CNetClient(){}

	static CNetClient* getShareInstance();

	///当创建成功 开始连接时调用
	virtual void onCreate();
	///当连接成功时调用
	virtual void onConnected();
	///当断开连接时调用
	virtual void onDisconnected();
	///当连接错误时调用
	virtual void onConnectError();
	///当连接超时时调用
	virtual void onConnectTimeout();
	///当读到消息时调用
	virtual void onMessage(STREAM stream);

	void dispatchNetMessage(int protoType, google::protobuf::Message * message);

	void saveRecvMsg(int protoType, google::protobuf::Message * message, string typeName);

	void sendData(google::protobuf::Message *msg, int type, bool isSave=false);

	virtual void update(float delta);

	virtual bool init();

	virtual void loop();

	void sendHeartData();

	//关卡列表
	void sendStageList(int chapter=1);

	void sendDataType(int type,bool isSave = false);

	//注册消息处理类
	void registerMsgHandler(MessageHandler *handler);

	void registerMsgHandler(int type, CCObject* obj, handlerFunc func);

	//销毁消息处理类
	void unRegisterMsgHandler(MessageHandler *handler);
	void unRegisterMsgHandler(int type,CCObject *handler);
	void unRegisterAllMsgHandler(CCObject *handler);

	//进入游戏
	void sendInGameReq(int roleId);

	//创建角色
	void sendCreateRole(int roleNation, string roleName);
	//角色移动
	void sendRoleMove(int xPos, int yPos);

	//登录
	void sendLogin(string usrName, string usrPass, string access_code="", bool isReconn = false, int roleId = 0, string openId="" );  //isReconn 重连 

	//在线角色列表
	void sendRoleList(int mapId);

	void closeWait(float delt);	
	void setIsCloseConn(bool isClose);
	bool getIsCloseConn();
	//获取缓存消息
	Message* getSaveMsg(int protoType);

protected:
	void reconnectResult(int type, google::protobuf::Message *msg);
private:
	CNetClient():m_interalTime(0.0),m_isReconnet(false),m_reConnectType(ReconnNull),m_msgType(0),m_deltTime(0),m_isCloseConn(false)
	{
		
	}
	Message* getHeroInfoMsg(int protoType, Message *msg);
	void disconnectResult(int type, google::protobuf::Message *msg);
	static CNetClient* instance;
	float m_interalTime;

	list<MessageHandler *> m_HandlerList;

	typedef map<int, std::pair<CCObject*,handlerFunc>> HandlerMap;
	HandlerMap m_msgHandlerMap; // 消息处理函数表

	bool  m_isReconnet;

	enum {
		ReconnNull = 0, 
		ReconnBegin = 1,		//开始断线重连
		ReconnEnd = 2,			// 重连中
		ReconnNo = 3,			//不重连
	}m_reConnectType;

	int m_msgType;
	float m_deltTime;
	map<int,Message*> m_recvMsgMap;
	map<int,Message*> m_heroInfoMap;
	bool m_isCloseConn;
};
#define GetTcpNet CNetClient::getShareInstance()


#endif
