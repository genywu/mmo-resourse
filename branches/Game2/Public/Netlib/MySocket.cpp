//=============================================================================
/**
 *  file: MySocket.cpp
 *
 *  Brief:网络基类，提供基本的网络接口
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================


#include "stdafx.h"
#include "mmsystem.h"
#include "MySocket.h"
#include "MsgDescribe.h"
#include "MsgCry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

/// 默认所有通信无需加密
long CMySocket::s_lEncryptType = 0;

//------------------------------------------------------------------
// 初始化Winsock 2.0
//------------------------------------------------------------------
bool CMySocket::MySocketInit()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	int nResult = WSAStartup(wVersionRequested, &wsaData);
	if (nResult != 0)
	{
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"WSAStartup Failed!");
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"Is not Winsock 2.0!");
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------
// 程序退出时调用
//------------------------------------------------------------------
void CMySocket::MySocketClearUp()
{
	WSACleanup();
}

/////////////////////////////////////////////////////////////////////////////
// CMySocket Construction

CMySocket::CMySocket()
{
	m_lProtocol = SOCK_STREAM;
	strcpy(m_strPeerIP,"");
	m_dwPeerPort = 0;
	m_dwPeerIP = 0;

	m_hSocket = INVALID_SOCKET;
			
	m_dwLastRecvPort = 0;		// [UDP] 上一条收到的消息的端口
	m_strLastRecvIP[0] = 0;		// [UDP] 上一条收到的消息的IP

	m_bClose = false;
	m_bShutDown = false;
	m_lIndexID = 0;

	m_lFlag = 0;
}

CMySocket::~CMySocket()
{
	if( m_hSocket != INVALID_SOCKET )
	{
		int nRet = closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

//------------------------------------------------------------------
// 创建SOCKET
//------------------------------------------------------------------
bool CMySocket::Create(UINT nSocketPort,			// 端口
					   LPCTSTR lpszSocketAddress,	// 地址					   
					   int nSocketType,				// 类型( TCP:SOCK_STREAM UDP:SOCK_DGRAM )
					   long lEvent,
					   bool bAsyncSelect  //defaultly,the flag==false,if create for client socket,set the flag=true
					   )
{
	m_lProtocol = nSocketType;
	m_hSocket = socket(PF_INET, nSocketType, 0);	// 初始化套接字
	if( m_hSocket == INVALID_SOCKET )
	{
        PutErrorString(NET_MODULE,"%-15s Create Socket Error = %d",__FUNCTION__,WSAGetLastError());
		return FALSE;
	}

	if( !Bind(nSocketPort, lpszSocketAddress) )	// 绑定端口和地址
	{
        PutErrorString(NET_MODULE,"%-15s Create Socket Bind Error = %d",__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}


//创建 overlapped socket
bool CMySocket::CreateEx(UINT nSocketPort,LPCTSTR lpszSocketAddress,int nSocketType)
{
	m_lProtocol = nSocketType;
	//创建一个 overlapped socket
	m_hSocket = WSASocket(AF_INET,nSocketType,0,NULL,0,WSA_FLAG_OVERLAPPED);
		
	//创建出错
	if(m_hSocket == INVALID_SOCKET)
	{
        PutErrorString(NET_MODULE,"%-15s CreateEx Socket Error = %d",__FUNCTION__,WSAGetLastError());
		return FALSE;
	}

	//邦定端口和地址
	if(!Bind(nSocketPort, lpszSocketAddress))
	{
        PutErrorString(NET_MODULE,"%-15s CreateEx Socket Bind Error = %d",__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}


// 绑定端口和地址
bool CMySocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	LPSTR lpszAscii = (LPTSTR)lpszSocketAddress;
	sockAddr.sin_family = AF_INET;

	if (lpszAscii == NULL)
	{
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		ulong lResult = inet_addr(lpszAscii);
		if (lResult == INADDR_NONE)
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
		sockAddr.sin_addr.s_addr = lResult;
	}

	sockAddr.sin_port = htons((u_short)nSocketPort);

	long hr = bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if(  hr == SOCKET_ERROR )
	{
        PutErrorString(NET_MODULE,"%-15s Bind Error = %d",__FUNCTION__,WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}


//设置socket选项
void CMySocket::SetSocketOpt()
{
	if( m_hSocket == INVALID_SOCKET)	return;
	//关闭Socket的Nagle算法
	int bNodelay = 1; 
	int hr = setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&bNodelay,sizeof(bNodelay));//不采用延时算法 
	if(  hr == SOCKET_ERROR )
	{
        PutErrorString(NET_MODULE,"%-15s Disables the Nagle algorithm Error = %d",__FUNCTION__,WSAGetLastError());
	}
	return;
}


//得到本机的IP
void CMySocket::SetHostName()
{
	// 获取本机IP地址
	char strName[128];
	SOCKADDR_IN sockAddr;
	if(SOCKET_ERROR ==gethostname(strName, 128))
	{
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,WSAGetLastError());
		return;
	}
	LPHOSTENT lphost;
	lphost = gethostbyname(strName);
	if (lphost != NULL)
	{
		sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		SetPeerIP( sockAddr.sin_addr.S_un.S_addr );
		sprintf(strName, "%d.%d.%d.%d",
			sockAddr.sin_addr.S_un.S_un_b.s_b1,
			sockAddr.sin_addr.S_un.S_un_b.s_b2,
			sockAddr.sin_addr.S_un.S_un_b.s_b3,
			sockAddr.sin_addr.S_un.S_un_b.s_b4);

		SetPeerIP(strName);
	}
	else
	{
		assert(0);
	}
}

// ----------------------------------------------------
// 关闭Socket
// 先等待SocketThread线程结束
// 该函数不能在SocketThread线程中调用
// ----------------------------------------------------
bool CMySocket::Close()
{
	if( m_hSocket != INVALID_SOCKET )
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	m_bClose = true;
	return TRUE;
}

bool CMySocket::ShutDown()
{
	if( m_hSocket != INVALID_SOCKET )
	{
		if( shutdown(m_hSocket,SD_SEND)== SOCKET_ERROR)
		{
            PutErrorString(NET_MODULE,"%-15sshutdown() Error = %d",__FUNCTION__,WSAGetLastError());
		}
	}
	m_bShutDown = true;
	return true;
}

// 关闭socket (收到FD_CLOSE)
void CMySocket::OnClose(int nErrorCode)
{
	CMySocket::Close();
} 

// [TCP] 接受数据
int CMySocket::Recv(void* lpBuf, int nBufLen, int nFlags)
{
	int rt = recv(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
	if( rt == SOCKET_ERROR )
	{
		long error = WSAGetLastError();
		if( error != WSAEWOULDBLOCK )
		{
            PutErrorString(NET_MODULE,"%-15s 读取错误%d",__FUNCTION__,error);
		}
	}
	return rt;
}

// [TCP] 发送数据
int CMySocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	return true;
}


// [UDP] 发送数据
int CMySocket::RecvFrom(void* lpBuf, int nBufLen, char* strIP, ulong& dwPort, int nFlags )
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	int framlen = sizeof(addr);
	int rt = recvfrom(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (sockaddr*)&addr, &framlen);
	if( rt == SOCKET_ERROR )
	{
		long error = WSAGetLastError();
		if( error == WSAEWOULDBLOCK )
		{
			return 0;
		}
        PutErrorString(NET_MODULE,"%-15s Socket RecvFrom Error = %d",__FUNCTION__,error);
		return FALSE;
	}

	strcpy(strIP, inet_ntoa(addr.sin_addr));
	dwPort = ntohs(addr.sin_port);
	return rt;
}

// [UDP] 接收数据
int CMySocket::Sendto(const void* lpBuf, int nBufLen, const char* strIP, ulong dwPort, int nFlags)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)dwPort);
	addr.sin_addr.S_un.S_addr = inet_addr(strIP);

_Begin:
	if( sendto(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR )
	{
		long error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			goto _Begin;
		}
		return FALSE;
	}
	return TRUE;
}

// 获取与套接字相连的地址
bool CMySocket::GetPeerName(char* rPeerAddress, UINT& rPeerPort)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
    bool bResult = getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) ? true : false;
	if (bResult)
	{
		rPeerPort = ntohs(sockAddr.sin_port);
		rPeerAddress = inet_ntoa(sockAddr.sin_addr);
	}
	return bResult;
}

// 获取本地地址
bool CMySocket::GetSockName(char* rSocketAddress, UINT& rSocketPort)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
    bool bResult = getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &nSockAddrLen) ? true : false;
	if (bResult)
	{
		rSocketPort = ntohs(sockAddr.sin_port);
		rSocketAddress = inet_ntoa(sockAddr.sin_addr);
	}
	return bResult;
}

// 获取现有缓冲区数据的大小
bool CMySocket::IOCtl(long lCommand, ulong* lpArgument)
{
	int rt = ioctlsocket(m_hSocket,lCommand,lpArgument);
	if (rt == SOCKET_ERROR)
	{
		long error = WSAGetLastError();
		return FALSE;
	}
	return TRUE;
}

long GetSocketID()
{
	static long lSocketID = 0;
	return ++lSocketID;
}

unsigned long STR2UL(const char *szIP)
{
	return inet_addr(szIP);
}

/// 设置密钥(针对CServerClient & CClients & 机器人测试使用)
void CMySocket::SetKey(int h, int l)
{
	*((ulong*)m_key) = h;
	*((ulong*)m_key+1) = l;

	::KeyInit(m_key, m_kn);
}
