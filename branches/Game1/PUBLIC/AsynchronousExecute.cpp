
#include "stdafx.h"
#include "AsynchronousExecute.h"

const DWORD MIN_THREAD_NUM				= 4;
const DWORD MAX_THREAD_NUM				= 128;
const DWORD MIN_WAITTIME				= 5000;
const DWORD WAIT_WORKTHREAD_END_TIME	= 30000;

static LONG AsynchronousExecute_Instance_Num = 0;

AsynchronousExecute::AsynchronousExecute(BOOL bCallCom)
:m_dwMinThreadNum(MIN_THREAD_NUM), m_dwMaxThreadNum(MAX_THREAD_NUM), m_dwWaitTime(MIN_WAITTIME) 
,m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(FALSE)
,m_hWorkIoPort(NULL)
,m_bCallCom(bCallCom)
{
	if(1 < AsynchronousExecute_Instance_Num)
		throw;//! 创建多个实例引发异常
	++AsynchronousExecute_Instance_Num;
}
AsynchronousExecute::AsynchronousExecute(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace, BOOL bCallCom)
:m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(FALSE)
,m_hWorkIoPort(NULL)
,m_bCallCom(bCallCom)
{
	if(1 < AsynchronousExecute_Instance_Num)
		throw;
	++AsynchronousExecute_Instance_Num;
	SetWorkParam(dwMinThreadNum, dwMaxThreadNum, dwThreadNumAdjustSpace);
}
AsynchronousExecute::~AsynchronousExecute(VOID)
{	
	Exit();
	AsynchronousExecute_Instance_Num = 0;
}


///////////////////////////////////////////////////////////////
#pragma region 功能接口

//! 设置工作参数（在对象启动后，接口无效）
BOOL AsynchronousExecute::SetWorkParam(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace)
{
	if(m_bIsRunning) return FALSE;
	m_dwMinThreadNum	= min(max(dwMinThreadNum, MIN_THREAD_NUM), MAX_THREAD_NUM);
	m_dwMaxThreadNum	= max(min(dwMaxThreadNum, MAX_THREAD_NUM), 1);
	m_dwWaitTime		= max(dwThreadNumAdjustSpace, MIN_WAITTIME);
	return TRUE;
}

//! 初始化对象
VOID AsynchronousExecute::BeginWork(DWORD dwEmbryonicThreadNum, DWORD dwNumberOfConcurrentThreads)
{
	if(m_bIsRunning) return;

	InitializeCriticalSection(&m_CriticalSection);

	m_hManageIoPort = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	m_hMgrThread = (HANDLE)_beginthreadex(
		NULL, 
		0,
		ManagerProc,
		(LPVOID)this,
		0,
		&m_dwThreadId
		);

	m_bIsRunning = TRUE;

	m_hWorkIoPort = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	dwEmbryonicThreadNum = min(max(m_dwMinThreadNum, dwEmbryonicThreadNum), dwEmbryonicThreadNum);
	AddThreads(dwEmbryonicThreadNum);


}
//! 结束对象
VOID AsynchronousExecute::Exit()
{
	if(!m_bIsRunning) return;

	PostQueuedCompletionStatus(m_hManageIoPort, NULL, eCC_ManagerThreadExit, NULL);
	WaitForSingleObject(m_hMgrThread, INFINITE);
	CloseHandle(m_hMgrThread);
	CloseHandle(m_hManageIoPort);
	m_bIsRunning = FALSE;

	vector<HANDLE> vThreadHandle;
	GetAllThreadHandle(vThreadHandle);
	for (ULONG i = 0; i < vThreadHandle.size(); ++i)
	{
		PostQueuedCompletionStatus(m_hWorkIoPort, NULL, eEM_Exit, NULL);
	}

	DWORD dwRc = WaitForMultipleObjects((DWORD)vThreadHandle.size(), &vThreadHandle[0], TRUE, WAIT_WORKTHREAD_END_TIME * 10);
	CloseHandle(m_hWorkIoPort);

	if(dwRc >= WAIT_OBJECT_0 && dwRc < WAIT_OBJECT_0 + vThreadHandle.size())
	{
		for(ULONG i = 0; i < vThreadHandle.size(); ++i)
		{
			CloseHandle(vThreadHandle[i]);
		}
	}
	else if(dwRc == WAIT_TIMEOUT)
	{
		DWORD exitCode;
		for(ULONG i = 0; i < vThreadHandle.size(); ++i)
		{
			if (::GetExitCodeThread(vThreadHandle[i], &exitCode)==STILL_ACTIVE)
			{
				TerminateThread(vThreadHandle[i], 99);
			}
			CloseHandle(vThreadHandle[i]);
		}
	}

	DeleteCriticalSection(&m_CriticalSection);
}

//! 得到运行状态
BOOL AsynchronousExecute::IsRunning(VOID)
{
	return m_bIsRunning;
}


//! 异步执行一个无返回函数
VOID AsynchronousExecute::Execute(WorkFunction pfWork, LPVOID pParm)
{
	MyOverlapped *pMyOverlapped = CreateMyOverlapped();
	pMyOverlapped->pfWork = pfWork;
	pMyOverlapped->pParm  = pParm;

	PostQueuedCompletionStatus(
		m_hWorkIoPort,
		NULL,
		eEM_Normal,
		(LPOVERLAPPED)pMyOverlapped);
}
//! 异步执行一个返回函数，并将返回值作为参数传递到完成后的执行结果处理函数中
VOID AsynchronousExecute::Execute(WorkFunction pfWork, LPVOID pParm, WorkFunction pfCallBack)
{
	MyOverlapped *pMyOverlapped = CreateMyOverlapped();
	pMyOverlapped->pfWork		= pfWork;
	pMyOverlapped->pParm		= pParm;
	pMyOverlapped->pfCallBack	= pfCallBack;

	PostQueuedCompletionStatus(
		m_hWorkIoPort,
		NULL,
		eEM_CallBack,
		(LPOVERLAPPED)pMyOverlapped);
}


#pragma endregion
///////////////////////////////////////////////////////////////
#pragma region 运行管理
unsigned WINAPI AsynchronousExecute::ManagerProc(LPVOID p)
{
#ifndef _DEBUG
	__try
	{
#endif
		//printf("ManagerProc()开始\r\n");
		AsynchronousExecute *pThis = (AsynchronousExecute*)p;

		DWORD			dwNumberOfBytesTransferred	= 0;
		ULONG_PTR		lCompletionKey				= 0;
		MyOverlapped	*lpOverlapped				= NULL;

		for(;;)
		{
			BOOL bRet = GetQueuedCompletionStatus(
				pThis->m_hManageIoPort, &dwNumberOfBytesTransferred, &lCompletionKey, 
				(LPOVERLAPPED*)&lpOverlapped, /*INFINITE*/pThis->m_dwWaitTime
				);

			if(bRet)
			{
				//printf("ManagerProc() ManageControl = %d\r\n", lCompletionKey);
				switch(lCompletionKey)
				{
				case eCC_ManagerThreadExit:
					//printf("ManagerProc()停止\r\n");
					_endthreadex( 0 );
					return 0;
				case eCC_WaitWorkThreadExit:
					{
						ThreadInfo* pThreadInfo = (ThreadInfo*)lpOverlapped->pParm;
						if(WAIT_TIMEOUT == WaitForSingleObject(pThreadInfo->m_hThread, WAIT_WORKTHREAD_END_TIME))
							TerminateThread(pThreadInfo->m_hThread, 99);
						CloseHandle(pThreadInfo->m_hThread);
						pThis->DelThreadInfo(pThreadInfo->m_dwThredId);
						pThis->RestoreMyOverlapped(&lpOverlapped);
					}
					break;
				default:
					break;
				}
			}
			else
			{
				DWORD dwError = GetLastError();
				if (WAIT_TIMEOUT == dwError)
				{
					switch(pThis->GetThreadPoolStatus())
					{
					case eTPS_BUSY:
						{
							if(pThis->GetThreadNum() < pThis->m_dwMaxThreadNum)
							{
								//printf("增加了一个线程\r\n");
								pThis->AddThreads();
							}
						}
						break;
					case eTPS_IDLE:
						{
							if(pThis->GetThreadNum() > pThis->m_dwMinThreadNum)
							{
								//printf("关闭了一个线程\r\n");
								pThis->RemoveThreads();
							}
						}
						break;
					case eTPS_NORMAL:
						break;
					default:
						assert(FALSE);
						break;
					}

					continue;
				}
			}
		}
#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		throw("AsynchronousExecute::ManagerProc exception!!!");
	}
#endif

	_endthreadex( 0 );
	return 0;
}


VOID AsynchronousExecute::AddThreads(DWORD dwAddNum)
{
	if(NULL == m_hWorkIoPort) return;
	for(ULONG u = 0; u < dwAddNum; u++)
	{
		HANDLE	hThread;
		unsigned	dwThreadId;
		hThread = (HANDLE)_beginthreadex(
			NULL,
			0,
			WorkerProc,
			(LPVOID)this,
			0,
			&dwThreadId );
		AddThreadInfo(dwThreadId, hThread, FALSE);
	}
}

VOID AsynchronousExecute::RemoveThreads(VOID)
{
	if(NULL == m_hWorkIoPort) return;
	PostQueuedCompletionStatus(m_hWorkIoPort, NULL, eEM_MidwayExit, NULL);
}



#pragma endregion
///////////////////////////////////////////////////////////////
#pragma region 工作线程
unsigned WINAPI AsynchronousExecute::WorkerProc(void* p)
{
#ifndef _DEBUG
	__try
	{
#endif
		//printf("WorkerProc()开始\r\n");
		AsynchronousExecute *pThis = (AsynchronousExecute*)p;

		DWORD			dwNumberOfBytesTransferred	= 0;
		DWORD			dwCompletionKey				= 0;
		MyOverlapped	*lpOverlapped				= NULL;

		if(pThis->m_bCallCom)
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		for(;;)
		{
			BOOL bRet = GetQueuedCompletionStatus(
				pThis->m_hWorkIoPort, &dwNumberOfBytesTransferred, &dwCompletionKey, 
				(LPOVERLAPPED*)&lpOverlapped, INFINITE
				);

			if(bRet)
			{
				//printf("WorkerProc() WorkControl = %d\r\n", dwCompletionKey);
				DWORD dwCurrThreadId = GetCurrentThreadId();
				switch(dwCompletionKey)
				{
				case eEM_MidwayExit:
					{
						MyOverlapped *pTmpOverlapped = pThis->CreateMyOverlapped();
						pTmpOverlapped->pParm = (LPVOID)pThis->GetThreadInfo(dwCurrThreadId);
						PostQueuedCompletionStatus(
							pThis->m_hManageIoPort,
							NULL,
							eCC_WaitWorkThreadExit,
							pTmpOverlapped
							);
						//printf("WorkerProc()中途停止\r\n");
					}
					if(pThis->m_bCallCom)
						CoUninitialize();

					_endthreadex( 0 );
					return 0;
				case eEM_Exit:
					{
						//printf("WorkerProc()停止\r\n");
					}
					if(pThis->m_bCallCom)
						CoUninitialize();

					_endthreadex( 0 );
					return 0;
				case eEM_Normal:
					{
						pThis->SetThreadInfo(dwCurrThreadId, TRUE);
						lpOverlapped->pfWork(lpOverlapped->pParm);
						pThis->SetThreadInfo(dwCurrThreadId, FALSE);
					}
					break;
				case eEM_CallBack:
					{
						pThis->SetThreadInfo(dwCurrThreadId, TRUE);
						lpOverlapped->pfCallBack(lpOverlapped->pfWork(lpOverlapped->pParm));
						pThis->SetThreadInfo(dwCurrThreadId, FALSE);
					}
					break;
				default:
					assert(FALSE);
					break;
				}
				pThis->RestoreMyOverlapped(&lpOverlapped);
			}
		}

		if(pThis->m_bCallCom)
			CoUninitialize();
#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		throw("AsynchronousExecute::WorkerProc exception!!!");
	}
#endif
	
	_endthreadex( 0 );
	return 0;
}

inline VOID AsynchronousExecute::AddThreadInfo(DWORD dwThredId, HANDLE hThread, BOOL bBusy)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.insert(m_mapThreadInfo.end(), ThreadInfoMap::value_type(dwThredId,ThreadInfo(dwThredId, hThread, FALSE)));
	LeaveCriticalSection(&m_CriticalSection);
}
inline VOID AsynchronousExecute::DelThreadInfo(DWORD dwThredId)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.erase(dwThredId);
	LeaveCriticalSection(&m_CriticalSection);
}
inline VOID AsynchronousExecute::SetThreadInfo(DWORD dwThredId, BOOL bBusy)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo[dwThredId].m_bBusyWorking = bBusy;
	LeaveCriticalSection(&m_CriticalSection);
}
inline AsynchronousExecute::ThreadInfo* AsynchronousExecute::GetThreadInfo(DWORD dwThredId)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));

	EnterCriticalSection(&m_CriticalSection);
	ThreadInfo* pThreadInfo = &m_mapThreadInfo[dwThredId];
	LeaveCriticalSection(&m_CriticalSection);

	return pThreadInfo;
}
inline DWORD AsynchronousExecute::GetThreadNum(VOID)
{
	DWORD dwRe = 0;

	EnterCriticalSection(&m_CriticalSection);
	dwRe = (DWORD)m_mapThreadInfo.size();
	LeaveCriticalSection(&m_CriticalSection);

	return dwRe;
}
VOID AsynchronousExecute::GetAllThreadHandle(vector<HANDLE> &vThreadHandle)
{
	vThreadHandle.clear();
	EnterCriticalSection(&m_CriticalSection);
	for (ThreadInfoMap::iterator ite = m_mapThreadInfo.begin(); ite != m_mapThreadInfo.end(); ++ite)
	{
		vThreadHandle.push_back(ite->second.m_hThread);
	}
	LeaveCriticalSection(&m_CriticalSection);
}
AsynchronousExecute::eThreadPoolStatus AsynchronousExecute::GetThreadPoolStatus(VOID)
{
	DWORD lSum = GetThreadNum(), lBusyNum = 0;
	if(0 == lSum) return eTPS_BUSY;

	EnterCriticalSection(&m_CriticalSection);
	for (ThreadInfoMap::iterator ite = m_mapThreadInfo.begin(); ite != m_mapThreadInfo.end(); ++ite)
	{
		if(ite->second.m_bBusyWorking)
			++lBusyNum;
	}
	LeaveCriticalSection(&m_CriticalSection);

	LONG lScal = lBusyNum * 100 / lSum;

	if(20 >= lScal) return eTPS_IDLE;
	if(80 <= lScal) return eTPS_BUSY;
	return eTPS_NORMAL;
}

#pragma endregion
