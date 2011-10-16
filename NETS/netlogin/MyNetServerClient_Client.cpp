#include "StdAfx.h"
#include "myNetServerclient_Client.h"
#include "message.h"
#include "../PUBLIC/Crc32Static.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_CLIENT_MESSAGE_LENGTH = 20480;
const long MaxSendBuffSize = 1024*50;

CMyNetServerClient_Client::CMyNetServerClient_Client(CServer* pServer)
:CMyServerClient(pServer)
{
	m_nBufferSize = MAX_CLIENT_MESSAGE_LENGTH;
	m_nIniMsgLength = MAX_CLIENT_MESSAGE_LENGTH;
	m_pBuffer = new char[m_nBufferSize]; 

	m_lMaxSendBuffSize = MaxSendBuffSize;
	m_lSendBufferSize = MaxSendBuffSize;
	m_pSendBuffer = new char[m_lSendBufferSize];
}

CMyNetServerClient_Client::~CMyNetServerClient_Client(void)
{
	SAFE_DELETE(m_pBuffer);
}
// 与对方断开的时候激活
void CMyNetServerClient_Client::OnClose(int errorCode)
{
	// 服务器断线
	if(strlen(GetMapStr())==0)
		return;
	CMessage* msg;
	msg = new CMessage(MSG_L2L_LOG_CLIENTCLOSE);
	msg->Add(const_cast<char*>(GetMapStr()));
	GetServer()->GetRecvMessages()->PushMessage(msg);

	CMySocket::OnClose();
}

bool CMyNetServerClient_Client::CheckMsg(CBaseMessage *pMsg)
{
	if(pMsg->GetType() <= MSG_C2L || pMsg->GetType() >= MSG_W2W)
	{
		m_pServers->AddForbidIP(GetDwordIP());
		m_pServers->QuitClientBySocketID(GetIndexID());
		char str[200];
		sprintf(str, "CDKey%s(IP:%s)发来的消息。错误的消息类型。",GetMapStr(),GetIP());
		PutDebugString(str);
		return false;
	}
	return true;
}

//-------------------------------------------------------------
// 使用完成端口
//-------------------------------------------------------------
//void CMyNetServerClient_Client::OnReceive(int nErrorCode)
//{	
//	if(m_pServers==NULL)	return;
//
//
//	long lPoint = 0;	// 当前有效缓冲区地址
//	while( m_nSize >= 12 )
//	{
//		long lRLESize = *((long*)(&m_pBuffer[lPoint]));
//
//		//对接受消息作校验
//		if(m_pServers->GetCheck())
//		{
//			//消息的长度做校验
//			if(lRLESize > m_pServers->GetMaxMsgLen())
//			{
//				OnOneMessageSizeOver(lRLESize,m_pServers->GetMaxMsgLen());
//				m_pServers->AddForbidIP(GetDwordIP());
//				m_pServers->QuitClientBySocketID(GetIndexID());
//				return;
//			}
//			//对长度内容作校验
//			DWORD dwLenCrc32;
//			CCrc32Static::DataCrc32(&lRLESize,4,dwLenCrc32);
//			DWORD lClientSize = *((DWORD*)(&m_pBuffer[lPoint]+4));
//			if(dwLenCrc32 != lClientSize)
//			{
//				m_pServers->AddForbidIP(GetDwordIP());
//				m_pServers->QuitClientBySocketID(GetIndexID());
//				char str[200];
//				sprintf(str, "CDKey%s(IP:%s)发来的消息。消息长度校验失败。",GetMapStr(),GetIP());
//				PutDebugString(str);
//				return;
//			}
//		}
//
//		// 缓冲区数据大于等于一个消息的长度
//		if( lRLESize <= m_nSize )
//		{
//			//对消息的内容做校验
//			if(m_pServers->GetCheckMsgCon())
//			{
//				DWORD dwConCrc32;
//				CCrc32Static::DataCrc32(&m_pBuffer[lPoint]+12,lRLESize-12,dwConCrc32);
//				DWORD lClientConCrc32= *((DWORD*)(&m_pBuffer[lPoint]+8));
//				//检测到内容不一致
//				if(dwConCrc32 != lClientConCrc32)
//				{
//					m_pServers->AddForbidIP(GetDwordIP());
//					m_pServers->QuitClientBySocketID(GetIndexID());
//					m_nSize = 0;
//					char str[200];
//					sprintf(str, "CDKey%s(IP:%s)发来的消息。消息内容校验失败。",GetMapStr(),GetIP());
//					PutDebugString(str);
//					return;
//				}
//			}
//
//			CMessage* pMsg = CreateMsg(&m_pBuffer[lPoint]+12, lRLESize-12);
//			// 消息创建成功并压入消息队列
//			if( pMsg )
//			{
//				if(CheckMsg(pMsg) == false)
//				{
//					m_nSize= 0;
//					delete pMsg;
//					return;
//				}
//				pMsg->SetClientSocketID(GetIndexID());
//				pMsg->SetMapID(GetMapID());
//				pMsg->SetCdkey(GetMapStr());
//				pMsg->SetIP(GetDwordIP());
//
//				m_pServers->GetRecvMessages()->PushMessage( pMsg );
//				m_nSize -= lRLESize;
//				lPoint += lRLESize;		// 移动缓冲区指针
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
//
//	// 把不完全的消息移动到缓冲区头部
//	if( lPoint > 0 )
//	{
//		memmove(m_pBuffer, &m_pBuffer[lPoint], m_nSize);
//		lPoint = 0;
//	}
//}

void CMyNetServerClient_Client::OnOneMessageSizeOver(long lSize,long lPermitSize)
{
	char str[256];
	sprintf(str, "客户端(CDkey:%s,IP:%s)发来的消息(长度:%d BYTE)超过允许的最大值(%d BYTE),被强制中断。",
		GetMapStr(),GetIP(),lSize,lPermitSize);
	PutDebugString(str);
}
void CMyNetServerClient_Client::OnTotalMessageSizeOver(long lSize,long lPermitSize)
{
	char str[256];
	sprintf(str, "客户端(CDkey:%s,IP:%s)每秒发来的消息大小(%d BYTE)超过允许的最大值(%d BYTE),被强制中断。",
		GetMapStr(),GetIP(),lSize,lPermitSize);
	PutDebugString(str);
}
