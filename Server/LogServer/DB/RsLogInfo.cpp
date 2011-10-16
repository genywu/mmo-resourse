#include "StdAfx.h"
#include ".\rsloginfo.h"
//#include "..\..\nets\basemessage.h"
//#include "..\..\server\loginserver\loginserver\game.h"
CRsLogInfo::CRsLogInfo(void)
{
}

CRsLogInfo::~CRsLogInfo(void)
{
}

//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2004/12/24]    
//--------------------------------------------------------------------------------
bool CRsLogInfo::AccountEnterLog(LPCSTR szAccount, DWORD dwIP)
{
	// 无帐户
	if(!szAccount) return false;
	char sql[512];
	char szAccountEnterTime[20];
	char szIP[16];
	sprintf(szIP, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));
	
	_ConnectionPtr cn;
	GetSystemTimeAsString(szAccountEnterTime);

	if(!CreateCn(cn)) return false;
	if(!OpenCn(cn)) return false;

	sprintf(sql,"INSERT INTO LogInfo(Account,AccountEnterTime,IP) VALUES('%s','%s','%s')", 
		szAccount,szAccountEnterTime,szIP);
	if(!ExecuteCn(sql, cn/*m_Connection*/)) 
	{
		ReleaseCn(cn);
		PrintErr("日志：写入帐户登录错");
		return false;
	}
	ReleaseCn(cn);
	return true;
}

//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2004/12/24]    
//--------------------------------------------------------------------------------
bool CRsLogInfo::RoleEnterLog(LPCSTR szAccount, LPCSTR szRoleName, BYTE btRoleLevel, BYTE btWorldNumber)
{
	char sql[512];
	char szCurrentTime[20];
	_ConnectionPtr cn;
	GetSystemTimeAsString(szCurrentTime);

	if(!CreateCn(cn)) return false;
	if(!OpenCn(cn)) return false;
	sprintf(sql,"UPDATE LogInfo SET RoleEnterTime='%s', RoleName='%s', RoleLevel='%d', WorldNumber='%d' \
				 WHERE LogInfoID='%d'", szCurrentTime, szRoleName, btRoleLevel, btWorldNumber, GetLogInfoIDForUpdate(szAccount));
	if(!ExecuteCn(sql, cn/*m_Connection*/))
	{
		ReleaseCn(cn);
		PrintErr("日志：写人角色登录错");
		return false;
	}
	ReleaseCn(cn);
	return true;
}

bool CRsLogInfo::LeaveLog(LPCSTR szAccount)
{
	char sql[512];
	char szCurrentTime[20];
	_ConnectionPtr cn;
	GetSystemTimeAsString(szCurrentTime);

	if(!CreateCn(cn)) return false;
	if(!OpenCn(cn)) return false;

	sprintf(sql,"UPDATE logInfo SET RoleLeaveTime = '%s', AccountLeaveTime = '%s' WHERE (LogInfoID = '%d')",
		szCurrentTime,szCurrentTime,GetLogInfoIDForUpdate(szAccount));
	if(!ExecuteCn(sql, cn/*m_Connection*/)) 
	{
		ReleaseCn(cn);
		PrintErr("日志：写人帐户和角色登出错");
		return false;
	}
	ReleaseCn(cn);
	return true;
}

bool CRsLogInfo::AccountLeaveLog(LPCSTR szAccount)
{
	char sql[512];
	char szCurrentTime[20];
	
	_ConnectionPtr cn;
	GetSystemTimeAsString(szCurrentTime);

	if(!CreateCn(cn)) return false;
	if(!OpenCn(cn)) return false;
	sprintf(sql,"UPDATE logInfo SET AccountLeaveTime = '%s' WHERE (LogInfoID = '%d')",
		szCurrentTime,GetLogInfoIDForUpdate(szAccount));
	if(!ExecuteCn(sql, cn/*m_Connection*/))
	{
		ReleaseCn(cn);
		PrintErr("日志：写人帐户和角色登出错");
		return false;
	}

	ReleaseCn(cn);
	return true;
}
//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2004/12/24]    
//--------------------------------------------------------------------------------
void CRsLogInfo::GetSystemTimeAsString(OUT LPSTR szCurrentTime)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(szCurrentTime,"%d-%d-%d %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
} 

//--------------------------------------------------------------------------------
//  Function:   
//  Parameters: 
//      
//  Return:     
//
//  Purpose:    
//  [2004/12/24]    
//--------------------------------------------------------------------------------
DWORD CRsLogInfo::GetLogInfoIDForUpdate(LPCSTR szAccount)
{
	if(!szAccount) return -1;
	char sql[512];
	_ConnectionPtr cn;
	_RecordsetPtr rs;
	DWORD dwLogInfoID = -1;

	try
	{
		CreateCn(cn);
		CreateRs(rs);

		OpenCn(cn);
		sprintf(sql,"SELECT TOP 1 LogInfoID FROM logInfo WHERE (Account = '%s') ORDER BY AccountEnterTime DESC", szAccount);
		OpenRs(sql, rs, cn/*m_Connection*/);

		if(!rs->GetadoEOF()) dwLogInfoID = rs->GetCollect("logInfoID");

		ReleaseRs(rs);
		ReleaseCn(cn);
		return dwLogInfoID;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		ReleaseCn(cn);
		PrintErr("日志：获取日志信息ID错", e);
		return -1;
	}
}