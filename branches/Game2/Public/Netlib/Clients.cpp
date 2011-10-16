//=============================================================================
/**
 *  file: Clients.cpp
 *
 *  Brief:主要是客户端使用的网络封装，用事件机制实现的网络接口
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2005-4-15
 *
 *	Modify:2007-4-13,整理了代码和结构，优化了效率
 */
//=============================================================================

#include "stdafx.h"
#include "Clients.h"
#include "mmsystem.h"
#include "BaseMessage.h"
#include "../Module/Crc32Static.h"


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

CClient::CClient()
:m_pDBAllocator(NULL)
{
	m_bConnect = false;
	m_bSendData= false;
	m_bControlSend=false;
	
	m_hSocketThread = NULL;
	m_hNetClientThread=NULL;

	m_hConnectEventSuccess = CreateEvent(NULL,0,0,0);
	m_hSocketEvent[0] = CreateEvent(NULL,0,0,0);
	m_hSocketEvent[1] = CreateEvent(NULL,0,0,0);

	m_nReadDataSize = 0;
	m_SocketOperaCommands.Clear();

	m_bCheck = true;
	m_dwSendStartTime = timeGetTime();			//统计开始时间
	m_lTotalSendSize = 0;						//发送的总数量
	m_lSendSizePerSecond = 0;					//美秒发送的数量
	
	m_dwRecvStartTime = timeGetTime();
	m_lTotalRecvSize = 0;
	m_lRecvSizePerSecond = 0;
	m_llRecvSize = 0;
}

CClient::~CClient()
{
	CloseHandle(m_hSocketEvent[0]);
	CloseHandle(m_hSocketEvent[1]);	
	CloseHandle(m_hConnectEventSuccess);

	m_hSocketEvent[0]=NULL;
	m_hSocketEvent[1]=NULL;
	m_hConnectEventSuccess=NULL;
}

void CClient::PushReadDataBack(CDataBlock *pDB)
{
	if(NULL == pDB)	return;
	m_ReadDataBlocks.push_back(pDB);
	m_nReadDataSize += pDB->GetCurSize();
}
CDataBlock *CClient::PopReadDataFront()
{
	CDataBlock *pDB = NULL;
	if(m_ReadDataBlocks.size() > 0)
	{
		pDB = m_ReadDataBlocks.front();
		m_ReadDataBlocks.pop_front();
	}
	return pDB;
}
void CClient::PushReadDataFront(CDataBlock *pDB)
{
	if(NULL == pDB)	return;

	m_ReadDataBlocks.push_front(pDB);
}

//------------------------------------------------------------------
// 创建SOCKET
//------------------------------------------------------------------
bool CClient::Create(UINT nSocketPort,			// 端口
					 LPCTSTR lpszSocketAddress,	// 地址					   
					 int nSocketType,				// 类型( TCP:SOCK_STREAM UDP:SOCK_DGRAM )
					 long lEvent,
					 bool bAsyncSelect  //defaultly,the flag==false,if create for client socket,set the flag=true
					 )
{
	if(!CMySocket::Create(nSocketPort,lpszSocketAddress,nSocketType,lEvent,bAsyncSelect))
	{
        PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在CClient::Create(...)中,CMySocket::Create()操作出错!");
		return FALSE;
	}
	
	if( bAsyncSelect && !AsyncSelect(lEvent) )
	{
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"在CClient::Create(...)中,AsyncSelect(..)出错!");
		return FALSE;
	}
		
	//设置发送和接收缓冲区
	SetSendRevBuf();
	//设置套接字选项
	SetSocketOpt();
	long lID = GetSocketID();
	SetIndexID(lID);
	return TRUE;

}



// 设置网络事件的响应
bool CClient::AsyncSelect(long lEvent)
{
	long rt = WSAEventSelect(m_hSocket, m_hSocketEvent[0], lEvent);
	if (rt == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

bool CClient::Start(CDataBlockAllocator* pDBAllocator,long nMaxFreeSockOperNum,long lFlag)
{
	SetFlag(lFlag);

	m_pDBAllocator = pDBAllocator;
	m_nMaxFreeSockOperNum = nMaxFreeSockOperNum;

	//创建套结字
	if( false == Create() )
	{
		return false;
	}
	//预分配命令操作结构
	InitializeCriticalSection(&m_CSSockOper);
	ulong i = 0;
	for(;i<m_nMaxFreeSockOperNum;i++)
	{
		tagSocketOper* pSockOper = new tagSocketOper();
		m_FreeSocketOpers.push_back(pSockOper);
	}

	CreateSocketThread();
	return true;
}

void CClient::Exit()
{
	//优雅的关闭
	ShutDown();
	int i = 0;
	while(false == IsClose() && i++ < 40)
	{
		Sleep(100);
	}

	ExitSocketThread();

	itSockOP itSock = m_FreeSocketOpers.begin();
	for(;itSock != m_FreeSocketOpers.end();itSock++)
	{
		SAFE_DELETE((*itSock));
	}
	DeleteCriticalSection(&m_CSSockOper);

	//释放消息队列里的消息
	CBaseMessage* pMsg = NULL;
	while(pMsg = GetRecvMessages()->PopMessage())
	{
		CBaseMessage::FreeMsg(pMsg);
	}
}

// 建立独立线程管理socket
bool CClient::CreateSocketThread()
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
bool CClient::ExitSocketThread()
{
	SetEvent( m_hSocketEvent[1] );//激活结束SocketThread的Event flag
	WaitForSingleObject(m_hSocketThread,INFINITE);
	CloseHandle(m_hSocketThread);
	m_hSocketThread=NULL;

	//发送退出主线程操作命令
	tagSocketOper* pSocketOpera = AllocSockOper();
	pSocketOpera->Init(SCOT_Client_ExitThread,0,0);
	m_SocketOperaCommands.Push_Back(pSocketOpera);

	WaitForSingleObject(m_hNetClientThread,INFINITE);
	CloseHandle(m_hNetClientThread);
	m_hNetClientThread=NULL;

	return TRUE;
}

tagSocketOper* CClient::AllocSockOper()
{
	tagSocketOper* pSockOper = NULL;

	EnterCriticalSection(&m_CSSockOper);
	if(m_FreeSocketOpers.size() > 0)
	{
		pSockOper = m_FreeSocketOpers.front();
		m_FreeSocketOpers.pop_front();
	}
	LeaveCriticalSection(&m_CSSockOper);

	if(pSockOper == NULL)
		pSockOper = new tagSocketOper();
	return pSockOper;
}


void CClient::FreeSockOper(tagSocketOper* pSockOper)
{
	if(pSockOper == NULL)	return;
	EnterCriticalSection(&m_CSSockOper);
	if(m_FreeSocketOpers.size() < m_nMaxFreeSockOperNum)
	{
		m_FreeSocketOpers.push_back(pSockOper);
	}
	else
	{
		SAFE_DELETE(pSockOper);
	}
	LeaveCriticalSection(&m_CSSockOper);
}


bool CClient::Close()
{
	m_bConnect = false;
	CMySocket::Close();
	return TRUE;
}


//异步发送数据
int CClient::ASend(CBaseMessage *pMsg,int nFlags)
{
	//加入发送命令到操作队列
	tagSocketOper* pSocketOpera = AllocSockOper();
	pSocketOpera->Init(SCOT_Client_Send,GetIndexID(),pMsg,nFlags);
	m_SocketOperaCommands.Push_Back(pSocketOpera);
	return true;
}

bool CClient::AddSendMsg(CBaseMessage* pMsg)
{
	if(NULL == pMsg)	return false;
	m_SendMessages.push_back(pMsg);

	//如果已发送的数据小于设置大小，则发送数据
	Send();
	return true;
}

int CClient::Send()
{
	if(m_bSendData==false)
		return false;

	itMsg it = m_SendMessages.begin();
	for(;it != m_SendMessages.end();)
	{
		CBaseMessage *pMsg = (*it);
		if(pMsg)
		{
			if (IsEncryptType(GetFlag()))
			{
				pMsg->Encrypt(m_kn);
			}
			vector<CDataBlock*>& DataBlocks = pMsg->GetMsgData();
			vector<CDataBlock*>::iterator itDB = DataBlocks.begin();
			for(;itDB!=DataBlocks.end();)
			{
				CDataBlock* pDB = (*itDB);
				long nDBSize = pDB->GetCurSize();
				long nRet = Send(pDB->Base(),nDBSize);
				if( nRet != nDBSize )
				{
					memmove(pDB->Base(),pDB->Base()+nRet,nDBSize-nRet);
					pDB->SetCurSize(nDBSize-nRet);
					return false;
				}
				else
				{
					itDB = DataBlocks.erase(itDB);
					m_pDBAllocator->FreeDB(pDB);
				}
			}
			CBaseMessage::FreeMsg(pMsg);
		}
		it = m_SendMessages.erase(it);
	}
	return true;
}
// [TCP] 发送数据
int CClient::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	int nSendSize = nBufLen;
	int nPos = 0;
	int nRet = 0;

	while(nSendSize > 0)
	{
		nRet = send(m_hSocket,(LPSTR)lpBuf+nPos,nSendSize,nFlags);
		if( nRet == SOCKET_ERROR )
		{
			m_bSendData=false;
			long nError = WSAGetLastError();
			PutErrorString(NET_MODULE,"%-15s 向服务器发送消息错误(error:%d)",__FUNCTION__,nError);
			break;
		}
		nSendSize -= nRet;
		nPos += nRet;
	}
	
	//添加统计量
	AddSendSize(nPos);
	return nPos;
}


void CClient::OnSend(int nErrorCode)
{
	if(nErrorCode != 0)
	{
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__, "FD_WRITE failed with error %d!",nErrorCode);
		return;
	}

	PutTraceString(NET_MODULE,"FD_WRITE Event ok!");
	//表示可以发送数据
	m_bSendData=true;
	//继续发送数据
	Send();
}


void CClient::OnReceive(int nErrorCode)
{	
	if(nErrorCode != 0)
	{
		//客户端接受数据失败
		PutErrorString(NET_MODULE,"%-15s FD_READ failed with error %d\n",__FUNCTION__,nErrorCode);
		return;
	}

	//while (true)
	{
		CDataBlock *pDB = m_pDBAllocator->AllocDB(4);
		int nRetsize = Recv(pDB->Base(),pDB->GetMaxSize() );
		if (nRetsize == SOCKET_ERROR)
		{
			int nError = GetLastError();
			if(nError != WSAEWOULDBLOCK)
			{
                PutErrorString(NET_MODULE,"%-15s Client OnReceive Socket Error = %d",__FUNCTION__,nError);
			}
			m_pDBAllocator->FreeDB(pDB);
			return;
		}
		else if ( nRetsize == 0 || nRetsize == WSAECONNRESET )
		{
			//Connection Closed
			m_pDBAllocator->FreeDB(pDB);
			return;
		}
		pDB->SetCurSize(nRetsize);
		tagSocketOper* pSocketOpera = AllocSockOper();
		pSocketOpera->Init(SCOT_Client_OnRecv,GetIndexID(),pDB,nRetsize);
		m_SocketOperaCommands.Push_Back(pSocketOpera);		
	}
}


void CClient::OnReceive(CDataBlock *pDB,long lReadNum)
{
	if(NULL == pDB)	return;
	PushReadDataBack(pDB);
	DecordMsg();
}

//从数据头开始获得一个long值
long CClient::GetCurMsgLen()
{
	char strInfo[512]="";
	//赋一个最大值
	long len = 0x7FFFFFFF;
	if(m_nReadDataSize < 4)	return len;

	BYTE* pLen = (BYTE*)&len;
	long lPos = 0;
	long size = sizeof(long);	
	itDB it = m_ReadDataBlocks.begin();
	for(;it != m_ReadDataBlocks.end() && size > 0;it++)
	{
		CDataBlock *pDB = (*it);
		if(pDB)
		{
			long minsize = min(size,pDB->GetCurSize());
			memcpy(pLen+lPos,pDB->Base(),minsize);
			if(len<=0)
			{
                PutErrorString(NET_MODULE,"%-15s 错误，得到消息长度为0,ReadDataBlocksSize:%d,minsize%d",__FUNCTION__,m_ReadDataBlocks.size(),minsize);
			}
			lPos+=minsize;
			size -= minsize;
			//退出
			if(size <= 0 )
				break;
		}
	}
	if(size > 0)
	{
        PutErrorString(NET_MODULE,"%-15s 错误，未取到完整的当前消息长度,m_nReadDataSize:%d,ReadDataBlocksSize:%d,size%",
            __FUNCTION__,m_nReadDataSize,m_ReadDataBlocks.size(),size);
		len = 0x7FFFFFFF;
	}
	return len;
}

//解码消息
void CClient::DecordMsg()
{
	long lCurMsgLen = 0;
	//判断是否接受了足够长的数据
	while((m_nReadDataSize >= HEAD_SIZE && ( lCurMsgLen = GetCurMsgLen()) <= m_nReadDataSize) )
	{
		long lSize = lCurMsgLen;
		vector<CDataBlock*> MsgDB;

		//计算数据的开始位置
		CDataBlock *pDB = PopReadDataFront();
		long lDBSize = pDB->GetCurSize();
		long lDBPos = 0;
		CDataBlock *pTemptDB = m_pDBAllocator->AllocDB(5);
		long lTemptSize = pTemptDB->GetMaxSize();
		long lTemptPos = 0;

		while( lSize > 0 && pDB && pTemptDB)
		{
			long minsize = min(lSize,lDBSize-lDBPos);
			minsize = min(minsize,lTemptSize-lTemptPos);

			memcpy(pTemptDB->Base()+lTemptPos,pDB->Base()+lDBPos,minsize);
			lDBPos += minsize;
			lTemptPos += minsize;
			lSize -= minsize;

			if(lDBPos >= lDBSize && lSize > 0)
			{
				m_pDBAllocator->FreeDB(pDB);
				pDB = PopReadDataFront();
				lDBSize = pDB->GetCurSize();
				lDBPos = 0;
			}

			if(lTemptPos >= lTemptSize && lSize > 0)
			{
				pTemptDB->SetCurSize(lTemptSize);
				MsgDB.push_back(pTemptDB);

				pTemptDB = m_pDBAllocator->AllocDB(6);
				lTemptSize = pTemptDB->GetMaxSize();
				lTemptPos = 0;
			}
		}

		pTemptDB->SetCurSize(lTemptPos);
		MsgDB.push_back(pTemptDB);
		if(lDBPos < lDBSize)
		{
			memmove(pDB->Base(), pDB->Base()+lDBPos, lDBSize-lDBPos);
			pDB->SetCurSize(lDBSize-lDBPos);
			PushReadDataFront(pDB);
		}
		else if(lDBPos == lDBSize)
		{
			m_pDBAllocator->FreeDB(pDB);
		}
		m_nReadDataSize -= lCurMsgLen;
		CBaseMessage* pMsg = CBaseMessage::AllocMsg();
		pMsg->Init(MsgDB, m_kn, IsEncryptType(GetFlag()));
		pMsg->SetSocketID(GetIndexID());
		m_RecvMessages.PushMessage( pMsg );
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
	
	CBaseMessage* pMsg = CBaseMessage::AllocMsg();
	pMsg->Init(BASEMSG_Socket_Close);
	pMsg->SetNetFlag(GetFlag());
	pMsg->SetSocketID(GetIndexID());
	pMsg->SetTotalSize();
	GetRecvMessages()->PushMessage( pMsg );
}

// 连接
bool CClient::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
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

	SetPeerIP(strIP);
	SetPeerPort(nHostPort);	

	if( GetProtocol() == SOCK_STREAM )
	{
		// 直接连接
		if( ConnectServer(strIP, nHostPort) )
		{
			// 等待连接
			WaitForConnect();
		}
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
bool CClient::ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	assert(lpszHostAddress != NULL);

	this->Create();

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

	if (IsEncryptType(GetFlag()))
	{
		SetKey(sockAddr.sin_addr.S_un.S_addr, sockAddr.sin_port);
	}

	int ret = connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if( ret == SOCKET_ERROR )
	{
		ret = WSAGetLastError();
		if(  ret == WSAEWOULDBLOCK )
		{
			return TRUE;
		}

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
	return m_bConnect;
}

long	CClient::AddSendSize(long lSize)
{
	m_lTotalSendSize += lSize;
	m_llSendSize += lSize;
	//当数据累计达10M的时候开始统计
	if(m_lTotalSendSize >= 10485760)
	{
		ulong dwTime = timeGetTime();
		ulong Time = dwTime-m_dwSendStartTime;
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
		ulong dwTime = timeGetTime();
		ulong Time = dwTime-m_dwRecvStartTime;
		if(Time >= 1)
		{
			m_lRecvSizePerSecond = m_lTotalRecvSize*1000/Time;
			m_dwRecvStartTime = dwTime;
			m_lTotalRecvSize = 0;
		}
	}
	return m_lRecvSizePerSecond;
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
				tagSocketOper* pSocketOpera = pClient->AllocSockOper();
				pSocketOpera->Init(SCOT_Client_OnSend,pClient->GetIndexID()
					,NULL,events.iErrorCode[FD_WRITE_BIT]);
				pClient->m_SocketOperaCommands.Push_Back(pSocketOpera);
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
	PutTraceString(NET_MODULE,"the Client socket thread start.");
	CClient* pClient = (CClient*) pArguments;
	if(pClient==NULL)
	{
		PutTraceString(NET_MODULE,"the Client socket thread Quit.");
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
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"网络的客户端工作线程出错，请查看最新的错误报告文件");
	}
#endif

	PutTraceString(NET_MODULE,"the Client socket thread Quit.");
	_endthreadex( 0 );
	return 0;
}

//void DoNetClientThreadFunc(CClient* pClient)
//{
//	int nLen = 0;
//	int nPos = 0;
//	int nSendNum = 0;
//	int nRet = 0;
//	char str[200]="";
//	CommandsQueue TemptCommands;
//	TemptCommands.clear();
//	//获取一个操作命令
//	while(true)
//	{
//		Sleep(1);
//		bool bContrlSend = pClient->GetControlSend();
//
//		if(pClient->m_bNetClientThreadExit &&
//			(pClient->GetSocketCommand().GetSize() <=0 || pClient->IsConnect()==false ||
//			pClient->m_bSendData == false) )
//			break;
//
//		int nSize = pClient->m_ClientSocketOnSendCommands.GetSize();
//		for(int i = 0; i < nSize ;i++)
//		{
//			tagSocketOper* pSocketOper = pClient->m_ClientSocketOnSendCommands.Pop_Front();
//			if(pSocketOper)
//			{
//				pClient->OnSend(pSocketOper->lNum1);
//			}
//			delete pSocketOper;
//		}
//
//		if(pClient->m_bSendData == false || bContrlSend == false || pClient->IsConnect()==false)
//			continue;
//
//		pClient->GetSocketCommand().CopyAllCommand(TemptCommands);
//
//		CommandsQueue::iterator itCommand = TemptCommands.begin();
//		for(; itCommand != TemptCommands.end();)
//		{		
//
//			tagSocketOper* pSocketOper = (*itCommand);
//
//			itCommand = TemptCommands.erase(itCommand);
//
//			if(pSocketOper)
//			{
//				//向服务器发送数据,以1024个字节大小发。。。
//				nLen = pSocketOper->lNum1;
//				nPos = 0;
//				BYTE* pByte = (BYTE*)pSocketOper->pBuf;
//				do
//				{
//					//nSendNum = min(nLen,nSendMaxLen);
//					//nRet = pClient->Send(&pByte[nPos],nSendNum);
//					nRet = pClient->Send(&pByte[nPos],nLen);
//
//					//测试
//					if(nRet == FALSE)
//					{
//						pClient->m_bSendData=false;
//						//sprintf(str, "向服务器发送消息,未发送完数据S:%d,C:%d。",pSocketOper->lNum1,nRet);
//						//PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,str);
//						break;
//					}
//					nPos += nRet;
//					nLen -= nRet;
//				} while(nLen > 0);
//
//				if(pClient->m_bSendData==false)
//				{
//					char* pBuf = new char[nLen];
//					memcpy(pBuf,&pByte[nPos],nLen);
//
//					tagSocketOper* pTemptSocketOpera = new tagSocketOper(SCOT_ESend,
//						pClient->GetIndexID(),pBuf,nLen,pSocketOper->lNum2);
//					TemptCommands.push_front(pTemptSocketOpera);
//
//					delete []pSocketOper->pBuf;
//					delete pSocketOper;
//					break;
//				}
//
//				delete []pSocketOper->pBuf;
//				delete pSocketOper;
//
//			}
//		}
//
//		if(pClient->m_bSendData == false)
//		{
//		}
//		TemptCommands.clear();
//	}
//
//	TemptCommands.clear();
//}


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
	bool bRun = true;
	while(bRun)
	{
		pClient->m_SocketOperaCommands.CopyAllCommand(TemptCommands);
		CommandsQueue::iterator itCommand = TemptCommands.begin();
		for(; itCommand != TemptCommands.end();itCommand++)
		{
			tagSocketOper* pSocketOper = (*itCommand);
			if(pSocketOper)
			{
				switch(pSocketOper->OperaType)
				{
							//初始化操作
				case SCOT_Client_OnSend:
					{
						pClient->OnSend(pSocketOper->lNum1);
					}
					break;
					//主动断开某个连接
				case SCOT_Client_Send:
					{
						pClient->AddSendMsg((CBaseMessage*)pSocketOper->pBuf);
					};
					break;
				case SCOT_Client_OnRecv:
					{
						pClient->OnReceive((CDataBlock*)pSocketOper->pBuf,pSocketOper->lNum1);
					}
					break;
					//退出线程操作
				case SCOT_Client_ExitThread:
					{
						bRun = false;
					}
					break;
				}
				pClient->FreeSockOper(pSocketOper);
			}
		}
		TemptCommands.clear();
		//Sleep(1);
	}
}

//客户端网络主线程 
unsigned __stdcall NetClientThreadFunc(void* pArguments)
{
	PutTraceString(NET_MODULE,"the Client socket Net_main_thread start.");
	CClient* pClient = (CClient*) pArguments;
	if(pClient==NULL)
	{
		PutTraceString(NET_MODULE,"the Client socket Net_main_thread Quit.");
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
		PutErrorString(NET_MODULE,"%-15s %s",__FUNCTION__,"网络的客户端主线程出错!，请查看最新的错误报告文件");
	}
#endif

	PutTraceString(NET_MODULE,"the Client socket Net_main_thread Quit.");
	_endthreadex( 0 );
	return 0;
}