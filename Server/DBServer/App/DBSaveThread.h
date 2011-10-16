#pragma once

#include "..\..\..\dbaccess\myadobase.h"
struct SaveObj;
class CBaseDBEntity;
struct SaveThreadData;
class DBAdoBase;
//! ---------------------------------------数据库线程类---------------------------------------
//!	工作线程由调用Begin（）开始，调用Exit（）结束。
//! 一旦调用了Exit（）接口，将不再接受新的任务，但已经压入的语句会继续执行完成。
//! 析构时会自动结束工作线程，但推荐手动调用Exit（）。
//!---------------------------------------------------------------------------------------------------
class DBSaveThread
{
public:
	DBSaveThread(string strProvider, 
				string strDataSource, 
				string strInitialCatalog,
				string strUserID, 
				string strPassword
				);
	~DBSaveThread(void);

public:
	//!								开始工作线程工作
	bool							Begin(void);
	//!								结束工作线程
	void							Exit(void);
	//!								是否还在运行
	bool							GetRunningFlag(void) { return m_SaveRunning; }
	//!								设置是否还在运行
	void							SetRunningFlag(bool flag) { m_SaveRunning = flag; }
	//!								设置退出事件句柄
	void							SetSaveWaitThread(void);
	//!								取得线程ID
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
	HANDLE				m_hSaveWaitThread;
	//!					线程运行标记
	bool				m_SaveRunning;
	//!					线程ID
	unsigned			m_lThreadID;
	//!					线程退出等待句柄
	HANDLE				m_hQuitWait;
};