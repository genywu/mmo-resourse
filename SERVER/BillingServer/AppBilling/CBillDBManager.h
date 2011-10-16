#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class BillWorkerThread;
class CBillOper;

class CBillDBManager 
{	
public:
	CBillDBManager();
	~CBillDBManager();	

private:	
	static HANDLE m_hWorkerEvent;

	list<CBillOper*> m_listOpers;

	map<long, BillWorkerThread*>	m_mapWorkerThread;

	CRITICAL_SECTION m_WorkerCriticalSection;

public:
	BOOL Init();

	VOID Release();

	void InitWorkerThread(string strProvider, 
		string strDataSource, 
		string strInitialCatalog,
		string strUserID, 
		string strPassword);

	static VOID	 SetWorkerEvent(void);
	
	static VOID	 WaitWorkerEvent(void);

	static VOID	 ResetWorkerEvent(void);

	VOID AddOpers(CBillOper* pOper);

	VOID GetOpers(list<CBillOper*>& listRet);
};