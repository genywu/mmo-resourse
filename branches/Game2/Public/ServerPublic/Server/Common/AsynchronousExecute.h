/*
*	file:		
*	Brief:		
*	detail:					
*	Authtor:	张科智
*	Date:		2008-10-13
*/
  

#include "../Module/MultiObjPool.h"

#pragma once

//! 异步执行类
class AsynchronousExecute
{
public:
	//!		bCallCom:				线程是否会调用COM组件
	AsynchronousExecute(bool bCallCom = false);
	//! 等同于AsynchronousExecute() + SetWorkParam()
	//!		dwMinThreadNum：		最小保持的线程数（最小有效值0）
	//!		dwMinThreadNum：		最大保持的线程数（最大有效值128）
	//!		dwThreadNumAdjustSpace：自动调整线程数量的间隔时间（最小值3000，单位：毫秒）
	//!		bCallCom:				线程是否会调用COM组件
	AsynchronousExecute(ulong dwMinThreadNum, ulong dwMaxThreadNum, ulong dwThreadNumAdjustSpace, bool bCallCom = false);
	~AsynchronousExecute(void);

	/*----------------------------------------------------------------
	/*
	----------------------------------------------------------------*/
#pragma region 功能接口
public:
	//!										设置工作参数（在对象启动后，接口无效）
	//!											dwMinThreadNum：				最小保持的线程数（最小有效值0）
	//!											dwMinThreadNum：				最大保持的线程数（最大有效值128）
	//!											dwThreadNumAdjustSpace：		自动调整线程数量的间隔时间（最小值3000，单位：毫秒）
	bool									SetWorkParam(ulong dwMinThreadNum, ulong dwMaxThreadNum, ulong dwThreadNumAdjustSpace);
	//!										开始工作
	//!											dwEmbryonicThreadNum：			初始线程数
	//!											dwNumberOfConcurrentThreads：	最大并发数（0 = CUP数）
	void									BeginWork(ulong dwEmbryonicThreadNum, ulong dwNumberOfConcurrentThreads = 0);
	//!										结束对象
	void									Exit(void);

	//!										得到运行状态
	bool									IsRunning(void);

	//!										工作函数指针
	typedef		LPVOID						(*WorkFunction)(LPVOID);

	//!										异步执行一个无返回函数
	//!											pfWork：						工作函数指针
	//!											pParm：							工作函数参数
	void									Execute(WorkFunction pfWork, LPVOID pParm);
	//!										异步执行一个返回函数，并将返回值作为参数传递到完成后的执行结果处理函数中
	//!											pfWork：						工作函数指针
	//!											pParm：							工作函数参数
	//!											pParm：							回调函数指针
	//!											当前的实现并没有对pfWork和pfCallBack分线程处理，如有必要在进行添加
	void									Execute(WorkFunction pfWork, LPVOID pParm, WorkFunction pfCallBack);

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
	void									AddThreads(ulong dwAddNum = 1);
	void									RemoveThreads(void);


private:

	HANDLE									m_hManageIoPort;
	HANDLE									m_hMgrThread;
	unsigned								m_dwThreadId;
	//!										运行状态
	bool									m_bIsRunning;
	//!										最小保持线程数
	ulong									m_dwMinThreadNum;
	//!										最大保持线程数
	ulong									m_dwMaxThreadNum;
	//!										超时时间
	ulong									m_dwWaitTime;
	//!										开启的线程是否要调用COM组件
	const		bool						m_bCallCom;


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
		ThreadInfo(void)										{m_dwThredId=0; m_hThread=NULL; m_bBusyWorking=false; }
		ThreadInfo(ulong dwThredId, HANDLE handle, bool bBusy)	{m_dwThredId=dwThredId; m_hThread=handle; m_bBusyWorking=bBusy; }
		ThreadInfo(const ThreadInfo& info)						{m_dwThredId=info.m_dwThredId; m_hThread=info.m_hThread; m_bBusyWorking=info.m_bBusyWorking; }

		ulong		m_dwThredId;
		HANDLE		m_hThread;
		bool		m_bBusyWorking;
	};

	typedef		map<ulong, ThreadInfo>		ThreadInfoMap;

private:
	static		unsigned		WINAPI		WorkerProc(void* p);

private:
	void									AddThreadInfo(ulong dwThredId, HANDLE hThread, bool bBusy);
	void									DelThreadInfo(ulong dwThredId);
	void									SetThreadInfo(ulong dwThredId, bool bBusy);
	ThreadInfo*								GetThreadInfo(ulong dwThredId);
	ulong									GetThreadNum(void);
	void									GetAllThreadHandle(vector<HANDLE> &vThreadHandle);
	eThreadPoolStatus						GetThreadPoolStatus(void);

	MyOverlapped*							CreateMyOverlapped(void);
	void									RestoreMyOverlapped(MyOverlapped** ppMyOverlapped);

private:

	HANDLE									m_hWorkIoPort;
	CRITICAL_SECTION						m_CriticalSection;
	ThreadInfoMap							m_mapThreadInfo;

	MultiObjPool<MyOverlapped, LockOfWin32>	m_MultiObjPool;

#pragma endregion


};