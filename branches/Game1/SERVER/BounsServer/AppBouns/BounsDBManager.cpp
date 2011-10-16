#include "StdAfx.h"
#include "BounsDBManager.h"
#include "BounsOper.h"
#include "BounsOperThread.h"

HANDLE CBounsDBManager::m_hManagerEvent;

CBounsDBManager::CBounsDBManager(void)
{
	m_hManagerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&m_csOperCriticalSection);
}

CBounsDBManager::~CBounsDBManager(void)
{
	//添加线程退出操作
	CBounsOper* pOper = new CBounsOper(OT_BOUNS_EXIT_THREAD);
	GetGame()->GetBounsDBManager()->AddOper(pOper);

	map<long, CBounsOperThread*>::iterator itor = m_mapOperThread.begin();
	for(; itor != m_mapOperThread.end(); ++itor)
	{
		WaitForSingleObject(itor->second->GetQuitWaitHandle(), INFINITE);
	}

	// 释放工作线程
	itor = m_mapOperThread.begin();
	for(; itor != m_mapOperThread.end(); ++itor)
	{
		itor->second->End();
		SAFE_DELETE(itor->second);
	}

	// 释放操作列表
	list<CBounsOper*>::iterator itOper = m_listOpers.begin();
	for(; itOper != m_listOpers.end(); ++itOper)
	{
		SAFE_DELETE((*itOper));
	}
	m_listOpers.clear();

	CloseHandle(m_hManagerEvent);
	DeleteCriticalSection(&m_csOperCriticalSection);

}

void CBounsDBManager::InitOperThread(std::string strProvider, 
									 std::string strDataSource, 
									 std::string strInitialCatalog, 
									 std::string strUserID, 
									 std::string strPassword)
{
	for(int i=0; i<GetGame()->GetSetup()->dwWorkerThreadNum; i++)
	{
		CBounsOperThread* pOperThread = new CBounsOperThread(strProvider, strDataSource, 
									strInitialCatalog,	strUserID, strPassword);
		m_mapOperThread[i] = pOperThread;
		pOperThread->Begin();
	}
}

void CBounsDBManager::SetManageEvent()
{
	SetEvent(m_hManagerEvent);
}

void CBounsDBManager::WaitManagerEvent()
{
	WaitForSingleObject(m_hManagerEvent, INFINITE);
}

void CBounsDBManager::RestManagerEvent()
{
	ResetEvent(m_hManagerEvent);
}

void CBounsDBManager::AddOper(CBounsOper *pOper)
{
	if(pOper)
	{
		EnterCriticalSection(&m_csOperCriticalSection);

		if(m_listOpers.size() == 0)
		{
			CBounsDBManager::SetManageEvent();
		}

		m_listOpers.push_back(pOper);

		LeaveCriticalSection(&m_csOperCriticalSection);
	}
}

void CBounsDBManager::GetOpers(std::list<CBounsOper*> &listRet)
{
	EnterCriticalSection(&m_csOperCriticalSection);

	while( m_listOpers.size() == 0 )
	{
		CBounsDBManager::RestManagerEvent();
		LeaveCriticalSection(&m_csOperCriticalSection);
		// 等待通知事件
		CBounsDBManager::WaitManagerEvent();
		EnterCriticalSection(&m_csOperCriticalSection);
	}
	listRet = m_listOpers;
	m_listOpers.clear();
	LeaveCriticalSection(&m_csOperCriticalSection);
}