
// HttpDownload.h: interface for the CHttpDownload class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPDOWNLOAD_H__4E55FDB1_0DE8_4ACD_94F4_CD097B5EAED0__INCLUDED_)
#define AFX_HTTPDOWNLOAD_H__4E55FDB1_0DE8_4ACD_94F4_CD097B5EAED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BufSocket.h"

////////////////////////////////////////////////////////////////////////////////
// 结构定义

#pragma pack(push, 1)

typedef struct _tagHttpDownloadStatus
{
	int		nStatusType;				// 状态类型
	DWORD	dwFileSize;					// 文件大小
	DWORD	dwFileDownloadedSize;		// 已经下载大小
	CString	strFileName;				// 文件名
	int		nErrorCount;				// 错误计数
	CString strError;					// 错误原因
	DWORD	dwErrorCode;				// 错误代码	
} HTTPDOWNLOADSTATUS,*PHTTPDOWNLOADSTATUS;

#pragma pack(pop)


////////////////////////////////////////////////////////////////////////////////
//					常量定义
////////////////////////////////////////////////////////////////////////////////
#undef	IN	
#define IN	// 输入参数
#undef	OUT
#define OUT	// 输出参数

////////////////////////////////////////////////////////////////////////////////
//					外部公用
////////////////////////////////////////////////////////////////////////////////
// 下载结果
const int	HTTP_RESULT_SUCCESS			= 0;	// 成功
const int	HTTP_RESULT_SAMEAS			= 1;	// 要下载的文件已经存在并且与远程文件一致，不用下载
const int	HTTP_RESULT_STOP			= 2;	// 中途停止(用户中断)
const int	HTTP_RESULT_FAIL			= 3;	// 下载失败
const int	HTTP_RESULT_REDIRECT_FTP	= 4;	// 重定向到FTP

// Added by Linsuyi  2002/02/01
const int   HTTP_RESULT_REDIRECT_HTTP   = 5;    // 重定向到HTTP


// 消息
const WPARAM	MSG_HTTPDOWNLOAD_NETWORK	= (WPARAM)1; // 网络状态
const WPARAM	MSG_HTTPDOWNLOAD_STATUS		= (WPARAM)2; // 下载状态
const WPARAM	MSG_HTTPDOWNLOAD_MAX		= (WPARAM)32; //保留最大可供扩充


// 重试类别
const int	HTTP_RETRY_NONE			= 0;	// 不重试
const int	HTTP_RETRY_TIMES		= 1;	// 重试一定次数
const int	HTTP_RETRY_ALWAYS		= 2;	// 总是重试(有可能死循环)
const int	DEFAULT_HTTP_RETRY_MAX	= 10;	//缺省的重试次数

// PROXY的类型
const int	HTTP_PROXY_NONE			= 0;	// 没有代理
const int	HTTP_PROXY_HTTPGET		= 1;	// 通常的HTTP的GET型代理
const int	HTTP_PROXY_HTTPCONNECT	= 2;	// HTTP CONNECT代理
const int	HTTP_PROXY_SOCKS4		= 3;	// SOCKS4 代理	
const int	HTTP_PROXY_SOCKS4A		= 4;	// SOCKS4A代理
const int	HTTP_PROXY_SOCKS5		= 5;	// SOCKS 5代理
const int	HTTP_PROXY_USEIE		= 6;	// 使用IE配置的代理服务器设置

//下载状态类型
const int	HTTP_STATUS_FILENAME			= 1;	// 文件名
const int	HTTP_STATUS_FILESIZE			= 2;	// 文件大小
const int	HTTP_STATUS_FILEDOWNLOADEDSIZE	= 3;	// 文件已经下载大小
const int	HTTP_STATUS_ERRORCOUNT			= 4;	// 发生错误次数
const int	HTTP_STATUS_ERRORCODE			= 5;	// 错误代码
const int	HTTP_STATUS_ERRORSTRING			= 6;	// 错误原因

// HTTP支持的命令
const int	HTTP_VERB_MIN		= 0;
const int	HTTP_VERB_POST      = 0;
const int	HTTP_VERB_GET       = 1;
const int	HTTP_VERB_HEAD      = 2;
const int	HTTP_VERB_PUT       = 3;
const int	HTTP_VERB_OPTIONS   = 4;
const int	HTTP_VERB_DELETE    = 5;
const int	HTTP_VERB_TRACE	    = 6;
const int	HTTP_VERB_CONNECT   = 7;
const int	HTTP_VERB_MAX		= 7;

////////////////////////////////////////////////////////////////////////////////
//							内部私用
////////////////////////////////////////////////////////////////////////////////
// 缺省超时参数
static const DWORD HTTP_CONN_TIMEOUT	= 60*1000;	// 60秒
static const DWORD HTTP_SEND_TIMEOUT	= 60*1000;	// 60秒
static const DWORD HTTP_RECV_TIMEOUT	= 60*1000;	// 60秒

// 发送请求的结果
static const int	HTTP_REQUEST_SUCCESS		= 0;	// 成功
static const int	HTTP_REQUEST_ERROR			= 1;	// 一般网络错误，可以重试
static const int	HTTP_REQUEST_STOP			= 2;	// 中途停止(用户中断) (不用重试)
static const int	HTTP_REQUEST_FAIL			= 3;	// 失败 (不用重试)
static const int	HTTP_REQUEST_REDIRECT_FTP	= 4;	// 重定向到FTP

// HTTP STATUS CODE分类
static const int	HTTP_SUCCESS		= 0;	// 成功		
static const int	HTTP_REDIRECT		= 1;	// 重定向
static const int	HTTP_FAIL			= 2;	// 失败
static const int	HTTP_REDIRECT_FTP	= 3;	// 重定向到FTP

// 缺省端口号
static const int	DEFAULT_HTTP_PORT	= 80;

// HTTP命令
static const char*	HTTP_VERB_STR[] = 
{
	_T("POST "),
	_T("GET "),
	_T("HEAD "),
	_T("PUT "),
	_T("OPTIONS "),
	_T("DELETE "),
	_T("TRACE "),
	_T("CONNECT ")
};


////////////////////////////////////////////////////////////////////////////////
//						类定义
////////////////////////////////////////////////////////////////////////////////
class CHttpDownload  
{
public:
	CHttpDownload();
	virtual ~CHttpDownload();

public:
	BOOL IsUserStop();
	// 静态函数，用于64编码、解码
	static int Base64Encode(IN LPCTSTR lpszEncoding,OUT CString& strEncoded );
	static int Base64Decode(IN LPCTSTR lpszDecoding,OUT CString& strDecoded );

	// 下载函数
	void SetCookie(LPCTSTR lpszCookie,BOOL bUseIECookie = FALSE);
	void SetAuthorization(LPCTSTR lpszUsername,LPCTSTR lpszPassword,BOOL bAuthorization = TRUE );
	void SetReferer(LPCTSTR lpszReferer);
	void SetUserAgent(LPCTSTR lpszUserAgent);
	void SetTimeout(DWORD dwSendTimeout,DWORD dwRecvTimeout,DWORD dwConnTimeout);
	void SetRetry(int nRetryType,int nRetryDelay=0,int nRetryMax = 0);
	void SetNotifyWnd(HWND hNotifyWnd,int nNotifyMsg,BOOL bNotify = TRUE );
	void SetProxy(LPCTSTR lpszProxyServer,int nProxyPort,BOOL bProxy=TRUE,BOOL bProxyAuthorization = FALSE,LPCTSTR lpszProxyUsername = NULL,LPCTSTR lpszProxyPassword = NULL,int nProxyType = HTTP_PROXY_HTTPGET);
	void StopDownload();
	BOOL ParseURL(IN LPCTSTR lpszURL,OUT CString& strServer,OUT CString& strObject,OUT int& nPort);
	BOOL GetDownloadFileStatus(IN LPCTSTR lpszDownloadUrl,OUT DWORD &dwFileSize,OUT CTime &FileTime);
	int  Download(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSavePath = NULL,BOOL bForceDownload = FALSE,BOOL bSaveResponse = TRUE,int nVerb = HTTP_VERB_GET,LPCTSTR lpszData = NULL,LPTSTR lpszFtpURL = NULL );

private:
	BOOL	CreateSocket();
	void	CloseSocket();
	int		SendRequest(int nVerb = HTTP_VERB_GET,LPTSTR lpszFtpURL = NULL);
	int		GetInfo(IN LPCTSTR lpszHeader,OUT DWORD& dwContentLength,OUT DWORD& dwStatusCode,OUT CTime& TimeLastModified,OUT CString& strCookie,LPTSTR lpszFtpURL);
	CTime	GetTime(LPCTSTR lpszTime);
	void	GetFileName();
	int		MakeConnection(CBufSocket* pBufSocket,CString strServer,int nPort);
	BOOL	GetIEProxy(CString& strProxyServer,int& nProxyPort,int& nProxyType);
	BOOL	GetIECookie(CString& strCookie,LPCTSTR lpszURL);
	BOOL	IsValidFileName(LPCTSTR lpszFileName);
	
private:	
	// 下载参数
	// 待下载URL和本地保存路径
	CString		m_strDownloadUrl;
	CString		m_strSavePath;		// 可以是全路径或保存目录	
	CString		m_strTempSavePath;	//临时文件的路径
	BOOL		m_bSaveResponse;

	// 停止下载
	BOOL		m_bStopDownload;	// 停止下载	
	HANDLE		m_hStopEvent;		// 停止下载事件

	// 强制重新下载(不管已有的文件是否与远程文件相同)
	BOOL		m_bForceDownload;

	// 是否支持断点续传
	BOOL		m_bSupportResume;

	// 文件以及下载大小
	DWORD		m_dwFileSize;				// 文件总的大小	
	DWORD		m_dwFileDownloadedSize;		// 文件总共已经下载的大小

	DWORD		m_dwDownloadSize;			// 本次需要下载的大小
	DWORD		m_dwDownloadedSize;			// 本次已经下载的大小

	DWORD		m_dwHeaderSize;				// 返回头的大小 (暂时没有作用)
	CString		m_strHeader;				// 保存头部信息 (暂时没有作用)

	// 文件日期(远程文件的信息)
	CTime		m_TimeLastModified;

	// Referer
	CString		m_strReferer;
	
	// UserAgent
	CString		m_strUserAgent;

	// 超时TIMEOUT	连接超时、发送超时、接收超时(单位：毫秒)
	DWORD		m_dwConnTimeout;	
	DWORD		m_dwRecvTimeout;
	DWORD		m_dwSendTimeout;

	// 重试机制
	int			m_nRetryType;	//重试类型(0:不重试 1:重试一定次数 2:总是重试)
	int			m_nRetryTimes;	//重试次数
	int			m_nRetryDelay;	//重试延迟(单位：毫秒)
	int			m_nRetryMax;    //重试的最大次数

	// 错误处理
	int			m_nErrorCount;	//错误次数 
	CString		m_strError;		//错误信息 
	DWORD		m_dwErrorCode;	//错误代码

	// 向其他窗口发送消息
	BOOL		m_bNotify;			// 是否向外发送通知消息	
	HWND		m_hNotifyWnd;		// 被通知的窗口
	int			m_nNotifyMessage;	// 被通知的消息

	// 是否进行验证 : Request-Header: Authorization
	BOOL		m_bAuthorization;
	CString		m_strUsername;
	CString		m_strPassword;

	// 是否使用代理 
	BOOL		m_bProxy;
	CString		m_strProxyServer;
	int			m_nProxyPort;
	int			m_nProxyType;
	
	// 代理是否需要验证: Request-Header: Proxy-Authorization
	BOOL		m_bProxyAuthorization;
	CString		m_strProxyUsername;
	CString		m_strProxyPassword;

	// 下载过程中所用的变量
	CString		m_strServer;
	CString		m_strObject;
	CString		m_strFileName;
	int			m_nPort;
	int			m_nVerb;
	CString		m_strData;// 请求时发送的数据

	// 是否使用Cookie
	CString		m_strCookie;
	BOOL		m_bUseIECookie;

	CBufSocket	m_cBufSocket;// 下载连接的SOCKET
	
	// 用于BASE64编码、解码
	static int		m_nBase64Mask[];
	static CString	m_strBase64TAB;
};

#endif // !defined(AFX_HTTPDOWNLOAD_H__4E55FDB1_0DE8_4ACD_94F4_CD097B5EAED0__INCLUDED_)
