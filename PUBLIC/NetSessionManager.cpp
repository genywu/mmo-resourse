#include "StdAfx.h"
#include ".\netsessionmanager.h"
#include "NetSession.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNetSessionManager* CNetSessionManager::instance = NULL;
CNetSessionManager::CNetSessionManager(void)
{
}

CNetSessionManager::~CNetSessionManager(void)
{
}

CNetSessionManager* CNetSessionManager::getInstance()
{
	if(instance == NULL)
	{
		instance = new CNetSessionManager();
	}
	return instance;
}


void CNetSessionManager::Release()
{
	MulTiThMap<CGUID,CNetSession*>::iterator it = m_Sessions.begin();
	for(;it!=m_Sessions.end();it++)
	{
		SAFE_DELETE((*it).second);
	}

	if(instance != NULL)
	{
		SAFE_DELETE(instance);
	}
}

//创建一个网络会话实例
CNetSession* CNetSessionManager::CreateSession(const CGUID& UserGuid)
{
	CGUID NetGuid;
	CGUID::CreateGUID(NetGuid);

	long lPassword = random(30000);
	CNetSession* pSessin = new CNetSession(NetGuid,make_pair(UserGuid,lPassword));

	m_Sessions.Lock();
	m_Sessions[NetGuid] = pSessin;
	m_Sessions.UnLock();

	return pSessin;
}

//网络回调处理
void CNetSessionManager::OnDo(const CGUID& guid, const CGUID& UserGuid, long lPassWord,...)
{
	CNetSession* pSession = NULL;
	//锁住资源
	m_Sessions.Lock();
	if( m_Sessions.find(guid) != m_Sessions.end() )
	{
		pSession = m_Sessions[guid];
	}
	//释放资源
	m_Sessions.UnLock();

	//验证cookie
	if(pSession && pSession->CheckCookie(UserGuid,lPassWord) )
	{
		va_list va;
		va_start(va,lPassWord);
		pSession->OnDo(va);
		va_end(va);
	}
}

//网络回调触发处理
void CNetSessionManager::OnSyncCallbackResult(const CGUID& guid, const CGUID& UserGuid, long lPassWord,...)
{
	CNetSession* pSession = NULL;
	//锁住资源
	m_Sessions.Lock();
		if( m_Sessions.find(guid) != m_Sessions.end() )
		{
			pSession = m_Sessions[guid];
		}
	//释放资源
	m_Sessions.UnLock();

	//验证cookie
	if(pSession && pSession->CheckCookie(UserGuid,lPassWord) )
	{
		va_list va;
		va_start(va,lPassWord);
		pSession->OnResult(va);
		va_end(va);

		//删除会话实例
		m_Sessions.Lock();
			m_Sessions.erase(guid);
		m_Sessions.UnLock();
		SAFE_DELETE(pSession);
	}
}

void CNetSessionManager::Run()
{
	m_Sessions.Lock();
	MulTiThMap<CGUID,CNetSession*>::iterator it = m_Sessions.begin();
	for(;it != m_Sessions.end();)
	{
		CNetSession* pSession = (*it).second;
		if (pSession->GetLeftTime()>0)
		{
			pSession->SetLeftTime(pSession->GetLeftTime()-1);
			++it;
		}
		else
		{
			pSession->OnTimeOut();
			m_Sessions.erase(it++);
			SAFE_DELETE(pSession);
		}
	}
	m_Sessions.UnLock();
}

CNetSessionManager* GetNetSessionManager()
{
	return CNetSessionManager::getInstance();
}