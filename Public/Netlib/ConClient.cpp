//=============================================================================
/**
 *  file: ConClient.cpp
 *
 *  Brief:用完成端口实现的主动连接端。
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-6-11
 */
//=============================================================================
#include "StdAfx.h"
#include "ConClient.h"
#include "Servers.h"
#include "basemessage.h"
#include "../PUBLIC/Crc32Static.h"

CConClient::CConClient(CServer*	pServers)
:m_pServers(pServers)
{
	assert(m_pServers);
	m_nBufferSize = MAX_MESSAGE_LENGTH;
	m_nIniMsgLength = MAX_MESSAGE_LENGTH;
	m_nSize = 0;
	m_pBuffer = new char[m_nBufferSize];

	m_lMaxSendBuffSize = MAX_MESSAGE_LENGTH;
	m_lSendBufferSize = MAX_MESSAGE_LENGTH;
	m_lCurSendBufferSize = 0;
	m_pSendBuffer = new char[m_lSendBufferSize];

	m_lIOOperatorNum = 0;
	m_bCloseFlag = false;
}

CConClient::~CConClient(void)
{
}

//创建一个和完成端口邦定的SOCKET
BOOL CConClient::CreateEx(	UINT nSocketPort,
							LPCTSTR lpszSocketAddress,
							int nSocketType)
{
	if(!CMySocket::CreateEx(nSocketPort,lpszSocketAddress,nSocketType))
	{
		PutDebugString("在CConClient::CreateEx(...)中,CMySocket::CreateEx()操作出错!");
		return FALSE;
	}
	//设置发送和接收缓冲区
	SetSendRevBuf();
	long lID = GetSocketID();
	SetIndexID(lID);
	return TRUE;
}

BOOL CConClient::Close()
{
	if(INVALID_SOCKET == m_hSocket)
		return FALSE;
	CancelIo((HANDLE)m_hSocket);
	CMySocket::Close();
	return TRUE;
}

//连接指定服务器
//dwTimeOut==0表示异步，如果大于0,表示同步等待时间。
BOOL CConClient::Connect(LPCTSTR lpszHostAddress, UINT nHostPort,DWORD dwTimeOut)
{
	if(m_hSocket == INVALID_SOCKET)	return FALSE;
	
	//设置连接的服务器IP和端口
	SetPeerIP(lpszHostAddress);
	SetPeerPort(nHostPort);

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));
	LPSTR lpszAscii = (LPTSTR)lpszHostAddress;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		PutDebugString("在函数CConClient::Connect(...)中,地址无效。");
		return FALSE;
	}

	sockAddr.sin_port = htons((u_short)nHostPort);
	int ret = connect(m_hSocket,(SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if( ret == SOCKET_ERROR )
	{
		DWORD dwError = WSAGetLastError();
		if(  dwError != WSAEWOULDBLOCK )
		{
			char str[200];
			sprintf(str, "Clients Connect Socket Error = %d",dwError);
			PutDebugString(str);
			return FALSE;
		}

		//等候一段时间,用是否可写来判断连接成功与否
		fd_set writefd;
		FD_ZERO(&writefd);
		FD_SET(m_hSocket,&writefd);
		timeval waittime = {dwTimeOut/1000,(dwTimeOut%1000)*1000};
		int nRet = select(0,NULL,&writefd,NULL,&waittime);
		if(nRet == 0 || nRet == SOCKET_ERROR)
		{
			dwError = WSAGetLastError();
			char str[200];
			sprintf(str, "Clients Connect Socket Error = %d",dwError);
			PutDebugString(str);
			return FALSE;
		}
	}
	return	TRUE;
}

bool CConClient::Read(PER_IO_OPERATION_DATA* pIOData)
{

	pIOData->DataBuf.len = pIOData->nBufSize;
	pIOData->DataBuf.buf = pIOData->pBuffer;
	pIOData->OperationType = SOT_CONReceive;
	DWORD dwFlag = 0;
	DWORD dwReceivByte;
	int nRet = WSARecv(m_hSocket,&(pIOData->DataBuf),1,&dwReceivByte,&dwFlag,&(pIOData->OverLapped),NULL);
	if(nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			return false;
		}
	}
	return true;
}
//发送数据
int CConClient::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	if(m_lCurSendBufferSize <= 0)
	{
		return true;
	}

	PER_IO_OPERATION_DATA* pPerIOData = (PER_IO_OPERATION_DATA*)calloc(1,sizeof(PER_IO_OPERATION_DATA));
	//当分配不出内存时
	if(pPerIOData == NULL)
	{
#ifdef _DEBUG
		char str[200] = "向服务器发送消息时不能分配内存。";
		PutDebugString(str);
#endif // _DEBUG

		return false;
	}

	char* pTemptBuf = new char[m_lCurSendBufferSize];
	memcpy(pTemptBuf,m_pSendBuffer,m_lCurSendBufferSize);

	pPerIOData->OperationType = SOT_CONSend;
	pPerIOData->nBufSize = m_lCurSendBufferSize;
	pPerIOData->pBuffer= (CHAR*)pTemptBuf;
	pPerIOData->DataBuf.buf=pPerIOData->pBuffer;
	pPerIOData->DataBuf.len=pPerIOData->nBufSize;
	pPerIOData->hSocket = m_hSocket;

	DWORD dwSentNum = 0;
	int ret = WSASend(m_hSocket, &(pPerIOData->DataBuf), 1, &dwSentNum, nFlags,
		(OVERLAPPED*)pPerIOData, NULL);

	if (ret == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();
		if ( nError != WSA_IO_PENDING)
		{
			free(pPerIOData);
			char str[200];
			sprintf(str, "向服务器发送消息错误(errorID:%d)。",nError);
			PutDebugString(str);
			return false;
		}
	}

	m_lCurSendBufferSize = 0;
	m_lIOOperatorNum++;
	//如果发送缓冲区大于规定的则还原
	if(m_lSendBufferSize > m_lMaxSendBuffSize)
	{
		m_lSendBufferSize = m_lMaxSendBuffSize;
		SAFE_DELETE(m_pSendBuffer);
		m_pSendBuffer = new char[m_lSendBufferSize];

	}
	return true;
}
void CConClient::OnReceive(int nErrorCode)
{
	if(m_pServers==NULL)	return;
	long lPoint = 0;	// 当前有效缓冲区地址
	while( m_nSize >= 12 )
	{
		long lRLESize = *((long*)(&m_pBuffer[lPoint]));
		//对接受消息作校验
		if(m_pServers->GetCheck())
		{
			//对长度内容作校验
			DWORD dwLenCrc32;
			CCrc32Static::DataCrc32(&lRLESize,4,dwLenCrc32);
			DWORD lClientSize = *((DWORD*)(&m_pBuffer[lPoint]+4));
			if(dwLenCrc32 != lClientSize)
			{
				char str[200];
				sprintf(str, "服务器(IP:%s)发来的消息。消息长度校验失败。",GetPeerIP());
				PutDebugString(str);
				return;
			}
		}
		// 缓冲区数据大于等于一个消息的长度
		if( lRLESize <= m_nSize )
		{
			//对消息的内容做校验
			if(m_pServers->GetCheckMsgCon())
			{
				DWORD dwConCrc32;
				CCrc32Static::DataCrc32(&m_pBuffer[lPoint]+12,lRLESize-12,dwConCrc32);
				DWORD lClientConCrc32= *((DWORD*)(&m_pBuffer[lPoint]+8));
				//检测到内容不一致
				if(dwConCrc32 != lClientConCrc32)
				{
					m_pServers->AddForbidIP(GetDwordIP());
					m_pServers->QuitClientBySocketID(GetIndexID());
					char str[200];
					sprintf(str, "服务器端(IP:%s)发来的消息。消息内容校验失败。",GetPeerIP());
					PutDebugString(str);
					return;
				}
			}

			CBaseMessage* pMsg = CreateMsg(&m_pBuffer[lPoint]+12, lRLESize-12);
			// 消息创建成功并压入消息队列
			if( pMsg )
			{
				pMsg->SetClientSocketID(GetIndexID());
				m_pServers->GetRecvMessages()->PushMessage( pMsg );
				m_nSize -= lRLESize;
				lPoint += lRLESize;		// 移动缓冲区指针

				// 如果是超长的消息，把缓冲区大小调整回来
				if( m_nBufferSize > m_nIniMsgLength && m_nSize <= m_nIniMsgLength )
				{
					char *pTemp = new char[m_nSize];
					memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);

					SAFE_DELETE(m_pBuffer);
					m_nBufferSize = MAX_MESSAGE_LENGTH;
					m_pBuffer = new char[m_nBufferSize];

					memcpy(m_pBuffer, pTemp, m_nSize);
					SAFE_DELETE(pTemp);
					lPoint = 0;
				}
			}
			else
			{
				// 出错，抛弃当前收到的所有数据
				m_nSize = 0;
				char str[256];
				sprintf(str, "服务器(IP:%s)发来的消息出错，CreateMessage error",GetPeerIP());
				PutDebugString(str);
				return;
			}
		}
		else
		{
			// 出错，抛弃当前收到的所有数据
			if( lRLESize < 0 )
			{
				m_nSize = 0;
				char str[256];
				sprintf(str, "服务器(IP:%s)发来的消息出错，错误消息(RleSize=%d)，抛弃当前收到的所有数据",
							GetIP(),lRLESize);
				PutDebugString(str);
				return;
			}
			// 收到消息大于最大消息缓冲区，开新的缓冲区
			if( lRLESize > m_nBufferSize )
			{
				char *pTemp = new char[m_nSize];
				memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
				SAFE_DELETE(m_pBuffer);
				m_nBufferSize = lRLESize;
				m_pBuffer = new char[m_nBufferSize];
				memcpy(m_pBuffer, pTemp, m_nSize);
				SAFE_DELETE(pTemp);
				return;
			}
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
void CConClient::OnConnect(int nErrorCode)
{
}
void CConClient::OnClose(int nErrorCode)
{
}
void CConClient::OnSend(int nErrorCode)
{
}

// 给派生类使用的接口
void CConClient::HandleReceive()
{
}
void CConClient::HandleConnect()
{
}
void CConClient::HandleClose()
{
}

void CConClient::AddReceiveData(CHAR* pBuf,int nBufSize)
{
	if(pBuf == NULL || nBufSize <= 0)
		return;

	if(m_nSize + nBufSize <= m_nBufferSize)
	{
		memcpy(&m_pBuffer[m_nSize],pBuf,nBufSize);
		m_nSize = m_nSize+nBufSize;
	}
	else
	{
		CHAR* pNewBuf = new CHAR[m_nSize+nBufSize];
		memcpy(&pNewBuf[0],&m_pBuffer[0],m_nSize);
		memcpy(&pNewBuf[m_nSize],pBuf,nBufSize);

		delete []m_pBuffer;
		m_pBuffer = pNewBuf;
		m_nBufferSize = m_nSize+nBufSize;
		m_nSize = m_nBufferSize;
	}
}
bool CConClient::AddSendData(void* pBuf,int nBuffSize)
{
	if(pBuf == NULL)	return false;
	//已经提示了关闭
	if( GetCloseFlag() == true)
		return true;

	if(m_lCurSendBufferSize + nBuffSize > m_pServers->GetMaxClientSendBufSize() )
	{
	 	return false;
	}

	if(m_lCurSendBufferSize + nBuffSize <= m_lSendBufferSize)
	{
		memcpy(m_pSendBuffer+m_lCurSendBufferSize,pBuf,nBuffSize);
	}
	else
	{
		char* pTemptBuf = new char[m_lCurSendBufferSize];
		memcpy(pTemptBuf,m_pSendBuffer,m_lCurSendBufferSize);

		SAFE_DELETE(m_pSendBuffer);
		m_lSendBufferSize = (m_lCurSendBufferSize+nBuffSize)*2;
		m_pSendBuffer = new char[m_lSendBufferSize];

		memcpy(m_pSendBuffer,pTemptBuf,m_lCurSendBufferSize);
		memcpy(m_pSendBuffer+m_lCurSendBufferSize,pBuf,nBuffSize);

		SAFE_DELETE(pTemptBuf);
	}
	m_lCurSendBufferSize = m_lCurSendBufferSize+nBuffSize;
	return true;
}

CBaseMessage* CConClient::CreateMsg(void* pBuf, unsigned long nSize)
{
	return CBaseMessage::CreateWithoutRLE(pBuf,nSize);
}
