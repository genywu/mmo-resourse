#include "StdAfx.h"
#include "..\Shape.h"
#include "..\Player.h"
#include "CS2CContainerObjectAmountChange.h"
#include "..\Session\CSession.h"
#include "..\Session\CPlug.h"
#include "..\Session\CSessionFactory.h"



CS2CContainerObjectAmountChange::CS2CContainerObjectAmountChange()
{
	m_lSourceContainerType				= 0;
	m_SourceContainerID					= NULL_GUID;
	m_lSourceContainerExtendID			= 0;
	m_dwSourceContainerPosition			= 0;

	m_lObjectType						= 0;
	//m_lObjectID							= 0;
	m_dwAmount							= 0;
}

CS2CContainerObjectAmountChange::~CS2CContainerObjectAmountChange()
{
}

VOID CS2CContainerObjectAmountChange::SetSourceContainer( LONG lType, const CGUID& guid, DWORD dwPosition )
{
	m_lSourceContainerType				= lType;
	m_SourceContainerID					= guid;
	m_dwSourceContainerPosition			= dwPosition;
}

VOID CS2CContainerObjectAmountChange::SetObject( LONG lType,const CGUID& guid )
{
	m_lObjectType						= lType;
	//m_lObjectID							= 0;
	m_guObjectID						= guid;
}

VOID CS2CContainerObjectAmountChange::SetObjectAmount( DWORD dwAmount )
{
	m_dwAmount							= dwAmount;
}

VOID CS2CContainerObjectAmountChange::SetSourceContainerExtendID( LONG lID )
{
	m_lSourceContainerExtendID			= lID;
}

VOID CS2CContainerObjectAmountChange::Send( const CGUID& PlayerID )
{
	if( NULL_GUID != PlayerID )
	{
		CMessage mMsg( MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE );
		mMsg.Add( m_lSourceContainerType );
		mMsg.Add( m_SourceContainerID );
		mMsg.Add( m_lSourceContainerExtendID );
		mMsg.Add( m_dwSourceContainerPosition );
		mMsg.Add( m_lObjectType );
		//mMsg.Add( m_lObjectID );
		mMsg.Add( m_guObjectID );
		mMsg.Add( m_dwAmount );
		mMsg.SendToPlayer( PlayerID );
	}
}

VOID CS2CContainerObjectAmountChange::SendToSession( const CGUID& SessionID )
{
	CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(SessionID) );
	if( pSession )
	{
		list<long>::iterator it = pSession -> GetPlugList().begin();
		for( ; it != pSession -> GetPlugList().end(); it ++ )
		{
			CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
			if( pPlug && pPlug -> GetOwnerType() == TYPE_PLAYER )
			{
				Send( pPlug -> GetOwnerID() );
			}
		}
	}
}

LONG CS2CContainerObjectAmountChange::GetSourceContainerType()
{
	return m_lSourceContainerType;
}

const CGUID& CS2CContainerObjectAmountChange::GetSourceContainerID()
{
	return m_SourceContainerID;
}

DWORD CS2CContainerObjectAmountChange::GetSourceContainerPosition()
{
	return m_dwSourceContainerPosition;
}

LONG CS2CContainerObjectAmountChange::GetObjectType()
{
	return m_lObjectType;
}

//LONG CS2CContainerObjectAmountChange::GetObjectID()
//{
//	return 0;
//}

CGUID& CS2CContainerObjectAmountChange::GetguObjectID()
{
	return m_guObjectID;
}

DWORD CS2CContainerObjectAmountChange::GetObjectAmount()
{
	return m_dwAmount;
}

LONG CS2CContainerObjectAmountChange::GetSourceContainerExtendID()
{
	return m_lSourceContainerExtendID;
}