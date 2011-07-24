
///////////////////////////////////////////////////////////////////////////////
// FtpDownload.cpp: implementation of the CFtpDownload class.
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocksPacket.h"
#include "FtpDownload.h"
#include "Resource.h"

#define READ_BUFFER_SIZE 10240
#define DEFAULT_SAVE_DIR _T("C:\\")
#define Check(a,b) if((a)) return (b);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpDownload::CFtpDownload()
{
	m_strDownloadUrl	= _T("");
	m_strSavePath		= _T("");		// 可以是全路径或保存目录	
	m_strTempSavePath	= _T("");		//临时文件的路径

	// 停止下载
	m_bStopDownload		= FALSE;
	m_hStopEvent		= NULL;

	// 强制重新下载(不管已有的文件是否与远程文件相同)
	m_bForceDownload	= FALSE;

	// 是否支持断点续传
	m_bSupportResume	= FALSE;

	// PASV方式
	m_bPasv				= TRUE;

	// 文件以及下载大小
	m_dwFileSize			= 0;		// 文件总的大小	
	m_dwFileDownloadedSize	= 0;		// 文件总共已经下载的大小

	m_dwDownloadSize	= 0;			// 本次需要下载的大小
	m_dwDownloadedSize	= 0;			// 本次已经下载的大小


	// 超时TIMEOUT	连接超时、发送超时、接收超时(单位：毫秒)
	m_dwConnTimeout	= FTP_CONN_TIMEOUT;	
	m_dwRecvTimeout	= FTP_RECV_TIMEOUT;
	m_dwSendTimeout		= FTP_SEND_TIMEOUT;

	// 重试机制
	m_nRetryType		= FTP_RETRY_NONE;	//重试类型(0:不重试 1:重试一定次数 2:总是重试)
	m_nRetryTimes		= 0;				//重试次数
	m_nRetryDelay		= 0;				//重试延迟(单位：毫秒)
	m_nRetryMax			= 0;				//重试的最大次数

	// 错误处理
	m_nErrorCount		= 0;				//错误次数 (暂时没有作用)
	m_strError			= _T("");			//错误信息 (暂时没有作用)


	// 向其他窗口发送消息
	m_bNotify			= FALSE;			// 是否向外发送通知消息	
	m_hNotifyWnd		= NULL;				// 被通知的窗口
	m_nNotifyMessage	= 0;				// 被通知的消息
    
	// 是否使用代理 
	m_bProxy			= FALSE;
	m_strProxyServer	= _T("");
	m_nProxyPort		= 0;
	m_nProxyType		= FTP_PROXY_NONE;
	
	// 代理是否需要验证
	m_bProxyAuthorization	= FALSE;
	m_strProxyUsername		= _T("");
	m_strProxyPassword		= _T("");

	// FTP用户和口令
	m_bAuthorization		= FALSE;
	m_strUsername			= _T("anonymous");
	m_strPassword			= _T("mail@unknow.com");

	// 下载过程中所用的变量
	m_strServer			= _T("");
	m_strObject			= _T("");
	m_strFileName		= _T("");
	m_nPort				= DEFAULT_FTP_PORT;

}

CFtpDownload::~CFtpDownload()
{
	CloseControlSocket();
	CloseDataSocket();
}

//////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetIEProxy(
//				CString &strProxyServer,
//				int &nProxyPort,
//				int &nProxyType ) 
//	用  途：获取IE的Proxy设置
//	对全局变量的影响：无
//	参  数：
//		strProxyServer : 代理服务器地址
//		nProxyPort     : 代理服务器的端口
//		nProxyType     : 代理服务器的类型
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CFtpDownload::GetIEProxy(CString &strProxyServer, int &nProxyPort, int &nProxyType)
{
    // 读取注册表
    HKEY hRegKey;
    DWORD dwRet = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
        0L,
        KEY_QUERY_VALUE,&hRegKey
    );
    if (dwRet != ERROR_SUCCESS)
        return FALSE;
    
    TCHAR szAddr[256]	= { 0 };
    DWORD dwLen			= 256;
    dwRet = RegQueryValueEx(hRegKey,"ProxyServer",NULL,NULL,(LPBYTE)szAddr,&dwLen);
    if (dwRet != ERROR_SUCCESS)
    {
        RegCloseKey(hRegKey);
        return FALSE;
    }
    RegCloseKey(hRegKey);
    
    // 注：可能存在所有代理同一的情况，此时无 http=...ftp=...信息
    //    comment by Linsuyi, 2002/01/22  23:30
    // 
    // 分析Proxy的设置
    //http=193.168.10.1:80;socks=192.168.10.235:1080
    //193.168.10.1:1090
    //ftp=193.168.10.1:1090;gopher=193.168.10.1:1090;https=193.168.10.1:1090;socks=193.168.10.2:1080
    int nPos = -1;
    CString strProxy = szAddr;
    
    if (strProxy.IsEmpty())
        return FALSE;
    
    strProxy.MakeLower();
    nProxyType = FTP_PROXY_NONE;
       
    nPos = strProxy.Find("socks=");
    if (-1 == nPos)
        return FALSE;
    
    strProxy = strProxy.Mid( nPos+strlen("socks="));
    nProxyType = FTP_PROXY_SOCKS5;
    
    nPos = strProxy.Find(";");
    if (nPos != -1)
    	strProxy = strProxy.Left(nPos);
    
    nPos = strProxy.Find(":");
    strProxyServer = strProxy.Left(nPos);
    nProxyPort = _ttoi(strProxy.Mid(nPos+1));
    if (nProxyPort <= 0)
        return FALSE;
    
    return TRUE;
}

// 设置代理
void CFtpDownload::SetProxy(LPCTSTR lpszProxyServer,int nProxyPort, BOOL bProxy/*=TRUE*/, BOOL bProxyAuthorization /*=FALSE*/, LPCTSTR lpszProxyUsername /*=NULL*/, LPCTSTR lpszProxyPassword /*=NULL*/, int nProxyType /*=FTP_PROXY_NONE*/)
{
    if (bProxy && (FTP_PROXY_USEIE == nProxyType))
    {
        if (!GetIEProxy(m_strProxyServer,m_nProxyPort,m_nProxyType))
        {
            m_bProxy                = FALSE;
            m_bProxyAuthorization   = FALSE;
            m_nProxyPort            = 0;
            m_nProxyType            = FTP_PROXY_NONE;
            m_strProxyServer        = _T("");
            m_strProxyUsername      = _T("");
            m_strProxyPassword      = _T("");
        }
        else
        {
            m_bProxy                = TRUE;
            
            if (lpszProxyUsername != NULL)
            {
                m_bProxyAuthorization   = TRUE;
                m_strProxyUsername      = lpszProxyUsername;
                m_strProxyPassword      = lpszProxyPassword;
            }
            else
            {
                m_bProxyAuthorization   = FALSE;
                m_strProxyUsername      = _T("");
                m_strProxyPassword      = _T("");
            }
        }
    }
    else if (bProxy && (lpszProxyServer != NULL) && (nProxyPort != 0))
    {
        m_bProxy			= TRUE;
        m_strProxyServer	= lpszProxyServer;
        m_nProxyPort		= nProxyPort;
        m_nProxyType		= nProxyType;
        
        if (bProxyAuthorization && lpszProxyUsername != NULL)
        {
            m_bProxyAuthorization	= TRUE;
            m_strProxyUsername		= lpszProxyUsername;
            m_strProxyPassword		= lpszProxyPassword;
        }
        else
        {
            m_bProxyAuthorization	= FALSE;
            m_strProxyUsername		= _T("");
            m_strProxyPassword		= _T("");
        }
    }
    else
    {
        m_bProxy				= FALSE;
        m_bProxyAuthorization	= FALSE;
        m_nProxyPort			= 0;
        
        m_nProxyType			= FTP_PROXY_NONE;
        m_strProxyServer		= _T("");
        m_strProxyUsername		= _T("");
        m_strProxyPassword		= _T("");
    }
}

// 设置是否需要发送消息给调用窗口
void CFtpDownload::SetNotifyWnd(HWND hNotifyWnd, int nNotifyMsg, BOOL bNotify)
{
    if (bNotify && (hNotifyWnd != NULL) && ::IsWindow(hNotifyWnd) )
    {
        m_bNotify	 = TRUE;
        m_hNotifyWnd = hNotifyWnd;
        m_nNotifyMessage = nNotifyMsg;
    }
    else
    {
        m_bNotify	 = FALSE;
        m_hNotifyWnd = NULL;
        m_nNotifyMessage = 0;
    }
}

// 设置超时
void CFtpDownload::SetTimeout(DWORD dwSendTimeout, DWORD dwRecvTimeout, DWORD dwConnTimeout)
{
	if( dwSendTimeout > 0 )
		m_dwSendTimeout = dwSendTimeout;
	
	if( dwRecvTimeout > 0 )
		m_dwRecvTimeout = dwRecvTimeout;

	if( dwConnTimeout > 0 )
		m_dwConnTimeout = dwConnTimeout;


}
// 设置重试的机制
// nRetryType = 0  不重试					FTP_RETRY_NONE
// nRetryType = 1  重试一定次数				FTP_RETRY_TIMES
// nRetryType = 2  永远重试(可能陷入死循环)	FTP_RETRY_ALWAYS
void CFtpDownload::SetRetry(int nRetryType, int nRetryDelay, int nRetryMax)
{
	m_nRetryType  = nRetryType;
	m_nRetryDelay = nRetryDelay;
	m_nRetryMax	  = nRetryMax;
	
	// 检查一下m_nRetryMax,如果为0，设为缺省值
	if( (FTP_RETRY_TIMES == m_nRetryType) && (0 == m_nRetryMax) )
		m_nRetryMax = DEFAULT_FTP_RETRY_MAX;
}

// 设置用户名和口号
void CFtpDownload::SetAuthorization(LPCTSTR lpszUsername, LPCTSTR lpszPassword, BOOL byAuthorization)
{
	if( byAuthorization && lpszUsername != NULL )
	{
		m_bAuthorization = TRUE;
		m_strUsername	 = lpszUsername;
		m_strPassword	 = lpszPassword;
	}
	else
	{
		m_bAuthorization = FALSE;
		m_strUsername	 = _T("anonymous");
		m_strPassword	 = _T("hello@dont.know.com");
	}
}

// 设置PASV模式
void CFtpDownload::SetPasv(BOOL bPasv /*= TRUE*/)
{
	m_bPasv = bPasv;
}

// 停止下载
void CFtpDownload::StopDownload()
{
	m_bStopDownload = TRUE;

	if ( m_hStopEvent != NULL )
		SetEvent(m_hStopEvent);
}


// 创建控制SOCKET
BOOL CFtpDownload::CreateControlSocket()
{
	CloseControlSocket();
	return m_cControlSocket.Create(AF_INET,SOCK_STREAM,0,READ_BUFFER_SIZE);
}

// 创建数据SOCKET
BOOL CFtpDownload::CreateDataSocket( SOCKET hSocket /*= INVALID_SOCKET */)
{
	CloseDataSocket();
	return m_cDataSocket.Create(AF_INET,SOCK_STREAM,0,READ_BUFFER_SIZE,hSocket);
}


// 关闭控制SOCKET
void CFtpDownload::CloseControlSocket()
{
	m_cControlSocket.Close(TRUE);
}

// 关闭数据SOCKET
void CFtpDownload::CloseDataSocket()
{
	m_cDataSocket.Close(TRUE);
}


// 从URL里面拆分出Server和Object来
BOOL CFtpDownload::ParseURL(LPCTSTR lpszURL, CString &strServer, CString &strObject,int& nPort)
{
	CString strURL(lpszURL);
	strURL.TrimLeft();
	strURL.TrimRight();
	
	// 清除数据
	strServer = _T("");
	strObject = _T("");
	nPort	  = 0;

	int nPos = strURL.Find("://");
	if( nPos == -1 )
		return FALSE;

	// 进一步验证是否为Ftp://
	CString strTemp = strURL.Left( nPos+lstrlen("://") );
	//strTemp.MakeLower();
	//if( strTemp.Compare("ftp://") != 0 )
    if(strTemp.CompareNoCase("ftp://") != 0)
		return FALSE;

	strURL = strURL.Mid( strTemp.GetLength() );
	nPos = strURL.Find('/');
	if ( nPos == -1 )
		return FALSE;

	strObject = strURL.Mid(nPos);
	strTemp   = strURL.Left(nPos);
	
	// 查找是否有端口号
	nPos = strTemp.Find(":");
	if( nPos == -1 )
	{
		strServer = strTemp;
		nPort	  = DEFAULT_FTP_PORT;
	}
	else
	{
		strServer = strTemp.Left( nPos );
		strTemp	  = strTemp.Mid( nPos+1 );
		nPort	  = (int)_ttoi((LPCTSTR)strTemp);
	}
	return TRUE;
}


// 从下载URL中获取文件名
void CFtpDownload::GetFileName()
{
	// 获取的文件名
	int nSlash = m_strObject.ReverseFind(_T('/'));
	if (nSlash == -1)
		nSlash = m_strObject.ReverseFind(_T('\\'));

	if (nSlash != -1 && m_strObject.GetLength() > 1)
		m_strFileName = m_strObject.Right(m_strObject.GetLength() - nSlash - 1);
	else
		m_strFileName = m_strObject;
}


// 将字符串转化成时间
CTime CFtpDownload::GetTime(LPCTSTR lpszTime)
{
	CString strTime(lpszTime);
	CTime	dTime = CTime::GetCurrentTime();

	int nPos = strTime.FindOneOf(" \t");
	if( nPos == -1 )
		return dTime;

	CString strAllMonth = "jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec";
	CString strDay,strMonth,strYear,strHour,strMinute;
	int		nDay,nMonth,nYear,nHour,nMinute,nSecond;


	strMonth = strTime.Left( nPos );
	strTime = strTime.Mid( nPos +1 );
	strTime.TrimLeft();

	nPos = strAllMonth.Find(strMonth);
	if( nPos == -1 )
		return dTime;
	strMonth.Format("%d",((nPos/4)+1));	
	nMonth = atoi(strMonth);

	nPos = strTime.FindOneOf(" \t");
	if( nPos == -1 )
		return dTime;
	strDay = strTime.Left( nPos );
	nDay = atoi(strDay);

	strTime = strTime.Mid( nPos+1 );
	strTime.TrimLeft();

	nPos = strTime.FindOneOf(":");
	if (nPos != -1)
	{
		strHour = strTime.Left( nPos );
		nHour = atoi(strHour);
	
		strTime = strTime.Mid( nPos+1 );
		strTime.TrimLeft();

		nPos = strTime.FindOneOf(":");
		if( nPos != -1 )
		{
			strMinute = strTime.Left( nPos );
			nMinute = atoi(strMinute);

			strTime = strTime.Mid( nPos+1 );
			strTime.TrimLeft();

			nSecond = atoi(strTime);

		}
		else
		{
			nMinute = atoi(strTime);
			nSecond = 0;
		}


	}
	else
	{
		nHour	= 0;
		nMinute = 0;
		nSecond = 0;
	}
	nYear = dTime.GetYear();

	dTime = CTime(nYear,nMonth,nDay,nHour,nMinute,nSecond);
	return	dTime;
}

// 获得返回码
BOOL CFtpDownload::GetReplyCode(LPCTSTR lpszResponse,DWORD& dwReplyCode,BOOL& bMultiLine)
{
	CString strSource(lpszResponse);

	dwReplyCode = 0;
	bMultiLine = FALSE;
	
	if( strSource.GetLength() < 3 )
		return FALSE;

	dwReplyCode = (DWORD)_ttoi(strSource.Left(3));

	if( strSource.GetLength() >= 4 )
		bMultiLine = (strSource.GetAt(3) == '-' );
	else
		bMultiLine = FALSE;

	return TRUE;
}

// 获取数据通道的信息
BOOL CFtpDownload::GetDataReply(CString& strReply)
{
	char	szReadBuf[1025];
	strReply = _T("");

	// 接收信息
	ZeroMemory(szReadBuf,1025);
	if( m_cDataSocket.BSDGetString(szReadBuf,1024,m_dwRecvTimeout) != SOCKET_SUCCESS )
		return FALSE;

	strReply += szReadBuf;

	return TRUE;
}



// 获得Reply信息
BOOL CFtpDownload::GetReply(CString& strReply,DWORD& dwReplyCode)
{
	char	szReadBuf[1025];
	CString strResponse;
	DWORD	dwStartCode;
	BOOL	bMultiLine;
	
	strReply = _T("");
	dwReplyCode = 0;

	// 先接收一行
	ZeroMemory(szReadBuf,1025);
	if( m_cControlSocket.BSDGetString(szReadBuf,1024,m_dwRecvTimeout) != SOCKET_SUCCESS )
		return FALSE;

	strReply += szReadBuf;

	if( !GetReplyCode(szReadBuf,dwReplyCode,bMultiLine) )
		return FALSE;

	if( !bMultiLine )
		return TRUE;

	dwStartCode = dwReplyCode;
	while(TRUE)
	{
		ZeroMemory(szReadBuf,1025);
		if( m_cControlSocket.BSDGetString(szReadBuf,1024,m_dwRecvTimeout) != SOCKET_SUCCESS )
			return FALSE;

		strReply += szReadBuf;		
		strReply += "\r\n";

		if( !GetReplyCode(szReadBuf,dwReplyCode,bMultiLine) )
			return FALSE;

		if( dwStartCode == dwReplyCode && !bMultiLine )
			break;
	}
	return TRUE;
}


// 发送FTP命令
BOOL CFtpDownload::SendCommand(LPCTSTR lpszCmd,LPCTSTR lpszParameter)
{
	CString strCmd(lpszCmd);
	
	if( lpszParameter != NULL && lstrlen(lpszParameter)>0 )
	{
		strCmd += " ";
		strCmd += lpszParameter;
	}
	strCmd += "\r\n";
 
	if( m_cControlSocket.Send(strCmd,strCmd.GetLength(),m_dwSendTimeout) != strCmd.GetLength() )
		return FALSE;

	return TRUE;

}

// 从Pasv的返回信息中获得IP和端口
BOOL CFtpDownload::GetPasvIP(LPCTSTR lpszReply,CString& strPasvIP,int& nPasvPort)
{
	CString strReply(lpszReply);
	CString strPort1,strPort2;
	
	strPasvIP = _T("");
	nPasvPort = 0;

	int nPos = strReply.Find('(');
	if( nPos == -1 )
		return FALSE;

	strPasvIP = strReply.Mid( nPos + 1 );
	nPos = strPasvIP.Find(')');
	if( nPos == -1 )
		return FALSE;

	strPasvIP = strPasvIP.Left(nPos);
	
	nPos = strPasvIP.ReverseFind(',');
	if( nPos == -1)
		return FALSE;
	
	strPort2 = strPasvIP.Right(strPasvIP.GetLength() - nPos -1 );
	strPasvIP = strPasvIP.Left(nPos);

	nPos = strPasvIP.ReverseFind(',');
	if( nPos == -1 )
		return FALSE;

	strPort1 = strPasvIP.Right(strPasvIP.GetLength() - nPos -1 );
	strPasvIP = strPasvIP.Left(nPos);

	strPasvIP.Replace(',','.');
	nPasvPort = (int) ( atoi(strPort1)*256+atoi(strPort2) );
	return TRUE;
}

// 获取文件时间和大小
BOOL CFtpDownload::GetInfo(LPCTSTR lpszReply,DWORD& dwSize,CTime& TimeLastModified)
{
	CString strReply(lpszReply);
	
	dwSize = 0;
	TimeLastModified = CTime::GetCurrentTime();
	
	//-rwxr-xr-x   1 ftpuser  ftpusers  14712179 Sep 13 09:55 csmproxy50nt.exe
	//-r--r--r--  1 root  wheel  454454 Oct  5 10:54 /pub/socks/dante-1.1.4.tar.gz
	//-r-xr-xr-x   1 0        0         986400 Jun 16  1998 /pub/Networks/Socket/Winsock/ws2setup.exe

	strReply.TrimLeft();
	strReply.TrimRight();
	strReply.MakeLower();
	
	int nCount = 0;
	int nPos ;
	while( nCount < 4 )
	{	
		nPos = strReply.FindOneOf(" \t");
		if( nPos == -1 )
			return FALSE;

		strReply = strReply.Mid(nPos+1);
		strReply.TrimLeft();
	
		nCount ++;
	}

	nPos = strReply.FindOneOf(" \t");
	if( nPos == -1 )
		return FALSE;
	
	CString strSize = strReply.Left(nPos);
	dwSize = (DWORD)_ttol(strSize);

	strReply = strReply.Mid(nPos+1);
	strReply.TrimLeft();

	int nPos1 = strReply.ReverseFind(' ');
	int nPos2 = strReply.ReverseFind('\t');
	
	if( nPos1 != -1 )
	{
		if( nPos2 != -1 )
		{
			nPos = ( nPos1 <= nPos2 )? nPos1:nPos2;
		}
		else
		{
			nPos = nPos1;
		}
	}
	else
	{
		if( nPos2 != -1 )
		{
			nPos = nPos2;
		}
		else
		{
			return FALSE;
		}
	}

	strReply = strReply.Left(nPos);
	strReply.TrimRight();

	TimeLastModified = GetTime(strReply);
	return TRUE;
}


// 发送请求
int CFtpDownload::SendRequest(BOOL bPasv /*= TRUE*/)
{
	
	while (TRUE)
	{
		CString			strSend, strReply;
		DWORD			dwSize, dwReplyCode;
		
		// 用于保存PASV或PORT的参数
		CString			strIP;
		int				nPort;
		CString         sMes;

        strIP = _T("");
		nPort = 0;


		// 非PASV 
		CBufSocket	cListenSocket;

		m_dwFileDownloadedSize = 0;
		m_dwDownloadSize	   = 0;

		// 创建控制套接字
		Check( !CreateControlSocket(), FTP_REQUEST_FAIL);
		
		// 建立控制通道
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		int nRet = MakeConnection(&m_cControlSocket,m_strServer,m_nPort);
		Check( ( nRet != FTP_REQUEST_SUCCESS ), nRet );
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);// 先获取服务器的Greeting信息
		//seawind
		Check( m_bStopDownload,FTP_REQUEST_STOP);
        
        //Removed by linsuyi 2001/01/31 16:32
        // sMes.LoadString(IDS_CREATE_CHANNELS_SUCCESS);
        // sMes += strReply;
        // TRACE1("%s ", sMes);
        
		// 开始发送命令
		// USER
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !SendCommand(FTP_CMD_USER,m_strUsername) ,FTP_REQUEST_ERROR );
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR );
		//seawind
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		
        //Removed by linsuyi 2001/01/31 16:32
        // sMes.Empty();
        // sMes.LoadString(IDS_USER_COMMAND_SUCCESS);
        // sMes += strReply;
		// TRACE1("%s ", sMes);

		switch(dwReplyCode)
		{
		case 230:
			break;
		case 331: // Need Password
			// PASS
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !SendCommand(FTP_CMD_PASS,m_strPassword),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
			//seawind
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( dwReplyCode != 230 ,FTP_REQUEST_FAIL);
			
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_PASS_COMMAND_SUCCESS);
            // sMes += strReply;
			// TRACE1("%s ", sMes);

			break;
		default:
			return FTP_REQUEST_FAIL;
			break;
		}

		// 测试是否支持断点续传
		// REST 10
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !SendCommand(FTP_CMD_REST,_T("10")),FTP_REQUEST_ERROR);
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
		//seawind
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		if( dwReplyCode == 350 )
			m_bSupportResume = TRUE;
		else
			m_bSupportResume = FALSE;
        
        //Removed by linsuyi 2001/01/31 16:32
        // sMes.LoadString(IDS_REST10_COMMAND_SUCCESS);
        // sMes += strReply;
        // TRACE1("%s ", sMes);
        
		// 字符模式
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !SendCommand(FTP_CMD_TYPE,_T("A")),FTP_REQUEST_ERROR);
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
		//seawind
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( dwReplyCode != 200 ,FTP_REQUEST_FAIL);	// 必须至少支持这种模式

        //Removed by linsuyi 2001/01/31 16:32
        // sMes.Empty();
        // sMes.LoadString(IDS_TYPEA_COMMAND_SUCCESS);
        // sMes += strReply;
        // TRACE1("%s ", sMes);

		// 是否支持PASV
		if( bPasv )
		{
			// PASV
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !SendCommand(FTP_CMD_PASV,NULL),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
			//seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
	        
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_PASV_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
			
			if( dwReplyCode == 227 )
			{
				if( GetPasvIP(strReply,strIP,nPort) )
				{
					m_bPasv = TRUE;
					// 建立数据通道
					Check( m_bStopDownload,FTP_REQUEST_STOP);
					Check( !CreateDataSocket(),FTP_REQUEST_FAIL);
					nRet = MakeConnection(&m_cDataSocket,strIP,nPort);
					Check( nRet != FTP_REQUEST_SUCCESS,nRet);
				}
				else
					m_bPasv = FALSE;
			}
			else
				m_bPasv = FALSE;
		}

		dwSize = 0;
		m_TimeLastModified = CTime::GetCurrentTime();

        // 根据是否采用PASV，而有很大区别
        if (bPasv && m_bPasv)   // PASV
        {
            // 执行LIST命令
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_LIST, m_strObject ),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_LIST_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
            if ((dwReplyCode == 125) || ( dwReplyCode == 150 ))
            {
                //从数据通道读取信息
                Check(m_bStopDownload, FTP_REQUEST_STOP);
                if (GetDataReply(strReply))
                {
                    //Removed by linsuyi 2001/01/31 16:32
                    // sMes.Empty();
                    // sMes.LoadString(IDS_LIST_DATA_CHANNELS);
                    // sMes += strReply;
                    // TRACE1("%s ", sMes);
                    
                    if (!GetInfo(strReply,dwSize,m_TimeLastModified))
                    {
                        // TODO:
                    }
                }
                CloseDataSocket();
                
                //从控制通道读取信息
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
                //seawind
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( dwReplyCode != 250 && dwReplyCode != 226,FTP_REQUEST_ERROR);
                
                //Removed by linsuyi 2001/01/31 16:32
                // sMes.Empty();
                // sMes.LoadString(IDS_LIST_CONTROL_CHANNELS);
                // sMes += strReply;
                // TRACE1("%s ", sMes);
                
            }   //125.150 -- LIST COMMAND [PASV]
            
            //Bin模式
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_TYPE,_T("I")),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_TYPEI_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
                        
            if( dwReplyCode != 200 )
            {
                // TODO:
            }
            
            // PASV命令
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_PASV,NULL ),FTP_REQUEST_ERROR);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( dwReplyCode != 227,FTP_REQUEST_ERROR);
            Check( !GetPasvIP(strReply,strIP,nPort),FTP_REQUEST_ERROR);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_PASV_PASV_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
            // 建立数据通道
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !CreateDataSocket(),FTP_REQUEST_FAIL);
            nRet = MakeConnection(&m_cDataSocket,strIP,nPort);
            Check( nRet != FTP_REQUEST_SUCCESS,nRet);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_PASV_CREATE_DATA_CHANNELS_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
        }
        else    // 不采用PASV
        {
            
            char		szListenIP[128],szTemp[128];
            int			nListenPort;
            DWORD		dwListenIP;
            CString		strListenIP;
            
            // 是否为SOCKS代理
            if( m_nProxyType == FTP_PROXY_SOCKS4 ||
                m_nProxyType == FTP_PROXY_SOCKS4A||
                m_nProxyType == FTP_PROXY_SOCKS5 )
            {
                if( !CreateDataSocket() )
                    return FTP_REQUEST_FAIL;
                
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                nRet = MakeConnection( &m_cDataSocket,m_strServer,m_nPort,TRUE,&dwListenIP,&nListenPort);
                Check( nRet != FTP_REQUEST_SUCCESS,nRet);
                strListenIP.Format("%d,%d,%d,%d",(dwListenIP>>24)&0xFF,(dwListenIP>>16)&0xFF,(dwListenIP>>8)&0xFF,(dwListenIP)&0xFF);
            }
            else
            {
                //没有代理
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( !cListenSocket.Create(),FTP_REQUEST_FAIL);
                Check( !cListenSocket.Bind(0), FTP_REQUEST_ERROR);
                Check( !cListenSocket.Listen(1),FTP_REQUEST_ERROR);
                
                ZeroMemory(szListenIP,128);
                Check( !m_cControlSocket.GetSockName(szListenIP,nListenPort),FTP_REQUEST_ERROR);
                
                ZeroMemory(szTemp,128);
                Check( !cListenSocket.GetSockName(szTemp,nListenPort),FTP_REQUEST_ERROR);
                
                strListenIP =  _T("");
                strListenIP += szListenIP;
                strListenIP.Replace('.',',');
            }
            
            strSend.Format(",%d,%d",nListenPort/256,nListenPort%256);
            strSend = strListenIP+strSend;
            
            // PORT
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_PORT,strSend ),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( dwReplyCode != 200, FTP_REQUEST_FAIL );	// 必须支持该命令
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_NOPASV_PORT_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
            // 执行LIST命令
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_LIST, m_strObject),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_NOPASV_LIST_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
            if( (dwReplyCode == 125) || ( dwReplyCode == 150 ) )
            {
                if( m_nProxyType == FTP_PROXY_SOCKS4 ||
                    m_nProxyType == FTP_PROXY_SOCKS4A||
                    m_nProxyType == FTP_PROXY_SOCKS5 )
                {
                    CSocksPacket cSocks( &m_cDataSocket );
                    
                    switch ( m_nProxyType )
                    {
                    case FTP_PROXY_SOCKS4A:
                        Check( !cSocks.RecvPacket(PACKET_SOCKS4AREP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
                        Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
                        break;
                    case FTP_PROXY_SOCKS4:
                        Check( !cSocks.RecvPacket(PACKET_SOCKS4REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
                        Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
                        break;
                    case FTP_PROXY_SOCKS5:
                        Check( !cSocks.RecvPacket(PACKET_SOCKS5REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
                        Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
                        break;
                    }
                }
                else
                {
                    // ACCEPT
                    SOCKET hSocket = cListenSocket.Accept(NULL,0,m_hStopEvent,WSA_INFINITE);
                    Check( hSocket == INVALID_SOCKET,FTP_REQUEST_ERROR);	
                    Check( !CreateDataSocket(hSocket),FTP_REQUEST_FAIL);	
                }
                
                // 从数据通道读取信息
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                if (GetDataReply(strReply))
                {
                    //Removed by linsuyi 2001/01/31 16:32
                    // sMes.Empty();
                    // sMes.LoadString(IDS_NOPASV_LIST_DATA_CHANNELS);
                    // sMes += strReply;
                    // TRACE1("%s ", sMes);
                    
                    if( !GetInfo(strReply,dwSize,m_TimeLastModified) )
                    {
                        // TODO:
                    }
                }
                CloseDataSocket();
                
                // 从控制通道读取信息
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
                //seawind
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( dwReplyCode != 250 && dwReplyCode != 226,FTP_REQUEST_ERROR);
                
                //Removed by linsuyi 2001/01/31 16:32
                // sMes.Empty();
                // sMes.LoadString(IDS_NOPASV_LIST_CONTROL_CHANNELS);
                // sMes += strReply;
                // TRACE1("%s ", sMes);
            } // LIST
            
            // Bin模式
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_TYPE,_T("I")),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_NOPASV_TYPEI_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
            if (dwReplyCode != 200)
            {
                // TODO:
            }
            
            // PORT命令
            if (
                (m_nProxyType == FTP_PROXY_SOCKS4) ||
                (m_nProxyType == FTP_PROXY_SOCKS4A) ||
                (m_nProxyType == FTP_PROXY_SOCKS5)
            )
            {
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( !CreateDataSocket(),FTP_REQUEST_FAIL);
                nRet = MakeConnection( &m_cDataSocket,m_strServer,m_nPort,TRUE,&dwListenIP,&nListenPort);
                Check( nRet != FTP_REQUEST_SUCCESS,nRet);
                strListenIP.Format("%d,%d,%d,%d",(dwListenIP>>24)&0xFF,(dwListenIP>>16)&0xFF,(dwListenIP>>8)&0xFF,(dwListenIP)&0xFF);
            }
            else
            {
                Check( m_bStopDownload,FTP_REQUEST_STOP);
                Check( !cListenSocket.Create(),FTP_REQUEST_FAIL);
                Check( !cListenSocket.Bind(0),FTP_REQUEST_ERROR);
                Check( !cListenSocket.Listen(1),FTP_REQUEST_ERROR);
                
                ZeroMemory(szListenIP,128);
                Check( !m_cControlSocket.GetSockName(szListenIP,nListenPort),FTP_REQUEST_ERROR);
                
                ZeroMemory(szTemp,128);
                Check( !cListenSocket.GetSockName(szTemp,nListenPort),FTP_REQUEST_ERROR);
                
                strListenIP = _T("");
                strListenIP += szListenIP;
                strListenIP.Replace('.',',');
            }
            
            strSend.Format(",%d,%d",nListenPort/256,nListenPort%256);
            strSend = strListenIP+strSend;
            
            // PORT
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !SendCommand(FTP_CMD_PORT,strSend ),FTP_REQUEST_ERROR);
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
            //seawind
            Check( m_bStopDownload,FTP_REQUEST_STOP);
            Check( dwReplyCode != 200, FTP_REQUEST_FAIL);// 必须支持该命令
            
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_NOPASV_PORT_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
        }   // NO PASV
        
        // 准备下载
        if (dwSize == 0)
        {
			// 试一试SIZE命令
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !SendCommand(FTP_CMD_SIZE, m_strObject ),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
			//seawind
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			
            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_SIZE_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
            
			if( dwReplyCode == 213 )
			{
				strReply.TrimLeft();
				strReply.TrimRight();
				int nPos = strReply.Find(' ');
				if( nPos != -1 )
				{
					strReply = strReply.Mid( nPos + 1 );
					strReply.TrimLeft();
					dwSize = (DWORD)_ttol(strReply);
				}
			}
		}
		
		// 判断m_strSavePath是否为路径
		GetFileName();
		if( m_strSavePath.Right(1) == '\\' )
		{
			m_strTempSavePath = m_strSavePath;
			m_strTempSavePath += m_strFileName;
			m_strTempSavePath += ".tmp";
		}

		CString		strRange;
		m_dwFileDownloadedSize = 0;
		if( m_bSupportResume )
		{
			// 查看文件已经下载的长度
			CFileStatus fileDownStatus;
			if (CFile::GetStatus(m_strTempSavePath,fileDownStatus) && !m_bForceDownload )
				m_dwFileDownloadedSize = fileDownStatus.m_size;

			strRange.Format(_T("%d"),m_dwFileDownloadedSize );

			// REST COMMAND
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !SendCommand(FTP_CMD_REST,strRange ),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
			//seawind
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( dwReplyCode != 350,FTP_REQUEST_ERROR);

            //Removed by linsuyi 2001/01/31 16:32
            // sMes.Empty();
            // sMes.LoadString(IDS_REST_COMMAND_SUCCESS);
            // sMes += strReply;
            // TRACE1("%s ", sMes);
		}

		// 注意
		m_dwFileSize	 = dwSize;
		m_dwDownloadSize = dwSize - m_dwFileDownloadedSize;
		Check( m_dwDownloadSize < 0,FTP_REQUEST_FAIL);
		
		// 发送RETR命令
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !SendCommand(FTP_CMD_RETR,m_strObject ),FTP_REQUEST_ERROR);
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !GetReply(strReply,dwReplyCode),FTP_REQUEST_ERROR);
		//seawind
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( (dwReplyCode != 125 && dwReplyCode != 150 && dwReplyCode != 110),FTP_REQUEST_FAIL);
        
        //Removed by linsuyi 2001/01/31 16:32
        // sMes.Empty();
        // sMes.LoadString(IDS_RETR_COMMAND_SUCCESS);
        // sMes += strReply;
        // TRACE1("%s ", sMes);
        
        // 非PASV方式要创建数据通道
		if( !bPasv || !m_bPasv )
		{
			if( m_nProxyType == FTP_PROXY_SOCKS4 ||
				m_nProxyType == FTP_PROXY_SOCKS4A||
				m_nProxyType == FTP_PROXY_SOCKS5 )
			{
				CSocksPacket cSocks( &m_cDataSocket );
				switch ( m_nProxyType )
				{
				case FTP_PROXY_SOCKS4A:
					Check( m_bStopDownload,FTP_REQUEST_STOP);
					Check( !cSocks.RecvPacket(PACKET_SOCKS4AREP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
					Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
					break;
				case FTP_PROXY_SOCKS4:
					Check( m_bStopDownload,FTP_REQUEST_STOP);
					Check( !cSocks.RecvPacket(PACKET_SOCKS4REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
					Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
					break;
				case FTP_PROXY_SOCKS5:
					Check( m_bStopDownload,FTP_REQUEST_STOP);
					Check( !cSocks.RecvPacket(PACKET_SOCKS5REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
					Check( !cSocks.IsSocksOK(),FTP_REQUEST_FAIL);
					break;
				}
			}
			else
			{
				Check( m_bStopDownload,FTP_REQUEST_STOP);
				SOCKET hSocket = cListenSocket.Accept(NULL,0,m_hStopEvent,WSA_INFINITE);
				Check( hSocket == INVALID_SOCKET,FTP_REQUEST_ERROR);	
				Check( !CreateDataSocket(hSocket),FTP_REQUEST_FAIL);	
			}
		} // NO PASV 建立数据通道

		return FTP_REQUEST_SUCCESS;
	}// WHILE LOOP

	return FTP_REQUEST_SUCCESS;
}


// 开始下载
int CFtpDownload::Download(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSavePath,BOOL bForceDownload /*= FALSE */)
{
	m_bStopDownload	  = FALSE;
	m_bForceDownload  = bForceDownload;
	m_nRetryTimes	  = 0;
	m_nErrorCount	  = 0;
	m_strTempSavePath = _T("");
	
	// 检验要下载的URL是否为空
	m_strDownloadUrl = lpszDownloadUrl;
	m_strDownloadUrl.TrimLeft();
	m_strDownloadUrl.TrimRight();
	if( m_strDownloadUrl.IsEmpty() )
		return FTP_RESULT_FAIL;

	// 检验要下载的URL是否有效
	if ( !ParseURL(m_strDownloadUrl, m_strServer, m_strObject, m_nPort))
	{
		// 在前面加上"ftp://"再试
		m_strDownloadUrl = _T("ftp://") + m_strDownloadUrl;
		if ( !ParseURL(m_strDownloadUrl,m_strServer, m_strObject, m_nPort) )
		{
			TRACE(_T("Failed to parse the URL: %s\n"), m_strDownloadUrl);
			return FTP_RESULT_FAIL;
		}
	}

	// 检查本地保存路径
	m_strSavePath =  lpszSavePath;
	m_strSavePath.TrimLeft();
	m_strSavePath.TrimRight();
	if( m_strSavePath.IsEmpty() )
		m_strSavePath = DEFAULT_SAVE_DIR;
	
	if( m_strSavePath.Right(1) != '\\' )	// 只有路径，没有文件名
	{
		m_strTempSavePath =  m_strSavePath;
		m_strTempSavePath += ".tmp";
	}

	// 创建停止事件
	if( m_hStopEvent == NULL )
	{
		m_hStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		if( m_hStopEvent == NULL )
			return FTP_RESULT_FAIL;

		//seawind
		m_cControlSocket.m_hStopEvent = m_hStopEvent;
		m_cDataSocket.m_hStopEvent = m_hStopEvent;
	}
	ResetEvent( m_hStopEvent );

	// 设置下载数据
	m_dwDownloadedSize		= 0;
	m_dwFileDownloadedSize	= 0;
	m_dwFileSize			= 0;
	m_dwDownloadSize		= 0;

	BOOL bSendOnce = TRUE;		// 用于控制向hWndNotify窗口发送消息
	
ReDownload:
	int nRequestRet = SendRequest( m_bPasv ) ;

	//seawind
	//if (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
	//	return HTTP_RESULT_STOP;
	Check( m_bStopDownload,FTP_RESULT_STOP);

	switch(nRequestRet)
	{
	case FTP_REQUEST_SUCCESS:
		TRACE0("\n\n***FTP_REQUEST_SUCCESS");
		break;
	case FTP_REQUEST_STOP:
		TRACE0("\n\n***FTP_REQUEST_STOP");
		return FTP_RESULT_STOP;
		break;
	case FTP_REQUEST_FAIL:
		TRACE0("\n\n***FTP_REQUEST_FAIL");
		return FTP_RESULT_FAIL;
		break;
	case FTP_REQUEST_ERROR:
		TRACE0("\n\n***FTP_REQUEST_ERROR");
		// 停止下载?
		Check( m_bStopDownload,FTP_RESULT_STOP);

		switch( m_nRetryType )
		{
		case FTP_RETRY_NONE:
			return FTP_RESULT_FAIL;
			break;
		case FTP_RETRY_ALWAYS:
			if( m_nRetryDelay > 0 )
			{
				//为了让等待时也能退出，不能使用Sleep函数
				if( WaitForSingleObject(m_hStopEvent,m_nRetryDelay) == WAIT_OBJECT_0 )
					return FTP_RESULT_STOP;
			}
			goto ReDownload;
			break;
		case FTP_RETRY_TIMES:
			if( m_nRetryTimes > m_nRetryMax )
				return FTP_RESULT_FAIL;
			m_nRetryTimes++;
		
			if( m_nRetryDelay > 0 )
			{
				//为了让等待时也能退出，不能使用Sleep函数
				if( WaitForSingleObject(m_hStopEvent,m_nRetryDelay) == WAIT_OBJECT_0 )
					return FTP_RESULT_STOP;
			}
			
			goto ReDownload;
			break;
		default:
			return FTP_RESULT_FAIL;
			break;
		}
		break;
	default:
		return FTP_RESULT_FAIL;
		break;
	}

	
	// Now Support none content-length header
	//if (m_dwDownloadSize == 0)
	//	return FTP_RESULT_FAIL;


	if( m_strSavePath.Right(1) == '\\' )
		m_strSavePath += m_strFileName;

	if( !m_bForceDownload ) // 非强制下载，检查Last-Modified
	{
		CFileStatus fileStatus;
		if (CFile::GetStatus(m_strSavePath,fileStatus))
		{
			// 可能会存在1秒的误差
			if (( fileStatus.m_mtime - m_TimeLastModified <=2 && m_TimeLastModified-fileStatus.m_mtime<=2 ) && (DWORD)fileStatus.m_size == m_dwFileSize )
				return FTP_RESULT_SAMEAS;
		}
	}
	CFile fileDown;
	if(! fileDown.Open(m_strTempSavePath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyWrite) )
	{
		TRACE0("\n\n*** FILE OPEN FAILED!");
		return FTP_RESULT_FAIL;	
	}

	// 应该判断一下是否支持断点续传
	if( m_bSupportResume )
	{
		try
		{
			fileDown.SeekToEnd();
		}
		catch(CFileException* e)                                         
		{
		  e->Delete();
		  return FTP_RESULT_FAIL;
		}	
	}
	
	if( bSendOnce && m_bNotify )
	{
		FTPDOWNLOADSTATUS DownloadStatus;
		
		DownloadStatus.dwFileSize  = m_dwFileSize;
		DownloadStatus.strFileName = m_strFileName;
		DownloadStatus.dwFileDownloadedSize  = m_dwFileDownloadedSize;

		DownloadStatus.nStatusType = FTP_STATUS_FILESIZE;
		if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
			::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_FTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);	

		DownloadStatus.nStatusType = FTP_STATUS_FILENAME;
		if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
			::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_FTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);	
	
		DownloadStatus.nStatusType = FTP_STATUS_FILEDOWNLOADEDSIZE;
		if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
			::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_FTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);	
		
		bSendOnce = FALSE;
	}

	m_dwDownloadedSize = 0;
	// 现在开始读取数据
	char szReadBuf[READ_BUFFER_SIZE+1];

	/////////////////////////////////////////////
	// 如果m_dwDownloadSize = 0 (说明大小未知)
	if( m_dwDownloadSize == 0 )
		m_dwDownloadSize = 0xFFFFFFFF;
	////////////////////////////////////////////

	do
	{
		// 停止下载?
		Check( m_bStopDownload,FTP_RESULT_STOP);
		
		ZeroMemory(szReadBuf,READ_BUFFER_SIZE+1);
		int nRet = m_cDataSocket.BSDGetData(szReadBuf,READ_BUFFER_SIZE,m_dwRecvTimeout);

		//seawind
		Check( m_bStopDownload, FTP_RESULT_STOP);		

		if (nRet <= 0)
		{
			////////////////////////////////////////
			if( m_dwDownloadSize == 0xFFFFFFFF )
				break;
			///////////////////////////////////////

			fileDown.Close();
			m_nErrorCount++;
			goto ReDownload; //再次发送请求
		}

		// 将数据写入文件
		try
		{
			fileDown.Write(szReadBuf,nRet);
		}
		catch(CFileException* e)
		{
			e->Delete();
			fileDown.Close();
			goto ReDownload;
		}

		m_dwDownloadedSize		+= nRet;
		m_dwFileDownloadedSize	+= nRet;

		// 通知消息
		if (m_bNotify)
		{
			FTPDOWNLOADSTATUS DownloadStatus;
            
			DownloadStatus.nStatusType			= FTP_STATUS_FILEDOWNLOADEDSIZE;
			DownloadStatus.dwFileDownloadedSize = m_dwFileDownloadedSize;
			DownloadStatus.dwFileSize			= m_dwFileSize;
			DownloadStatus.strFileName			= m_strFileName;

			if (WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0)
				::SendMessage(m_hNotifyWnd,m_nNotifyMessage,MSG_FTPDOWNLOAD_STATUS,(LPARAM)&DownloadStatus);	
		}
    } while(m_dwDownloadedSize < m_dwDownloadSize);
    
	TRACE0("\n\n***FILE DOWNLOAD SUCCESS!");

	// 关闭文件
	fileDown.Close();
	
	//关闭SOCKET
	CloseControlSocket();
	CloseDataSocket();

	// 文件改名
	//首先将已有的文件删除
	::DeleteFile(m_strSavePath);
    
    //再将新下载的文件改名
	::MoveFile(m_strTempSavePath, m_strSavePath);
    
    //再将新下载的文件的时间改回去
    CFileStatus fileStatus;
    
    fileStatus.m_size = 0;
    fileStatus.m_attribute = 0;
    
    try
    {
        CFile::GetStatus(m_strSavePath, fileStatus);
        
        fileStatus.m_mtime = m_TimeLastModified;
        CFile::SetStatus(m_strSavePath,fileStatus);
    }
    catch(CFileException *e)
    {
        e->Delete();
    }
    
    // 不再进行其他操作
    return FTP_RESULT_SUCCESS;
}


// 建立连接
int CFtpDownload::MakeConnection(CBufSocket* pBufSocket,CString strServer,int nPort,BOOL bBind /*= FALSE*/ ,LPDWORD	lpdwIP /*= NULL*/,LPINT lpnPort /*= NULL*/)
{
	CSocksPacket	cSocks( pBufSocket );
	DWORD			dwIP;
	BYTE			byAuth,byAtyp,byCmd;
	SOCKSREPPACKET	pack;
	CString			strAuth,strAddr,strIP,strDigit;

	strAuth = _T("");
	strAddr = _T("");

	// 检查是否为BIND命令
	if( bBind && (	m_nProxyType == FTP_PROXY_SOCKS4  || 
					m_nProxyType == FTP_PROXY_SOCKS4A || 
					m_nProxyType == FTP_PROXY_SOCKS5 ) )
	{
		byCmd = CMD_BIND;
		if( lpdwIP == NULL || lpnPort == NULL )
			return FTP_REQUEST_ERROR;
	}
	else
	{
		bBind = FALSE;
		byCmd = CMD_CONNECT;
	}

	switch( m_nProxyType )
	{
	case FTP_PROXY_NONE:
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(strServer,nPort,m_dwConnTimeout,TRUE),FTP_REQUEST_ERROR);
		break;
	case FTP_PROXY_SOCKS4A:
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		if( dwIP == INADDR_NONE )
		{
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),FTP_REQUEST_ERROR);

			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !cSocks.SendSocks4aReq(byCmd,nPort,strServer,m_strProxyUsername,m_dwSendTimeout),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);

			ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
			Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS4AREP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
			Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS4AREP),FTP_REQUEST_FAIL);// 请求有错误，重试可能是没有用的

			// 如果是BIND命令
			if( bBind )
			{
				if( pack.socks4aRep.dwDestIP == 0 )
					*lpdwIP = ntohl(CBufSocket::GetIP( m_strProxyServer ));
				else
					*lpdwIP = pack.socks4aRep.dwDestIP;

				*lpnPort= (int)pack.socks4aRep.wDestPort;
			}
			
			break;// NOTICE:如果本地能够解析域名，可以使用SOCKS4 Proxy
		}
	case FTP_PROXY_SOCKS4:
		// 必须要得到Proxy Server的IP地址(不能为域名)
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		Check( dwIP == INADDR_NONE ,FTP_REQUEST_ERROR);

		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),FTP_REQUEST_ERROR);
		
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !cSocks.SendSocks4Req( byCmd,nPort,dwIP,m_strProxyUsername,m_dwSendTimeout),FTP_REQUEST_ERROR);

		ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
		Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS4REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS4REP),FTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的

		// 如果是BIND命令
		if( bBind )
		{
			if( pack.socks4Rep.dwDestIP == 0 )
				*lpdwIP = ntohl(CBufSocket::GetIP( m_strProxyServer) );
			else
				*lpdwIP = pack.socks4Rep.dwDestIP;
			*lpnPort= (int)pack.socks4Rep.wDestPort;
		}
		break;
	case FTP_PROXY_SOCKS5:

		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !pBufSocket->Connect(m_strProxyServer,m_nProxyPort,m_dwConnTimeout,TRUE),FTP_REQUEST_ERROR);

		if( m_bProxyAuthorization )
		{
			char ch	= (char)AUTH_NONE;
			strAuth += ch;
			ch 		= (char)AUTH_PASSWD;
			strAuth += ch;
		}
		else
		{
			char ch	= (char)AUTH_NONE;
			strAuth += ch;
		}
		byAuth =(BYTE)strAuth.GetLength();
			
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !cSocks.SendSocks5AuthReq(byAuth,(LPCTSTR)strAuth,m_dwSendTimeout),FTP_REQUEST_ERROR);
		Check( m_bStopDownload,FTP_REQUEST_STOP);

		ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
		Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS5AUTHREP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS5AUTHREP),FTP_REQUEST_FAIL);	// 请求有错误，重试可能是没有用的

		switch( pack.socks5AuthRep.byAuth )
		{
		case AUTH_NONE:
			break;
		case AUTH_PASSWD:

			Check( !m_bProxyAuthorization ,FTP_REQUEST_FAIL);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			Check( !cSocks.SendSocks5AuthPasswdReq(m_strProxyUsername,m_strProxyPassword,m_dwSendTimeout),FTP_REQUEST_ERROR);
			Check( m_bStopDownload,FTP_REQUEST_STOP);
			
			ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
			Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS5AUTHPASSWDREP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
			Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS5AUTHPASSWDREP) ,FTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的
			break;
		case AUTH_GSSAPI:
		case AUTH_CHAP:
		case AUTH_UNKNOWN:
		default:
			return FTP_REQUEST_FAIL;
			break;
		}
		
		Check( m_bStopDownload,FTP_REQUEST_STOP);
		dwIP = CBufSocket::GetIP(strServer,TRUE);
		if( dwIP != INADDR_NONE )
		{
			byAtyp = ATYP_IPV4ADDR;
			// 不需要转换字节序(已经是网络字节序)
			strAddr += (char)( (dwIP    ) &0x000000ff); 
			strAddr += (char)( (dwIP>>8 ) &0x000000ff); 
			strAddr += (char)( (dwIP>>16) &0x000000ff); 
			strAddr += (char)( (dwIP>>24) &0x000000ff); 

		}
		else
		{
			byAtyp = ATYP_HOSTNAME;
			char ch = (char)strServer.GetLength();
			strAddr += ch;
			strAddr += strServer;
		}

		Check( m_bStopDownload,FTP_REQUEST_STOP);
		Check( !cSocks.SendSocks5Req(byCmd,byAtyp,strAddr,nPort,m_dwSendTimeout),FTP_REQUEST_ERROR);
		Check( m_bStopDownload,FTP_REQUEST_STOP);

		ZeroMemory(&pack,sizeof(SOCKSREPPACKET));
		Check( !cSocks.RecvPacket(&pack,PACKET_SOCKS5REP,m_dwRecvTimeout),FTP_REQUEST_ERROR);
		Check( !cSocks.IsSocksOK(&pack,PACKET_SOCKS5REP),FTP_REQUEST_FAIL); // 请求有错误，重试可能是没有用的

		// 如果是BIND命令
		if( bBind )
		{
			Check( pack.socks5Rep.byAtyp != ATYP_IPV4ADDR,FTP_REQUEST_ERROR);

			strIP  = _T("");
	
			strDigit.Format("%d",(BYTE)pack.socks5Rep.pszBindAddr[0]);
			strIP += strDigit;
			strIP += ".";

			strDigit.Format("%d",(BYTE)pack.socks5Rep.pszBindAddr[1]);
			strIP += strDigit;
			strIP += ".";

			strDigit.Format("%d",(BYTE)pack.socks5Rep.pszBindAddr[2]);
			strIP += strDigit;
			strIP += ".";

			strDigit.Format("%d",(BYTE)pack.socks5Rep.pszBindAddr[3]);
			strIP += strDigit;

			*lpdwIP = ntohl( inet_addr(strIP) );
				
			if( *lpdwIP == 0 )
				*lpdwIP = ntohl( CBufSocket::GetIP( m_strProxyServer ) );

			*lpnPort= (int)pack.socks5Rep.wBindPort;
		}
		
		break;
	default:
		return FTP_REQUEST_FAIL;
		break;
	}
	return FTP_REQUEST_SUCCESS;
}

//seawind
BOOL CFtpDownload::IsUserStop()
{
	ASSERT(m_hStopEvent != NULL);

	if (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
		return TRUE;
	else
		return FALSE;
}
