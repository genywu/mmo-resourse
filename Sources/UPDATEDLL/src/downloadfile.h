////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DownloadFile..h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  11:43:34
//  Comment     :   CDownloadFile class header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOADFILE_H__E7F63D02_EA09_4DD2_8D47_AC9A4FD8A98A__INCLUDED_)
#define AFX_DOWNLOADFILE_H__E7F63D02_EA09_4DD2_8D47_AC9A4FD8A98A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BusyThread.h"

#include "FtpDownload.h"
#include "HttpDownload.h"


#define PROXY_MODE_NONE                     0
#define PROXY_MODE_HTTPGET                  1
#define PROXY_MODE_HTTPCONNECT              2
#define PROXY_MODE_SOCKS4                   3
#define PROXY_MODE_SOCKS4A                  4
#define PROXY_MODE_SOCKS5                   5
#define PROXY_MODE_USEIE                    6

#define DOWN_PROTOCOL_NONE                  0
#define DOWN_PROTOCOL_HTTP                  1
#define DOWN_PROTOCOL_FTP                   2
#define DOWN_PROTOCOL_FILE                  3

int GetUrlProtocolType(LPCTSTR lpszUrlPath);


#define DOWN_RESULT_SUCCESS                 HTTP_RESULT_SUCCESS         // 成功
#define DOWN_RESULT_SAMEAS                  HTTP_RESULT_SAMEAS          // 要下载的文件已经存在并且与远程文件一致，不用下载
#define DOWN_RESULT_STOP                    HTTP_RESULT_STOP            // 中途停止(用户中断)
#define DOWN_RESULT_FAIL                    HTTP_RESULT_FAIL            // 下载失败
#define DOWN_RESULT_REDIRECTFTP             HTTP_RESULT_REDIRECTFTP     // 重定向到FTP
#define DOWN_RESULT_REDIRECTHTTP            HTTP_RESULT_REDIRECTHTTP    // 重定向到FTP

// for download thread
#define MSG_DOWNLOAD_RESULT                 1000


#define DOWN_STATUS_FILENAME                FTP_STATUS_FILENAME // HTTP_
#define DOWN_STATUS_FILESIZE                FTP_STATUS_FILESIZE // HTTP_
#define DOWN_STATUS_FILEDOWNED              FTP_STATUS_FILEDOWNLOADEDSIZE   // HTTP_

//PFTPDOWNLOADSTATUS and PHTTPDOWNLOADSTATUS is the compatibility structure
typedef PFTPDOWNLOADSTATUS                  PDOWNLOADSTATUS;


int GetUrlProtocolType(LPCTSTR lpszUrlPath);


class CDownloadFile : public CBusyThread
{
public:
    CDownloadFile();
    virtual ~CDownloadFile();
    
public:
    int SetSavePath(LPCTSTR lpszSavePath);
    
public:
    void SetTimeout(
        ULONG ulSendTimeout,
        ULONG ulRecvTimeout,
        ULONG ulConnTimeout
    );
    void SetProxy(
        int nProxyMode,
        LPCTSTR lpszProxyHost,
        int nProxyPort,
        LPCTSTR lpszProxyUserName,
        LPCTSTR lpszProxyPassword
    );
    
public:
    int StartDownload(LPCTSTR lpszUrlPath, BOOL nEnableResume = TRUE, LPCTSTR lpszSaveAs = NULL);
    int StopDownload();
    
    void SetNotifyWnd(HWND hNotifyWnd, ULONG ulNotifyMsg);
    
protected:
    BOOL DownOneFile();
protected:
    virtual ULONG MainExecution();
    
protected:
    HWND  m_hNotifyWnd;
    ULONG m_ulNotifyMsg;
    
    ULONG m_ulNotifyCode;
    
    CFtpDownload m_FtpDownload;
    CHttpDownload m_HttpDownload;
    // CFileDownload m_FileDownload;
    
protected:
    CString m_strSaveDirPath;
    
    int m_nForceDown;
    CString m_strDownUrlPath;
    
    CString m_strSaveFilePath;
    
    int m_nDownProtocol;
	
	HANDLE m_hstartdown;
	HANDLE m_hdowning;
};

#endif // !defined(AFX_DOWNLOADFILE_H__E7F63D02_EA09_4DD2_8D47_AC9A4FD8A98A__INCLUDED_)

