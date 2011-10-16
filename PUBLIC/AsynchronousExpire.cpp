
#include "stdafx.h"
#include "AsynchronousExpire.h"

const DWORD MIN_THREAD_NUM				= 4;
const DWORD MAX_THREAD_NUM				= 128;
const DWORD MIN_WAITTIME				= 5000;
const DWORD WAIT_WORKTHREAD_END_TIME	= 30000;

static LONG AsynchronousExpire_Instance_Num = 0;

AsynchronousExpire::AsynchronousExpire(VOID)
:m_dwMinThreadNum(MIN_THREAD_NUM), m_dwMaxThreadNum(MAX_THREAD_NUM), m_dwWaitTime(MIN_WAITTIME) 
,m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(FALSE)
,m_hWorkIoPort(NULL)
,m_MultiObjPool(MIN_THREAD_NUM)
{
	if(1 < AsynchronousExpire_Instance_Num)
		throw;
	++AsynchronousExpire_Instance_Num;
}
AsynchronousExpire::AsynchronousExpire(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace)
:m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(FALSE)
,m_hWorkIoPort(NULL)
,m_MultiObjPool(dwMinThreadNum)
{
	if(1 < AsynchronousExpire_Instance_Num)
		throw;
	++AsynchronousExpire_Instance_Num;
	SetWorkParam(dwMinThreadNum, dwMaxThreadNum, dwThreadNumAdjustSpace);
}
AsynchronousExpire::~AsynchronousExpire(VOID)
{	
	Exit();
	AsynchronousExpire_Instance_Num = 0;
}


///////////////////////////////////////////////////////////////
#pragma region 功能接口

//! 设置工作参数（在对象启动后，接口无效）
BOOL AsynchronousExpire::SetWorkParam(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace)
{
	if(m_bIsRunning) return FALSE;
	m_dwMinThreadNum	= min(max(dwMinThreadNum, MIN_THREAD_NUM), MAX_THREAD_NUM);
	m_dwMaxThreadNum	= max(min(dwMaxThreadNum, MAX_THREAD_NUM), 1);
	m_dwWaitTime		= max(dwThreadNumAdjustSpace, MIN_WAITTIME);
	return TRUE;
}

//! 初始化对象
VOID AsynchronousExpire::BeginWork(DWORD dwEmbryonicThreadNum, DWORD dwNumberOfConcurrentThreads)
{
	if(m_bIsRunning) return;

	InitializeCriticalSection(&m_CriticalSection);

	m_hManageIoPort = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	m_hMgrThread = CreateThread(
		NULL, 
		0,
		(LPTHREAD_START_ROUTINE)ManagerProc,
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
VOID AsynchronousExpire::Exit()
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


//! 异步执行一个无返回函数
VOID AsynchronousExpire::Expire(WorkFunction pfWork, LPVOID pParm)
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
VOID AsynchronousExpire::Expire(WorkFunction pfWork, LPVOID pParm, WorkFunction pfCallBack)
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
DWORD WINAPI AsynchronousExpire::ManagerProc(LPVOID p)
{
	//printf("ManagerProc()开始\r\n");
	AsynchronousExpire *pThis = (AsynchronousExpire*)p;

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

	return 0;
}


VOID AsynchronousExpire::AddThreads(DWORD dwAddNum)
{
	if(NULL == m_hWorkIoPort) return;
	for(ULONG u = 0; u < dwAddNum; u++)
	{
		HANDLE	hThread;
		DWORD	dwThreadId;
		hThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)WorkerProc,
			(LPVOID)this,
			0,
			&dwThreadId );
		AddThreadInfo(dwThreadId, hThread, FALSE);
	}
}

VOID AsynchronousExpire::RemoveThreads(VOID)
{
	if(NULL == m_hWorkIoPort) return;
	PostQueuedCompletionStatus(m_hWorkIoPort, NULL, eEM_MidwayExit, NULL);
}



#pragma endregion
///////////////////////////////////////////////////////////////
#pragma region 工作线程
DWORD WINAPI AsynchronousExpire::WorkerProc(void* p)
{
	//printf("WorkerProc()开始\r\n");
	AsynchronousExpire *pThis = (AsynchronousExpire*)p;

	DWORD			dwNumberOfBytesTransferred	= 0;
	DWORD			dwCompletionKey				= 0;
	MyOverlapped	*lpOverlapped				= NULL;

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
				return 0;
			case eEM_Exit:
				{
					//printf("WorkerProc()停止\r\n");
				}
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

	return 0;
}

inline VOID AsynchronousExpire::AddThreadInfo(DWORD dwThredId, HANDLE hThread, BOOL bBusy)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.insert(m_mapThreadInfo.end(), ThreadInfoMap::value_type(dwThredId,ThreadInfo(dwThredId, hThread, FALSE)));
	LeaveCriticalSection(&m_CriticalSection);
}
inline VOID AsynchronousExpire::DelThreadInfo(DWORD dwThredId)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.erase(dwThredId);
	LeaveCriticalSection(&m_CriticalSection);
}
inline VOID AsynchronousExpire::SetThreadInfo(DWORD dwThredId, BOOL bBusy)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo[dwThredId].m_bBusyWorking = bBusy;
	LeaveCriticalSection(&m_CriticalSection);
}
inline AsynchronousExpire::ThreadInfo* AsynchronousExpire::GetThreadInfo(DWORD dwThredId)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));

	EnterCriticalSection(&m_CriticalSection);
	ThreadInfo* pThreadInfo = &m_mapThreadInfo[dwThredId];
	LeaveCriticalSection(&m_CriticalSection);

	return pThreadInfo;
}
inline DWORD AsynchronousExpire::GetThreadNum(VOID)
{
	DWORD dwRe = 0;

	EnterCriticalSection(&m_CriticalSection);
	dwRe = (DWORD)m_mapThreadInfo.size();
	LeaveCriticalSection(&m_CriticalSection);

	return dwRe;
}
VOID AsynchronousExpire::GetAllThreadHandle(vector<HANDLE> &vThreadHandle)
{
	vThreadHandle.clear();
	EnterCriticalSection(&m_CriticalSection);
	for (ThreadInfoMap::iterator ite = m_mapThreadInfo.begin(); ite != m_mapThreadInfo.end(); ++ite)
	{
		vThreadHandle.push_back(ite->second.m_hThread);
	}
	LeaveCriticalSection(&m_CriticalSection);
}
AsynchronousExpire::eThreadPoolStatus AsynchronousExpire::GetThreadPoolStatus(VOID)
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


inline AsynchronousExpire::MyOverlapped* AsynchronousExpire::CreateMyOverlapped(VOID)
{
	return m_MultiObjPool.Multi_Alloc();
}
inline VOID AsynchronousExpire::RestoreMyOverlapped(MyOverlapped** ppMyOverlapped)
{
	//delete *ppMyOverlapped;
	//*ppMyOverlapped = NULL;

	m_MultiObjPool.Multi_Free(*ppMyOverlapped);
	*ppMyOverlapped = NULL;
	
}

#pragma endregion
