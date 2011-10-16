#pragma once
#include <list>
#include "..\..\..\dbaccess\myadobase.h"
#include "ThreadDataDef.h"

class DBAdoBase;
class CDBPlayer;
class CDBLoginPlayer;
//! ---------------------------------------数据库线程类---------------------------------------
//!	工作线程由调用Begin（）开始，调用Exit（）结束。
//! 一旦调用了Exit（）接口，将不再接受新的任务，但已经压入的语句会继续执行完成。
//! 析构时会自动结束工作线程，但推荐手动调用Exit（）。
//!---------------------------------------------------------------------------------------------------

class DBLoadThread
{
public:
	DBLoadThread(string strProvider, 
				string strDataSource, 
				string strInitialCatalog,
				string strUserID, 
				string strPassword
				);
	~DBLoadThread(void);

public:
	//!								开始工作线程工作
	bool							Begin(void);
	//!								结束工作线程
	void							Exit(void);
	//!								是否还在运行
	bool							GetRunningFlag(void) { return m_LoadRunning; }
	//!								设置是否还在运行
	void							SetRunningFlag(bool flag) { m_LoadRunning = flag; }
	//!								设置退出事件
	void							SetLoadWaitThread(void);
	//!								得到线程ID
	long							GetThreadID(void) { return m_lThreadID; }
	//!								线程退出等待句柄
	HANDLE&							GetQuitWaitHandle(void) { return m_hQuitWait; }
private:
	//!		
	bool							ProcessData(void);
	//!								线程
	static	unsigned	__stdcall	RunThread(void *pThis);
	//!								ConnctionPtr
	_ConnectionPtr					GetConnPtr(void);
	bool							OpenCn(void);
	DBAdoBase*						GetAdoBasePtr(void);
private:
	//!
	DBAdoBase*			m_pConnPtr;
	//!					数据库名称
	string				m_strInitialCatalog;
	//!					线程句柄
	HANDLE				m_hLoadWaitThread;
	//!					线程运行标记
	bool				m_LoadRunning;
	//!					测试ID
	unsigned int		m_lThreadID;
	//!					线程退出等待句柄
	HANDLE				m_hQuitWait;
};