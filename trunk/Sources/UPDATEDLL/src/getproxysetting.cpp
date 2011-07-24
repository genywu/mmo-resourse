////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   GetProxySetting.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002.01.21  15:40
//  Comment     :   Kingsoft Windows Proxy Getting source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "GetProxySetting.h"


static const char g_szRegistryIESettingPath[] =     \
    "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";

static int GetIEProxySetString(int nProxySetSize, char *pszProxySetString)
{
    int nResult = 0;
    
    LONG lRetCode = 0;
    HKEY hRegKey = NULL;
    DWORD dwLength = 0;
    
    lRetCode = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        g_szRegistryIESettingPath,
        0L,
        KEY_QUERY_VALUE,
        &hRegKey
    );
    if (lRetCode != ERROR_SUCCESS)
        goto Exit0;
    
    ZeroMemory(pszProxySetString, nProxySetSize);
    nProxySetSize--;
    if (nProxySetSize < 250)
        goto Exit0;
    
    dwLength = (DWORD)nProxySetSize;
    
    lRetCode = RegQueryValueEx(
        hRegKey,
        "ProxyServer",
        NULL,
        NULL,
        (LPBYTE)pszProxySetString,
        &dwLength
    );
    if (lRetCode != ERROR_SUCCESS)
        goto Exit0;
    
    nResult = ::lstrlen(pszProxySetString);
    
Exit0:
    if (hRegKey != NULL)
    {
    	RegCloseKey(hRegKey);
        hRegKey = NULL;
    }
    
    return nResult;
}

int GetIEProxyValid()
{
    int nResult = IEPROXY_NONE;
    int nRetCode = false;
    
    int nPos = -1;
    CString strProxyString;
    
    int nProxySetSize = 1024;
    LPTSTR lpszProxyString = NULL;
    
    lpszProxyString = strProxyString.GetBuffer(nProxySetSize);
    if (NULL == lpszProxyString)
        goto Exit0;
    
    nRetCode = GetIEProxySetString(nProxySetSize, lpszProxyString);
    
    strProxyString.ReleaseBuffer();
    if (!nRetCode)
        goto Exit0;
    
    strProxyString.MakeLower();
    nPos = strProxyString.Find("http=");    // HTTP GET Proxy
    if (-1 == nPos)
    {
        nPos = strProxyString.Find(";");
        if (nPos != -1)
            goto Exit0;
        
        nResult |= IEPROXY_HTTP;
        goto Exit1;
    }
    nResult |= IEPROXY_HTTP;
    
    nPos = strProxyString.Find("socks=");
    if (nPos != -1)
        nResult |= IEPROXY_SOCKS;
    
    nPos = strProxyString.Find("ftp=");
    if (nPos != -1)
        nResult |= IEPROXY_FTP;
    
Exit1:
Exit0:
    return nResult;
}

int GetIEProxyHttp(int nSvrNameSize, char *pszProxySvrName, int *pnProxyPort)
{
    int nResult = false;
    int nRetCode = 0;
    
    int nPos = -1;
    CString strProxyString;
    CString strProxyServer;
    
    int nProxySetSize = 1024;
    LPTSTR lpszProxyString = NULL;
    
    //分析Proxy的设置
    //http=193.168.10.1:80;socks=192.168.10.235:1080
    //193.168.10.1:1090
    //ftp=193.168.10.1:1090;gopher=193.168.10.1:1090;https=193.168.10.1:1090;socks=193.168.10.2:1080
    
    lpszProxyString = strProxyString.GetBuffer(nProxySetSize);
    if (NULL == lpszProxyString)
        goto Exit0;
    nRetCode = GetIEProxySetString(nProxySetSize, lpszProxyString);
    
    strProxyString.ReleaseBuffer();
    if (nRetCode <= 0)
        goto Exit0;
    
    if (strProxyString.IsEmpty())
        goto Exit0;
    
    strProxyString.MakeLower();
    nPos = strProxyString.Find("http=");    // HTTP GET Proxy
    if (-1 == nPos)
    {
        nPos = strProxyString.Find(";");
        if (nPos != -1)
            goto Exit0;
    }
    else
    {
        strProxyString = strProxyString.Mid(nPos + lstrlen("http="));
        
        nPos = strProxyString.Find(";");
        if (nPos != -1)
            strProxyString = strProxyString.Left(nPos);
    }
    
    nPos = strProxyString.Find(":");
    if (nPos <= 0)
        goto Exit0;
    
    strProxyServer = strProxyString.Left(nPos);
    nRetCode = strProxyServer.GetLength();
    if (nRetCode >= nSvrNameSize)
        goto Exit0;
    
    ::lstrcpy(pszProxySvrName, strProxyServer);
    
    if (pnProxyPort != NULL)
        *pnProxyPort = _ttoi(strProxyString.Mid(nPos + 1)); 
    
    nResult = true;
    
Exit0:
    return nResult;
}

int GetIEProxySocks5(int nSvrNameSize, char *pszProxySvrName, int *pnProxyPort)
{
    int nResult = false;
    int nRetCode = 0;
    
    int nPos = -1;
    CString strProxyString;
    CString strProxyServer;
    
    int nProxySetSize = 1024;
    LPTSTR lpszProxyString = NULL;
    
    //分析Proxy的设置
    //http=193.168.10.1:80;socks=192.168.10.235:1080
    //193.168.10.1:1090
    //ftp=193.168.10.1:1090;gopher=193.168.10.1:1090;https=193.168.10.1:1090;socks=193.168.10.2:1080
    
    lpszProxyString = strProxyString.GetBuffer(nProxySetSize);
    if (NULL == lpszProxyString)
        goto Exit0;
    nRetCode = GetIEProxySetString(nProxySetSize, lpszProxyString);
    
    strProxyString.ReleaseBuffer();
    if (nRetCode <= 0)
        goto Exit0;
    
    if (strProxyString.IsEmpty())
        goto Exit0;
    
    strProxyString.MakeLower();
    nPos = strProxyString.Find("socks=");   // SOCKS Proxy (SOCKS5)
    if (-1 == nPos)
        goto Exit0;
    
    strProxyString = strProxyString.Mid(nPos + lstrlen("socks="));
    
    nPos = strProxyString.Find(";");
    if (nPos != -1)
        strProxyString = strProxyString.Left(nPos);
    
    nPos = strProxyString.Find(":");
    strProxyServer = strProxyString.Left(nPos);
    nRetCode = strProxyServer.GetLength();
    if (nRetCode >= nSvrNameSize)
        goto Exit0;
    
    ::lstrcpy(pszProxySvrName, strProxyServer);
    
    if (pnProxyPort != NULL)
        *pnProxyPort = _ttoi(strProxyString.Mid(nPos+1)); 
    
    nResult = true;
    
Exit0:
    return nResult;
}


int IsIEProxyEnable()
{
    int nResult     = -1;     
    LONG lRetCode   = 0;
    HKEY hRegKey    = NULL;
    DWORD dwLength  = 0;
    DWORD dwValue   = 0;
    
    lRetCode = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        g_szRegistryIESettingPath,
        0L,
        KEY_QUERY_VALUE,
        &hRegKey
    );
    if (lRetCode != ERROR_SUCCESS)
        goto Exit0;

    dwLength = sizeof(DWORD);
    lRetCode = RegQueryValueEx(
        hRegKey,
        "ProxyEnable",
        NULL,
        NULL,
        (LPBYTE)&dwValue,
        &dwLength
    );
    if (lRetCode != ERROR_SUCCESS)
        goto Exit0;

    nResult = dwValue;

Exit0:
    return nResult;
}