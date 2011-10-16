///
/// @file QuestionManager.cpp
/// @author Kevin Lynx
/// @brief manage the questions generation and cache details.
///
#include "stdafx.h"
#include "QuestionManager.h"
#include "../nets/networld/message.h"
#include "CConfiger.h"
#include "AntiCheatCommon.h"

#define ANTI_MEM_LOG_FILE "AntiCheatMemAllocInfo"
#define DUMP_QUESTION_INFO

namespace AntiCheat
{

	CQuestionManager::QuestionList *CQuestionManager::GenQuestions()
	{
		QuestionList *pQuestions = new QuestionList();
		
		int count = GetInst( CConfiger ).GetSystem().GenQuestionCount;
		for( int i = 0; i < count; ++ i )
		{
			Question ques = GenOneQuestion();
			if( ques.data.buf != 0 )
			{
				pQuestions->push_back( ques );
			}
		}

#ifdef DUMP_QUESTION_INFO
		char str[1024];
		_snprintf( str, sizeof( str ), "Gen [%u] questions.", pQuestions->size() );
		PutStringToFile( ANTI_MEM_LOG_FILE, str );
#endif
		return pQuestions;
	}

	void CQuestionManager::ReleaseQuestionCache( QuestionList *pQuestions )
	{
		for( QuestionList::iterator it = pQuestions->begin(); it != pQuestions->end(); ++ it )
		{
			Question &ques = *it;
			free( ques.data.buf );
		}

#ifdef DUMP_QUESTION_INFO
		char str[1024];
		_snprintf( str, sizeof( str ), "Release [%u] questions.", pQuestions->size() );
		PutStringToFile( ANTI_MEM_LOG_FILE, str );
#endif
		delete pQuestions;
	}

	void CQuestionManager::UpdateQuestionCache( QuestionList *pQuestions )
	{
		QuestionList *pOldQuestions = m_pQuestions;
		::EnterCriticalSection( &m_QuesMutex );
		m_pQuestions = pQuestions;
		if( pOldQuestions != NULL ) // at very first m_pQuestions is NULL.
		{
			ReleaseQuestionCache( pOldQuestions );
		}
		::LeaveCriticalSection( &m_QuesMutex );
	}

	bool CQuestionManager::Startup()
	{
		if( !GetInst( CConfiger ).IsEnable() )
		{
			return false;
		}

		// create events
		m_Events[ET_QUIT] = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		m_Events[ET_GEN] = ::CreateEvent( NULL, FALSE, FALSE, NULL );

		// create the mutex
		::InitializeCriticalSection( &m_QuesMutex );

		// startup update questions timer 
		int updateInterval = 1000 * GetInst( CConfiger ).GetSystem().UpdateQuestionInterval;
		m_UpdateTimer = m_Timer.Schedule( this, NULL, timeGetTime() + updateInterval, updateInterval );

		// generate questions at startup in the main thread.
		m_pQuestions = GenQuestions();

		// create the thread
		m_Thread = (HANDLE)::_beginthreadex( NULL, 0, threadFunc, this, 0, NULL );

		return true;
	}

	void CQuestionManager::Shutdown()
	{
		if( !GetInst( CConfiger ).IsEnable() )
		{
			return ;
		}
		m_Timer.Cancel( m_UpdateTimer );
		m_UpdateTimer = -1;

		::SetEvent( m_Events[ET_QUIT] );	
		::WaitForSingleObject( m_Thread, 6000 );
		::CloseHandle( m_Events[ET_QUIT] );
		::CloseHandle( m_Events[ET_GEN] );
		::DeleteCriticalSection( &m_QuesMutex );
		::CloseHandle( m_Thread );

		// release the questions cache
		ReleaseQuestionCache( m_pQuestions );
		m_pQuestions = NULL;
	}

	void CQuestionManager::Run()
	{
		DWORD dwCurTime = timeGetTime();
		m_Timer.Expire( dwCurTime );
	}

	long CQuestionManager::OnTimeOut( DWORD timerid, DWORD curtime, const void *var )
	{
		::SetEvent( m_Events[ET_GEN] );
		return 0;
	}

	void CQuestionManager::SendQuestionsToGS( long  lGSSocketID )
	{
		if( !GetInst( CConfiger ).IsEnable() )
		{
			return;
		}
		CMessage msg( MSG_W2S_AC_UPDATEQUESTIONS );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		EncodeQuestionCache( db );
		msg.SendToSocket( lGSSocketID );
	}

	void CQuestionManager::UpdateQuestionsToAll()
	{
		CMessage msg( MSG_W2S_AC_UPDATEQUESTIONS );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		EncodeQuestionCache( db );
		msg.SendAll();
	}

	void CQuestionManager::EncodeQuestionCache( DBWriteSet &db )
	{
		::EnterCriticalSection( &m_QuesMutex );

		// questions count
		db.AddToByteArray( (long) m_pQuestions->size() );
		for( QuestionList::const_iterator it = m_pQuestions->begin(); it != m_pQuestions->end(); ++ it )
		{
			const Question &ques = *it; 
			// answer
			db.AddToByteArray( (BYTE) ques.answer );
			// question buffer size
			db.AddToByteArray( (long) ques.data.size );
			// question buffer
			db.AddToByteArray( ques.data.buf, ques.data.size );
		}

		::LeaveCriticalSection( &m_QuesMutex );
	}

	void CQuestionManager::OnThreadCall()
	{
		// generate new questions cache
		QuestionList *pQuestions = GenQuestions();
		
		// update the questions cache
		UpdateQuestionCache( pQuestions );

		// send the questions to all gs
		UpdateQuestionsToAll();
	}

	unsigned int CQuestionManager::threadFunc( void *p )
	{
#ifndef _DEBUG
		__try
		{
#endif
		CQuestionManager *qmgr = (CQuestionManager*)p;
		while( true )
		{
			PutStringToFile("CQuestionMgrStackInfo", "CQuestionManager::threadFunc() start!");
			DWORD dwIndex = ::WaitForMultipleObjects( ET_COUNT, qmgr->m_Events, FALSE, INFINITE );
			dwIndex -= WAIT_OBJECT_0;
			if( dwIndex == ET_QUIT )
			{
				break;
			}
			else if( dwIndex == ET_GEN )
			{
				qmgr->OnThreadCall();
			}
			else
			{
				PutoutLog( LOG_FILE, LT_ERROR, "Some error occures in GenQuestion thread, WaitStatus:%d", dwIndex += WAIT_OBJECT_0 );
				// fatal error
				::Sleep( 100 );
			}
			PutStringToFile("CQuestionMgrStackInfo", "CQuestionManager::threadFunc() end!");
		}
		
#ifndef _DEBUG
		}
		__except( _Sword3::CrashFilter( GetExceptionInformation(), GetExceptionCode() ) )
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Gen question thread crashed." );
			_endthreadex( 0 );
			return 0;
		}
#endif

		::_endthreadex( 0 );
		return 0;
	}
}
