////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DownloadFile.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  21:52:16
//  Comment     :   CDownloadFile class source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DownloadFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDownloadFile::CDownloadFile()
{
    m_nForceDown = false;
    
    m_hNotifyWnd = NULL;
    m_ulNotifyMsg = 0;
    
    m_ulNotifyCode = -1;
    m_nDownProtocol = DOWN_PROTOCOL_NONE;

	m_hstartdown = NULL;
	m_hdowning = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CDownloadFile::~CDownloadFile()
{
	::CloseHandle(m_hdowning);
	m_hdowning = NULL;
	ASSERT(m_hstartdown == NULL);
}

int GetUrlProtocolType(LPCTSTR lpszUrlPath)
{
    int nResult = DOWN_PROTOCOL_NONE;
    
    int nRetCode = 0;
    CString strUrlPath;
    
    strUrlPath = lpszUrlPath;
    strUrlPath.MakeLower();
    
    strUrlPath.TrimLeft();
    nRetCode = strUrlPath.Find("http://");
    if (0 == nRetCode)
    {
        nResult = DOWN_PROTOCOL_HTTP;
        goto Exit1;
    }
    
    nRetCode = strUrlPath.Find("ftp://");
    if (0 == nRetCode)
    {
        nResult = DOWN_PROTOCOL_FTP;
        goto Exit1;
    }
    
    nRetCode = strUrlPath.Find("file://");
    if (0 == nRetCode)
    {
        nResult = DOWN_PROTOCOL_FILE;
        goto Exit1;
    }
    
Exit1:
    return nResult;
}

int CDownloadFile::SetSavePath(LPCTSTR lpszSavePath)
{
    int nResult = false;
    
    m_strSaveDirPath.Empty();
    
    if (lpszSavePath != NULL)
    {
        m_strSaveDirPath = lpszSavePath;
    }
    nResult = true;
    
    return nResult;
}

void CDownloadFile::SetTimeout(
    ULONG ulSendTimeout,
    ULONG ulRecvTimeout,
    ULONG ulConnTimeout
)
{
    m_FtpDownload.SetTimeout(ulSendTimeout, ulRecvTimeout, ulConnTimeout);
    m_HttpDownload.SetTimeout(ulSendTimeout, ulRecvTimeout, ulConnTimeout);
}

void CDownloadFile::SetProxy(
    int nProxyMode,
    LPCTSTR lpszProxyHost,
    int nProxyPort,
    LPCTSTR lpszProxyUserName,
    LPCTSTR lpszProxyPassword
)
{
    int nProtocolMode = PROXY_MODE_NONE;
    
    switch (nProxyMode)
    {
    case PROXY_MODE_SOCKS4:
        nProtocolMode = FTP_PROXY_SOCKS4;
        break;
    case PROXY_MODE_SOCKS4A:
        nProtocolMode = FTP_PROXY_SOCKS4A;
        break;
    case PROXY_MODE_SOCKS5:
        nProtocolMode = FTP_PROXY_SOCKS5;
        break;
    case PROXY_MODE_USEIE:
        nProtocolMode = FTP_PROXY_USEIE;
        break;
        
    case PROXY_MODE_NONE:
    default:
        nProtocolMode = FTP_PROXY_NONE;
        break;
    }
    m_FtpDownload.SetProxy(
        lpszProxyHost,
        nProxyPort,
        (nProtocolMode != FTP_PROXY_NONE),
        (lpszProxyUserName != NULL),
        lpszProxyUserName,
        lpszProxyPassword,
        nProtocolMode
    );
    
    switch (nProxyMode)
    {
    case PROXY_MODE_HTTPGET:
        nProtocolMode = HTTP_PROXY_HTTPGET;
        break;
    case PROXY_MODE_HTTPCONNECT:
        nProtocolMode = HTTP_PROXY_HTTPCONNECT;
        break;
    case PROXY_MODE_SOCKS4:
        nProtocolMode = HTTP_PROXY_SOCKS4;
        break;
    case PROXY_MODE_SOCKS4A:
        nProtocolMode = HTTP_PROXY_SOCKS4A;
        break;
    case PROXY_MODE_SOCKS5:
        nProtocolMode = HTTP_PROXY_SOCKS5;
        break;
        
    case PROXY_MODE_NONE:
    default:
        nProtocolMode = HTTP_PROXY_NONE;
        break;
    }
    m_HttpDownload.SetProxy(
        lpszProxyHost,
        nProxyPort,
        (nProtocolMode != HTTP_PROXY_NONE),
        (lpszProxyUserName != NULL),
        lpszProxyUserName,
        lpszProxyPassword,
        nProtocolMode
    );
}

int CDownloadFile::StartDownload(LPCTSTR lpszUrlPath, BOOL bEnableResume/*= TRUE*/, LPCTSTR lpszSaveAs/*= NULL*/)
{
	if (m_hdowning != NULL)
    {
		while (WaitForSingleObject(m_hdowning, 0) == WAIT_OBJECT_0)
			Sleep(1);
	}

    int nResult = false;
    int nRetCode = false;
    
    m_nForceDown = (bEnableResume == FALSE);
    
    m_strDownUrlPath = lpszUrlPath;
    if (lpszSaveAs != NULL)
    {
        m_strSaveFilePath = lpszSaveAs;
    }
    else
    {
        m_strSaveFilePath = m_strSaveDirPath;
    }    
    
    m_nDownProtocol = GetUrlProtocolType(lpszUrlPath);
    ASSERT(m_nDownProtocol != DOWN_PROTOCOL_NONE);
    
	if (IsThreadOK())
	{
		nRetCode = true;
	}
	else
	{
		ASSERT(NULL == m_hstartdown);
    
		m_hstartdown = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == m_hstartdown)
			nRetCode = FALSE;
		else
			nRetCode = StartThread();
	}
    if (!nRetCode)
    {
        ASSERT(false);
        goto Exit0;
    }

	SetEvent(m_hstartdown);	//green
 
    nResult = true;
    
Exit0:
    return nResult;
}

int CDownloadFile::StopDownload()
{
	if (m_hstartdown && WaitForSingleObject(m_hstartdown, 0) == WAIT_OBJECT_0)
	{		
		switch (m_nDownProtocol)
		{
		case DOWN_PROTOCOL_FTP:
			m_FtpDownload.StopDownload();
			break;
        
		case DOWN_PROTOCOL_HTTP:
			m_HttpDownload.StopDownload();
			break;
        
		default:
			ASSERT(false);
		}
		
		if (m_hdowning != NULL)
		{
			while (WaitForSingleObject(m_hdowning, 0) == WAIT_OBJECT_0)
				Sleep(1);
		}
	}

	if (m_hstartdown)
		CloseHandle(m_hstartdown);
	m_hstartdown = NULL;

    int nResult = false;
    
	StopThread();
    
    nResult = true;
    
    return nResult;
}

void CDownloadFile::SetNotifyWnd(HWND hNotifyWnd, ULONG ulNotifyMsg)
{
    m_hNotifyWnd = hNotifyWnd;
    m_ulNotifyMsg = ulNotifyMsg;
    
    m_FtpDownload.SetNotifyWnd(m_hNotifyWnd, m_ulNotifyMsg);
    m_HttpDownload.SetNotifyWnd(m_hNotifyWnd, m_ulNotifyMsg);
}

ULONG CDownloadFile::MainExecution()
{
	while (1)
    {
		if (WaitForSingleObject(m_hStop, 0) == WAIT_OBJECT_0)
			break;
		Sleep(1);
		if (m_hstartdown && WaitForSingleObject(m_hstartdown, 0) == WAIT_OBJECT_0)	//¿ªÊ¼down
		{
			ResetEvent(m_hstartdown);	//red
			DownOneFile();
		}
    }

	return 0;
}

BOOL CDownloadFile::DownOneFile()
{
	SetEvent(m_hdowning);
    BOOL nRetCode = FALSE;

	ULONG ulResult = -1;
    m_ulNotifyCode = -1;

    switch (m_nDownProtocol)
    {
    case DOWN_PROTOCOL_HTTP:
        ulResult = (ULONG)m_HttpDownload.Download(
            m_strDownUrlPath,
            m_strSaveFilePath,
            m_nForceDown
        );
        nRetCode = m_HttpDownload.IsUserStop();
        if (!nRetCode)
        {
            m_ulNotifyCode = ulResult;
        }
        break;
        
    case DOWN_PROTOCOL_FTP:
        ulResult = (ULONG)m_FtpDownload.Download(
            m_strDownUrlPath,
            m_strSaveFilePath,
            m_nForceDown
        );
        nRetCode = m_FtpDownload.IsUserStop();
        if (!nRetCode)
        {
            m_ulNotifyCode = ulResult;
        }
        break;
        
    default:
        ASSERT(false);
        break;
    }
    
	ResetEvent(m_hdowning);

    if (m_ulNotifyCode != -1)
    {
        ::SendMessage(
            m_hNotifyWnd,
            m_ulNotifyMsg,
            MSG_DOWNLOAD_RESULT,
            m_ulNotifyCode
        );
    }

    return nRetCode;
}
