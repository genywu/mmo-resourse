#include "StdAfx.h"
#include "NetSession.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CNetSession::CNetSession(const CGUID& guid, pair<CGUID,long> CookiePair)
:m_guid(guid)
,m_Cookie(CookiePair)
,m_pCallback(NULL)
,m_dwTimeOut(0)
{
}

CNetSession::~CNetSession(void)
{
	if(m_pCallback)
	{
		m_pCallback->Release();//释放他自己
	}
}


//验证Cookie
bool	CNetSession::CheckCookie(const CGUID& PlayerGuid,long lPassWord)
{
	if(PlayerGuid == m_Cookie.first && lPassWord == m_Cookie.second)	return true;
	return false;
}

//设置回调函数句柄
void CNetSession::SetCallbackHandle(IAsyncCallback* pCallbackHandle)
{
	assert(pCallbackHandle);
	m_pCallback = pCallbackHandle;
}

//开始异步调用
void CNetSession::Beging(ulong dwTimeOut,IAsyncCaller* pCaller,...)
{
	assert(pCaller);
	m_dwTimeOut	= dwTimeOut;

	va_list va;
	va_start(va,pCaller);

	//同步调用处理
	pCaller->DoAsyncCall(GetGUID(),m_Cookie.second,va);
	va_end(va);

}

//回调时处理
void	CNetSession::OnDo(va_list va)
{
	assert(m_pCallback);
	if(m_pCallback)
	{
		tagAsyncResult Result;
		Result.Result	 = AR_DO;
		Result.Arguments = va;
		//做回调处理
		m_pCallback->OnAsyncCallback(Result);
	}
}
//回调发生时处理
void CNetSession::OnResult(va_list va)
{
	assert(m_pCallback);
	if(m_pCallback)
	{
		tagAsyncResult Result;
		Result.Result	 = AR_OK;
		Result.Arguments = va;
		//做回调处理
		m_pCallback->OnAsyncCallback(Result);
	}
}

//监控超时
void CNetSession::OnTimeOut()
{
	assert(m_pCallback);
	if(m_pCallback)
	{
		tagAsyncResult Result;
		Result.Result	 = AR_TimeOut;
		//做回调处理
		m_pCallback->OnAsyncCallback(Result);
	}
}