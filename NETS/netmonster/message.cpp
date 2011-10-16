///
///
///
#include "stdafx.h"
#include "message.h"
#include "..\Servers.h"
#include "CGame.h"
#include "PeerManager.h"
#include "../Public/Message_Defs/MS_MessageDefs.h"

void OnWSMessage( CMessage *pMsg );
void OnGSMessage( CMessage *pMsg );

CServer	*CMessage::m_pClientServer;


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

void CMessage::Reset(void)
{
	m_lSocketID=0;
	m_dwIP=0;
}

bool CMessage::Initial(CDataBlockAllocator* pDBAllocator,long nMaxFreeMsgNum)
{

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

long CMessage:: SendToSocket(long lSocketID)
{
	*((long*)Base()) = GetSize()+12;

	CBaseMessage *pMsg = CBaseMessage::AllocMsg();
	Clone(pMsg);
	m_pClientServer->ASend(lSocketID,pMsg);

	return 1l;
}

long CMessage::Send()
{
	SendToSocket( CPeerManager::getSingleton().GetWSID() );
	return 0;
}

long CMessage::DefaultRun()
{
	switch( GetType() )
	{
	case BASEMSG_Socket_Close:
		{
			if( GetNetFlag() & PT_WORLD )
			{
				GetGame()->OnWSClose();
			}

			if( GetNetFlag() & PT_GAME )
			{
				GetGame()->OnGSClose( this->m_lSocketID );
			}
		}
		break;
	}

	return 1l;
}

long CMessage::Run()
{
	switch( GetType() - (GetType() & 0x00000ff) )	// GetType() - GetType() % 256
	{
	case 0:
		DefaultRun();
		break;

	case MSG_W2M_NORMAL:
		{
			OnWSMessage( this );
		}
		break;

	case MSG_G2M_NORMAL:
		{
			OnGSMessage( this );
		}
		break;
	}

	return 1l;
}
