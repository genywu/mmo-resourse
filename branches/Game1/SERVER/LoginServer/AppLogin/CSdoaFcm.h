





#include "../../sdoa4fcm/FCMsdkBase.h"


#pragma once


class CSdoaFcm
{
public:
	CSdoaFcm(VOID);
	~CSdoaFcm(VOID);

public:
	//!					
	bool								Init(VOID);
	//!
	VOID								Release(VOID);

	//!									响应用户上线
	int									UserOnline(const char* szRoleID, long lIP);
	//!									响应用户下线
	int									UserOffline(const char* szRoleID);

//! 3个回调函数
public:
	//!									响应防沉迷通知
	static	int							OnNotifyCall(const char* szUserID, const char* szRoleID,int iOnlineMinutes,int iOfflineMinutes);
	//!									响应防沉迷结果返回 needFCM = 1需要防沉迷 ，needFCM = 0不需要防沉迷
	static	void						OnSwitch(int needFCM);
	//!									响应查询请求返回
	static	void						OnQueryReturn(const char* szUserID, const char* szRoleID,int iState ,int iOnlineMinutes,int iOfflineMinutes);

private:
	//!									盛大SDK中的防沉迷验证对象
	CFCMSDKBase							*m_pCFCMSDKBase;
};