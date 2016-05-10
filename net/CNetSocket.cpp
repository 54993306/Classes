#include "CNetSocket.h"

bool InitSocket()
{
#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
	if (iResult!=0)
	{
		printf("WSAStartup socket failed");
		return false;
	}
	return true;
#endif // win
}

void ClearSocket()
{
#ifdef WIN32
	WSACleanup();
#endif // WIN32

}

CNetSocket::CNetSocket()
	:m_sock(-1),m_bUDP(false),m_bConnected(false),m_strHost(""),m_nPort(0)
{
}
CNetSocket::~CNetSocket()
{
	Close();
}
bool CNetSocket::Create(bool bUDP)
{
	m_bUDP = bUDP;
	InitSocket();
	if (!m_bUDP)
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
	else
	{
		m_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (m_sock != -1)
		{
			m_bConnected = true;
		}
	}
	return (m_sock != -1);
}

bool CNetSocket::Connect(const std::string& host, unsigned short port)
{
	if (m_sock == -1)
		return false;
	m_strHost = host;
	m_nPort = port;
	struct hostent * he = gethostbyname(host.c_str());
	if (he == NULL)
	{
		Close();
		return false;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr = *((struct in_addr*) he->h_addr);
	memset(sin.sin_zero, 0, 8);
	sin.sin_port = htons(port);
	if (connect(m_sock, (struct sockaddr *) &sin, sizeof(sin)))
	{
		Close();
		return false;
	}
	unsigned long rb=1;
#ifdef WIN32
	ioctlsocket(m_sock, FIONBIO, &rb);
#else
	ioctl(m_sock, FIONBIO, &rb);
#endif
	int bufsize = MAX_RECV_BUFFERSIZE;
	setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize));
	setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));
	//int timeout= 20000;
	//setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	m_bConnected = true;
	return true;
}


bool CNetSocket::isConnected()
{
	return m_bConnected&&(m_sock!=-1);
}

bool CNetSocket::isUDP()
{
	return m_bUDP;
}

bool CNetSocket::Bind(unsigned short nPort)
{
	if (m_sock == -1)
	{
		return false;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
#ifdef WIN32
	sin.sin_addr.S_un.S_addr=0;
#else
	sin.sin_addr.s_addr = 0;
#endif
	memset(sin.sin_zero, 0, 8);
	sin.sin_port = htons(nPort);
	if (::bind(m_sock, (sockaddr*) &sin, sizeof(sockaddr_in)) != 0)
		return false;
	listen(m_sock, 1024);
	m_bConnected = true;
	return true;
}

bool CNetSocket::Accept(CNetSocket& client)
{
	if (m_sock == -1)
	{
		return false;
	}
	client.m_sock = accept(m_sock, NULL, NULL);
	client.m_bConnected = true;
	return (client.m_sock != -1);
}

void CNetSocket::Close()
{
	if (m_sock != -1)
	{
#ifdef WIN32
		shutdown(m_sock,SD_BOTH);
		closesocket(m_sock);
#else
		shutdown(m_sock, SHUT_RDWR);
		close(m_sock);
#endif
		m_sock = -1;
	}
	m_bConnected = false;

}

int CNetSocket::Send(const char* buf, int buflen)
{
	int iSend = 0;
	if (m_sock!=-1)
	{
		do 
		{
			int len = send(m_sock,buf+iSend,buflen-iSend,0);
			if (len<0)
			{
				break;
			}
			iSend += len;
		} while (iSend<buflen);
		return  iSend;
	}
	return -1;
}

int CNetSocket::Recv(char* buf, int buflen)
{
	if (m_sock==-1)
	{
		return -1;
	}
/*
#ifdef WIN32
	if ( SOCKET_ERROR == setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (char* )&nTimeout, sizeof(nTimeout) ) )
	{
		return SOCKET_ERROR;
	}
#else
	timeval oVal = { nTimeout/1000, nTimeout%1000*1000 };
	if ( SOCKET_ERROR == setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (char* )&oVal, sizeof(oVal) ) )
	{
		return SOCKET_ERROR;
	}
#endif //WIN32
	*/
	return recv(m_sock,buf,buflen,0);
}

int CNetSocket::SendTo(const char* buf, int len,const struct sockaddr_in* toaddr, int tolen)
{
	return -1;
}

int CNetSocket::RecvFrom(char* buf, int len, struct sockaddr_in* fromaddr,int* fromlen)
{
	return -1;
}

bool CNetSocket::GetPeerName(std::string& strIP, unsigned short &nPort)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	int addrlen = sizeof(addr);
#ifdef WIN32
	if(getpeername(m_sock,(struct sockaddr*)&addr,&addrlen)!=0)
#else
	if (getpeername(m_sock, (struct sockaddr*) &addr, (socklen_t*) &addrlen)
		!= 0)
#endif
		return false;
	char szIP[64];
#ifdef WIN32
	sprintf(szIP,"%u.%u.%u.%u",addr.sin_addr.S_un.S_addr&0xFF,(addr.sin_addr.S_un.S_addr>>8)&0xFF,(addr.sin_addr.S_un.S_addr>>16)&0xFF,(addr.sin_addr.S_un.S_addr>>24)&0xFF);
#else
	sprintf(szIP, "%u.%u.%u.%u", addr.sin_addr.s_addr & 0xFF,
		(addr.sin_addr.s_addr >> 8) & 0xFF, (addr.sin_addr.s_addr >> 16)
		& 0xFF, (addr.sin_addr.s_addr >> 24) & 0xFF);
#endif
	strIP = szIP;
	nPort = ntohs(addr.sin_port);
	return true;
}

SOCKET CNetSocket::GetHandle()
{
	return m_sock;
}

int CNetSocket::SetBlock(bool bBlock)
{
	int bufsize = MAX_RECV_BUFFERSIZE;
	setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize));
	setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));

	int rt = -1;
	if(bBlock)
	{
		unsigned long rb = 1;
#ifdef WIN32
		rt = ioctlsocket(m_sock, FIONBIO, &rb);
#else
		rt = ioctl(m_sock, FIONBIO, &rb);
#endif
	}
	return rt;
}