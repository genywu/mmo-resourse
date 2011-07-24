//////////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   ProxyUtility.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-02-07 10:04:39
//  Comment     :   Download proxy utility routines source file
//  
//////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "GetProxySetting.h"
#include "ProxyUtility.h"
#include "UpdateData.h"

extern KUPDATE_DATA g_UpdateData;
//////////////////////////////////////////////////////////////////////////////////////
//
//PROXY process routines
//  
//  int  IsCurrentUseProxy
//  int  PromptProxySettting
//  int  ValidateProxySetting(KPROXY_SETTING& ProxySetting)       // NOT implemented
//  int SetDownloadProxy(CDownloadFile& DownloadFile, LPCTSTR lpszHostUrl)
//  
//////////////////////////////////////////////////////////////////////////////////////
int IsCurrentUseProxy()
{
    int nResult = false;
    int nRetCode = IEPROXY_NONE;
    
    switch (g_UpdateData.ProxySetting.nProxyMethod)
    {
    case PROXY_METHOD_DIRECT:
        goto Exit0;
        
    case PROXY_METHOD_USEIE:
        nRetCode = GetIEProxyValid();
        if (0 == (nRetCode & (IEPROXY_HTTP | IEPROXY_SOCKS)))
            goto Exit0;
        break;
        
    case PROXY_METHOD_CUSTOM:
        break;
        
    default:
        ASSERT(false);
        goto Exit0;
    }
    
    nResult = true;
    
Exit0:
    return nResult;
}

int ValidateProxySetting(KPROXY_SETTING& ProxySetting)
{
    int nResult = false;
    
    return nResult;
}

int SetDownloadProxy(CDownloadFile& DownloadFile, LPCTSTR lpszHostUrl)
{
    int nResult = false;
    int nRetCode = false;
    
    int nDownProto = DOWN_PROTOCOL_NONE;
    int nProxyMode = PROXY_MODE_NONE;
    
    switch (g_UpdateData.ProxySetting.nProxyMethod)
    {
    case PROXY_METHOD_USEIE:
        nDownProto = GetUrlProtocolType(lpszHostUrl);
        
        nRetCode = GetIEProxyValid();
        switch (nDownProto)
        {
        case DOWN_PROTOCOL_HTTP:
            if (nRetCode & (IEPROXY_HTTP | IEPROXY_SOCKS))
                nProxyMode = PROXY_MODE_USEIE;
            break;
        case DOWN_PROTOCOL_FTP:
            if (nRetCode & (IEPROXY_FTP  | IEPROXY_SOCKS))
                nProxyMode = PROXY_MODE_USEIE;
            break;
            
        default:
            break;
        }
        break;
        
    case PROXY_METHOD_CUSTOM:
        nProxyMode = g_UpdateData.ProxySetting.nProxyMode;
        break;
        
    case PROXY_METHOD_DIRECT:
    default:
        break;
    }
    
    DownloadFile.SetProxy(
        nProxyMode,
        g_UpdateData.ProxySetting.szHostAddr,
        g_UpdateData.ProxySetting.nHostPort,
        g_UpdateData.ProxySetting.szUserName,
        g_UpdateData.ProxySetting.szPassword
    );
    
    nResult = (nProxyMode != PROXY_MODE_NONE);
    
    return nResult;
}
