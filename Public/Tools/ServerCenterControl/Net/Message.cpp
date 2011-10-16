#include "stdafx.h"
#include "message.h"

#include "../../../SCMPublic/def/NetDef.h"
#include "../../../SCMPublic/def/MsgGMT_SCC2SM.h"
#include "../../../SCMPublic/def/MsgGMT_SCC2SUC.h"
#include "../ServerCenterControl/ServiceSCC.h"

//#include "CostNetClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern VOID OnSucMessage(CMessage *pMsg);
extern VOID OnSucJsonMessage(CMessage* pMsg);
extern VOID OnSmMessage(CMessage *pMsg);

CServer	*CMessage::m_pSUCServer;
CServer	*CMessage::m_pSMServer;
long	CMessage::m_lLogingSocket;
long	CMessage::m_lCostSocket;

map<DWORD, LONG>	CMessage::c_mapSMID2Socket;
map<LONG, DWORD>	CMessage::c_mapSMSocket2ID;

map<string,LONG>	CMessage::c_mapCdkey2Socket;
map<LONG,string>	CMessage::c_mapSocket2Cdkey;

BOOL				CMessage::c_bAllowSUCConn = FALSE;

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
	c_mapSMID2Socket.clear();
	c_mapSMSocket2ID.clear();

	c_mapCdkey2Socket.clear();
	c_mapSocket2Cdkey.clear();

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

void CMessage::DisSucConn(long lSocket)
{
	m_pSUCServer->DisConn(lSocket);
}
void CMessage::DisSmConn(long lSocket)
{
	m_pSMServer->DisConn(lSocket);
}


long CMessage::SendToSuc(long lSocketID)
{
	assert(m_pSUCServer);
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	//分配一个消息
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	//vector<CDataBlock*>& MsgData = GetMsgData();
	//pMsg->SetMsgData(MsgData);
	//MsgData.clear();
	Clone(pMsg);
	m_pSUCServer->ASend(lSocketID,pMsg);
	return true;
}

long CMessage::SendToSm(long lSocketID)
{
	assert(m_pSMServer);
	*((long*)Base()) = GetSize()+HEAD_SIZE;
	//分配一个消息
	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	//vector<CDataBlock*>& MsgData = GetMsgData();
	//pMsg->SetMsgData(MsgData);
	//MsgData.clear();
	Clone(pMsg);
	m_pSMServer->ASend(lSocketID,pMsg);
	return true;
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
			if(GetNetFlag() & NF_SCC_Server)
			{
				OnSUCAccept(GetSocketID());
			}
			if(GetNetFlag() & NF_SM_Server)
			{
				OnSMAccept(GetSocketID(), GetIP());
			}
		}
		break;
	case BASEMSG_Socket_Close:
		{
			if(GetNetFlag() & NF_SCC_Server)
			{
				OnSUCClose(GetSocketID());
			}
			if (GetNetFlag() & NF_SM_Server)
			{
				OnSMClose(GetSocketID());
			}
		}
		break;
	}
	return true;
}

void CMessage::OnSUCAccept(long lSocketID)
{
	ServiceSCC::GetInstance().OnConnSUC(lSocketID);
}

void CMessage::OnSUCClose(long lSocketID)
{
	ServiceSCC::GetInstance().OnLostSUC(lSocketID);
}

void CMessage::OnSMAccept(long lSocketID, long lIP)
{
	DWORD dwSMID = ServiceSCC::GetInstance().OnConnSM(lSocketID, lIP);
	if(0 == dwSMID)
	{
		m_pSMServer->DisConn(lSocketID);
		return;
	}

	c_mapSMID2Socket[dwSMID]	= lSocketID;
	c_mapSMSocket2ID[lSocketID]	= dwSMID;
}

void CMessage::OnSMClose(long lSocketID)
{
	map<LONG,DWORD>::iterator ite = c_mapSMSocket2ID.find(lSocketID);
	if(c_mapSMSocket2ID.end() != ite)
	{
		ServiceSCC::GetInstance().OnLostSM(ite->second);
		c_mapSMID2Socket.erase(ite->second);
		c_mapSMSocket2ID.erase(ite);
	}	
}

LONG CMessage::GetSMSocket(DWORD dwSMID)
{
	map<DWORD,LONG>::iterator ite = c_mapSMID2Socket.find(dwSMID);
	if(c_mapSMID2Socket.end() != ite)
	{
		return ite->second;
	}
	return -1;
}
DWORD CMessage::GetSMID(long lSMSocket)
{
	map<LONG,DWORD>::iterator ite = c_mapSMSocket2ID.find(lSMSocket);
	if(c_mapSMSocket2ID.end() != ite)
	{
		return ite->second;
	}
	return 0;
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
		//! 响应SUC消息
	case MSG_SUC2SCC_LOGIN:
	case MSG_SUC2SCC_INFO:
	case MSG_SUC2SCC_CTRL:
	case MSG_SUC2SCC_MANAGE:
	//	OnSucMessage(this);
        OnSucJsonMessage(this);

		//! 响应SM消息
	case MSG_SM2SCC_LOGIN:
	case MSG_SM2SCC_INFO:
	case MSG_SM2SCC_CTRL:
	case MSG_SM2SCC_MANAGE:
		{
			if(c_bAllowSUCConn)
				OnSmMessage(this);
			else
			{
				CMessage msg(MSG_SCC2SUC_OTHER_Notify_NotReady);
				msg.SendToSuc(this->GetSocketID());
			}
		}
		break;
	}


	return true;
}

void CMessage::AddJson(Json::Value& var)
{
    Json::FastWriter jWriter;
    std::string strSend = jWriter.write(var);
    AddEx((void*)strSend.c_str(), strSend.size());
}