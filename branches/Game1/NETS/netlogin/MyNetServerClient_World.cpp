#include "StdAfx.h"
#include "myNetServerclient_World.h"
#include "message.h"
#include "../PUBLIC/Crc32Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_WORLD_MESSAGE_LENGTH = 10485760;

const long MaxSendBuffSize = 1024*1024;

CMyNetServerClient_World::CMyNetServerClient_World(CServer* pServer)
:CMyServerClient(pServer)
{
	m_nBufferSize = MAX_WORLD_MESSAGE_LENGTH;
	m_nIniMsgLength = MAX_WORLD_MESSAGE_LENGTH;
	m_pBuffer = new char[m_nBufferSize];

	m_lMaxSendBuffSize = MaxSendBuffSize;
	m_lSendBufferSize = MaxSendBuffSize;
	m_pSendBuffer = new char[m_lSendBufferSize];
}

CMyNetServerClient_World::~CMyNetServerClient_World(void)
{
	SAFE_DELETE(m_pBuffer);
}


// 与对方断开的时候激活
void CMyNetServerClient_World::OnClose(int errorCode)
{
	// 服务器断线
	CMessage* msg;
	msg = new CMessage(MSG_L2L_SERVER_WORLDSERVERLOST);
	msg->SetMapID(GetMapID());
	GetServer()->GetRecvMessages()->PushMessage(msg);

	CMySocket::OnClose();
}

void CMyNetServerClient_World::SetSendRevBuf()
{
	//把内部套接字的发送缓冲区设为0
	int nValue = 0;
	int nSize  = sizeof(nValue);
	setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,(char*)&nValue,sizeof(nSize));
	//nValue = 1048576;
	//nSize  = sizeof(nValue);
	//int nRet = setsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,(char*)&nValue,sizeof(nSize));
	//if(nRet == SOCKET_ERROR)
	//{
	//	int nError = WSAGetLastError();
	//	char str[256];
	//	sprintf(str,"设置客户端接受缓冲区出错:%d.",nError);
	//	PutDebugString(str);
	//}
}
//创建消息
CBaseMessage* CMyNetServerClient_World::CreateMsg(void* pBuf, unsigned long nSize)
{
	return CBaseMessage::CreateWithoutRLE(pBuf,nSize);
}

////-------------------------------------------------------------
//// 使用完成端口
////-------------------------------------------------------------
//void CMyNetServerClient_World::OnReceive(int nErrorCode)
//{	
//	if(m_pServers==NULL)	return;
//
//
//	long lPoint = 0;	// 当前有效缓冲区地址
//	while( m_nSize >= 12 )
//	{
//		long lRLESize = *((long*)(&m_pBuffer[lPoint]));
//
//		//对长度内容作校验
//		DWORD dwLenCrc32;
//		CCrc32Static::DataCrc32(&lRLESize,4,dwLenCrc32);
//		DWORD lClientSize = *((DWORD*)(&m_pBuffer[lPoint]+4));
//		if(dwLenCrc32 != lClientSize)
//		{
//			char str[256];
//			sprintf(str, "WorldServer %d发来的消息,长度校验错误。",GetMapID());
//			PutDebugString(str);
//			m_nSize=0;
//			return;
//		}
//
//		// 缓冲区数据大于等于一个消息的长度
//		if( lRLESize <= m_nSize )
//		{
//			CMessage* pMsg = CreateMsg(&m_pBuffer[lPoint]+12, lRLESize-12);
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
//					sprintf(str, "WorldServer %d发来的消息%d,内容校验错误。",GetMapID(),pMsg->GetType());
//					PutDebugString(str);
//					m_nSize=0;
//					return;
//				}
//
//				pMsg->SetClientSocketID(GetIndexID());
//				pMsg->SetMapID(GetMapID());
//				pMsg->SetCdkey(GetMapStr());
//				pMsg->SetIP(GetDwordIP());
//
//				m_pServers->GetRecvMessages()->PushMessage( pMsg );
//				m_nSize -= lRLESize;
//				lPoint += lRLESize;		// 移动缓冲区指针
//
//				// 如果是超长的消息，把缓冲区大小调整回来
//				if( m_nBufferSize > m_nIniMsgLength && m_nSize <= m_nIniMsgLength )
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
//
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
//
//				char str[256];
//				sprintf(str, "出错，错误消息(RleSize=%d)，抛弃当前收到的所有数据", lRLESize);
//				PutDebugString(str);
//
//				return;
//			}
//
//			// 收到消息大于最大消息缓冲区，开新的缓冲区
//			if( lRLESize > m_nBufferSize )
//			{
//				char *pTemp = new char[m_nSize];
//				memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
//				SAFE_DELETE(m_pBuffer);
//				m_nBufferSize = lRLESize;
//				m_pBuffer = new char[m_nBufferSize];
//				memcpy(m_pBuffer, pTemp, m_nSize);
//				SAFE_DELETE(pTemp);
//
////#ifdef _DEBUG
//				char str[256];
//				sprintf(str, "WorldServer%d 超长的消息 Size = %d",GetMapID(),m_nBufferSize);
//				PutDebugString(str);
////#endif
//				return;
//			}
//
//			char str[256];
//			sprintf(str, "WorldServer%d 不完整的消息 m_nSize = %d,lRLESize = %d, Point = %d",GetMapID(),m_nSize,lRLESize, lPoint);
//			PutDebugString(str);
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