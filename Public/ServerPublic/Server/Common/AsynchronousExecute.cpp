
#include "StdAfx.h"
#include "AsynchronousExecute.h"

const ulong MIN_THREAD_NUM				= 4;
const ulong MAX_THREAD_NUM				= 128;
const ulong MIN_WAITTIME				= 5000;
const ulong WAIT_WORKTHREAD_END_TIME	= 30000;

static long AsynchronousExecute_Instance_Num = 0;

AsynchronousExecute::AsynchronousExecute(bool bCallCom)
:m_dwMinThreadNum(MIN_THREAD_NUM), m_dwMaxThreadNum(MAX_THREAD_NUM), m_dwWaitTime(MIN_WAITTIME) 
,m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(false)
,m_hWorkIoPort(NULL)
,m_MultiObjPool(MIN_THREAD_NUM)
,m_bCallCom(bCallCom)
{
	if(1 < AsynchronousExecute_Instance_Num)
		throw;//! 创建多个实例引发异常
	++AsynchronousExecute_Instance_Num;
}
AsynchronousExecute::AsynchronousExecute(ulong dwMinThreadNum, ulong dwMaxThreadNum, ulong dwThreadNumAdjustSpace, bool bCallCom)
:m_hManageIoPort(NULL),m_hMgrThread(NULL),m_dwThreadId(0),m_bIsRunning(false)
,m_hWorkIoPort(NULL)
,m_MultiObjPool(dwMinThreadNum)
,m_bCallCom(bCallCom)
{
	if(1 < AsynchronousExecute_Instance_Num)
		throw;
	++AsynchronousExecute_Instance_Num;
	SetWorkParam(dwMinThreadNum, dwMaxThreadNum, dwThreadNumAdjustSpace);
}
AsynchronousExecute::~AsynchronousExecute(void)
{	
	Exit();
	AsynchronousExecute_Instance_Num = 0;
}


///////////////////////////////////////////////////////////////
#pragma region 功能接口

//! 设置工作参数（在对象启动后，接口无效）
bool AsynchronousExecute::SetWorkParam(ulong dwMinThreadNum, ulong dwMaxThreadNum, ulong dwThreadNumAdjustSpace)
{
	if(m_bIsRunning) return false;
	m_dwMinThreadNum	= min(max(dwMinThreadNum, MIN_THREAD_NUM), MAX_THREAD_NUM);
	m_dwMaxThreadNum	= max(min(dwMaxThreadNum, MAX_THREAD_NUM), 1);
	m_dwWaitTime		= max(dwThreadNumAdjustSpace, MIN_WAITTIME);
	return true;
}

//! 初始化对象
void AsynchronousExecute::BeginWork(ulong dwEmbryonicThreadNum, ulong dwNumberOfConcurrentThreads)
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

	m_bIsRunning = true;

	m_hWorkIoPort = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	dwEmbryonicThreadNum = min(max(m_dwMinThreadNum, dwEmbryonicThreadNum), dwEmbryonicThreadNum);
	AddThreads(dwEmbryonicThreadNum);


}
//! 结束对象
void AsynchronousExecute::Exit()
{
	if(!m_bIsRunning) return;

	PostQueuedCompletionStatus(m_hManageIoPort, NULL, eCC_ManagerThreadExit, NULL);
	WaitForSingleObject(m_hMgrThread, INFINITE);
	CloseHandle(m_hMgrThread);
	CloseHandle(m_hManageIoPort);
	m_bIsRunning = false;

	vector<HANDLE> vThreadHandle;
	GetAllThreadHandle(vThreadHandle);
	for (ULONG i = 0; i < vThreadHandle.size(); ++i)
	{
		PostQueuedCompletionStatus(m_hWorkIoPort, NULL, eEM_Exit, NULL);
	}

	ulong dwRc = WaitForMultipleObjects((ulong)vThreadHandle.size(), &vThreadHandle[0], true, WAIT_WORKTHREAD_END_TIME * 10);
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
		ulong exitCode;
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
bool AsynchronousExecute::IsRunning(void)
{
	return m_bIsRunning;
}


//! 异步执行一个无返回函数
void AsynchronousExecute::Execute(WorkFunction pfWork, LPVOID pParm)
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
void AsynchronousExecute::Execute(WorkFunction pfWork, LPVOID pParm, WorkFunction pfCallBack)
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

		ulong			dwNumberOfBytesTransferred	= 0;
		ULONG_PTR		lCompletionKey				= 0;
		MyOverlapped	*lpOverlapped				= NULL;

		for(;;)
		{
			BOOL bRet = GetQueuedCompletionStatus(
				pThis->m_hManageIoPort, &dwNumberOfBytesTransferred, &lCompletionKey, 
				(LPOVERLAPPED*)&lpOverlapped, pThis->m_dwWaitTime
				);

			if(bRet == TRUE )
			{
				switch(lCompletionKey)
				{
				case eCC_ManagerThreadExit:
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
				ulong dwError = GetLastError();
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
						assert(false);
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


void AsynchronousExecute::AddThreads(ulong dwAddNum)
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
		AddThreadInfo(dwThreadId, hThread, false);
	}
}

void AsynchronousExecute::RemoveThreads(void)
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

		ulong			dwNumberOfBytesTransferred	= 0;
		ulong			dwCompletionKey				= 0;
		MyOverlapped	*lpOverlapped				= NULL;

		if(pThis->m_bCallCom)
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		for(;;)
		{
			BOOL bRet = GetQueuedCompletionStatus(
				pThis->m_hWorkIoPort, &dwNumberOfBytesTransferred, &dwCompletionKey, 
				(LPOVERLAPPED*)&lpOverlapped, INFINITE
				);

			if(bRet == TRUE)
			{
				//printf("WorkerProc() WorkControl = %d\r\n", dwCompletionKey);
				ulong dwCurrThreadId = GetCurrentThreadId();
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
						pThis->SetThreadInfo(dwCurrThreadId, true);
						lpOverlapped->pfWork(lpOverlapped->pParm);
						pThis->SetThreadInfo(dwCurrThreadId, false);
					}
					break;
				case eEM_CallBack:
					{
						pThis->SetThreadInfo(dwCurrThreadId, true);
						lpOverlapped->pfCallBack(lpOverlapped->pfWork(lpOverlapped->pParm));
						pThis->SetThreadInfo(dwCurrThreadId, false);
					}
					break;
				default:
					assert(false);
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

inline void AsynchronousExecute::AddThreadInfo(ulong dwThredId, HANDLE hThread, bool bBusy)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.insert(m_mapThreadInfo.end(), ThreadInfoMap::value_type(dwThredId,ThreadInfo(dwThredId, hThread, false)));
	LeaveCriticalSection(&m_CriticalSection);
}
inline void AsynchronousExecute::DelThreadInfo(ulong dwThredId)
{
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo.erase(dwThredId);
	LeaveCriticalSection(&m_CriticalSection);
}
inline void AsynchronousExecute::SetThreadInfo(ulong dwThredId, bool bBusy)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));
	EnterCriticalSection(&m_CriticalSection);
	m_mapThreadInfo[dwThredId].m_bBusyWorking = bBusy;
	LeaveCriticalSection(&m_CriticalSection);
}
inline AsynchronousExecute::ThreadInfo* AsynchronousExecute::GetThreadInfo(ulong dwThredId)
{
	//assert(m_mapThreadInfo.end() != m_mapThreadInfo.find(dwThredId));

	EnterCriticalSection(&m_CriticalSection);
	ThreadInfo* pThreadInfo = &m_mapThreadInfo[dwThredId];
	LeaveCriticalSection(&m_CriticalSection);

	return pThreadInfo;
}
inline ulong AsynchronousExecute::GetThreadNum(void)
{
	ulong dwRe = 0;

	EnterCriticalSection(&m_CriticalSection);
	dwRe = (ulong)m_mapThreadInfo.size();
	LeaveCriticalSection(&m_CriticalSection);

	return dwRe;
}
void AsynchronousExecute::GetAllThreadHandle(vector<HANDLE> &vThreadHandle)
{
	vThreadHandle.clear();
	EnterCriticalSection(&m_CriticalSection);
	for (ThreadInfoMap::iterator ite = m_mapThreadInfo.begin(); ite != m_mapThreadInfo.end(); ++ite)
	{
		vThreadHandle.push_back(ite->second.m_hThread);
	}
	LeaveCriticalSection(&m_CriticalSection);
}
AsynchronousExecute::eThreadPoolStatus AsynchronousExecute::GetThreadPoolStatus(void)
{
	ulong lSum = GetThreadNum(), lBusyNum = 0;
	if(0 == lSum) return eTPS_BUSY;

	EnterCriticalSection(&m_CriticalSection);
	for (ThreadInfoMap::iterator ite = m_mapThreadInfo.begin(); ite != m_mapThreadInfo.end(); ++ite)
	{
		if(ite->second.m_bBusyWorking)
			++lBusyNum;
	}
	LeaveCriticalSection(&m_CriticalSection);

	long lScal = lBusyNum * 100 / lSum;

	if(20 >= lScal) return eTPS_IDLE;
	if(80 <= lScal) return eTPS_BUSY;
	return eTPS_NORMAL;
}


inline AsynchronousExecute::MyOverlapped* AsynchronousExecute::CreateMyOverlapped(void)
{
	return m_MultiObjPool.Multi_Alloc(_FILE_AND_LINE_);
}
inline void AsynchronousExecute::RestoreMyOverlapped(MyOverlapped** ppMyOverlapped)
{
	//delete *ppMyOverlapped;
	//*ppMyOverlapped = NULL;

	m_MultiObjPool.Multi_Free(*ppMyOverlapped);
	*ppMyOverlapped = NULL;
	
}

#pragma endregion
