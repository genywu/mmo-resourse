#include "StdAfx.h"
#include "..\Nets\NetBilling\Message.h"
#include ".\billingplayermanager.h"
#include "..\DBACCESS\DBBilling\rsplayeraccount.h"

HANDLE CBillingPlayerManager::s_hLogThread = NULL;
CRITICAL_SECTION CBillingPlayerManager::m_CriticalSectionAC;
CRITICAL_SECTION CBillingPlayerManager::m_CriticalSectionIL;
CRITICAL_SECTION CBillingPlayerManager::m_CriticalSectionTR;
list<CBillingPlayerManager::tagAccInfo> CBillingPlayerManager::m_AccInfoReqQueue;
list<CBillingPlayerManager::tagIncLogNode> CBillingPlayerManager::m_IncLogQueue;
list<CBillingPlayerManager::tagTradeNode> CBillingPlayerManager::m_TradeReqQueue;

unsigned __stdcall BPMThreadFunc(void* pArguments);
unsigned __stdcall LogThreadFunc(void* pArguments);

bool CBillingPlayerManager::Start(bool bLogSwitch)
{
	InitializeCriticalSection(&m_CriticalSectionAC);
	InitializeCriticalSection(&m_CriticalSectionIL);
	InitializeCriticalSection(&m_CriticalSectionTR);

	/// log for incrementshop switch
	if( bLogSwitch )
	{
		unsigned threadID;
		s_hLogThread = (HANDLE)_beginthreadex(NULL,0,LogThreadFunc,NULL,0,&threadID);
	}

	return true;
}

bool CBillingPlayerManager::End(bool bLogSwitch)
{
	if( bLogSwitch )
	{
		WaitForSingleObject(s_hLogThread, INFINITE);
		CloseHandle(s_hLogThread);
	}

	DeleteCriticalSection(&m_CriticalSectionTR);
	DeleteCriticalSection(&m_CriticalSectionIL);
	DeleteCriticalSection(&m_CriticalSectionAC);
	return true;
}

CBillingPlayerManager::CBillingPlayerManager(void)
: m_dwThreadID(0)
, m_hThread(NULL)
{
	//::CoInitialize(NULL);
}

CBillingPlayerManager::~CBillingPlayerManager(void)
{
	//::CoUninitialize();
}


void CBillingPlayerManager::Release()
{
	PostThreadMessage(m_dwThreadID, WM_USER+618, 0, 0);
	WaitForSingleObject(m_hThread, INFINITE );
	CloseHandle(m_hThread);
}

///
/// Add some a request account info into queue
/// return the result of adding
///
bool CBillingPlayerManager::PushACQueue( const tagAccInfo &rAccInfo )
{
	/// Here, some sync code may be added
	/// If no, the res has no actual meanings.
	EnterCriticalSection(&m_CriticalSectionAC);
	m_AccInfoReqQueue.push_back( rAccInfo );
	LeaveCriticalSection(&m_CriticalSectionAC);
	return true;
}

///
/// Check empty or not
///
bool CBillingPlayerManager::IsEmptyACQueue()
{
	bool bRes;
	EnterCriticalSection(&m_CriticalSectionAC);
	bRes = m_AccInfoReqQueue.empty();
	LeaveCriticalSection(&m_CriticalSectionAC);
	return bRes;
}

///
/// Add some a incrementshop log into queue
/// return the result of adding
///
bool CBillingPlayerManager::PushILQueue( const tagIncLogNode &rIncLog )
{
	/// Here, some sync code may be added
	/// If no, the res has no actual meanings.
	EnterCriticalSection(&m_CriticalSectionIL);
	m_IncLogQueue.push_back( rIncLog );
	LeaveCriticalSection(&m_CriticalSectionIL);
	return true;
}

///
/// Check empty or not
///
bool CBillingPlayerManager::IsEmptyILQueue()
{
	bool bRes;
	EnterCriticalSection(&m_CriticalSectionIL);
	bRes = m_IncLogQueue.empty();
	LeaveCriticalSection(&m_CriticalSectionIL);
	return bRes;
}

///
/// Add some a request trade info into queue
/// return the result of adding
///
bool CBillingPlayerManager::PushTRQueue( const tagTradeNode &rTradeNode )
{
	/// Here, some sync code may be added
	/// If no, the res has no actual meanings.
	EnterCriticalSection(&m_CriticalSectionTR);
	m_TradeReqQueue.push_back( rTradeNode );
	LeaveCriticalSection(&m_CriticalSectionTR);
	return true;
}

///
/// Check empty or not
///
bool CBillingPlayerManager::IsEmptyTRQueue()
{
	bool bRes;
	EnterCriticalSection(&m_CriticalSectionTR);
	bRes = m_TradeReqQueue.empty();
	LeaveCriticalSection(&m_CriticalSectionTR);
	return bRes;
}

///
/// Run in Mainloop process query or charge request
///
bool CBillingPlayerManager::Run()
{
	/// exit the thread
	MSG msg;
	memset( &msg, 0, sizeof( MSG ) );
	if( PeekMessage( &msg, (HWND)INVALID_HANDLE_VALUE, 0, 0, PM_REMOVE ) && WM_USER+618 == msg.message )
		return false;

	// process accinofoqueue and tradenodequeue
	list<tagAccInfo> ACQueue;
	list<tagTradeNode> TRQueue;

	EnterCriticalSection(&m_CriticalSectionAC);
	if( !m_AccInfoReqQueue.empty() )
	{
		ACQueue = m_AccInfoReqQueue;
		m_AccInfoReqQueue.clear();
	}
	LeaveCriticalSection(&m_CriticalSectionAC);
	if( !ACQueue.empty() )
	{
		for( list<tagAccInfo>::iterator it=ACQueue.begin(); it!=ACQueue.end(); ++it )
		{
			long lPoint = 0;
			long lRes = CRsPlayerAccount::GetUserPoint(it->strPlayerID.c_str(), lPoint);

			// Query DB for player account info
			CMessage msg(CMessage::MSG_B2S_BILLING_AUTHEN);
			msg.Add( it->lPlayerID );
			msg.Add( lRes );
			if( !lRes )
			{
				msg.Add( lPoint );
			}
			msg.SendToGS( it->lGSID );
		}
		ACQueue.clear();
	}

	EnterCriticalSection(&m_CriticalSectionTR);
	if( !m_TradeReqQueue.empty() )
	{
		TRQueue = m_TradeReqQueue;
		m_TradeReqQueue.clear();
	}
	LeaveCriticalSection(&m_CriticalSectionTR);
	if( !TRQueue.empty() )
	{
		for( list<tagTradeNode>::iterator it=TRQueue.begin(); it!=TRQueue.end(); ++it )
		{
			// Query DB for player account info
			char szTransCode[512] = "";
			long lBuyerPoint = 0;
			long lSellerPoint = 0;
			long lRes = 0;
			if( it->strSellerID.empty() )
			{
				lRes = CRsPlayerAccount::BuyItemCode( *it, szTransCode, lBuyerPoint );

				CMessage msg( CMessage::MSG_B2S_BILLING_INCBUY );
				msg.Add( it->lBuyerID );
				msg.Add( lRes );
				if( !lRes )
				{
					msg.Add( lBuyerPoint );
					msg.Add( it->dwYuanbao );
					msg.Add( it->lGoodsID );
					msg.Add( it->lGoodsNum );
					msg.Add( it->lSellerID );
					msg.Add(szTransCode);
				}
				msg.SendToGS( it->lGSID );
			}
			else
			{
				lRes = CRsPlayerAccount::BuyPlayerItem( *it, szTransCode, lBuyerPoint, lSellerPoint );

				CMessage msg( CMessage::MSG_B2S_BILLING_TRADE );
				msg.Add( it->lBuyerID );
				msg.Add( it->lSellerID );
				msg.Add( lRes );
				if( !lRes )
				{
					msg.Add( lBuyerPoint );
					msg.Add( lSellerPoint );
					msg.Add( it->lType );
					msg.Add( it->lSessID );
					msg.Add( it->lPlugID );
					msg.Add( it->dwYuanbao );
					msg.Add( it->guidGoods );
					msg.Add(szTransCode);
				}
				msg.SendToGS(it->lGSID);
			}
		}
		TRQueue.clear();
	}

	return true;
}


///
/// Process log queue for incrementshop
///
bool CBillingPlayerManager::OnLogProcess()
{
	list<tagIncLogNode> IncLogQueue;
	EnterCriticalSection(&m_CriticalSectionIL);
	if( !m_IncLogQueue.empty() )
	{
		IncLogQueue = m_IncLogQueue;
		m_IncLogQueue.clear();
	}
	LeaveCriticalSection(&m_CriticalSectionIL);
	if( !IncLogQueue.empty() )
	{
		CRsPlayerAccount::PutCashLog(IncLogQueue);
		IncLogQueue.clear();
	}

	Sleep(GetGame()->GetSetup()->dwSaveLogSvrTime);

	return true;
}

///
/// Create billing players manager's thread
///
HANDLE CBillingPlayerManager::CreateThread()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, BPMThreadFunc, this, 0, (unsigned *)&m_dwThreadID);
	
	DWORD dwErr;
	if( !m_hThread )
	{
		dwErr = GetLastError();
	}
	return m_hThread;
}

///
/// BPManager thread
///
unsigned __stdcall BPMThreadFunc(void* pArguments)
{
	// process arguments
	CBillingPlayerManager *pBPM = (CBillingPlayerManager *)pArguments;
	if( !pBPM )
	{
		_endthreadex( 0 );
		return 0;
	}

	::CoInitialize(NULL);
#ifndef _DEBUG
	__try
	{
#endif
		while( pBPM->Run() )
		{
			Sleep(1);
		}

#ifndef _DEBUG
	}
	__except( _Sword3::CrashFilter( GetExceptionInformation(), GetExceptionCode() ) )
	{
		PutDebugString("********BPMThreadFunc ERROR, Cheak Last ERROR********");
	}
#endif

	::CoUninitialize();
	_endthreadex( 0 );
	return 0;
}

unsigned __stdcall LogThreadFunc(void* pArguments)
{
	::CoInitialize(NULL);
#ifndef _DEBUG
	__try
	{
#endif
		do
		{
			CBillingPlayerManager::OnLogProcess();
		}while( !g_bGameThreadExit );

#ifndef _DEBUG
	}
	__except( _Sword3::CrashFilter( GetExceptionInformation(), GetExceptionCode() ) )
	{
		PutDebugString("********LogThreadFunc ERROR, Cheak Last ERROR********");
	}
#endif

	::CoUninitialize();
	_endthreadex( 0 );
	return 0;
}