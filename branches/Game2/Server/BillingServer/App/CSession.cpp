#include "stdafx.h"
#include "CSession.h"

#include "CSessionFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSession::CSession(const char* strCdkey)
{
	m_strCdkey=strCdkey;
	m_eBusinessState=eBSTATE_PRELOCK;
}

CSession::~CSession()
{	
	
}

VOID CSession::AI()
{
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
		/*
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
		}*/
	}
}

BOOL CSession::Start( BOOL bFlag )
{
	m_dwStartingTimeStamp	= timeGetTime();
	m_bSessionStarted		= TRUE;
	return OnSessionStarted( bFlag );
}

BOOL CSession::End( BOOL bFlag )
{
	m_dwStartingTimeStamp	= 0;
	m_bSessionEnded			= TRUE;
	m_bRemoveFlag			= TRUE;
	return OnSessionEnded( bFlag );
}

BOOL CSession::Abort( BOOL bFlag )
{
	m_dwStartingTimeStamp	= 0;
	m_bSessionAborted		= TRUE;
	m_bRemoveFlag			= TRUE;
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

BOOL CSession::IsSessionShouldBeenRemoved()
{
	return m_bRemoveFlag;
}

BOOL CSession::IsSessionAvailable()
{
	//##该函数的作用是判定目前的会话是否还是有效的会话。
	//##主要是根据Plug的状态来判断。Plug如果在用户主动结束，
	//##用户异常退出的情况下会返回FALSE。

	BOOL bResult = FALSE;
	DWORD dwAvailablePlugAmount = 0;
	
	return bResult;
}

BOOL CSession::IsSessionEnded()
{
	BOOL bResult = FALSE;
	if( m_bSessionStarted && m_bSessionEnded )
	{
		bResult = TRUE;
	}
	return bResult;
}