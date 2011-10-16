#include "stdafx.h"
#include "Clients.h"
#include "mmsystem.h"

#ifdef _WORLD_SERVER_
#include "networld\message.h"
#endif

#ifdef _LOGIN_SERVER_
#include "netlogin\message.h"
//#include "loginserver\app\tools.h"
//#include "loginserver\loginserver\loginserver.h"
#endif

#ifdef _SERVER
#include "netserver\message.h"
//#include "gameserver\app\public\tools.h"
//#include "gameserver\server.h"
#endif

#ifdef _CLIENT
#include "netclient\message.h"
#include "netclient\NetDelayTest.h"
#endif

#ifdef _BILLING_SERVER_
#include "NetBilling\Message.h"
#endif

#ifdef _VALUE_SERVER
#include "netvalue\message.h"
#include "valueserver\app\public\tools.h"
#endif

#ifdef _GM_ASSISTOR
#include "message.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketCommands		m_ClientSocketOperaCommands;		//对网络操作的命令队列
int nSendMaxLen = 1024*8;
extern HWND g_hWnd;
CClient::CClient()
{
	m_nSize = 0;
	m_bConnect = false;
	m_bSendData= false;
	m_bControlSend=false;
	m_bSocketThreadExit = false;
	m_bNetClientThreadExit = false;

	m_hSocketThread = NULL;
	m_hNetClientThread=NULL;

	m_hConnectEventSuccess = CreateEvent(NULL,0,0,0);
	m_hSocketEvent[0] = CreateEvent(NULL,0,0,0);
	m_hSocketEvent[1] = CreateEvent(NULL,0,0,0);

	m_bUseProxy = false;
	m_strProxyIP[0] = 0;		// 代理服务器IP
	m_lProxyPort = 0;			// 代理服务器端口
	m_strProxyUser[0] = 0;		// 用户
	m_strProxyPass[0] = 0;		// 密码
	m_lConnectProxyStep = PROXY_INVALID;
	m_bProxyIsValid = PROXY_INVALID;

	pFinishCallback=NULL;

	m_ClientSocketOnSendCommands.Clear();

	m_nBufferSize = MAX_MESSAGE_LENGTH;
	m_pBuffer = new char[m_nBufferSize];

	m_bCheck = true;
	m_dwSendStartTime = timeGetTime();			//统计开始时间
	m_lTotalSendSize = 0;						//发送的总数量
	m_lSendSizePerSecond = 0;					//美秒发送的数量
	m_llSendSize = 0;
	m_dwRecvStartTime = timeGetTime();
	m_lTotalRecvSize = 0;
	m_lRecvSizePerSecond = 0;
	m_llRecvSize = 0;
}

CClient::~CClient()
{
	ExitSocketThread();

	CloseHandle(m_hSocketEvent[0]);
	CloseHandle(m_hSocketEvent[1]);	
	CloseHandle(m_hConnectEventSuccess);

	m_hSocketEvent[0]=NULL;
	m_hSocketEvent[1]=NULL;
	m_hConnectEventSuccess=NULL;

	SAFE_DELETE_ARRAY(m_pBuffer);
}

CSocketCommands& CClient::GetSocketCommand()
{
	return m_ClientSocketOperaCommands;
}
//------------------------------------------------------------------
// 创建SOCKET
//------------------------------------------------------------------
BOOL CClient::Create(UINT nSocketPort,			// 端口
					   LPCTSTR lpszSocketAddress,	// 地址					   
					   int nSocketType,				// 类型( TCP:SOCK_STREAM UDP:SOCK_DGRAM )
					   long lEvent,
					   bool bAsyncSelect  //defaultly,the flag==false,if create for client socket,set the flag=true
					   )
{
	m_dwPort = nSocketPort;
	m_lProtocol = nSocketType;
	m_hSocket = socket(PF_INET, nSocketType, 0);	// 初始化套接字
	if( m_hSocket != INVALID_SOCKET )
	{
		if( Bind(nSocketPort, lpszSocketAddress, 1, 0) )	// 绑定端口和地址
		{
			if(!bAsyncSelect)
				return TRUE;
			if( AsyncSelect(lEvent) )			// 设置网络事件的响应
			{
				//设置发送和接收缓冲区
				SetSendRevBuf();
             
				long lID = GetSocketID();
				SetIndexID(lID);
				return TRUE;
			}
			else
				return FALSE;
		}
		else
		{
			char str[256];
			sprintf(str, "Socket Bind Error = %d",WSAGetLastError());
			PutDebugString(str);
			return FALSE;
		}
	}

	char str[256];
	sprintf(str, "Create SocketError = %d",WSAGetLastError());
	PutDebugString(str);
	return FALSE;
}

// 设置网络事件的响应
BOOL CClient::AsyncSelect(long lEvent)
{
	long rt = WSAEventSelect(m_hSocket, m_hSocketEvent[0], lEvent);
	if (rt == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

// 建立独立线程管理socket
BOOL CClient::CreateSocketThread()
{
	// 创建SocketThread处理网络事件
	extern unsigned __stdcall SocketThread(void* pArguments);
	unsigned threadID;
	m_hSocketThread = (HANDLE)_beginthreadex(NULL,0,SocketThread,this,0,&threadID);
	
	extern unsigned __stdcall NetClientThreadFunc(void* pArguments);
	m_hNetClientThread = (HANDLE)_beginthreadex(NULL,0,NetClientThreadFunc,this,0,&threadID);
	return TRUE;
}

//推出ClientSocket的线程
BOOL CClient::ExitSocketThread()
{
	SetEvent( m_hSocketEvent[1] );//激活结束SocketThread的Event flag
	WaitForSingleObject(m_hSocketThread,INFINITE);
	CloseHandle(m_hSocketThread);
	m_hSocketThread=NULL;

	m_bNetClientThreadExit = true;
	WaitForSingleObject(m_hNetClientThread,INFINITE);
	CloseHandle(m_hNetClientThread);
	m_hNetClientThread=NULL;

	return TRUE;
}


BOOL CClient::Close()
{
	//关闭Socket线程
	ExitSocketThread();

	m_bConnect = false;
	CMySocket::Close();
	return TRUE;
}


// [TCP] 发送数据
int CClient::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	if(m_bSendData==false)
		return false;
	int nRet = send(m_hSocket,(LPSTR)lpBuf, nBufLen, nFlags);

	if( nRet == SOCKET_ERROR )
	{
		char str[200];
		long nError = WSAGetLastError();
		sprintf(str, "Send MSG To Server(error:%d)。",nError);
		PutDebugString(str);
		return FALSE;
	}
	//添加统计量
	AddSendSize(nRet);
	return nRet;
}

int CClient::SendToServer(const void* lpBuf, int nBufLen, bool bFront, int nFlags)
{
	//加入发送命令到操作队列
	char* pBuf = new char[nBufLen];
	memcpy(pBuf,lpBuf,nBufLen);
	long lSize = sizeof(tagSocketOper);
	tagSocketOper* pSocketOpera = new tagSocketOper(SOCKETOPERATYPE_SENDTOSOCKET,GetIndexID(),pBuf,nBufLen,nFlags);
	if(bFront==true)
	{

		GetSocketCommand().Push_Front(pSocketOpera);
	}
	else
	{
		GetSocketCommand().Push_Back(pSocketOpera);
	}
	return TRUE;
}

void CClient::OnSend(int nErrorCode)
{
	if(nErrorCode != 0)
	{
		char str[256];
		sprintf(str, "FD_WRITE FAILED with error %d!",nErrorCode);
		PutDebugString(str);
		return;
	}

	PutDebugString("FD_WRITE Event OK!");
	//表示可以发送数据
	m_bSendData=true;
}

//-----------------------------------------------------------
// 接受到数据的处理
// 这里接受到了从服务器发来的数据，做游戏的处理
//-----------------------------------------------------------
void CClient::OnReceive(int nErrorCode)
{
	// 处理代理服务器的初始化
	//if( IsUseProxy() && m_lConnectProxyStep != PROXY_VALID )
	//{
	///	ConnectProxy(m_lConnectProxyStep);
	//	return;
	//}
	
	while (true) {
	int nReadSize = min(m_nBufferSize - m_nSize,10240);


	int nRetsize = Recv(&m_pBuffer[m_nSize], nReadSize);
	if (nRetsize == SOCKET_ERROR)
	{
		int nError = GetLastError();
		if(nError != WSAEWOULDBLOCK)
		{
			char str[200];
			sprintf(str, "Client OnReceive Socket Error = %d",nError);
			PutDebugString(str);
		}
		return;
	}
	else if (nRetsize == 0)
	{
		char str[256];
		sprintf(str, "Client OnReceive size is 0");
		PutDebugString(str);
		return;
	}

	m_nSize += nRetsize;
	long lPoint = 0;	// 当前有效缓冲区地址
	while( m_nSize >= sizeof(long) )
	{
		long lRLESize = *((long*)&m_pBuffer[lPoint]);

		// 缓冲区数据大于等于一个消息的长度
		if( lRLESize <= m_nSize )
		{
			CMessage* pMsg = CMessage::CreateMessage(&m_pBuffer[lPoint]+4, lRLESize-4);
			// 消息创建成功并压入消息队列
			if( pMsg )
			{

//##记录日志
#ifdef _DEBUG
#ifdef _CLIENT
				Log( pMsg );
#endif
#endif

// 模拟网络延时
#ifdef _CLIENT
				extern bool g_bDebug;
				if( g_bDebug && g_bNetDelay )
				{
					GetMsgQeue()->PushMessage(pMsg);
					g_MsgTime.push(GetCurTickCount()+random(g_lNetDelayInRange)+g_lNetDelayInMin);
				}
				else
				{
					m_RecvMessages.PushMessage( pMsg );
				}
#else
				m_RecvMessages.PushMessage( pMsg );
#endif

				m_nSize -= lRLESize;
				lPoint += lRLESize;		// 移动缓冲区指针

				// 如果是超长的消息，把缓冲区大小调整回来
				if( m_nBufferSize > MAX_MESSAGE_LENGTH && m_nSize <= MAX_MESSAGE_LENGTH )
				{
					char *pTemp = new char[m_nSize];
					memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);

					SAFE_DELETE_ARRAY(m_pBuffer);
					m_nBufferSize = MAX_MESSAGE_LENGTH;
					m_pBuffer = new char[m_nBufferSize];

					memcpy(m_pBuffer, pTemp, m_nSize);
					SAFE_DELETE_ARRAY(pTemp);
					lPoint = 0;
				}
			}
			else
			{
				// 出错，抛弃当前收到的所有数据
				m_nSize = 0;

				char str[256];
				sprintf(str, "CreateMessage ERROR");
				PutDebugString(str);
				lPoint = 0;
				break;
			}
		}
		else
		{
			// 出错，抛弃当前收到的所有数据
			if( lRLESize < 0 )
			{
				m_nSize = 0;

				char str[256];
				sprintf(str, "ERROR MSG(RleSize=%d), Discard Recent Data.", lRLESize);
				PutDebugString(str);
				lPoint = 0;
				break;
			}

#ifdef _CLIENT
			// 收到消息大于1MB，丢弃此消息
			if(lRLESize > MAX_MESSAGE_LENGTH)
			{
				char str[256];
				sprintf(str, "ERROR MSG : Overflow(RleSize=%d), Discard Recent Data.", lRLESize);
				PutDebugString(str);
				lRLESize = 0;
				lPoint = 0;
				m_nSize = 0;
				break;
			}
#endif
			// 收到消息大于最大消息缓冲区，开新的缓冲区
			if( lRLESize > m_nBufferSize )
			{
				char *pTemp = new char[m_nSize];
				memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
				SAFE_DELETE_ARRAY(m_pBuffer);
				m_nBufferSize = lRLESize;
				m_pBuffer = new char[m_nBufferSize];
				memcpy(m_pBuffer, pTemp, m_nSize);
				SAFE_DELETE_ARRAY(pTemp);

				char str[256];
				sprintf(str, "Overflaw MSG Size = %d", m_nBufferSize);
				PutDebugString(str);
				lPoint = 0;
				break;
			}

			char str[256];
			sprintf(str, "Half-baked MSG m_nSize = %d,Point = %d,m_nBufferSize =%d",
				m_nSize, lPoint,m_nBufferSize);
			PutDebugString(str);

			break;
		}
	}

	// 把不完全的消息移动到缓冲区头部
	if( lPoint > 0 )
	{
		memmove(m_pBuffer, &m_pBuffer[lPoint], m_nSize);
		lPoint = 0;
	}
	}
}

// 连接成功
void CClient::OnConnect( int nErrorCode )
{
	
	CMySocket::OnConnect(nErrorCode);
	if (nErrorCode == 0)
	{
		m_bConnect = true;
	}
	else
	{
		m_bConnect = false;
	}

	SetEvent(m_hConnectEventSuccess);
	HandleConnect();
}

// 关闭连接
void CClient::OnClose(int nErrorCode)
{
	CMySocket::OnClose();

	m_bConnect = false;
	HandleClose();
}

// 连接
BOOL CClient::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	char strIP[64];
	strcpy(strIP, lpszHostAddress);

	// 解析域名
	if( lpszHostAddress[0] < '0' || lpszHostAddress[0] > '9' )
	{
		hostent* host;
		host = gethostbyname(lpszHostAddress);
		if( host )
		{
			sprintf(strIP, "%d.%d.%d.%d",
					(BYTE)host->h_addr_list[0][0],
					(BYTE)host->h_addr_list[0][1],
					(BYTE)host->h_addr_list[0][2],
					(BYTE)host->h_addr_list[0][3]);
		}
	}

	SetServerIP(strIP);
	SetServerPort(nHostPort);

	if( GetProtocol() == SOCK_STREAM )
	{
		// 使用代理
		// 直接连接
		ConnectServer(strIP, nHostPort);
		// 等待连接
		WaitForConnect();

		// 连接成功
		if( IsConnect() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}


//连接
BOOL CClient::ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	assert(lpszHostAddress != NULL);

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	LPSTR lpszAscii = (LPTSTR)lpszHostAddress;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
		{
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}

	sockAddr.sin_port = htons((u_short)nHostPort);

	int ret = connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if( ret == SOCKET_ERROR )
	{
		ret = WSAGetLastError();
		if(  ret == WSAEWOULDBLOCK )
		{
			return TRUE;
		}

		char str[200];
		sprintf(str, "Clients Connect Socket Error = %d",ret);
		PutDebugString(str);

		return false;
	}
	return TRUE;
}
// 等待连接
long CClient::WaitForConnect()
{
	// TCP协议
	if( GetProtocol() == SOCK_STREAM )
	{
		int nRet = WaitForSingleObject(m_hConnectEventSuccess, CONNECT_TIMEOUT);
		if( nRet != WAIT_OBJECT_0 )
		{
			m_bConnect = false;
		}
	}
	return TRUE;
}

// 是否连接
bool CClient::IsConnect()
{
	// TCP协议
	return m_bConnect;
}

long	CClient::AddSendSize(long lSize)
{
	m_lTotalSendSize += lSize;
	m_llSendSize += lSize;
	//当数据累计达10M的时候开始统计
	if(m_lTotalSendSize >= 10485760)
	{
		DWORD dwTime = timeGetTime();
		DWORD Time = dwTime-m_dwSendStartTime;
		if(Time >= 1)
		{
			m_lSendSizePerSecond = m_lTotalSendSize*1000/Time;
			m_dwSendStartTime = dwTime;
			m_lTotalSendSize = 0;
		}
	}
	return m_lSendSizePerSecond;
}

long	CClient::AddRecvSize(long lSize)
{
	m_lTotalRecvSize += lSize;
	m_llRecvSize += lSize;
	//当数据累计达10M的时候开始统计
	if(m_lTotalRecvSize >= 10485760)
	{
		DWORD dwTime = timeGetTime();
		DWORD Time = dwTime-m_dwRecvStartTime;
		if(Time >= 1)
		{
			m_lRecvSizePerSecond = m_lTotalRecvSize*1000/Time;
			m_dwRecvStartTime = dwTime;
			m_lTotalRecvSize = 0;
		}
	}
	return m_lRecvSizePerSecond;
}

/*
如何用代理TCP协议 

1.向服务器的1080端口建立tcp连接。 
2.向服务器发送 05 01 00 （此为16进制码，以下同） 
3.如果接到 05 00 则是可以代理 
4.发送 05 01 00 01 + 目的地址(4字节） + 目的端口（2字节），目的地址和端口都是16进制码（不是字符串！！）。 例202.103.190.27 - 7201 则发送的信息为：05 01 00 01 CA 67 BE 1B 1C 21 (CA=202 67=103 BE=190 1B=27 1C21=7201) 
5.接受服务器返回的自身地址和端口，连接完成 
6.以后操作和直接与目的方进行TCP连接相同。 

如何用代理UDP连接 

1.向服务器的1080端口建立udp连接 
2.向服务器发送 05 01 00 
3.如果接到 05 00 则是可以代理 
4.发送 05 03 00 01 00 00 00 00 + 本地UDP端口（2字节） 
5.服务器返回 05 00 00 01 +服务器地址+端口 
6.需要申请方发送 00 00 00 01 +目的地址IP（4字节）+目的端口 +所要发送的信息 
7.当有数据报返回时 向需要代理方发出00 00 00 01 +来源地址IP（4字节）+来源端口 +接受的信息 

注：此为不需要密码的代理协议，只是socks5的一部分，完整协议请看RFC1928
*/

// 设置代理
void CClient::SetProxy(const char* ip, long port, const char* user, const char* pass)
{
	strcpy(m_strProxyIP, ip);
	m_lProxyPort = port;
	if( user ) strcpy(m_strProxyUser, user);
	else m_strProxyUser[0] = 0;
	if( pass ) strcpy(m_strProxyPass, pass);
	else m_strProxyPass[0] = 0;
}

// 连接代理服务器
void CClient::ConnectProxy(long step)
{
	char buff[600];
	m_lConnectProxyStep = step;

	switch( m_lConnectProxyStep )
	{
	case 2:		// 2.向服务器发送 05 01 00 （此为16进制码，以下同）
		{
			struct sock5req1 *m_proxyreq1;
			m_proxyreq1 = (struct sock5req1 *)buff;
			m_proxyreq1->Ver = 5;
			m_proxyreq1->nMethods = 2;
			m_proxyreq1->Methods[0] = 0;
			m_proxyreq1->Methods[1] = 2;
			Send(buff,4);
			m_lConnectProxyStep = 3;
			m_bProxyIsValid = PROXY_INVALID;
		}
		break;

	case 3:		// 3.如果接到 05 00 则是可以代理 
		{
			struct sock5ans1 *m_proxyans1;
			m_proxyans1 = (struct sock5ans1 *)buff;
			memset(buff,0,600);
			Recv(buff,600);

			if(m_proxyans1->Ver != 5 || (m_proxyans1->Method!=0 && m_proxyans1->Method!=2))
			{
				// 连接失败
				m_bConnect = false;
				SetEvent(m_hConnectEventSuccess);

				MessageBox(NULL, "No Proxy Function!", "Msg", MB_OK);
				m_lConnectProxyStep = PROXY_INVALID;
				m_bProxyIsValid = PROXY_INVALID;
				return;
			}

			// 需要密码验证
			if(m_proxyans1->Method == 2)
			{
				int nUserLen = lstrlen(m_strProxyUser);
				int nPassLen = lstrlen(m_strProxyPass);
				struct authreq *m_authreq;
				m_authreq = (struct authreq *)buff;
				m_authreq->Ver = 1;
				m_authreq->Ulen = nUserLen;
				strcpy(m_authreq->Name, m_strProxyUser);
				m_authreq->PLen = nPassLen;
				strcpy(m_authreq->Pass, m_strProxyPass);
				Send(buff,513);
				m_lConnectProxyStep = 4;
				break;
			}

			// 4.发送 05 01 00 01 + 目的地址(4字节） + 目的端口（2字节），目的地址和端口都是16进制码（不是字符串！！）。 例202.103.190.27 - 7201 则发送的信息为：05 01 00 01 CA 67 BE 1B 1C 21 (CA=202 67=103 BE=190 1B=27 1C21=7201) 
			struct sock5req2 *m_proxyreq2;
			m_proxyreq2 = (struct sock5req2 *)buff;
			m_proxyreq2->Ver = 5;
			m_proxyreq2->Cmd = 1;
			m_proxyreq2->Rsv = 0;
			m_proxyreq2->Atyp = 1;
			unsigned long tmpLong = inet_addr(GetServerIP());
			unsigned short port = ntohs((unsigned short)GetServerPort());
			memcpy(m_proxyreq2->other,&tmpLong,4);
			memcpy(m_proxyreq2->other+4,&port,2);
			Send(buff,sizeof(struct sock5req2)+5);
			m_lConnectProxyStep = 5;
			m_bProxyIsValid = PROXY_VALID;
		}
		break;

	case 4:		// 返回密码验证结果
		{
			struct authans *m_authans;
			m_authans = (struct authans *)buff;
			memset(buff,0,600);
			Recv(buff,600);
			if(m_authans->Ver != 1 || m_authans->Status != 0)
			{
				// 连接失败
				m_bConnect = false;
				SetEvent(m_hConnectEventSuccess);

				MessageBox(NULL, "Proxy Server User Check FAILED!", "Msg", MB_OK);
				m_lConnectProxyStep = PROXY_INVALID;
				m_bProxyIsValid = PROXY_INVALID;
				return;
			}

			// 发送 05 01 00 01 + 目的地址(4字节） + 目的端口（2字节），目的地址和端口都是16进制码（不是字符串！！）。 例202.103.190.27 - 7201 则发送的信息为：05 01 00 01 CA 67 BE 1B 1C 21 (CA=202 67=103 BE=190 1B=27 1C21=7201) 
			struct sock5req2 *m_proxyreq2;
			m_proxyreq2 = (struct sock5req2 *)buff;
			m_proxyreq2->Ver = 5;
			m_proxyreq2->Cmd = 1;
			m_proxyreq2->Rsv = 0;
			m_proxyreq2->Atyp = 1;
			unsigned long tmpLong = inet_addr(GetServerIP());
			unsigned short port = ntohs((unsigned short)GetServerPort());
			memcpy(m_proxyreq2->other,&tmpLong,4);
			memcpy(m_proxyreq2->other+4,&port,2);
			Send(buff,sizeof(struct sock5req2)+5);
			m_lConnectProxyStep = 5;
			m_bProxyIsValid = PROXY_VALID;
		}
		break;

	case 5:		// 5.接受服务器返回的自身地址和端口，连接完成 
		{
			struct sock5ans2 *m_proxyans2;
			memset(buff,0,600);
			m_proxyans2 = (struct sock5ans2 *)buff;
			Recv(buff,600);
			if(m_proxyans2->Ver != 5 || m_proxyans2->Rep != 0)
			{
				// 连接失败
				m_bConnect = false;
				SetEvent(m_hConnectEventSuccess);

				MessageBox(NULL, "Connect To Main Site By Proxy FAILED!", "Msg", MB_OK);
				m_lConnectProxyStep = 1;
				return;
			}
			m_lConnectProxyStep = PROXY_VALID;
			
			// 连接成功
			m_bConnect = true;
			SetEvent(m_hConnectEventSuccess);
			HandleConnect();
		}
		break;
	}
}

//------------------------------------------------------------------
// 套接字线程
// [服务器] 处理客户端连接
// [客户端] 和服务器通讯
//------------------------------------------------------------------

void DoSocketThread(CClient* pClient)
{
	while(1)
	{
		/*
		*等待线程结束EVENT或网络事件EVENT
		*/
		long rt = WSAWaitForMultipleEvents(2, pClient->m_hSocketEvent,FALSE, WSA_INFINITE,FALSE);
		if(rt-WAIT_OBJECT_0 == 1 )// 结束线程
			break;

		WSANETWORKEVENTS events;
		// 枚举网络事件
		long ret = WSAEnumNetworkEvents(pClient->m_hSocket, pClient->m_hSocketEvent[0], &events);
		if( events.lNetworkEvents != 0 )
		{
			// 收到消息
			if (events.lNetworkEvents & FD_READ)
			{
				pClient->OnReceive(events.iErrorCode[FD_READ_BIT]);
			}

			// 关闭套接字
			if (events.lNetworkEvents & FD_CLOSE)
			{
				pClient->OnClose(events.iErrorCode[FD_CLOSE_BIT]);
			}

			// 当发送消息
			if (events.lNetworkEvents & FD_WRITE)
			{
				tagSocketOper* pSocketOpera = new tagSocketOper(SOCKETOPERATYPE_ONSEND,pClient->GetIndexID()
					,NULL,events.iErrorCode[FD_WRITE_BIT]);
				pClient->m_ClientSocketOnSendCommands.Push_Back(pSocketOpera);
				//pClient->OnSend(events.iErrorCode[FD_WRITE_BIT]);
			}

			// [CLIENT] 连接成功
			if (events.lNetworkEvents & FD_CONNECT)
			{
				pClient->OnConnect(events.iErrorCode[FD_CONNECT_BIT]);
			}
		}
	}
}

unsigned __stdcall SocketThread(void* pArguments)
{
	PutDebugString("the Client socket thread start.");
	CClient* pClient = (CClient*) pArguments;
	if(pClient==NULL)
	{
		PutDebugString("the Client socket thread Quit.");
		_endthreadex( 0 );
		return 0;
	}

#ifndef _DEBUG
	__try
	{
#endif

		DoSocketThread(pClient);

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		PutDebugString("Network client workerthread err, check out the latest err report file!");
	}
#endif

	PutDebugString("the Client socket thread Quit.");
	_endthreadex( 0 );
	return 0;
}

void DoNetClientThreadFunc(CClient* pClient)
{
	int nLen = 0;
	int nPos = 0;
	int nSendNum = 0;
	int nRet = 0;
	char str[200]="";
	CommandsQueue TemptCommands;
	TemptCommands.clear();
	//获取一个操作命令
	while(true)
	{
		Sleep(1);
		bool bContrlSend = pClient->GetControlSend();

		if(pClient->m_bNetClientThreadExit &&
			(pClient->GetSocketCommand().GetSize() <=0 || pClient->IsConnect()==false ||
			pClient->m_bSendData == false) )
			break;

		int nSize = pClient->m_ClientSocketOnSendCommands.GetSize();
		for(int i = 0; i < nSize ;i++)
		{
			tagSocketOper* pSocketOper = pClient->m_ClientSocketOnSendCommands.Pop_Front();
			if(pSocketOper)
			{
				pClient->OnSend(pSocketOper->lNum1);
			}
			delete pSocketOper;
		}

		if(pClient->m_bSendData == false || bContrlSend == false || pClient->IsConnect()==false)
			continue;

		pClient->GetSocketCommand().CopyAllCommand(TemptCommands);

		CommandsQueue::iterator itCommand = TemptCommands.begin();
		for(; itCommand != TemptCommands.end();)
		{		

			tagSocketOper* pSocketOper = (*itCommand);

			itCommand = TemptCommands.erase(itCommand);

			if(pSocketOper)
			{
				//向服务器发送数据,以1024个字节大小发。。。
				nLen = pSocketOper->lNum1;
				nPos = 0;
				BYTE* pByte = (BYTE*)pSocketOper->pBuf;

				do
				{
					//nSendNum = min(nLen,nSendMaxLen);
					//nRet = pClient->Send(&pByte[nPos],nSendNum);
					nRet = pClient->Send(&pByte[nPos],nLen);

					//测试
					if(nRet == FALSE)
					{
						pClient->m_bSendData=false;
						//sprintf(str, "Send MSG To Server, Send Half-baked MSG S:%d,C:%d。",pSocketOper->lNum1,nRet);
						//PutDebugString(str);
						break;
					}
					nPos += nRet;
					nLen -= nRet;
				} while(nLen > 0);

				if(pClient->m_bSendData==false)
				{
					char* pBuf = new char[nLen];
					memcpy(pBuf,&pByte[nPos],nLen);

					tagSocketOper* pTemptSocketOpera = new tagSocketOper(SOCKETOPERATYPE_SENDTOSOCKET,
						pClient->GetIndexID(),pBuf,nLen,pSocketOper->lNum2);
					TemptCommands.push_front(pTemptSocketOpera);

					delete []pSocketOper->pBuf;
					delete pSocketOper;
					break;
				}

				delete []pSocketOper->pBuf;
				delete pSocketOper;

			}
		}

		if(pClient->m_bSendData == false)
		{
			pClient->GetSocketCommand().AddCommandsQueueToFront(TemptCommands);
		}
		TemptCommands.clear();
	}

	TemptCommands.clear();
}


//客户端网络主线程 
unsigned __stdcall NetClientThreadFunc(void* pArguments)
{
	PutDebugString("the Client socket Net_main_thread start.");
	CClient* pClient = (CClient*) pArguments;
	if(pClient==NULL)
	{
		PutDebugString("the Client socket Net_main_thread Quit.");
		_endthreadex( 0 );
		return 0;
	}

#ifndef _DEBUG
	__try
	{
#endif

		DoNetClientThreadFunc(pClient);

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		PutDebugString("Network client mainthread err, check out the latest err report file!");
	}
#endif

	PutDebugString("the Client socket Net_main_thread Quit.");
	_endthreadex( 0 );
	return 0;
}
