// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "MMSystem.h"
#include "CPlug.h"
#include "CSession.h"
#include "CSessionFactory.h"
#include "../Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41DCD1EF008C
CSession::CSession(DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime)
{
	// TODO: Add your specialized code here.
	m_dwSessionType		= CSessionFactory::ST_NORMAL_SESSION;
	m_dwMinimumPlug		= dwMinPlugs;
	m_dwMaximumPlug		= dwMaxPlugs;
	m_dwLifeTime		= dwLifeTime;
	m_bSessionAborted	= FALSE;
	m_bSessionStarted	= FALSE;
	m_bSessionEnded		= FALSE;
	m_bRemoveFlag		= FALSE;
}
CSession::CSession(void* pParam)
{
	if(pParam)
	{
		m_dwMinimumPlug		= ((tagParam*)pParam)->minPlugs;
		m_dwMaximumPlug		= ((tagParam*)pParam)->maxPlugs;
		m_dwLifeTime		= ((tagParam*)pParam)->lifeTime;
	}
	// TODO: Add your specialized code here.
	m_dwSessionType		= CSessionFactory::ST_NORMAL_SESSION;
	m_bSessionAborted	= FALSE;
	m_bSessionStarted	= FALSE;
	m_bSessionEnded		= FALSE;
	m_bRemoveFlag		= FALSE;
}
//##ModelId=41DCD1F60157
CSession::CSession(const CGUID& guid, DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime)
{
	// TODO: Add your specialized code here.
	m_dwSessionType		= CSessionFactory::ST_NORMAL_SESSION;
	m_dwMinimumPlug		= dwMinPlugs;
	m_dwMaximumPlug		= dwMaxPlugs;
	m_dwLifeTime		= dwLifeTime;
	m_bSessionAborted	= FALSE;
	m_bSessionStarted	= FALSE;
	m_bSessionEnded		= FALSE;
	m_bRemoveFlag		= FALSE;
	SetExID( guid );
}

//##ModelId=41DCD2070109
CSession::~CSession()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41DCD273036B
void CSession::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##AI部分会首先判断所有的Plug是否有效，如果有无效的Plug，
	//##则执行一些操作，并通知其他的Plug后，把该Plug断掉。
	if( m_bSessionStarted == TRUE )
	{
		if( m_bSessionEnded || m_bSessionAborted )
		{
			return;
		}
		if( m_dwLifeTime && timeGetTime() >= m_dwStartingTimeStamp + m_dwLifeTime )
		{
			End();
			return;
		}
		list<long>::iterator it = m_lPlugs.begin();
		while( it != m_lPlugs.end() )
		{
			CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
			if( pPlug )
			{
				if( pPlug -> IsPlugAvailable() == FALSE )
				{
					OnPlugAborted( *it );

					//##回收该Plug并删除掉引用
					CSessionFactory::GarbageCollect_Plug( *it );
					it = m_lPlugs.erase( it );
				}
				else if( pPlug -> IsPlugEnded() )
				{
					OnPlugEnded( *it );

					//##回收该Plug并删除掉引用
					CSessionFactory::GarbageCollect_Plug( *it );
					it = m_lPlugs.erase( it );
				}
				else
				{
					it ++;
				}
			}
			else 
			{
				it = m_lPlugs.erase( it );
			}
		}
	}
}

//##ModelId=41DCD60D0119
BOOL CSession::Start( BOOL bFlag )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwStartingTimeStamp	= timeGetTime();
	m_bSessionStarted		= TRUE;
	return OnSessionStarted( bFlag );
}

//##ModelId=41DCD61A00DA
BOOL CSession::End( BOOL bFlag )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwStartingTimeStamp	= 0;
	m_bSessionEnded			= TRUE;
	m_bRemoveFlag			= TRUE;

	//##通知所有的Plug会话即将关闭的消息。
	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
		if( pPlug )
		{
			pPlug -> OnSessionEnded();
		}
	}
	return OnSessionEnded( bFlag );
}

//##ModelId=41DCD62A0186
BOOL CSession::Abort( BOOL bFlag )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwStartingTimeStamp	= 0;
	m_bSessionAborted		= TRUE;
	m_bRemoveFlag			= TRUE;

	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
		if( pPlug )
		{
			pPlug -> OnSessionAborted();
		}
		
	}
	return OnSessionAborted( bFlag );
}


BOOL CSession::OnSessionStarted( BOOL bFlag )
{
	return TRUE;
}


BOOL CSession::OnSessionEnded( BOOL bFlag )
{
	return TRUE;
}

BOOL CSession::OnSessionAborted( BOOL bFlag )
{
	return TRUE;
}

//##ModelId=41DDE95103A9
BOOL CSession::OnPlugChangeState(long PlugID, LONG lState, BYTE* pData, BOOL bSendToTrigger)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##此函数主要是用于让下层的Plug向上层的会话提交自身状态改变的消息。
	//##会话接收到这个事件后会主动通知其他的Plugs。其他的Plugs会因此做出
	//##相应的改变。会话会选择性的通知其他Plug。条件是该Plug是否有效。
	//##另外，该会话不会对该Plug的状态改变做出任何处理。

	BOOL bResult = FALSE;
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
	if( pPlug )
	{
		if( pPlug -> IsPlugAvailable() )
		{
			list<long>::iterator it = m_lPlugs.begin();
			for( ; it != m_lPlugs.end(); it ++ )
			{
				//##如果不要求发送给触发者
				if( (*it) == PlugID && bSendToTrigger == FALSE)
				{
					continue;
				}
				CPlug* pOtherPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
				if( pOtherPlug )
				{
					if( pOtherPlug -> IsPlugAvailable() && pOtherPlug -> IsPlugEnded() == FALSE )
					{
						pOtherPlug -> OnChangeState( PlugID, lState, pData );
					}
				}
			}
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CSession::IsSessionShouldBeenRemoved()
{
	return m_bRemoveFlag;
}

//##ModelId=41DDF8FA00EA
BOOL CSession::IsSessionAvailable()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##该函数的作用是判定目前的会话是否还是有效的会话。
	//##主要是根据Plug的状态来判断。Plug如果在用户主动结束，
	//##用户异常退出的情况下会返回FALSE。

	BOOL bResult = FALSE;
	DWORD dwAvailablePlugAmount = 0;
	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
		if( pPlug )
		{
			if( pPlug -> IsPlugAvailable() )
			{
				++ dwAvailablePlugAmount;
			}
		}
	}

	if( dwAvailablePlugAmount >= m_dwMinimumPlug )
	{
		bResult = TRUE;
	}

	return bResult;
}

//##ModelId=41DDF2B20157
BOOL CSession::InsertPlug(long PlugID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##只有会话在开始，没有结束，没有异常中断的情况下才能加入新的Plug
	BOOL bResult = FALSE;
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
	if( m_bSessionStarted && !m_bSessionEnded && !m_bSessionAborted )
	{
		if( m_lPlugs.size() < m_dwMaximumPlug )
		{
			if( pPlug && pPlug -> IsPlugAvailable() )
			{
				//##首先把这个Plug插入，然后通知其他的Plug.
				pPlug -> SetSession( GetExID() );
				m_lPlugs.push_back( PlugID );
				OnPlugInserted( PlugID );
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

//##ModelId=41DE28EC0242
BOOL CSession::IsSessionEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bSessionStarted && m_bSessionEnded )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41DE28F8031C
CPlug* CSession::QueryPlugByOwner(LONG lType, const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CPlug* pResult = NULL;
	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerType()	== lType &&
				pPlug -> GetOwnerID()	== guid )
			{
				pResult = pPlug;
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41DE36050109
CPlug* CSession::QueryPlugByID(long PlugID)
{
	// TODO: Add your specialized code here.
	CPlug* pResult = NULL;

	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		if( (*it) == PlugID )
		{
			pResult = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
			break;
		}
	}

	return pResult;
}

//##ModelId=41DE46B6004E
BOOL CSession::OnPlugInserted(long PlugID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	
	//##首先通知自己,然后通知其他Plug
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
	if( pPlug )
	{
		pPlug -> OnPlugInserted();
		bResult = TRUE;
	}

	OnPlugChangeState( PlugID, CPlug::PS_INSERTED, NULL );
	return bResult;
}

//##ModelId=41DCD7D0033C
BOOL CSession::OnPlugAborted(long PlugID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	//##首先通知自己,然后通知其他Plug
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
	if( pPlug )
	{
		pPlug -> OnPlugAborted();
		OnPlugChangeState( PlugID, CPlug::PS_ABORTED, NULL );
		CSessionFactory::GarbageCollect_Plug( PlugID );
		bResult = TRUE;
	}

	//##移出该Plug的引用, 不在这儿做，外面会自动删除该引用的。
	return bResult;
}

//##ModelId=41DE24E400FA
BOOL CSession::OnPlugEnded(long PlugID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	//##首先通知自己,然后通知其他Plug
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(PlugID) );
	if( pPlug )
	{
		pPlug -> OnPlugEnded();
		OnPlugChangeState( PlugID, CPlug::PS_REMOVED, NULL );
		CSessionFactory::GarbageCollect_Plug( PlugID );
		bResult = TRUE;
	}

	//##移出该Plug的引用, 不在这儿做，外面会自动删除该引用的。
	return bResult;
}

//##ModelId=41DE197F0290
void CSession::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##Release只是负责删除所有的Plug，并不负责处理其他的事件。
	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
		if( pPlug )
		{
			//##首先通知Plug Session的结束。
			pPlug -> OnSessionEnded();
			//##然后触发Plug离开的事件，并且删除掉该Plug。
			OnPlugEnded( *it );
		}
	}
	m_lPlugs.clear();
}

//##ModelId=41E3634A0109
list<long>& CSession::GetPlugList()
{
	// TODO: Add your specialized code here.
	return m_lPlugs;
}

//##ModelId=421158C70330
void CSession::SendNotification(const char* strMsg)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( strMsg )
	{
		list<long>::iterator it = m_lPlugs.begin();
		for( ; it != m_lPlugs.end(); it ++ )
		{
			CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
			if( pPlug )
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				if( pPlayer )
				{
					//pPlayer -> SendNotifyMessage(strMsg,dwColor);
					pPlayer->SendNotifyMessage(strMsg);
				}
			}
		}
	}
}

void CSession::PlugExit(long lPlugId)
{
	list<long>::iterator it = m_lPlugs.begin();
	for( ; it != m_lPlugs.end(); it ++ )
	{
		if(lPlugId!=*it)
		{
			CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
			if( pPlug )
			{
				pPlug->Exit();
			}
		}		
	}
}

BOOL CSession::Serialize( vector<BYTE>* pStream )
{
	if( pStream == NULL ) return FALSE;

	//##首先计算剩余时间
	DWORD dwLifeTime = 0;
	if( m_dwLifeTime )
	{
		INT64 iLifeTime	= m_dwLifeTime - ( timeGetTime() - m_dwStartingTimeStamp );
		if( iLifeTime < 0 )
		{
		}
		else if( iLifeTime > 0xffffffff )
		{
			dwLifeTime = 0xffffffff;
		}
		else
		{
			dwLifeTime = static_cast<DWORD>( iLifeTime );
		}
	}

	_AddToByteArray( pStream, m_dwSessionType );
	_AddToByteArray( pStream, m_dwMinimumPlug );
	_AddToByteArray( pStream, m_dwMaximumPlug );
	_AddToByteArray( pStream, dwLifeTime );

	return TRUE;
}

BOOL CSession::Unserialize( BYTE* pStream, LONG& lOffset )
{
	//##这些资料都会在创建Session的时候，由CSessionFactory取出
	return TRUE;
}