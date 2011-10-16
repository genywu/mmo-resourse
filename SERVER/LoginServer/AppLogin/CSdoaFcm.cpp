


#include "StdAfx.h"
#include "CSdoaFcm.h"




CSdoaFcm::CSdoaFcm(VOID)
:m_pCFCMSDKBase(NULL)
{}
CSdoaFcm::~CSdoaFcm(VOID)
{}


//!					
bool CSdoaFcm::Init(VOID)
{
	if(NULL != m_pCFCMSDKBase)
		return false;

	m_pCFCMSDKBase = CreateSDKInstance();
	return m_pCFCMSDKBase->initFCMSDKEASY(CSdoaFcm::OnNotifyCall, CSdoaFcm::OnSwitch, CSdoaFcm::OnQueryReturn);
}
//!
VOID CSdoaFcm::Release(VOID)
{

}

//! 响应用户上线
int CSdoaFcm::UserOnline(const char* szRoleID, long lIP) 
{
	if(NULL == m_pCFCMSDKBase)
		return -1;

	char szClientIP[64] = {0};
	if(0 == lIP)
		sprintf(szClientIP, "a");
	else
	{
		UCHAR arrIp[4] = {0};
		memmove(arrIp, &lIP, sizeof(LONG));
		sprintf(szClientIP, "%u.%u.%u.%u", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);
	}
	

	int nRe = m_pCFCMSDKBase->FCMIDOnline(szRoleID, "a", szClientIP);
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "【%s】【%s】请求开始防沉迷，返回结果【%d】", szRoleID, szClientIP, nRe);
#endif
	return nRe;
}

//! 响应用户下线
int CSdoaFcm::UserOffline(const char* szRoleID)
{
	if(NULL == m_pCFCMSDKBase)
		return -1;

	int nRe = m_pCFCMSDKBase->FCMIDOffline(szRoleID, "a", "a");
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "【%s】请求结束防沉迷，返回结果【%d】", szRoleID, nRe);
#endif
	return nRe;
}

//! 响应防沉迷通知
int CSdoaFcm::OnNotifyCall(const char* szUserID, const char* szRoleID,int iOnlineMinutes,int iOfflineMinutes)
{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "响应防沉迷通知，【%s】在线时间累积【%d分钟】离线时间累积【%d分钟】", szUserID, iOnlineMinutes, iOfflineMinutes);
#endif

	int nHoldSecond = (3 * 60 - iOnlineMinutes) * 60;
	nHoldSecond = (nHoldSecond > 0) ? nHoldSecond : 0;
	
	//! 组装消息发送到主线程同步处理
	AinMessage *pMsg = AinMessage::Create(AIN_MSG_SDOA_FCM_NOTIFY);
	if(NULL != pMsg)
	{
		pMsg->AddDword(nHoldSecond);
		pMsg->AddStr(szUserID);

		GetGame()->SendToSelf(pMsg);
	}

	return 0;
}
//! 响应防沉迷结果返回 needFCM = 1需要防沉迷 ，needFCM = 0不需要防沉迷
void CSdoaFcm::OnSwitch(int needFCM)
{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "CSdoaFcm::OnSwitch(%d)", needFCM);
#endif
}

//! 响应查询请求返回
void CSdoaFcm::OnQueryReturn(const char* szUserID, const char* szRoleID,int iState ,int iOnlineMinutes,int iOfflineMinutes)
{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
	PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "CSdoaFcm::OnQueryReturn(%s, %d, %d, %d)", szRoleID, iState, iOnlineMinutes, iOfflineMinutes);
#endif
}