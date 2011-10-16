#include "StdAfx.h"
#include "myserverclient.h"
#include "nets\netserver\message.h"
#include "server\gameserver\appserver\Player.h"
#include "server\gameserver\appserver\message\sequencestring.h"
#include "../PUBLIC/Crc32Static.h"
#include <MMSystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_CLIENT_MESSAGE_LENGTH = 20480;

const long MaxSendBuffSize = 1024*50;

CMyServerClient::CMyServerClient(CServer*	pServers)
:CServerClient(pServers)
{
	m_nBufferSize = MAX_CLIENT_MESSAGE_LENGTH;
	m_nIniMsgLength = MAX_CLIENT_MESSAGE_LENGTH;
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
	//CMessage::GetRecvQueue()->ClearSocket(this);
	long lMapID = GetMapID();

	if(lMapID==0)
		return;
	CMessage* msg = new CMessage(MSG_S2S_LOG_PLAYER_ONLOST);

	msg->Add(lMapID);
	msg->Add("");
	GetServer()->GetRecvMessages()->PushMessage(msg);


	CMySocket::OnClose();
}

bool CMyServerClient::CheckMsg(CBaseMessage *pMsg)
{
	//错误的消息类型
	if( pMsg->GetType() <= MSG_C2S || pMsg->GetType() >= MSG_C2C)
	{
		m_pServers->AddForbidIP(GetDwordIP());
		m_pServers->QuitClientBySocketID(GetIndexID());
		char str[200];
		sprintf(str, "玩家%d(IP:%s)发来的消息。错误的消息类型。",GetMapID(),GetIP());
		PutDebugString(str);
		return false;
	}

	ULONG lPlayerID = GetMapID();
	if(lPlayerID)
	{
		// 消息耗时验证
		if( GetGame()->GetSetup()->dwMsgValidateTime )
		{
			unsigned long ulClientTime = pMsg->GetMsgSendTime();
			if(ulClientTime)
			{
				unsigned long cur_time = /*(ULONG)time(NULL)*/ timeGetTime();
				long time_stamp = /*timeGetTime()*/ cur_time - ulClientTime;

				// 消息发送耗时在 -x <= time_stamp <= +x 是合法的
				// | ±time_stamp | <= x	合法
				//	     		     > x	非法
				bool *enable = GetGame()->GetValidateTime(lPlayerID);
				if( enable && *enable )
				{
					if( ulClientTime==0 )
					{
						AddLogText("启用时间效验后收到一次0时间 玩家%d",lPlayerID);
					}
					else
					{
						if( abs(time_stamp) > GetGame()->GetSetup()->dwMsgValidateTime )
						{
							AddLogText("<worning>玩家 %d 消息耗时太长 %d 毫秒，S:%d C:%d (消息类型 %d)", lPlayerID, time_stamp, cur_time, ulClientTime, pMsg->GetType());
							m_pServers->AddForbidIP(GetDwordIP());
							m_pServers->QuitClientBySocketID(GetIndexID());
							return false;
						}
					}
				}
			}
		}
		//启动了这个功能
		if( GetGame()->GetSetup()->dwUseSeqeunce && CSequenceString::m_sSequenceString.size())
		{
			ULONG cssCode = pMsg->GetCode();
			if(cssCode)
			{
				// 通过序列字符串来效验消息
				CSequenceString *css = GetGame()->GetSequence(lPlayerID);
				if( css && css->m_bIsUsable && css->GetPos() != -1)
				{
					if( cssCode == 0 )
					{
						AddLogText("启用序列后收到一次0序列 玩家%d",lPlayerID);
					}
					else
					{
						if( css->Check(cssCode) == 0 )
						{
							AddErrorLogText("客户端[%d] 序列[%d] 序列位子[%d]",lPlayerID,cssCode, css->GetPos());
							AddErrorLogText( "消息类型 %d", pMsg->GetType() );
							m_pServers->AddForbidIP(GetDwordIP());
							m_pServers->QuitClientBySocketID(GetIndexID());
							return false;
						}
					}
				}
			}
		}

	}
	return true;
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
//				sprintf(str, "玩家%d(IP:%s)发来的消息。消息长度校验失败。",GetMapID(),GetIP());
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
//					char str[200];
//					sprintf(str, "玩家%d(IP:%s)发来的消息。消息内容校验失败。",GetMapID(),GetIP());
//					PutDebugString(str);
//					return;
//				}
//			}
//
//			CBaseMessage* pMsg = CreateMsg(&m_pBuffer[lPoint]+12, lRLESize-12);
//			// 消息创建成功并压入消息队列
//			if( pMsg )
//			{
//				if(CheckMsg(pMsg) == false)
//				{
//					m_nSize = 0;
//					delete pMsg;
//					return;
//				}
//
//				pMsg->SetClientSocketID(GetIndexID());
//				pMsg->SetMapID(GetMapID());
//				pMsg->SetCdkey(GetMapStr());
//				pMsg->SetIP(GetDwordIP());
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

void CMyServerClient::OnOneMessageSizeOver(long lSize,long lPermitSize)
{
	char str[256];
	sprintf(str, "客户端(PlayerID:%d,IP:%s)发来的消息(长度:%d BYTE)超过允许的最大值(%d BYTE),被强制中断。",
		GetMapID(),GetIP(),lSize,lPermitSize);
	PutDebugString(str);
}
void CMyServerClient::OnTotalMessageSizeOver(long lSize,long lPermitSize)
{
	char str[256];
	sprintf(str, "客户端(PlayerID:%d,IP:%s)每秒发来的消息大小(%d BYTE)超过允许的最大值(%d BYTE),被强制中断。",
		GetMapID(),GetIP(),lSize,lPermitSize);
	PutDebugString(str);
}