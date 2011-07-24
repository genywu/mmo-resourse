////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   UserCheck.h.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-17  17:37:18
//  Comment     :   KAVuser-net check source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "UpdatePublic.h"
#include "KString.h"
#include "UserCheck.h"
#include "UpdateData.h"


extern KUPDATE_DATA g_UpdateData;


void GetUserInfo(USER_INFO& UserInfo, BOOL bWaitForResult)
{
    SYSTEMTIME SystemTime = { 0 };
    
    ZeroMemory(&UserInfo, sizeof(UserInfo));
    
    strcpy(UserInfo.szFlag, defCHECK_CURRENT_USERFLAG);
    
    UserInfo.nStructVersion = defCHECK_CURRENT_VERSION;
    UserInfo.nWaitForResult = bWaitForResult;
    strcpy(UserInfo.szSerialNumber, g_UpdateData.szVerifyInfo);
    
//#pragma message("Alarm Linsuyi: NEED set current user information IN " __FILE__)
    
    ::GetLocalTime(&SystemTime);
    
    sprintf(
        UserInfo.szDateTime, 
        "%02d/%02d/%04d %02d:%02d:%02d",
        SystemTime.wMonth,
        SystemTime.wDay,
        SystemTime.wYear,
        SystemTime.wHour,
        SystemTime.wMinute,
        SystemTime.wSecond
    );
    
    return;
}
