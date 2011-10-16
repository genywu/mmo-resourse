


/*
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-13
*/



#pragma once



//! 异步执行类
class AsynchronousExecute
{
public:
	//!		bCallCom:				线程是否会调用COM组件
	AsynchronousExecute(BOOL bCallCom = FALSE);
	//! 等同于AsynchronousExecute() + SetWorkParam()
	//!		dwMinThreadNum：		最小保持的线程数（最小有效值0）
	//!		dwMinThreadNum：		最大保持的线程数（最大有效值128）
	//!		dwThreadNumAdjustSpace：自动调整线程数量的间隔时间（最小值3000，单位：毫秒）
	//!		bCallCom:				线程是否会调用COM组件
	AsynchronousExecute(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace, BOOL bCallCom = FALSE);
	~AsynchronousExecute(VOID);

	/*----------------------------------------------------------------
	/*
	----------------------------------------------------------------*/
#pragma region 功能接口
public:
	//!										设置工作参数（在对象启动后，接口无效）
	//!											dwMinThreadNum：				最小保持的线程数（最小有效值0）
	//!											dwMinThreadNum：				最大保持的线程数（最大有效值128）
	//!											dwThreadNumAdjustSpace：		自动调整线程数量的间隔时间（最小值3000，单位：毫秒）
	BOOL									SetWorkParam(DWORD dwMinThreadNum, DWORD dwMaxThreadNum, DWORD dwThreadNumAdjustSpace);
	//!										开始工作
	//!											dwEmbryonicThreadNum：			初始线程数
	//!											dwNumberOfConcurrentThreads：	最大并发数（0 = CUP数）
	VOID									BeginWork(DWORD dwEmbryonicThreadNum, DWORD dwNumberOfConcurrentThreads = 0);
	//!										结束对象
	VOID									Exit(VOID);

	//!										得到运行状态
	BOOL									IsRunning(VOID);

	//!										工作函数指针
	typedef		LPVOID						(*WorkFunction)(LPVOID);

	//!										异步执行一个无返回函数
	//!											pfWork：						工作函数指针
	//!											pParm：							工作函数参数
	VOID									Execute(WorkFunction pfWork, LPVOID pParm);
	//!										异步执行一个返回函数，并将返回值作为参数传递到完成后的执行结果处理函数中
	//!											pfWork：						工作函数指针
	//!											pParm：							工作函数参数
	//!											pParm：							回调函数指针
	//!											当前的实现并没有对pfWork和pfCallBack分线程处理，如有必要在进行添加
	VOID									Execute(WorkFunction pfWork, LPVOID pParm, WorkFunction pfCallBack);

#pragma endregion
	/*----------------------------------------------------------------
	/*
	----------------------------------------------------------------*/
#pragma region 运行管理
private:
	enum eWorkControl
	{
		eCC_ManagerThreadExit,
		eCC_WaitWorkThreadExit,
	};
	enum eThreadPoolStatus
	{
		eTPS_BUSY,
		eTPS_IDLE,
		eTPS_NORMAL 
	};

private:
	static		unsigned		WINAPI		ManagerProc(LPVOID p);

private:
	VOID									AddThreads(DWORD dwAddNum = 1);
	VOID									RemoveThreads(VOID);


private:
	HANDLE									m_hManageIoPort;
	HANDLE									m_hMgrThread;
	unsigned								m_dwThreadId;
	//!										运行状态
	BOOL									m_bIsRunning;
	//!										最小保持线程数
	DWORD									m_dwMinThreadNum;
	//!										最大保持线程数
	DWORD									m_dwMaxThreadNum;
	//!										超时时间
	DWORD									m_dwWaitTime;
	//!										开启的线程是否要调用COM组件
	const		BOOL						m_bCallCom;


#pragma endregion
	/*----------------------------------------------------------------
	/*
	----------------------------------------------------------------*/
#pragma region 工作线程

private:
	enum eExecuteMode
	{
		eEM_MidwayExit,
		eEM_Exit,
		eEM_Normal,
		eEM_CallBack,
	};
	struct MyOverlapped : public OVERLAPPED
	{
		WorkFunction	pfWork;
		LPVOID			pParm;
		WorkFunction	pfCallBack;
	};

	class ThreadInfo
	{
	public:  
		ThreadInfo(VOID)										{m_dwThredId=0; m_hThread=NULL; m_bBusyWorking=FALSE; }
		ThreadInfo(DWORD dwThredId, HANDLE handle, BOOL bBusy)	{m_dwThredId=dwThredId; m_hThread=handle; m_bBusyWorking=bBusy; }
		ThreadInfo(const ThreadInfo& info)						{m_dwThredId=info.m_dwThredId; m_hThread=info.m_hThread; m_bBusyWorking=info.m_bBusyWorking; }

		DWORD		m_dwThredId;
		HANDLE		m_hThread;
		BOOL		m_bBusyWorking;
	};

	typedef		map<DWORD, ThreadInfo>		ThreadInfoMap;

private:
	static		unsigned		WINAPI		WorkerProc(void* p);

private:
	VOID									AddThreadInfo(DWORD dwThredId, HANDLE hThread, BOOL bBusy);
	VOID									DelThreadInfo(DWORD dwThredId);
	VOID									SetThreadInfo(DWORD dwThredId, BOOL bBusy);
	ThreadInfo*								GetThreadInfo(DWORD dwThredId);
	DWORD									GetThreadNum(VOID);
	VOID									GetAllThreadHandle(vector<HANDLE> &vThreadHandle);
	eThreadPoolStatus						GetThreadPoolStatus(VOID);

	MyOverlapped*							CreateMyOverlapped(VOID);
	VOID									RestoreMyOverlapped(MyOverlapped** ppMyOverlapped);

private:
	HANDLE									m_hWorkIoPort;
	CRITICAL_SECTION						m_CriticalSection;
	ThreadInfoMap							m_mapThreadInfo;

#pragma endregion


};