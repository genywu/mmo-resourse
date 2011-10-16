#include "stdafx.h"
#include "message.h"
#include <MMSystem.h>

#include "..\Servers.h"
#include "../PUBLIC/Crc32Static.h"
//#include "CostNetClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void OnClientMessage(CMessage* pMsg);				// 响应日志消息
extern void OnGapp2SMMessage(CMessage* pMsg);				//GAPP发送到SM的消息处理
extern void OnLoginMessage(CMessage* pMsg);					//SCC登录消息处理
extern void OnCtrlMessage(CMessage* pMsg);					//控制消息
extern void OnManageMessage(CMessage* pMsg);				//管理消息
CServer	*CMessage::m_pClientServer;
long	CMessage::m_lLogingSocket;
long	CMessage::m_lSccSocket;

CMessage::HMapLongLong	CMessage::m_SocketSCCID;
CMessage::HMapLongLong	CMessage::m_SCCIDSocket;

CMessage::CMessage()
{
	m_lSocketID	=0;
	m_dwIP		=0;
	m_dwRecvTime = 0;
}

CMessage::CMessage(long lType)
{
	
	m_lSocketID	=0;
	m_dwIP		=0;
	m_dwRecvTime = 0;

	Init(lType);
}

CMessage::~CMessage(void)
{
	UnInit();
}

 //重设消息相关变量
void CMessage::Reset(void)
{
	m_lSocketID=0;
	m_dwIP=0;
}

bool CMessage::Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum)
{
	m_SocketSCCID.clear();
	m_SCCIDSocket.clear();

	CBaseMessage::NewMsg = NewMessage;
	return CBaseMessage::Initial(pDBAllocator,nMaxFreeMsgNum);
}
bool CMessage::Release()
{
	CBaseMessage::NewMsg = NULL;
	return CBaseMessage::Release();
}

CBaseMessage* CMessage::NewMessage()
{
	return new CMessage();
}

// //发送到Gameserver的Socket
long CMessage:: SendToSocket(long lSocketID)
{
	assert(m_pClientServer);
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	//分配一个消息
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	//vector<CDataBlock*>& MsgData = GetMsgData();
	//pMsg->SetMsgData(MsgData);
	//MsgData.clear();
	Clone(pMsg);
	m_pClientServer->ASend(lSocketID,pMsg);
	return true;
}

// //发送到Gameserver的Socket
long CMessage:: SendGSID(long lGSID)
{
	long lSocket = GetSocketBySCCID(lGSID);
	SendToSocket(lSocket);
	return true;
}

// [服务器] 发送给所有客户端
long CMessage::SendAll()
{
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	itLL it = m_SocketSCCID.begin();
	for(;it != m_SocketSCCID.end();it++)
	{
		CBaseMessage *pMsg = CBaseMessage::AllocMsg();
		Clone(pMsg);
		m_pClientServer->ASend((*it).first,pMsg);
	}
	return true;
}

long CMessage::Send(bool bFront)
{
	assert(m_pClientServer);
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	//分配一个消息
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	vector<CDataBlock*>& MsgData = GetMsgData();
	pMsg->SetMsgData(MsgData);
	MsgData.clear();
	m_pClientServer->ASend(m_lLogingSocket,pMsg);
	return 0;
}

void CMessage::SendToCost(bool bFront)
{	
	return;
}

long CMessage::GetSCCID()
{
	return GetSCCIDBySocket(m_lSocketID);
}

void CMessage::MapSCCIDSocket(long lSCCID,long lSocketID)
{
	m_SocketSCCID[lSocketID]=lSCCID;
	m_SCCIDSocket[lSCCID]=lSocketID;
}
long CMessage::GetSocketBySCCID(long lSCCID)
{
	itLL it = m_SCCIDSocket.find(lSCCID);
	if(it != m_SCCIDSocket.end())
		return (*it).second;
	return 0;
}
long CMessage::GetSCCIDBySocket(long lSocketID)
{
	itLL it = m_SocketSCCID.find(lSocketID);
	if(it != m_SocketSCCID.end())
		return (*it).second;
	return 0;
}

void CMessage::UnMapSocket(long lSocketID,long lNetFlag)
{
	//if(lNetFlag&NF_Server_Client)
	//{
	//	UnMapGSIDSocket(lSocketID);
	//}
}
void CMessage::UnMapSCCIDSocket(long lSocketID)
{
	itLL it =  m_SocketSCCID.find(lSocketID);
	if(it != m_SocketSCCID.end() )
	{
		m_SCCIDSocket.erase((*it).second);
		m_SocketSCCID.erase(it);
	}
}
long CMessage::SendToAllSCC()
{
	/*itLL it =  m_SocketSCCID.begin();
	for (;it!=m_SocketSCCID.end();it++)
	{
		SendToSocket( it->first );
	}*/
	SendToSocket(m_lSccSocket);
	return 0;
}

long CMessage::SendToScc(long sccID)
{
	itLL it = m_SCCIDSocket.find(sccID);
	if (it!=m_SCCIDSocket.end())
	{
		SendToSocket(it->second);
	}
	return 0;
}

long CMessage::SendToGapp(long lID)
{
	GetInst(CMemServer).Send(lID,this);
	return 0;
}
////////////////////////////////////////////////////////////////////////
//	消息处理
////////////////////////////////////////////////////////////////////////

long CMessage::DefaultRun()
{
	switch( GetType() )
	{
	case BASEMSG_Socket_Accept:
		{
			//GetGame()->OnGameServerConnect(GetIP(), GetSocketID());
		}
		break;
	case BASEMSG_Socket_Close:
		{
			if(GetNetFlag()&NF_SCC_Client)
			{
				GetSM()->OnSCCClose();
			//	CMessage::UnMapSCCIDSocket(GetSocketID());
			}
		}
		break;
	}
	return true;
}

// 服务器端对GAPP消息处理
long CMessage::Run()
{
	// 消息分类处理
	switch( GetType() - (GetType() & 0x00000ff) )	// GetType() - GetType() % 256
	{
	case 0:
		DefaultRun();
		break;
	//----------------------------------------------------/客户端连接消息消息/
	case MSG_SCC2SM_LOGIN:
		OnLoginMessage(this);
		break;
	case MSG_SCC2SM_CTRL:
		OnCtrlMessage(this);
		break;
	case MSG_GAPP2SM_OPERATOR:
		OnGapp2SMMessage(this);
		break;
	case MSG_SCC2SM_MANAGE:
		OnManageMessage(this);
	}


	return true;
}
