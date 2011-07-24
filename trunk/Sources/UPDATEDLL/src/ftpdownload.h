
// FtpDownload.h: interface for the CFtpDownload class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPDOWNLOAD_H__0866041E_80F8_4547_A298_A5B387FC77CB__INCLUDED_)
#define AFX_FTPDOWNLOAD_H__0866041E_80F8_4547_A298_A5B387FC77CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BufSocket.h"


// 常量定义
// 下载结果
const int	FTP_RESULT_SUCCESS	= 0;	// 成功
const int	FTP_RESULT_SAMEAS	= 1;	// 要下载的文件已经存在并且与远程文件一致，不用下载
const int	FTP_RESULT_STOP		= 2;	// 中途停止(用户中断)
const int	FTP_RESULT_FAIL		= 3;	// 下载失败

// 消息
const WPARAM	MSG_FTPDOWNLOAD_NETWORK	= (WPARAM)33;
const WPARAM	MSG_FTPDOWNLOAD_STATUS	= (WPARAM)34;
const WPARAM	MSG_FTPDOWNLOAD_RESULT	= (WPARAM)35;
const WPARAM	MSG_FTPDOWNLOAD_MAX		= (WPARAM)64; //保留最大可供扩充

//下载状态
const int		FTP_STATUS_FILENAME				= 1;
const int		FTP_STATUS_FILESIZE				= 2;
const int		FTP_STATUS_FILEDOWNLOADEDSIZE	= 3;
/*
const int		FTP_STATUS_ERRORCOUNT			= 4;
const int		FTP_STATUS_ERRORCODE			= 5;
const int		FTP_STATUS_ERRORSTRING			= 6;
*/

// 重试类别
const int 		FTP_RETRY_NONE			= 0;
const int 		FTP_RETRY_TIMES			= 1;
const int 		FTP_RETRY_ALWAYS		= 2;
const int 		DEFAULT_FTP_RETRY_MAX	= 10; //缺省的重试次数

// PROXY的类型
const int		FTP_PROXY_NONE			= 0;
const int		FTP_PROXY_SOCKS4		= 1;
const int		FTP_PROXY_SOCKS4A		= 2;
const int		FTP_PROXY_SOCKS5		= 3;

//For use IE proxy setting
//  Added by Linsuyi
//  2001/02/01  01:20
const int       FTP_PROXY_USEIE         = 4;

// 缺省端口号
static const int	 	DEFAULT_FTP_PORT= 21;

// 缺省超时参数
static const DWORD FTP_CONN_TIMEOUT	= 120*1000;// 120秒
static const DWORD FTP_SEND_TIMEOUT	= 120*1000;// 120秒
static const DWORD FTP_RECV_TIMEOUT	= 120*1000;// 120秒

// 发送请求
static const int	FTP_REQUEST_SUCCESS	= 0; // 成功
static const int	FTP_REQUEST_ERROR	= 1; // 一般网络错误，可以重试
static const int	FTP_REQUEST_STOP	= 2; // 中途停止(用户中断) (不用重试)
static const int	FTP_REQUEST_FAIL	= 3; // 失败 (不用重试)	 

// FTP命令
static const char FTP_CMD_USER[] = _T("USER");
static const char FTP_CMD_PASS[] = _T("PASS");
static const char FTP_CMD_ACCT[] = _T("ACCT");
static const char FTP_CMD_QUIT[] = _T("QUIT");
static const char FTP_CMD_PORT[] = _T("PORT");
static const char FTP_CMD_PASV[] = _T("PASV");
static const char FTP_CMD_TYPE[] = _T("TYPE");
static const char FTP_CMD_RETR[] = _T("RETR");
static const char FTP_CMD_REST[] = _T("REST");
static const char FTP_CMD_LIST[] = _T("LIST");
static const char FTP_CMD_STAT[] = _T("STAT");
static const char FTP_CMD_SITE[] = _T("SITE");
static const char FTP_CMD_ABOR[] = _T("ABOR");
static const char FTP_CMD_SIZE[] = _T("SIZE");

#pragma pack(push, 1)

// 结构定义
typedef struct _tagFtpDownloadStatus
{
	int		nStatusType;
	DWORD	dwFileSize;
	DWORD	dwFileDownloadedSize;
	CString	strFileName;
	/*
	int		nErrorCount;
	CString strError;
	DWORD	dwErrorCode;
	*/
} FTPDOWNLOADSTATUS,*PFTPDOWNLOADSTATUS;

#pragma pack(pop)


class CFtpDownload  
{
public:
    CFtpDownload();
    virtual ~CFtpDownload();
    
public:
    BOOL IsUserStop();
    void SetPasv(BOOL bPasv = TRUE);
    void SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization = TRUE );
    void SetProxy(LPCTSTR lpszProxyServer,int nProxyPort,BOOL bProxy=TRUE,BOOL bProxyAuthorization = FALSE,LPCTSTR lpszProxyUsername = NULL,LPCTSTR lpszProxyPassword = NULL,int nProxyType = FTP_PROXY_NONE);
    void SetTimeout(DWORD dwSendTimeout,DWORD dwRecvTimeout,DWORD dwConnTimeout);
    void SetRetry(int nRetryType,int nRetryDelay=0,int nRetryMax = 0);
    void SetNotifyWnd(HWND hNotifyWnd,int nNotifyMsg,BOOL bNotify = TRUE );
    void StopDownload();
    int  Download(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSavePath = NULL,BOOL bForceDownload = FALSE);
    
private:
    BOOL    GetIEProxy(CString &strProxyServer, int &nProxyPort, int &nProxyType);
    BOOL    SendCommand(LPCTSTR lpszCmd,LPCTSTR lpszParameter);
    BOOL    GetDataReply(CString& strReply);
    BOOL    GetInfo(LPCTSTR lpszReply,DWORD& dwSize,CTime& TimeLastModified);
    BOOL    GetPasvIP(LPCTSTR lpszReply,CString& strPasvIP,int& nPasvPort);
    BOOL    GetReplyCode(LPCTSTR lpszResponse,DWORD& dwReplyCode,BOOL& bMultiLine);
    BOOL    GetReply(CString& strReply,DWORD& dwReplyCode);
    int     SendRequest(BOOL bPasv = TRUE);
    BOOL    CreateControlSocket();
    void    CloseControlSocket();
    BOOL    CreateDataSocket( SOCKET hSocket = INVALID_SOCKET );
    void    CloseDataSocket();
    BOOL    ParseURL(LPCTSTR lpszURL,CString& strServer,CString& strObject,int& nPort);
    void    GetFileName();
    CTime   GetTime(LPCTSTR lpszTime);
    int     MakeConnection(CBufSocket* pBufSocket,CString strServer,int nPort,BOOL bBind = FALSE ,LPDWORD lpdwIP = NULL,LPINT lpnPort = NULL);
    
private:
    // 待下载URL和本地保存路径
    CString		m_strDownloadUrl;
    CString		m_strSavePath;	  // 可以是全路径或保存目录	
    CString		m_strTempSavePath;//临时文件的路径
    
    // 停止下载
    BOOL		m_bStopDownload;
    HANDLE		m_hStopEvent;
    
    // 强制重新下载(不管已有的文件是否与远程文件相同)
    BOOL		m_bForceDownload;
    
    // 文件日期(远程文件的信息)
    CTime		m_TimeLastModified;
    
    // 是否支持断点续传
    BOOL		m_bSupportResume;
    
    // 是否使用PASV方式
    BOOL		m_bPasv;
    
    // 文件以及下载大小
    DWORD		m_dwFileSize;				// 文件总的大小	
    DWORD		m_dwFileDownloadedSize;		// 文件总共已经下载的大小
    
    DWORD		m_dwDownloadSize;			// 本次需要下载的大小
    DWORD		m_dwDownloadedSize;			// 本次已经下载的大小
    
    
    // 超时TIMEOUT	连接超时、发送超时、接收超时(单位：毫秒)
    DWORD		m_dwConnTimeout;	
    DWORD		m_dwRecvTimeout;
    DWORD		m_dwSendTimeout;
    
    // 重试机制
    int		m_nRetryType;	//重试类型(0:不重试 1:重试一定次数 2:总是重试)
    int		m_nRetryTimes;	//重试次数
    int		m_nRetryDelay;	//重试延迟(单位：毫秒)
    int		m_nRetryMax;    //重试的最大次数
    
    // 错误处理
    int		m_nErrorCount;	//错误次数 (暂时没有作用)
    CString	m_strError;		//错误信息 (暂时没有作用)
    
    
    // 向其他窗口发送消息
    BOOL		m_bNotify;			// 是否向外发送通知消息	
    HWND		m_hNotifyWnd;		// 被通知的窗口
    int			m_nNotifyMessage;	// 被通知的消息
    
    // 是否使用代理 
    BOOL		m_bProxy;
    CString		m_strProxyServer;
    int			m_nProxyPort;
    int			m_nProxyType;
    
    // 代理是否需要验证
    BOOL		m_bProxyAuthorization;
    CString		m_strProxyUsername;
    CString		m_strProxyPassword;
    
    // FTP用户和口令
    BOOL		m_bAuthorization;
    CString		m_strUsername;
    CString		m_strPassword;
    
    
    // 下载过程中所用的变量
    CString		m_strServer;
    CString		m_strObject;
    CString		m_strFileName;
    int			m_nPort;
    
    CBufSocket	m_cControlSocket;	// 控制连接的SOCKET
    CBufSocket	m_cDataSocket;		// 数据连接的SOCKET
};


#endif // !defined(AFX_FTPDOWNLOAD_H__0866041E_80F8_4547_A298_A5B387FC77CB__INCLUDED_)
