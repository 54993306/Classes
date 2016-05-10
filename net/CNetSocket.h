#ifndef _CNET_SOCKET_H_
#define _CNET_SOCKET_H_

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#define SOCKET int
#endif
#include <errno.h>
#include <string>

#define SOCKET_TIMEOUT 30
#define MAX_RECV_BUFFERSIZE 65535

bool InitSocket();
void ClearSocket();

class CNetSocket
{
public:
	SOCKET m_sock;
public:
	CNetSocket();
	virtual ~CNetSocket();
	bool Create(bool bUDP = false);
	//请求连接
	virtual bool Connect(const std::string& host, unsigned short port);
	virtual bool isConnected();
	virtual bool isUDP();
	virtual bool Bind(unsigned short nPort);
	virtual bool Accept(CNetSocket& client);
	virtual void Close();
	virtual int Send(const char* buf, int buflen);
	virtual int Recv(char* buf, int buflen);
	virtual int SendTo(const char* buf, int len,
		const struct sockaddr_in* toaddr, int tolen);
	virtual int RecvFrom(char* buf, int len, struct sockaddr_in* fromaddr,
		int* fromlen);
	virtual bool GetPeerName(std::string& strIP, unsigned short &nPort);
	SOCKET GetHandle();
	int SetBlock(bool bBlock = true);
private:
	bool m_bUDP;
	bool m_bConnected;
	std::string m_strHost;
	unsigned short m_nPort;
};

#endif
