// BusyThread.cpp: implementation of the CBusyThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BusyThread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusyThread::CBusyThread()
{
	m_hStop = NULL;
	m_hProcessor = NULL;
}

CBusyThread::~CBusyThread()
{
	ASSERT(m_hStop == NULL);
	ASSERT(m_hProcessor == NULL);
}


int CBusyThread::IsThreadOK()
{
    return (m_hProcessor != NULL);
}


int CBusyThread::StartThread()
{
	int nRet = 0;
	if ( NULL != m_hProcessor )	// The thread has been running.
	{
		return nRet;
	}
	if (NULL == m_hStop)
	{
		m_hStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	DWORD dwThreadID = 0;
	m_hProcessor = CreateThread(NULL, 0, CBusyThread::InnerThreadProc, (LPVOID)this, 0, &dwThreadID);

	nRet = m_hProcessor != NULL;

	return nRet;
}

int CBusyThread::StopThread()
{
	int bRet = FALSE;

	if (NULL == m_hProcessor
		|| NULL == m_hStop)
	{
		bRet = TRUE;
		return bRet;
	}

	SetEvent(m_hStop);

	DWORD dwResult = WaitForSingleObject(m_hProcessor, INFINITE);
	if (WAIT_FAILED == dwResult)
	{
		bRet = FALSE;
	}
	else if (WAIT_OBJECT_0 == dwResult)
	{
		bRet = TRUE;
	}
	else if (WAIT_TIMEOUT == dwResult)	// Time out.
	{
		if (TerminateThread(m_hProcessor, 0))
		{
			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}
	}

	if (bRet)
	{
		CloseHandle(m_hStop);
		m_hStop = NULL;
		CloseHandle(m_hProcessor);
		m_hProcessor = NULL;
	}

	return bRet;
}

int CBusyThread::PreExecution()
{
    return true;
}

void CBusyThread::PostExecution()
{
}

DWORD WINAPI CBusyThread::InnerThreadProc(LPVOID lpThisParam)
{
    ULONG ulResult = -1;
    
    int nRetCode = false;
    CBusyThread *pThis = (CBusyThread *)lpThisParam;
    
    ASSERT_POINTER(pThis, CBusyThread);
    
    if (NULL == pThis)
        goto Exit0;
    
    nRetCode = pThis->PreExecution();
    if (!nRetCode)
        goto Exit0;
    
    ulResult = pThis->MainExecution();
    pThis->PostExecution();
    
Exit0:
    return ulResult;
}
