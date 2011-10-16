#include "StdAfx.h"
#include "MMSystem.h"
#include "CBillSession.h"
#include "CBillSessionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBillSession::CBillSession(const char* strCdkey)
{
	m_strCdkey=strCdkey;
	m_eBusinessState=eBSTATE_PRELOCK;
}
CBillSession::CBillSession(void* pParam)
{
	const char* pStrCdkey = const_cast<char*>((char*)pParam);
	if(pStrCdkey)
		m_strCdkey = pStrCdkey;
	m_eBusinessState=eBSTATE_PRELOCK;
}
CBillSession::~CBillSession()
{	
	
}

void CBillSession::AI()
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
	}
}

BOOL CBillSession::Start( BOOL bFlag )
{
	m_dwStartingTimeStamp	= timeGetTime();
	m_bSessionStarted		= TRUE;
	return OnSessionStarted( bFlag );
}

BOOL CBillSession::End( BOOL bFlag )
{
	m_dwStartingTimeStamp	= 0;
	m_bSessionEnded			= TRUE;
	m_bRemoveFlag			= TRUE;
	return OnSessionEnded( bFlag );
}

BOOL CBillSession::Abort( BOOL bFlag )
{
	m_dwStartingTimeStamp	= 0;
	m_bSessionAborted		= TRUE;
	m_bRemoveFlag			= TRUE;
	return OnSessionAborted( bFlag );
}


BOOL CBillSession::OnSessionStarted( BOOL bFlag )
{
	return TRUE;
}


BOOL CBillSession::OnSessionEnded( BOOL bFlag )
{
	return TRUE;
}

BOOL CBillSession::OnSessionAborted( BOOL bFlag )
{
	return TRUE;
}

BOOL CBillSession::IsSessionShouldBeenRemoved()
{
	return m_bRemoveFlag;
}

BOOL CBillSession::IsSessionAvailable()
{
	//##该函数的作用是判定目前的会话是否还是有效的会话。
	//##主要是根据Plug的状态来判断。Plug如果在用户主动结束，
	//##用户异常退出的情况下会返回FALSE。

	BOOL bResult = FALSE;
	DWORD dwAvailablePlugAmount = 0;
	
	return bResult;
}

BOOL CBillSession::IsSessionEnded()
{
	BOOL bResult = FALSE;
	if( m_bSessionStarted && m_bSessionEnded )
	{
		bResult = TRUE;
	}
	return bResult;
}