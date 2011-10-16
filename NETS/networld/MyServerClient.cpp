#include "stdafx.h"
#include <mmsystem.h>
#include "myserverclient.h"
#include "MyNetServer.h"
#include "message.h"
#include "../PUBLIC/Crc32Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int MAX_GAMESERVER_MESSAGE_LENGTH = 10485760*2;
const long MaxSendBuffSize = 1024*1024*5;


CMyServerClient::CMyServerClient(CServer* pServer)
:CServerClient(pServer)
{
	m_nBufferSize = MAX_GAMESERVER_MESSAGE_LENGTH;
	m_nIniMsgLength = MAX_GAMESERVER_MESSAGE_LENGTH;
	m_pBuffer = new char[m_nBufferSize];

	m_lMaxSendBuffSize = MaxSendBuffSize;
	m_lSendBufferSize = MaxSendBuffSize;
	m_pSendBuffer = new char[m_lSendBufferSize];
}

CMyServerClient::~CMyServerClient(void)
{
	SAFE_DELETE(m_pBuffer);
}


// 与对方断开的时候激活
void CMyServerClient::OnClose(int errorCode)
{
	// 服务器断线
	CMessage* msg;
	msg = new CMessage(MSG_W2W_SERVER_GAMESERVERLOST);
	msg->Add(GetMapID());
	
	GetServer()->GetRecvMessages()->PushMessage(msg);

	CMySocket::OnClose();
}

void CMyServerClient::SetSendRevBuf()
{
	//把内部套接字的发送缓冲区设为0
	/*int nValue = 0;
	int nSize  = sizeof(nValue);
	setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,(char*)&nValue,sizeof(nSize));
	nValue = 1048576*6;
	nSize  = sizeof(nValue);
	setsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,(char*)&nValue,sizeof(nSize));*/
}

//创建消息
CBaseMessage* CMyServerClient::CreateMsg(void* pBuf, unsigned long nSize)
{
	return CBaseMessage::CreateWithoutRLE(pBuf,nSize);
}
//-------------------------------------------------------------
// 使用完成端口
//-------------------------------------------------------------
//void CMyServerClient::OnReceive(int nErrorCode)
//{	
//	if(m_pServers==NULL)	return;
//	long lPoint = 0;	// 当前有效缓冲区地址
//	while( m_nSize >= 12 )
//	{
//		long lRLESize = *((long*)(&m_pBuffer[lPoint]));
//		//对长度内容作校验
//		DWORD dwLenCrc32;
//		CCrc32Static::DataCrc32(&lRLESize,4,dwLenCrc32);
//		DWORD lClientSize = *((DWORD*)(&m_pBuffer[lPoint]+4));
//		if(dwLenCrc32 != lClientSize)
//		{
//			char str[256];
//			sprintf(str, "Server%d发来的消息,长度校验错误(CRC1:%d,CRC2:%d)。",GetMapID(),dwLenCrc32,lClientSize);
//			PutDebugString(str);
//			m_nSize=0;
//			return;
//		}
//
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
//					sprintf(str, "GameServer%d发来的消息%d,内容校验错误。(CRC1:%d,CRC2:%d)",GetMapID(), pMsg->GetType(),dwConCrc32,lClientConCrc32);
//					PutDebugString(str);
//					m_nSize=0;
//					return;
//				}
//				pMsg->SetClientSocketID(GetIndexID());
//				pMsg->SetMapID(GetMapID());
//				pMsg->SetCdkey(GetMapStr());
//				pMsg->SetIP(GetDwordIP());
//				m_pServers->GetRecvMessages()->PushMessage( pMsg );
//				m_nSize -= lRLESize;
//				lPoint += lRLESize;		// 移动缓冲区指针
//				// 如果是超长的消息，把缓冲区大小调整回来
//				if( m_nBufferSize > m_nIniMsgLength && m_nSize <= m_nIniMsgLength )
//				{
//					char *pTemp = new char[m_nSize];
//					memcpy(pTemp, &m_pBuffer[lPoint], m_nSize);
//					SAFE_DELETE(m_pBuffer);
//					m_nBufferSize = m_nIniMsgLength;
//					m_pBuffer = new char[m_nBufferSize];
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
//				sprintf(str, "GameServer%d CreateMessage error",GetMapID());
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
//				sprintf(str, "GameServer%d 错误消息(RleSize=%d)，抛弃当前收到的所有数据",GetMapID(),lRLESize);
//				PutDebugString(str);
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

