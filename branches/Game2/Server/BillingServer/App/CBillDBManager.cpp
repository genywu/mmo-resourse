#include "stdafx.h"
#include "CBilldbmanager.h"

#include "BillWorkerThread.h"
#include "CBillOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE CBillDBManager::m_hWorkerEvent;

CBillDBManager::CBillDBManager()
{	
	m_hWorkerEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	InitializeCriticalSection(&m_WorkerCriticalSection);
}

CBillDBManager::~CBillDBManager()
{

	//设置线程退出
	map<long, BillWorkerThread*>::iterator threadIt=m_mapWorkerThread.begin();
	
	CBillOper* pOper=new CBillOper(eBILL_OT_EXIT_THREAD);
	GetGame()->GetBillDBManager()->AddOpers(pOper);	

	//threadIt=m_mapWorkerThread.begin();
	for(;threadIt!=m_mapWorkerThread.end();threadIt++)
	{
		WaitForSingleObject(threadIt->second->GetQuitWaitHandle(), INFINITE);
	}

	//释放工作线程	
	threadIt=m_mapWorkerThread.begin();
	for(;threadIt!=m_mapWorkerThread.end();threadIt++)
	{
		threadIt->second->Exit();
		SAFE_DELETE(threadIt->second);
	}
	m_mapWorkerThread.clear();


	list<CBillOper*>::iterator operIt=m_listOpers.begin();
	for(;operIt!=m_listOpers.end();operIt++)
	{
		SAFE_DELETE(*operIt);
	}
	m_listOpers.clear();
	

	CloseHandle(m_hWorkerEvent);
	DeleteCriticalSection(&m_WorkerCriticalSection);
}

void CBillDBManager::InitWorkerThread(string strProvider, 
									string strDataSource, 
									string strInitialCatalog,
									string strUserID, 
									string strPassword)
{
	// 创建读写线程
	for(uint i=0; i<GetGame()->GetSetup()->dwWorkerThreadNum; i++)
	{
		BillWorkerThread* pWorkThread = new BillWorkerThread(strProvider, strDataSource, 
			strInitialCatalog, strUserID, strPassword);
		m_mapWorkerThread[i] = pWorkThread;
		pWorkThread->Begin();
	}

	//CMyAdoBase::Initialize(strProvider, strDataSource, strInitialCatalog, strUserID, strPassword);	
}

BOOL CBillDBManager::Init()
{
	return TRUE;
}

VOID CBillDBManager::Release()
{
}

VOID CBillDBManager::SetWorkerEvent(void)
{
	SetEvent(m_hWorkerEvent);
}

VOID CBillDBManager::WaitWorkerEvent(void)
{
	WaitForSingleObject(m_hWorkerEvent,INFINITE);
}

VOID CBillDBManager::ResetWorkerEvent(void)
{
	ResetEvent(m_hWorkerEvent);
}

VOID CBillDBManager::AddOpers(CBillOper* pOper)
{
	if(!pOper)
		return;

	EnterCriticalSection(&m_WorkerCriticalSection);
	if(m_listOpers.size() == 0)
	{		
		CBillDBManager::SetWorkerEvent();
	}
	m_listOpers.push_back(pOper);
	LeaveCriticalSection(&m_WorkerCriticalSection);	
}

VOID CBillDBManager::GetOpers(list<CBillOper*>& listRet)
{	
	EnterCriticalSection(&m_WorkerCriticalSection);
	while(m_listOpers.size() == 0)
	{
		CBillDBManager::ResetWorkerEvent();
		//如果没有操作命令，则等待。
		LeaveCriticalSection(&m_WorkerCriticalSection);
		//等待通知事件
		CBillDBManager::WaitWorkerEvent();		

		EnterCriticalSection(&m_WorkerCriticalSection);
	}
	listRet = m_listOpers;
	m_listOpers.clear();
	LeaveCriticalSection(&m_WorkerCriticalSection);
}