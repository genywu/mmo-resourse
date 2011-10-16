#pragma once

#include "..\..\..\dbaccess\myadobase.h"


class BillAdoBase;

class BillWorkerThread
{
public:
	BillWorkerThread(string strProvider, 
				string strDataSource, 
				string strInitialCatalog,
				string strUserID, 
				string strPassword
				);
	~BillWorkerThread(void);

public:	
	bool							Begin(void);	
	void							Exit(void);	
	bool							GetRunningFlag(void) { return m_ThreadRunning; }	
	void							SetRunningFlag(bool flag) { m_ThreadRunning = flag; }	
	void							SetSaveWaitThread(void);	
	long							GetThreadID(void) { return m_lThreadID; }	
	HANDLE&							GetQuitWaitHandle(void) { return m_hQuitWait; }
	HANDLE&							GetThreadHandle(){return m_hWaitWorkerThread;}

	
private:	
	bool							ProcessData(void);	
	static	unsigned	__stdcall	WorkerFunction(void *pThis);	
	_ConnectionPtr					GetConnPtr(void);
	bool							OpenCn(void);
	BillAdoBase*					GetAdoBasePtr(void);
	
private:	
	BillAdoBase*		m_pConnPtr;	
	string				m_strInitialCatalog;	
	HANDLE				m_hWaitWorkerThread;	
	bool				m_ThreadRunning;	
	unsigned			m_lThreadID;	
	HANDLE				m_hQuitWait;
	
};