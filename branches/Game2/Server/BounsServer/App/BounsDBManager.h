#pragma once

class CBounsOper;
class CBounsOperThread;


class CBounsDBManager
{
public:
	CBounsDBManager(void);
	virtual ~CBounsDBManager(void);
	

	void InitOperThread(string strProvider,
						string strDataSource,
						string strInitialCatalog,
						string strUserID,
						string strPassword);


	static void SetManageEvent();
	static void WaitManagerEvent();
	static void RestManagerEvent();

	void AddOper(CBounsOper* pOper);
	void GetOpers(list<CBounsOper*>& listRet);

private:
	static HANDLE m_hManagerEvent;
	list<CBounsOper*> m_listOpers;
	map<long, CBounsOperThread*>	m_mapOperThread;
	CRITICAL_SECTION	m_csOperCriticalSection;
};
