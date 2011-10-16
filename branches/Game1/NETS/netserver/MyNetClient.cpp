#include "StdAfx.h"
#include "mynetclient.h"
#include "message.h"
#include "../PUBLIC/Crc32Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMyNetClient::CMyNetClient(void)
{
}

CMyNetClient::~CMyNetClient(void)
{
}

void CMyNetClient::HandleReceive()
{
	return;
}

void CMyNetClient::HandleClose()
{
	// 服务器断线
	CMessage* msg;
	msg = new CMessage(MSG_S2S_SERVER_WORLDSERVERCLOSE);

	GetRecvMessages()->PushMessage(msg);
}

void CMyNetClient::HandleConnect()
{
}

CBaseMessage* CMyNetClient::CreateMsg(void* pBuf, unsigned long nSize)
{
	return CBaseMessage::CreateWithoutRLE(pBuf, nSize);
}
//-----------------------------------------------------------
// 接受到数据的处理
// 这里接受到了从服务器发来的数据，做游戏的处理
//-----------------------------------------------------------
//void CMyNetClient::OnReceive(int nErrorCode)
//{
//	if(nErrorCode != 0)
//	{
//		//客户端接受数据失败
//		char str[256];
//		sprintf(str, "FD_READ failed with error %d\n",nErrorCode);
//		PutDebugString(str);
//		return;
//	}
//	int nReadSize = min(m_nBufferSize - m_nSize,10240);
//	int nRetsize = Recv(&m_pBuffer[m_nSize], nReadSize);		
//	if (nRetsize == SOCKET_ERROR)
//	{
//		int nError = GetLastError();
//		if(nError != WSAEWOULDBLOCK)
//		{
//			char str[200];
//			sprintf(str, "Client OnReceive Socket Error = %d",nError);
//			PutDebugString(str);
//		}
//		return;
//	}
//	else if (nRetsize == 0)
//	{
//		char str[256];
//		sprintf(str, "Client OnReceive size is 0");
//		PutDebugString(str);
//		return;
//	}
//	//添加接受统计量
//	AddRecvSize(nRetsize);
//	m_nSize += nRetsize;
//	long lPoint = 0;	// 当前有效缓冲区地址
//	while( m_nSize >= 12 )
//	{
//		long lRLESize = *((long*)&m_pBuffer[lPoint]);
//		//对长度内容作校验
//		DWORD dwLenCrc32;
//		CCrc32Static::DataCrc32(&lRLESize,4,dwLenCrc32);
//		DWORD lClientSize = *((DWORD*)(&m_pBuffer[lPoint]+4));
//		if(dwLenCrc32 != lClientSize)
//		{
//			char str[256];
//			sprintf(str, "从WorldServer 来的消息，长度校验错误,lPoint = %d,lRLESize=%d。",lPoint,lRLESize);
//			PutDebugString(str);
//			m_nSize=0;
//			return;
//		}
//		// 缓冲区数据大于等于一个消息的长度
//		if( lRLESize <= m_nSize )
//		{
//			CBaseMessage* pMsg = CreateMsg(&m_pBuffer[lPoint]+12, lRLESize-12);
//			// 消息创建成功并压入消息队列
//			if( pMsg )
//			{
//				DWORD dwConCrc32;
//				CCrc32Static::DataCrc32(pMsg->GetMsgBuf(),pMsg->GetSize(),dwConCrc32);
//				DWORD lClientConCrc32= *((DWORD*)(&m_pBuffer[lPoint]+8));
//				//检测到内容不一致
//				if(dwConCrc32 != lClientConCrc32)
//				{
//					char str[256];
//					sprintf(str, "从WorldServer 来的%d消息 内容校验错误。",pMsg->GetType());
//					PutDebugString(str);
//					m_nSize=0;
//					return;
//				}
//				m_RecvMessages.PushMessage( pMsg );
//
//				m_nSize -= lRLESize;
//				lPoint += lRLESize;		// 移动缓冲区指针
//				// 如果是超长的消息，把缓冲区大小调整回来
//				if( m_nBufferSize > MAX_MESSAGE_LENGTH && m_nSize <= MAX_MESSAGE_LENGTH )
//				{
//					char *pTemp = new char[m_nSize];
//					memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
//
//					SAFE_DELETE(m_pBuffer);
//					m_nBufferSize = MAX_MESSAGE_LENGTH;
//					m_pBuffer = new char[m_nBufferSize];
//
//					memcpy(m_pBuffer, pTemp, m_nSize);
//					SAFE_DELETE(pTemp);
//					lPoint = 0;
//				}
//			}
//			else
//			{
//				// 出错，抛弃当前收到的所有数据
//				m_nSize = 0;
//				char str[256];
//				sprintf(str, "CreateMessage error");
//				PutDebugString(str);
//				return;
//			}
//		}
//		else
//		{
//			// 出错，抛弃当前收到的所有数据
//			if( lRLESize < 0 )
//			{
//				m_nSize = 0;
//				char str[256];
//				sprintf(str, "出错，错误消息(RleSize=%d)，抛弃当前收到的所有数据", lRLESize);
//				PutDebugString(str);
//				return;
//			}
//
//			// 收到消息大于最大消息缓冲区，开新的缓冲区
//			if( lRLESize > m_nBufferSize )
//			{
//				char *pTemp = new char[m_nSize];
//				memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
//
//				SAFE_DELETE(m_pBuffer);
//				m_nBufferSize = lRLESize;
//				m_pBuffer = new char[m_nBufferSize];
//				memcpy(m_pBuffer, pTemp, m_nSize);
//				SAFE_DELETE(pTemp);
//
//				char str[256];
//				sprintf(str, "超长的消息 Size = %d", m_nBufferSize);
//				PutDebugString(str);
//				return;
//			}
//			break;
//		}
//	}
//	// 把不完全的消息移动到缓冲区头部
//	if( lPoint > 0 )
//	{
//		memmove(m_pBuffer, &m_pBuffer[lPoint], m_nSize);
//		lPoint = 0;
//	}
//}

void CMyNetClient::SetSendRevBuf()
{
	int nValue = 1024*1024*3;
	int nSize  = sizeof(nValue);
	setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,(char*)&nValue,nSize);
}