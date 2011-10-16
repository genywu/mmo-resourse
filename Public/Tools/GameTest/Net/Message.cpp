//=============================================================================
/**
 *  file: Message.cpp
 *
 *  Brief:消息类，包装了构建消息、添加数据 、发送消息的功能
 *
 *  Authtor:wangqiao
 *	
 *	Date:2007-7-25
 */
//=============================================================================

#include "StdAfx.h"
#include "message.h"

class CDriveClient;

CServer	*CMessage::m_pServer = NULL;
CMemServer *CMessage::m_pMemServerManager = NULL;

CMessage::HMapLongGUID	CMessage::m_SocketPlayerID;
CMessage::HMapGUIDLong	CMessage::m_PlayerIDSocket;

CMessage::CMessage(void)
:m_guDiscardFlag(NULL_GUID)
{
	Reset();
}

CMessage::~CMessage(void)
{
	UnInit();
}
CMessage::CMessage(long lType)
:m_guDiscardFlag(NULL_GUID)
{
	Reset();
	Init(lType);
}

 //重设消息相关变量
void CMessage::Reset(void)
{
	m_lSocketID=0;
	m_dwIP=0;
	m_guDiscardFlag = NULL_GUID;
}

bool CMessage::Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum)
{
	m_SocketPlayerID.clear();
	m_PlayerIDSocket.clear();

	CBaseMessage::NewMsg = NewMessage;
	return CBaseMessage::Initial(pDBAllocator,nMaxFreeMsgNum);

}
bool CMessage::Release()
{
	return CBaseMessage::Release();
}

CBaseMessage* CMessage::NewMessage()
{
	return new CMessage();
}

//分配一个消息
CMessage* CMessage::AllocMessage()
{
	return static_cast<CMessage*>(CBaseMessage::AllocMsg());
}

//发送一个消息给套接字
long CMessage::SendBySocket(long lSocketID,bool bClone)
{
	if ( Base() == NULL ) return 0;			//add by MartySa 2009.7.30 
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	//分配一个消息
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	Clone(pMsg);
	pMsg->SetSocketID(lSocketID);
	m_pServer->ASend(lSocketID,pMsg);
	return true;
}

// 发送一个消息给玩家
long CMessage::SendByPlayer(const CGUID& lPlayerID)
{
	long lSocketID = GetSocketByPlayerID(lPlayerID);
	SendBySocket(lSocketID);
	return true;
}

// 发送一个消息数据给client On WirteMappingFile
//bool CMessage::SendToClient(unsigned uID)
//{	
//	if ( uID>0 && m_pMemServerManager!=NULL)
//	{	
		//*((long*)Base()) = GetSize()+HEAD_SIZE;
		//CBaseMessage *pMsg = CBaseMessage::AllocMsg();
		//Clone(pMsg);
		//m_pMemServerManager->Send(uID, pMsg);
		//CBaseMessage::FreeMsg(pMsg);
		//pMsg->Reset();
//		return true;
//	}
//	return false;
//}

const CGUID& CMessage::GetPlayerID()
{
	return GetPlayerIDBySocket(m_lSocketID);
}


//映射WorldID和SocketID
void CMessage::MapPlayerIDSocket(const CGUID& lPlayerID,long lSocketID)
{
	m_SocketPlayerID[lSocketID]=lPlayerID;
	m_PlayerIDSocket[lPlayerID]=lSocketID;
}
long CMessage::GetSocketByPlayerID(const CGUID& lPlayerID)
{
	itGL it = m_PlayerIDSocket.find(lPlayerID);
	if(it != m_PlayerIDSocket.end())
		return (*it).second;
	return 0;
}
const CGUID& CMessage::GetPlayerIDBySocket(long lSocketID)
{
	itLG it = m_SocketPlayerID.find(lSocketID);
	if(it != m_SocketPlayerID.end())
		return (*it).second;
	return CGUID::GUID_INVALID;
}
void CMessage::UnMapSocket(long lSocketID,long lNetFlag)
{
	//if(lNetFlag&NF_Server_Client)
	{
		UnMapPlayerIDSocket(lSocketID);
	}
}
void CMessage::UnMapPlayerIDSocket(long lSocketID)
{
	itLG it =  m_SocketPlayerID.find(lSocketID);
	if(it != m_SocketPlayerID.end() )
	{
		m_PlayerIDSocket.erase((*it).second);
		m_SocketPlayerID.erase(it);
	}
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
			//if(GetNetFlag()&NF_Server_Client)
			{
				//GetGame()->OnPlayerLost(GetPlayerID());
				CMessage::UnMapPlayerIDSocket(GetSocketID());
			}
		}
		break;
	}
	return true;
}

// 客户端消息处理
long CMessage::Run()
{
	// 消息分类处理
	switch( GetType() - (GetType() & 0x00000ff) )	// GetType() - GetType() % 256
	{
	case 0:
		DefaultRun();
		break;
	//----------------------------------------------------/登陆消息/
	case MSG_C2C_LOG:
	case MSG_L2C_LOG:
	case MSG_S2C_LOG:
		break;
	//----------------------------------------------------/场景消息/
	case MSG_S2C_RGN:
		break;
	//----------------------------------------------------/SHAPE消息/
	case MSG_S2C_SHAPE:
		break;
	//----------------------------------------------------/玩家消息/
	case MSG_S2C_PLAYER:
		break;
	//---------------------------------------------------/物品消息/
	case MSG_S2C_GOODS:
		break;
	case MSG_S2C_SHOP:
		break;
	case MSG_S2C_INCREMENTSHOP:
		break;
	case MSG_S2C_OTHER:
		break;
	case MSG_S2C_TEAM:
		break;
	case MSG_S2C_DEPOT:
		break;
	case MSG_S2C_GM:
	case MSG_S2C_SKILL:
		break;
	case MSG_S2C_ORGSYS:
		break;
	case MSG_S2C_PLAYERSHOP:
		break;
	case MSG_S2C_CONTAINER:
		break;
	case MSG_S2C_PET:
		break;
	case MSG_S2C_COST:
		break;
	case MSG_S2C_CARD:
		break;
	case MSG_S2C_COUNTRY:
		break;
	}
	return true;
}

