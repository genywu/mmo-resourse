#include "StdAfx.h"
#include "BillWorkerThread.h"

#include "BillAdoBase.h"
#include "CBillDBManager.h"
#include "CBillOper.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BillWorkerThread::BillWorkerThread(string strProvider, 
						 string strDataSource, 
						 string strInitialCatalog,
						 string strUserID, 
						 string strPassword
						 )
:
m_ThreadRunning(false), 
m_hWaitWorkerThread(0)
{

	m_pConnPtr = new BillAdoBase(string("Provider=" + strProvider +
									"; Server=" + strDataSource + 
									"; Database=" + strInitialCatalog + 
									"; UID=" + strUserID + 
									"; PWD=" + strPassword + 
									"; OLE DB Services=-1; Driver={SQL Server}"
									));
	m_strInitialCatalog = strInitialCatalog;
	m_hQuitWait = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_hQuitWait);
}

//-----------------------------------------------------------------------------------------------------------------
BillWorkerThread::~BillWorkerThread(void)
{
	Exit();
	delete m_pConnPtr;
}

_ConnectionPtr BillWorkerThread::GetConnPtr(void)
{
	return m_pConnPtr->GetConnPtr();
}

bool BillWorkerThread::OpenCn(void)
{
	return m_pConnPtr->OpenCn();
}

//-----------------------------------------------------------------------------------------------------------------
bool BillWorkerThread::Begin(void)
{
	if (0 != m_hWaitWorkerThread)
		return false;

	m_ThreadRunning	= true;

	if(0 == (m_hWaitWorkerThread = (HANDLE)::_beginthreadex(NULL, 0, WorkerFunction, (void*)this, 0, &m_lThreadID)))
	{
		
		m_ThreadRunning	= false;
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------------------------
void BillWorkerThread::Exit(void)
{
	if (0 == m_hWaitWorkerThread)
		return;

	m_hWaitWorkerThread = 0;
}
//-----------------------------------------------------------------------------------------------------------------
bool BillWorkerThread::ProcessData(void)
{
	bool ret = true;	
	
	list<CBillOper*> listOpers;
	GetGame()->GetBillDBManager()->GetOpers(listOpers);	

	list<CBillOper*>::iterator operIt = listOpers.begin();
	for(; operIt != listOpers.end(); operIt++)
	{		
		if(*operIt)
		{
			if((*operIt)->GetOperType()!=eBILL_OT_EXIT_THREAD)
			{
				(*operIt)->SetSqlConnection(m_pConnPtr->GetConnPtr());
				(*operIt)->DoOpertaion();
			}
			else
			{
				CBillOper* pOper=new CBillOper(eBILL_OT_EXIT_THREAD);
				GetGame()->GetBillDBManager()->AddOpers(pOper);
				ret=false;
			}
			
		}
		SAFE_DELETE(*operIt)
	}	
	listOpers.clear();
	
	return ret;
}
BillAdoBase* BillWorkerThread::GetAdoBasePtr(void)
{
	return m_pConnPtr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned __stdcall BillWorkerThread::WorkerFunction(void *pThis)
{
#ifndef _DEBUG
	__try
	{
#endif
		HRESULT hr = ::CoInitializeEx( NULL, COINIT_MULTITHREADED );
		BillWorkerThread *pWorkerThread = (BillWorkerThread*)pThis;
		if (NULL != pWorkerThread)
		{
			pWorkerThread->GetAdoBasePtr()->OpenCn();
			while(pWorkerThread->ProcessData())
			{
				
			}
			pWorkerThread->GetAdoBasePtr()->CloseCn();
			
			char outStr[1024];
			sprintf(outStr, "ID:[%d] 线程成功退出!\r\n", pWorkerThread->GetThreadID());
			//OutputDebugStr(outStr);
			SetEvent(pWorkerThread->GetQuitWaitHandle());
		}
		::CoUninitialize();

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		//	MessageBox(g_hWnd, "程序出现错误，请查看最新的错误报告文件", "Error", MB_OK);

		PutDebugString("程序出现错误，请查看最新的错误报告文件");		

		if(GetGame())
			GetGame()->Release();
		DeleteGame();

		Sleep(5000);

		WinExec("DBServer.exe", SW_SHOW);

//		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
		SetEvent(g_hGameThreadExitEvent);
		_endthreadex( 0 );

		return 0;
	}
#endif

	_endthreadex(0);
	return 0;
}