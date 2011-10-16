#include "stdafx.h"
#include "message.h"
#include <MMSystem.h>
#include "nets\MemClient.h"
#include "..\Servers.h"
#include "../PUBLIC/Crc32Static.h"
#include "..\public\Singleton.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void OnServerMessage(CMessage* pMsg);		// 响应Server类型消息
extern void OnDBOPMessage(CMessage* pMsg);			// 相应数据库操作消息
extern void OnSMMessage(CMessage* pMsg);			// SM消息
CServer	*CMessage::m_pClientServer;
long	CMessage::m_lLogingSocket;
long	CMessage::m_lCostSocket;

CMessage::HMapLongLong	CMessage::m_SocketGSID;
CMessage::HMapLongLong	CMessage::m_GSIDSocket;
CMessage::HMapLongLong	CMessage::m_SocketWSID;
CMessage::HMapLongLong	CMessage::m_WSIDSocket;



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
	m_SocketGSID.clear();
	m_GSIDSocket.clear();

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
	vector<CDataBlock*>& MsgData = GetMsgData();
	pMsg->SetMsgData(MsgData);
	MsgData.clear();
	m_pClientServer->ASend(lSocketID,pMsg);
	return true;
}

// //发送到Gameserver的Socket
long CMessage:: SendGSID(long lGSID)
{
	long lSocket = GetSocketByGSID(lGSID);
	SendToSocket(lSocket);
	return true;
}


// //发送到WS的Socket
long CMessage:: SendWSID(long lWSID)
{
	long lSocket = GetSocketByWSID(lWSID);
	SendToSocket(lSocket);
	return true;
}

// [服务器] 发送给所有客户端
long CMessage::SendAll()
{
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	itLL it = m_SocketGSID.begin();
	for(;it != m_SocketGSID.end();it++)
	{
		CBaseMessage *pMsg = CBaseMessage::AllocMsg();
		Clone(pMsg);
		m_pClientServer->ASend((*it).first,pMsg);
	}
	return true;
}
//发送一个消息给WorldServer套接字
long CMessage::SendToWorld(long lSocketID)
{
	assert(m_pClientServer);
	*((long*)Base()) = GetSize()+HEAD_SIZE;

	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	vector<CDataBlock*>& MsgData = GetMsgData();
	pMsg->SetMsgData(MsgData);
	MsgData.clear();
	m_pClientServer->ASend(lSocketID,pMsg);
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

long CMessage::GetGSID()
{
	return GetGSIDBySocket(m_lSocketID);
}

void CMessage::MapGSIDSocket(long lGSID,long lSocketID)
{
	m_SocketGSID[lSocketID]=lGSID;
	m_GSIDSocket[lGSID]=lSocketID;
}
long CMessage::GetSocketByGSID(long lGSID)
{
	itLL it = m_GSIDSocket.find(lGSID);
	if(it != m_GSIDSocket.end())
		return (*it).second;
	return 0;
}
long CMessage::GetGSIDBySocket(long lSocketID)
{
	itLL it = m_SocketGSID.find(lSocketID);
	if(it != m_SocketGSID.end())
		return (*it).second;
	return 0;
}

void CMessage::UnMapSocketGS(long lSocketID,long lNetFlag)
{
	if(lNetFlag&NF_Server_Client)
	{
		UnMapGSIDSocket(lSocketID);
	}
}
void CMessage::UnMapGSIDSocket(long lSocketID)
{
	itLL it =  m_SocketGSID.find(lSocketID);
	if(it != m_SocketGSID.end() )
	{
		m_GSIDSocket.erase((*it).second);
		m_SocketGSID.erase(it);
	}
}



//------------------
long CMessage::GetWSID()
{
	return GetWSIDBySocket(m_lSocketID);
}

void CMessage::MapWSIDSocket(long lWSID,long lSocketID)
{
	m_SocketWSID[lSocketID]=lWSID;
	m_WSIDSocket[lWSID]=lSocketID;
}
long CMessage::GetSocketByWSID(long lWSID)
{
	itLL it = m_WSIDSocket.find(lWSID);
	if(it != m_WSIDSocket.end())
		return (*it).second;
	return 0;
}
long CMessage::GetWSIDBySocket(long lSocketID)
{
	itLL it = m_SocketWSID.find(lSocketID);
	if(it != m_SocketWSID.end())
		return (*it).second;
	return 0;
}

void CMessage::UnMapSocketWS(long lSocketID,long lNetFlag)
{
	if(lNetFlag&NF_Server_Client)
	{
		UnMapWSIDSocket(lSocketID);
	}
}
void CMessage::UnMapWSIDSocket(long lSocketID)
{
	itLL it =  m_SocketWSID.find(lSocketID);
	if(it != m_SocketWSID.end() )
	{
		m_WSIDSocket.erase((*it).second);
		m_SocketWSID.erase(it);
	}
}

long CMessage::SendToSM()
{
	GetInst(CMemClient).Send(this);
	return 0;
}

//打开文件
void CMessage::AsyOpenFile(const char* pszFileName)
{
	CMessage MsgLog0x(MSG_MEM_OPEN_FILE);
	MsgLog0x.Add(pszFileName);
	MsgLog0x.SendToSM();
}
//关闭文件
void CMessage::AsyCloseFile(const char* pszFileName)
{
	CMessage MsgLog0x(MSG_MEM_CLOSE_FILE);
	MsgLog0x.Add(pszFileName);
	MsgLog0x.SendToSM();
}
long CMessage::c_SmInfoOrder = 0;
//清空文件内容
void CMessage::AsyClearFile(const char* pszFileName)
{
	c_SmInfoOrder = 0;

	CMessage MsgLog0x(MSG_MEM_CLEAR_FILE);
	MsgLog0x.Add(pszFileName);
	MsgLog0x.SendToSM();
}

//异步写文件的接口
void CMessage::AsyWriteFile(const char* pszFileName,const char* pszContent)
{
	CMessage MsgLog0x(MSG_MEM_WRITE_FILE);
	MsgLog0x.Add(pszFileName);
	MsgLog0x.Add(c_SmInfoOrder++);
	MsgLog0x.Add(pszContent);
	MsgLog0x.SendToSM();
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
	
		}
		break;
	case BASEMSG_Socket_Close:
		{
			if(GetNetFlag()&NF_Server_Client)
			{
				if(GetGame()->OnWorldServerClose(GetWSID()))
				{
					// 定时刷新数据
					lConnects--;

					CMessage::UnMapWSIDSocket(GetSocketID());
				}
			}
			else if(GetNetFlag()&NF_LServer_Client)
			{
			}
			else if(GetNetFlag()&NF_CServer_Client)
			{
			}
		}
		break;
	}
	return true;
}

// 服务器端消息处理
long CMessage::Run()
{
	// 消息分类处理
	switch( GetType() - (GetType() & 0x00000ff) )	// GetType() - GetType() % 256
	{
	case 0:
		DefaultRun();
		break;
	//----------------------------------------------------/服务器消息/
	case MSG_S2DB_SERVER:
	case MSG_W2DB_SERVER:
		OnServerMessage(this);
		break;
	//----------------------------------------------------/存盘消息/
	case MSG_S2DB_OPER:
	case MSG_W2DB_OPER:
		OnDBOPMessage(this);
		break;
	case MSG_SM2GAPP_OPERATOR:
		OnSMMessage(this);
		break;
	//----------------------------------------------------/传递消息/
	//case MSG_W2DB_PASS:
	//	OnDBPassMessage(this);
	//	break;
	//case MSG_W2DB_LINKMAN:
	//	OnWsLinkmanMessage(this);
	//	break;
	//case MSG_W2DB_MAIL:
	//	OnMailMessage(this);
	//	break;
	}


	return true;
}
//发送给所有WorldServer
long CMessage::SendAllWS()
{
	*((long*)Base()) = GetSize()+HEAD_SIZE;

	itLL it = m_SocketWSID.begin();
	for(;it != m_SocketWSID.end();it++)
	{
		CBaseMessage *pMsg = CBaseMessage::AllocMsg();
		Clone(pMsg);
		m_pClientServer->ASend((*it).first,pMsg);
	}
	return true;
}

