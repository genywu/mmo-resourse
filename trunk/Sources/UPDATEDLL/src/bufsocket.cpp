
//
///////////////////////////////////////////////////////////////////////////////
// BufSocket.cpp: implementation of the CBufSocket class.
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BufSocket.h"

// 需要WS2_32.lib文件
//#pragma comment(lib,"ws2_32.lib")

////////////////////////////////////////////////////////////////////////////////
//			构造函数
////////////////////////////////////////////////////////////////////////////////
CBufSocket::CBufSocket()
{
	m_hSocket		= INVALID_SOCKET;
	m_pBSD			= NULL;
	m_nErrorCode	= 0;	

	//seawind
	m_hStopEvent = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//			析构函数
////////////////////////////////////////////////////////////////////////////////
CBufSocket::~CBufSocket()
{
	Close(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL InitLibrary(
//				int nMajorVer /*= 2*/,
//				int nMinorVer /*= 2*/ ) 
//	用  途：初始化WinSock2动态链接库
//	对全局变量的影响：无
//	参  数：
//		nMajorVer : 主版本号
//		nMinorVer : 次版本号
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::InitLibrary( int nMajorVer /*= 2*/ ,int nMinorVer /*= 2*/)
{

	WSADATA         WSD;
	WORD			wVersionRequired = MAKEWORD( nMajorVer, nMinorVer );
	
	ZeroMemory(&WSD,sizeof(WSADATA));

	int nErrorCode = WSAStartup(wVersionRequired, &WSD);

	if ( SOCKET_SUCCESS != nErrorCode )
		return FALSE;

	if ( LOBYTE( WSD.wVersion ) != nMinorVer ||
		 HIBYTE( WSD.wVersion ) != nMajorVer ) 
	{
		WSACleanup( );
		return FALSE; 
	}
 
	//成功初始化
	return TRUE;
}
	

////////////////////////////////////////////////////////////////////////////////
//	函数名：void CleanupLibrary()
//	用  途：释放WinSock2动态链接库
//	对全局变量的影响：无
//	参  数：无
//	返回值：void
////////////////////////////////////////////////////////////////////////////////
void CBufSocket::CleanupLibrary()
{
    WSACleanup();	
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int GetLastError( void ) 
//	用  途：获取最近一次操作的错误码
//	对全局变量的影响：无
//	参  数：无
//	返回值：int，最近一次的操作的错误码
////////////////////////////////////////////////////////////////////////////////
int	CBufSocket::GetLastError(void)
{
	return m_nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：void SetLastError( int nErrorCode ) 
//	用  途：设置最近一次操作的错误码
//	对全局变量的影响：无
//	参  数：
//		nErrorCode : 待设置的新的错误码
//	返回值：void
////////////////////////////////////////////////////////////////////////////////
void CBufSocket::SetLastError(int nErrorCode)
{
	m_nErrorCode = nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Create(
//				int nAddressFamily /*= AF_INET*/,
//				int nType /*= SOCK_STREAM*/,
//				int nProtocol /*= 0*/,
//				int nBufferSize /*= DEFAULT_BUFFER_SIZE*/,
//				SOCKET hSocket /*= INVALID_SOCKET*/,
//				DWORD dwFlags /*= WSA_FLAG_OVERLAPPED*/ ) 
//	用  途：创建SOCKET和BSD结构
//	对全局变量的影响：无
//	参  数：
//		nAddressFamily : SOCKET地址家族
//		nType          : SOCKET类型
//		nProtocol      : SOCKET采用的协议
//		nBufferSize    : 缓冲区的大小
//		hSocket        : 类实例绑定的套接字
//		dwFlags        : 套接字的属性
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL	CBufSocket::Create(	int		nAddressFamily	/*= AF_INET*/, 
							int		nType			/*= SOCK_STREAM*/, 
							int		nProtocol		/*= 0*/,
							int		nBufferSize		/*= DEFAULT_BUFFER_SIZE*/,
							SOCKET	hSocket			/*= INVALID_SOCKET*/,
							DWORD	dwFlags			/*= WSA_FLAG_OVERLAPPED*/)
{
    if (INVALID_SOCKET == hSocket)
    {
        m_hSocket = WSASocket(nAddressFamily, nType, nProtocol, NULL,0,dwFlags);
        
        if (INVALID_SOCKET == m_hSocket)
        {
            SetLastError( WSAGetLastError() );
            return FALSE;
        }
    }
    else
    {
        m_hSocket = hSocket;
    }
    
    if (SOCK_STREAM == nType)   //TCP
    {
        //设置套接字选项
        if (!SetSocketOption(m_hSocket))    //设置属性失败
        {
            Close(TRUE);
            m_hSocket = INVALID_SOCKET;
            return FALSE;
        }
        
        //创建BSD
        
        //分配内存
        m_pBSD = (PBSD) malloc(sizeof(BSD));
        
        if (NULL == m_pBSD)
        {
            Close(TRUE);
            return FALSE;
        }
        
        char *pszBuffer = (char *) malloc(nBufferSize);
        
        if (NULL == pszBuffer)
        {
            Close(TRUE);
            
            free(m_pBSD);
            m_pBSD = NULL;
            return FALSE;
        }
        
        // 设置结构成员
        ZeroMemory(m_pBSD,sizeof(BSD));
        ZeroMemory(pszBuffer,nBufferSize);
        
        m_pBSD->hSocket			= m_hSocket;
        m_pBSD->nBufferSize		= nBufferSize;
        m_pBSD->pszBuffer		= pszBuffer;
        m_pBSD->nBytesInBuffer	= 0;
        m_pBSD->nReadIndex		= 0;
        m_pBSD->nBufferIndex	= 0;
    }
    else	// UDP
    {
        m_pBSD = NULL;
    }
    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：void Close( BOOL bHardClose /*= FALSE */ ) 
//	用  途：关闭SOCKET
//	对全局变量的影响：无
//	参  数：
//		bHardClose : 是否立即关闭(非优雅关闭)
//	返回值：void
////////////////////////////////////////////////////////////////////////////////
void CBufSocket::Close(	BOOL bHardClose /*= FALSE */)
{
    if (m_pBSD != NULL)
    {
        // 释放内存
        free(m_pBSD->pszBuffer);
        free(m_pBSD);
        m_pBSD = NULL;
    }
    
    if (INVALID_SOCKET == m_hSocket)
        return;
    
    // 不需要捕获错误
    if (!bHardClose) // 优雅关闭 Graceful close
    {
        // 不再发送数据，对于TCP套接字，在所有的数据都发送完毕之后，
        // 将发送一个 FIN ，通知接收方所有数据已经发送完毕。
        shutdown(m_hSocket, SD_SEND);
        
        // 接收缓冲区有可能还有未接收的数据，在关闭套接字之前应该先
        // 读取残留的数据。
        int		nRecvResult;
        HANDLE	hSocketEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        char	szBuffer[256];
        do
        {
            if (hSocketEvent != NULL)
            {
                //注册网络事件
                WSAEventSelect(m_hSocket, (WSAEVENT) hSocketEvent, FD_READ | FD_CLOSE);
                WSAWaitForMultipleEvents(1, &hSocketEvent, TRUE,DEFAULT_SHUTDOWN_RECV_TIMEOUT, TRUE);
                //清除网络事件
                WSAEventSelect(m_hSocket, (WSAEVENT) hSocketEvent, 0);
            }
            ZeroMemory(szBuffer,256);
            nRecvResult = RecvOnce(m_hSocket,szBuffer, sizeof(szBuffer));
        } while (nRecvResult > 0);
        
        if (hSocketEvent != NULL)
            CloseHandle(hSocketEvent);
        
        //不再允许接收和发送
        shutdown(m_hSocket, SD_BOTH);
    }
    
    // 关闭套接字
    closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SetSocketOption( SOCKET hSocket ) 
//	用  途：设置Socket的属性
//	对全局变量的影响：无
//	参  数：
//		hSocket : 设置属性的套接字
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::SetSocketOption(SOCKET hSocket)
{
    int	nActivate = 1;

	//允许地址重用
    if (setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &nActivate,sizeof(nActivate)) == SOCKET_ERROR )
    {
		SetLastError( WSAGetLastError() );
        return FALSE;
    }

	//  如果支持，设置KEEPALIVE属性 (这样做会带来其他不良后果)
	//setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, (const char *) &nActivate,sizeof(nActivate));
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int RecvOnce(
//				SOCKET hSocket,
//				OUT char *pszBuffer,
//				int nBufferSize ) 
//	用  途：一次性接收数据(重叠IO)
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 待读取数据的套接字
//		pszBuffer   : 接收数据的缓冲区
//		nBufferSize : 缓冲区大小
//	返回值：int
//		>=0			: 成功，返回的是接收到字节数
//		<0			: 失败，返回的是错误码的相反数
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::RecvOnce(SOCKET hSocket,OUT char *pszBuffer, int nBufferSize)
{

	DWORD		dwRecvBytes = 0,
				dwFlags		= 0;
	WSABUF		WSABuff;

	//清空缓冲
	ZeroMemory(&WSABuff,sizeof(WSABUF));

	WSABuff.len = nBufferSize;
	WSABuff.buf = pszBuffer;

	return ((WSARecv(hSocket, &WSABuff, 1, &dwRecvBytes, &dwFlags,NULL, NULL) == SOCKET_SUCCESS) ? (int) dwRecvBytes : -WSAGetLastError());

}


////////////////////////////////////////////////////////////////////////////////
//	函数名：int RecvData(
//				SOCKET hSocket,
//				OUT char *pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout ) 
//	用  途：接收数据(阻塞直至收到数据为止)
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 待接收数据的套接字
//		pszBuffer   : 缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 接收超时
//	返回值：int
//		>=0			: 接收到的字节数
//		<0			: 失败(SOCKET_FAIL: -1)
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::RecvData(SOCKET hSocket, OUT char *pszBuffer, int nBufferSize, DWORD dwTimeout)
{
	HANDLE hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (hReadEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return ( SOCKET_FAIL );
	}

	int		nRecvBytes = 0;
	DWORD	dwWaitResult;
	for (;;)
	{
		// 注册FD_READ | FD_CLOSE 事件 
		// (因为可能在等待FD_READ事件中，对方关闭套接字，所以要关注FD_CLOSE)
		if( WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
	
		// 等等FD_READ | FD_CLOSE事件的发生
		//dwWaitResult = WSAWaitForMultipleEvents(1, &hReadEvent, TRUE,dwTimeout, TRUE);

		//seawind
		DWORD dwBeginTime = GetTickCount();
		BOOL bDataCome = FALSE;
		
		while(GetTickCount() - dwBeginTime < dwTimeout)
		{			
			//和HttpDownload,FtpDownload类共享一个Event Handle
			if (m_hStopEvent != NULL && (WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0))
			{
				//要赶快跳走，用户点了Stop

				CloseHandle(hReadEvent);
				SetLastError( WSAGetLastError() );
				return (SOCKET_FAIL);
				
				//break;
			}

			dwWaitResult = WSAWaitForMultipleEvents(1, &hReadEvent, TRUE, 0, TRUE);
			
			if (dwWaitResult == WSA_WAIT_EVENT_0)
			{
				//数据来啦
				bDataCome = TRUE;
				break;
			}

			//空出CPU时间
			Sleep(DEFAULT_BLOCKED_SNDRCV_SLEEP);
		}

		//if (dwWaitResult != WSA_WAIT_EVENT_0)
		if (!bDataCome)
		{
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		
		////////////////////////////////////////////////////////////// 
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hReadEvent,&NetEvent) == SOCKET_ERROR)
		{
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		//判断发生了什么事件 FD_READ 或 FD_CLOSE	
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( NetEvent.lNetworkEvents	== FD_READ && 
			  NetEvent.iErrorCode[FD_READ_BIT] !=0 ) )	// 发生错误
		{
			// 清除事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError(WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		////////////////////////////////////////////////////////////////
		// 清除事件
		WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
	
		// 接收数据
		if ((nRecvBytes = RecvOnce(hSocket, pszBuffer,nBufferSize)) >= 0)
			break;	// 跳出循环

		int nErrorCode = -nRecvBytes;

		if ( nErrorCode != WSAEWOULDBLOCK )	//太多的未完成重叠操作
		{
			CloseHandle(hReadEvent);
			SetLastError( nErrorCode );
			return (SOCKET_FAIL);
		}

		//阻塞住了
		////////////////////////////////////////////////////////////////////////
		//  如果发生阻塞,就等待一定时间后重试,以免CPU轮询浪费时间
		////////////////////////////////////////////////////////////////////////
		Sleep(DEFAULT_BLOCKED_SNDRCV_SLEEP);
    }

    CloseHandle(hReadEvent);
    return (nRecvBytes);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int Recv(
//				OUT char *pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/ ) 
//	用  途：接收数据直至达到规定的长度(缓冲区满)或超时或没有数据可读取时
//	对全局变量的影响：无
//	参  数：
//		pszBuffer   : 缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 接收超时
//	返回值：int，接收到的字节数
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::Recv(OUT char *pszBuffer, int nBufferSize, DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	int	nRecvBytes		= 0,
		nRecvCurrent	= 0;

	while (nRecvBytes < nBufferSize)
	{
		nRecvCurrent = RecvData(m_hSocket, (pszBuffer + nRecvBytes),(nBufferSize - nRecvBytes), dwTimeout);

		if (nRecvCurrent < 0)	//没有数据可读取
			return (nRecvBytes);

		nRecvBytes += nRecvCurrent;
	}
	return (nRecvBytes);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int SendOnce(
//				SOCKET hSocket,
//				const char * pszBuffer,
//				int nBufferSize ) 
//	用  途：一次性发送数据(重叠IO)
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 待发送的套接字
//		pszBuffer   : 发送数据缓冲区
//		nBufferSize : 缓冲区大小
//	返回值：int
//		>=0			: 成功，已成功发送的字节数
//		<0			: 失败，错误码的相反数
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::SendOnce(SOCKET hSocket, const char * pszBuffer, int nBufferSize)
{
	DWORD	dwSendBytes = 0;
	WSABUF	WSABuff;

	ZeroMemory(&WSABuff,sizeof(WSABUF));
	WSABuff.len = nBufferSize;
	WSABuff.buf = (char *) pszBuffer;

	return ((WSASend(hSocket, &WSABuff, 1, &dwSendBytes, 0,NULL, NULL) == SOCKET_SUCCESS) ? (int) dwSendBytes : -WSAGetLastError());

}


////////////////////////////////////////////////////////////////////////////////
//	函数名：int SendData(
//				SOCKET hSocket,
//				const char * pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout ) 
//	用  途：发送数据
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 待发送的套接字
//		pszBuffer   : 发送数据缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 发送超时
//	返回值：int
//		>=0			: 成功，已成功发送的字节数
//		<0			: 失败(SOCKET_FAIL:-1)
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::SendData(SOCKET hSocket, const char * pszBuffer, int nBufferSize, DWORD dwTimeout)
{
	HANDLE	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (hWriteEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return (SOCKET_FAIL);
	}

	int nSendBytes = 0;

	for (;;)
	{
		////////////////////////////////////////////////////////////////
		// 发送数据成功
		if ((nSendBytes = SendOnce(hSocket, pszBuffer, nBufferSize)) >= 0)
			break;

		int nErrorCode = -nSendBytes;

		if (nErrorCode != WSAEWOULDBLOCK)
		{
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		
		///////////////////////////////////////////////////////////////////////////////
		//  睡眠一段时间
		///////////////////////////////////////////////////////////////////////////////
		Sleep(DEFAULT_BLOCKED_SNDRCV_SLEEP);

		// 注册FD_WRITE | FD_CLOSE 事件
		if( WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
		{
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		// 等待事件发生
		DWORD dwWaitResult = WSAWaitForMultipleEvents(1, &hWriteEvent, TRUE,dwTimeout, TRUE);

		if (dwWaitResult != WSA_WAIT_EVENT_0)
		{
			// 清除网络事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		////////////////////////////////////////////////////////////// 
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hWriteEvent,&NetEvent) == SOCKET_ERROR)
		{
  			// 清除网络事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( NetEvent.lNetworkEvents == FD_WRITE   &&
			  NetEvent.iErrorCode[FD_WRITE_BIT] !=0 ) )	// 发生错误
		{
			// 清除网络事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		// 清除网络事件
		WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
   }

    CloseHandle(hWriteEvent);
    return (nSendBytes);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int Send(
//				const char * pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout /*= DEFAULT_SEND_TIMEOUT*/ ) 
//	用  途：发送完所有数据或超时
//	对全局变量的影响：无
//	参  数：
//		pszBuffer   : 发送数据缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 发送超时
//	返回值：int，已经成功发送的字节数
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::Send(const char * pszBuffer, int nBufferSize, DWORD dwTimeout /*= DEFAULT_SEND_TIMEOUT*/)
{
	int nSendBytes		= 0,
		nSendCurrent	= 0;

	while (nSendBytes < nBufferSize)
	{
		nSendCurrent = SendData(m_hSocket, (pszBuffer + nSendBytes),(nBufferSize - nSendBytes), dwTimeout);
		if (nSendCurrent < 0)
			return (nSendBytes);

		nSendBytes += nSendCurrent;
	}
	return (nSendBytes);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：int BSDReadData(
//				PBSD pBSD,
//				DWORD dwTimeout ) 
//	用  途：从缓冲区读取数据
//	对全局变量的影响：无
//	参  数：
//		pBSD      : BufSocketData结构指针
//		dwTimeout : 接收超时
//	返回值：int
//		>=0			: 读取的字节数
//		<0			: 失败 (SOCKET_FAIL: -1)	
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::BSDReadData(PBSD pBSD, DWORD dwTimeout)
{
	// 计算缓冲区空闲区域
	int		nMaxRead = pBSD->nBufferSize - pBSD->nBytesInBuffer;
	char	*pszBuffer = (char *) malloc(nMaxRead + 1);

	if (pszBuffer == NULL)
		return (SOCKET_FAIL);
	
	// 清空
	ZeroMemory(pszBuffer,nMaxRead+1);

	// 接收数据
	int nReadedBytes = RecvData(pBSD->hSocket, pszBuffer, nMaxRead, dwTimeout);

	if (nReadedBytes > 0)
	{
		int nHeadSize = Min(pBSD->nBufferSize - pBSD->nBufferIndex, nReadedBytes);

		if (nHeadSize > 0)
			memcpy(pBSD->pszBuffer + pBSD->nBufferIndex, pszBuffer, nHeadSize);

		pBSD->nBufferIndex += nHeadSize;

		if (pBSD->nBufferIndex == pBSD->nBufferSize)
			pBSD->nBufferIndex = 0;

		int nBackSize = nReadedBytes - nHeadSize;

		if (nBackSize > 0)
			memcpy(pBSD->pszBuffer + pBSD->nBufferIndex, pszBuffer + nHeadSize, nBackSize);

		pBSD->nBufferIndex	 += nBackSize;
		pBSD->nBytesInBuffer += nReadedBytes;
	}

	free(pszBuffer);
	return (nReadedBytes);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int BSDGetChar( DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/ ) 
//	用  途：从缓冲区读取一个字符
//	对全局变量的影响：无
//	参  数：
//		dwTimeout : 接收超时
//	返回值：int，一个字符(错误时返回INVALID_CHAR)
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::BSDGetChar(DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	if( m_pBSD == NULL)
		return (INVALID_CHAR);

	if ((m_pBSD->nBytesInBuffer == 0) && (BSDReadData(m_pBSD, dwTimeout) <= 0))
		return (INVALID_CHAR);

	int nChar =( (int) m_pBSD->pszBuffer[m_pBSD->nReadIndex]) & 0x000000ff;

	m_pBSD->nReadIndex = INext(m_pBSD->nReadIndex, m_pBSD->nBufferSize);

	--m_pBSD->nBytesInBuffer;

	return (nChar);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int BSDGetString(
//				OUT char *pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/ ) 
//	用  途：从缓冲区读取一行(包括单换行0x0A)(注意，结尾的回车换行不包括在其中)
//	对全局变量的影响：无
//	参  数：
//		pszBuffer   : 接收缓冲区
//		nBufferSize : 缓冲大小
//		dwTimeout   : 接收超时
//	返回值：int
//		 0			: 成功 (SOCKET_SUCCESS)
//		-1			: 失败 (SOCKET_FAIL)	
//		 1			: 缓冲太小(SOCKET_BUFFER_TOO_SMALL)
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::BSDGetString(OUT char *pszBuffer, int nBufferSize,DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	if( m_pBSD == NULL)
		return (SOCKET_FAIL);
	
	int	nChar,nLastChar;
	BOOL bEnough = FALSE;

	nLastChar = INVALID_CHAR;
	for (int i = 0; i < (nBufferSize - 1);)
	{
		nChar = BSDGetChar(dwTimeout);

		if (nChar == INVALID_CHAR)
			return SOCKET_FAIL;

		if (nChar == 0x0A)	// 换行符号
		{
			bEnough = TRUE;
			if ( nLastChar == 0x0D)
				i-- ;	
			break;
		}
		else
			pszBuffer[i++] = (char) nChar;

		nLastChar = nChar;
			
	}
	pszBuffer[i] = '\0';
	
	return (bEnough)?(SOCKET_SUCCESS):(SOCKET_BUFFER_TOO_SMALL);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int BSDGetData(
//				OUT char *pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/ ) 
//	用  途：从缓冲区读取一定数量的数据
//	对全局变量的影响：无
//	参  数：
//		pszBuffer   : 接收缓冲区
//		nBufferSize : 缓冲大小
//		dwTimeout   : 接收超时
//	返回值：int
//		>=0			: 读取的字节数
//		<0			: 失败(SOCKET_FAIL:-1)
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::BSDGetData(OUT char *pszBuffer, int nBufferSize,DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	if( m_pBSD == NULL)
		return (SOCKET_FAIL);

	if (( m_pBSD->nBytesInBuffer == 0) && ( BSDReadData(m_pBSD, dwTimeout) <= 0))
		return SOCKET_FAIL; 

	int nReadBytes	 = 0;
	int nHeadSize,nBackSize;

	if( m_pBSD->nBytesInBuffer < nBufferSize ) // 数据不够多
	{
		nHeadSize = Min( m_pBSD->nBufferSize - m_pBSD->nReadIndex ,m_pBSD->nBytesInBuffer );
		memcpy(pszBuffer+nReadBytes,m_pBSD->pszBuffer+m_pBSD->nReadIndex,nHeadSize);
			
		m_pBSD->nReadIndex += nHeadSize;
		if( m_pBSD->nReadIndex == m_pBSD->nBufferSize )
			m_pBSD->nReadIndex = 0;

		nReadBytes += nHeadSize;
		nBackSize	= m_pBSD->nBytesInBuffer - nHeadSize;
			
		if( nBackSize > 0)
			memcpy(pszBuffer+nReadBytes,m_pBSD->pszBuffer+m_pBSD->nReadIndex,nBackSize);

		nReadBytes				+= nBackSize;
		m_pBSD->nReadIndex		+= nBackSize;
		m_pBSD->nBytesInBuffer	 = 0; // 数据全部读完
	}
	else // 这次的数据足够多了
	{
		nHeadSize = Min( m_pBSD->nBufferSize - m_pBSD->nReadIndex,nBufferSize - nReadBytes );
		memcpy(pszBuffer+nReadBytes,m_pBSD->pszBuffer+m_pBSD->nReadIndex,nHeadSize);
			
		m_pBSD->nReadIndex += nHeadSize;
		if( m_pBSD->nReadIndex == m_pBSD->nBufferSize )
			m_pBSD->nReadIndex = 0;

		nReadBytes += nHeadSize;
		nBackSize = nBufferSize - nReadBytes;

		if( nBackSize > 0)
			memcpy(pszBuffer+nReadBytes,m_pBSD->pszBuffer+m_pBSD->nReadIndex,nBackSize);

		nReadBytes				+= nBackSize;
		m_pBSD->nReadIndex		+= nBackSize;
		m_pBSD->nBytesInBuffer	-= (nHeadSize+nBackSize);
	}
	return nReadBytes;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetSockName(
//				OUT char *pszIP,
//				OUT int& nPort ) 
//	用  途：获取Socket绑定的IP地址和端口
//	对全局变量的影响：无
//	参  数：
//		pszIP : 用于保存IP地址
//		nPort : 用于保存端口
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::GetSockName(OUT char *pszIP,OUT int& nPort)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	
	if( getsockname(m_hSocket,(SOCKADDR*)&sockAddr, &nSockAddrLen) == SOCKET_ERROR )
	{
		SetLastError(WSAGetLastError());
		return FALSE;
	}
	
	char* pszTemp = inet_ntoa(sockAddr.sin_addr);
	memcpy(pszIP,pszTemp,strlen(pszTemp) );
	nPort = ntohs(sockAddr.sin_port);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：static DWORD WINAPI DNSThread( LPVOID pParam ) 
//	用  途：域名解析线程，引入该函数的目的是为了避免NT下对域名解析的CACHE造成的问题
//	对全局变量的影响：无
//	参  数：
//		pParam : 待解析的域名
//	返回值：DWORD，IP地址的值，出现错误返回INADDR_NONE
////////////////////////////////////////////////////////////////////////////////
static DWORD WINAPI DNSThread(  LPVOID pParam )
{
	DWORD dwIP = INADDR_NONE;
	PHOSTENT pHost = gethostbyname( (char *)pParam );
	if(pHost == NULL)
		return INADDR_NONE;
	dwIP = inet_addr( inet_ntoa(*(IN_ADDR *)*pHost->h_addr_list) );
	return dwIP;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：DWORD GetIP(
//				const char* pszName,
//				BOOL bFixNtDNS /*= FALSE*/ ) 
//	用  途：获取IP地址
//	对全局变量的影响：无
//	参  数：
//		pszName   : 待解析的域名
//		bFixNtDNS : 是否使用线程来解决NT下域名解析的Cache问题
//	返回值：DWORD，IP地址的值，出现错误返回INADDR_NONE
////////////////////////////////////////////////////////////////////////////////
DWORD CBufSocket::GetIP(const char* pszName,BOOL bFixNtDNS  /*= FALSE*/)
{
	DWORD dwIP = inet_addr(pszName);
	if( dwIP != INADDR_NONE )
		return dwIP;

	if( bFixNtDNS )
	{
		OSVERSIONINFO		osVersion;
		osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if( GetVersionEx(&osVersion) )
		{
			if(osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				DWORD	dwThreadId = 0;
				HANDLE	hThread = CreateThread(NULL,0,DNSThread,(LPVOID)pszName,0,&dwThreadId);
				if( hThread != NULL)
				{
					WaitForSingleObject(hThread,INFINITE);
					if( GetExitCodeThread(hThread,&dwIP))
						return dwIP;
				}
			}
		}
	}

	PHOSTENT pHost = gethostbyname(pszName);
	if(pHost == NULL)
		return INADDR_NONE;
		
	dwIP = inet_addr( inet_ntoa(*(IN_ADDR *)*pHost->h_addr_list) );
	return dwIP;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Connect(
//				SOCKET hSocket,
//				const struct sockaddr * pSocketAddr,
//				int nAddrLen,
//				DWORD dwTimeout ) 
//	用  途：建立连接
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 用来建立连接到套接字
//		pSocketAddr : SOCKADDR结构指针，代表目标地址
//		nAddrLen    : 地址结构的长度
//		dwTimeout   : 连接超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Connect(SOCKET hSocket, const struct sockaddr * pSocketAddr, int nAddrLen,DWORD dwTimeout)
{
	HANDLE hConnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (hConnectEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return FALSE;
	}

	// 注册FD_CONNECT事件
	if( WSAEventSelect(hSocket, (WSAEVENT) hConnectEvent, FD_CONNECT) == SOCKET_ERROR)
	{
		CloseHandle(hConnectEvent);
		SetLastError( WSAGetLastError() );
		return FALSE;
	}

	int	nConnectResult = WSAConnect(hSocket, pSocketAddr, nAddrLen, NULL, NULL, NULL, NULL);
	int	nConnectError  = WSAGetLastError();
	
	if ((nConnectResult == SOCKET_ERROR) && (nConnectError == WSAEWOULDBLOCK))
	{
		DWORD dwWaitResult = WSAWaitForMultipleEvents(1, &hConnectEvent, TRUE,dwTimeout, TRUE);
		
		if (dwWaitResult != WSA_WAIT_EVENT_0)
		{
			SetLastError( WSAGetLastError() );
			nConnectResult = SOCKET_FAIL;
		}
		else
		{
			////////////////////////////////////////////////////////////// 
			///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
			///			进一步检查网络是否发生错误
			///////////////////////////////////////////////////////////////
			WSANETWORKEVENTS NetEvent;
			if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hConnectEvent,&NetEvent) == SOCKET_ERROR)
			{
				SetLastError( WSAGetLastError() );
				nConnectResult = SOCKET_FAIL;
			}
			else if(NetEvent.iErrorCode[FD_CONNECT_BIT] !=0 )	// 发生错误
			{
				SetLastError( NetEvent.iErrorCode[FD_CONNECT_BIT] );
				nConnectResult = SOCKET_FAIL;
			}
			else
				nConnectResult = SOCKET_SUCCESS;
			////////////////////////////////////////////////////////////////
		}
	}

	// 注销网络事件
	WSAEventSelect(hSocket, (WSAEVENT) hConnectEvent, 0);
	CloseHandle(hConnectEvent);

	return (nConnectResult == SOCKET_SUCCESS)?TRUE:FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Connect(
//				const char * pszServer,
//				int nPort,
//				DWORD dwTimeout /*= DEFAULT_CONNECT_TIMEOUT*/,
//				BOOL bFixNtDNS /*= FALSE*/ ) 
//	用  途：建立连接
//	对全局变量的影响：无
//	参  数：
//		pszServer : 待连接到目标地址
//		nPort     : 待连接的端口
//		dwTimeout : 连接超时
//		bFixNtDNS : 是否解决NT域名解析的CACHE问题
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Connect(const char * pszServer, int nPort,DWORD dwTimeout /*= DEFAULT_CONNECT_TIMEOUT*/,BOOL bFixNtDNS /*= FALSE*/)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));

	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_port			= htons((u_short)nPort);
	sockAddr.sin_addr.s_addr	= GetIP(pszServer,bFixNtDNS);
	
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////

	return Connect(m_hSocket, (SOCKADDR *)&sockAddr,sizeof(sockAddr),dwTimeout);

}


////////////////////////////////////////////////////////////////////////////////
//	函数名：SOCKET GetSocket()
//	用  途：获取套接字
//	对全局变量的影响：无
//	参  数：无
//	返回值：SOCKET
////////////////////////////////////////////////////////////////////////////////
SOCKET CBufSocket::GetSocket()
{
	return m_hSocket;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Listen( int nConnections /* = 5 */ ) 
//	用  途：监听套接字
//	对全局变量的影响：无
//	参  数：
//		nConnections : 监听队列的长度
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Listen(int nConnections /* = 5 */ )
{
	if(listen(m_hSocket, nConnections) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Bind( int nPort /* = 0 */ ) 
//	用  途：绑定套接字
//	对全局变量的影响：无
//	参  数：
//		nPort : 绑定的端口
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Bind(int nPort /* = 0 */)
{
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr,sizeof(sockAddr));

	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	sockAddr.sin_port			= htons((u_short)nPort);

	return	Bind( m_hSocket,(SOCKADDR *)&sockAddr, sizeof(sockAddr));

}

////////////////////////////////////////////////////////////////////////////////
//	函数名：SOCKET Accept(
//				struct sockaddr * pSocketAddr,
//				int *nAddrLen,
//				DWORD dwTimeout /*= DEFAULT_ACCEPT_TIMEOUT*/ ) 
//	用  途：接受套接字连接
//	对全局变量的影响：无
//	参  数：
//		pSocketAddr : SOCKET地址
//		nAddrLen    : 地址参数的长度
//		dwTimeout   : 超时
//	返回值：SOCKET
////////////////////////////////////////////////////////////////////////////////
SOCKET CBufSocket::Accept(struct sockaddr * pSocketAddr, int *nAddrLen,DWORD dwTimeout /*= DEFAULT_ACCEPT_TIMEOUT*/)
{
	HANDLE hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hAcceptEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return (INVALID_SOCKET);
	}

	// 注册FD_ACCEPT事件
	if( WSAEventSelect(m_hSocket, (WSAEVENT) hAcceptEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		CloseHandle(hAcceptEvent);
		SetLastError( WSAGetLastError() );
		return (INVALID_SOCKET);
	}

	SOCKET hSocketAccept = WSAAccept(m_hSocket, pSocketAddr, nAddrLen, NULL, 0);
	int	   nConnectError = WSAGetLastError();

	if ((hSocketAccept == INVALID_SOCKET) && (nConnectError == WSAEWOULDBLOCK))
	{

		// 阻塞
		DWORD dwWaitResult = WSAWaitForMultipleEvents(1, &hAcceptEvent, TRUE,dwTimeout, TRUE);

		if (dwWaitResult == WSA_WAIT_EVENT_0)
		{
			////////////////////////////////////////////////////////////// 
			///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
			///			进一步检查网络是否发生错误
			///////////////////////////////////////////////////////////////
			WSANETWORKEVENTS NetEvent;
			if(WSAEnumNetworkEvents(m_hSocket,hAcceptEvent,&NetEvent) == SOCKET_ERROR)
				SetLastError( WSAGetLastError() );
			else if(NetEvent.iErrorCode[FD_ACCEPT_BIT] !=0 )	// 发生错误
				SetLastError( NetEvent.iErrorCode[FD_ACCEPT_BIT] );
			else
				hSocketAccept = WSAAccept(m_hSocket, pSocketAddr, nAddrLen, NULL, 0);
		}
		else
			SetLastError( WSAGetLastError() );
	}
	
	// 注销网络事件
	WSAEventSelect( m_hSocket, (WSAEVENT) hAcceptEvent, 0);
	CloseHandle(hAcceptEvent);

	if (hSocketAccept != INVALID_SOCKET)
	{
		// 设置套接字的属性为地址可重用并且为非阻塞的
		if ( !Block(hSocketAccept, 0) || !SetSocketOption(hSocketAccept) )
		{
			closesocket(hSocketAccept);
			return (INVALID_SOCKET);
		}
	}
	return (hSocketAccept);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：SOCKET Accept(
//				struct sockaddr * pSocketAddr,
//				int *nAddrLen,
//				HANDLE hEndEvent,
//				DWORD dwTimeout /*= DEFAULT_ACCEPT_TIMEOUT*/ ) 
//	用  途：接受套接字连接(允许中断)
//	对全局变量的影响：无
//	参  数：
//		pSocketAddr : SOCKET地址
//		nAddrLen    : 地址长度
//		hEndEvent   : 结束监听事件
//		dwTimeout   : 超时
//	返回值：SOCKET
////////////////////////////////////////////////////////////////////////////////
SOCKET CBufSocket::Accept(struct sockaddr * pSocketAddr, int *nAddrLen,HANDLE hEndEvent,DWORD dwTimeout /*= DEFAULT_ACCEPT_TIMEOUT*/)
{
	if( hEndEvent == NULL)
		return Accept(pSocketAddr,nAddrLen,dwTimeout);

	HANDLE	hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hAcceptEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return (INVALID_SOCKET);
	}

	WSAEVENT hEvent[2]; 
	hEvent[0] = (WSAEVENT)hAcceptEvent;
	hEvent[1] = (WSAEVENT)hEndEvent;

	// 注册FD_ACCEPT事件
	if( WSAEventSelect(m_hSocket, (WSAEVENT) hAcceptEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		CloseHandle(hAcceptEvent);
		SetLastError( WSAGetLastError() );
		return (INVALID_SOCKET);
	}

	SOCKET hSocketAccept = WSAAccept(m_hSocket, pSocketAddr, nAddrLen, NULL, 0);
	int	   nConnectError = WSAGetLastError();

	if ((hSocketAccept == INVALID_SOCKET) && (nConnectError == WSAEWOULDBLOCK))
	{
		// 阻塞
		DWORD dwWaitResult = WSAWaitForMultipleEvents(2, hEvent, FALSE,dwTimeout, TRUE);
		if (dwWaitResult == WSA_WAIT_EVENT_0)
		{
			////////////////////////////////////////////////////////////// 
			///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
			///			进一步检查网络是否发生错误
			///////////////////////////////////////////////////////////////
			WSANETWORKEVENTS NetEvent;
			if(WSAEnumNetworkEvents(m_hSocket,hAcceptEvent,&NetEvent) == SOCKET_ERROR)
				SetLastError( WSAGetLastError() );
			else if(NetEvent.iErrorCode[FD_ACCEPT_BIT] !=0 )	// 发生错误
				SetLastError( NetEvent.iErrorCode[FD_ACCEPT_BIT] );
			else
				hSocketAccept = WSAAccept(m_hSocket, pSocketAddr, nAddrLen, NULL, 0);
		}
		else
			SetLastError( WSAGetLastError() );
	}
	
	// 注销网络事件
	WSAEventSelect(m_hSocket, (WSAEVENT) hAcceptEvent, 0);
	CloseHandle(hAcceptEvent);

	if (hSocketAccept != INVALID_SOCKET)
	{
		// 设置套接字的属性为地址可重用并且为非阻塞的
		if ( !Block(hSocketAccept, 0) || !SetSocketOption(hSocketAccept) )
		{
			closesocket(hSocketAccept);
			return (INVALID_SOCKET);
	
		}
	}
	return (hSocketAccept);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Bind(
//				SOCKET hSocket,
//				const struct sockaddr * pSocketAddr,
//				int nAddrLen ) 
//	用  途：绑定套接字
//	对全局变量的影响：无
//	参  数：
//		hSocket     : SOCKET
//		pSocketAddr : SOCKET地址
//		nAddrLen    : 地址长度
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Bind(SOCKET hSocket, const struct sockaddr * pSocketAddr, int nAddrLen)
{
	if (bind(hSocket, pSocketAddr, nAddrLen) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Block(
//				SOCKET hSocket,
//				BOOL bBlock ) 
//	用  途：设置套接字是否为阻塞的
//	对全局变量的影响：无
//	参  数：
//		hSocket : 套接字
//		bBlock  : 是否阻塞
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::Block(SOCKET hSocket, BOOL bBlock)
{
	u_long  IoctlLong = (bBlock) ? 0 : 1;

	if (ioctlsocket(hSocket, FIONBIO, &IoctlLong) == SOCKET_ERROR)
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
    }

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：int RecvFrom(
//				SOCKET hSocket,
//				struct sockaddr * pFrom,
//				int nFromlen,
//				char *pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout ) 
//	用  途：接收数据报
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 套接字
//		pFrom       : 数据报的源地址
//		nFromlen    : 地址长度
//		pszBuffer   : 数据缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 超时
//	返回值：int
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::RecvFrom(SOCKET hSocket, struct sockaddr * pFrom, int nFromlen,char *pszBuffer, int nBufferSize, DWORD dwTimeout)
{
	HANDLE hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL)
	{
		SetLastError((int)GetLastError() );
		return (SOCKET_ERROR);
	}

	DWORD		dwRecvBytes = 0,
				dwFlags		= 0;
	WSABUF		WSABuff;

	ZeroMemory(&WSABuff,sizeof(WSABUF));
	WSABuff.len = nBufferSize;
	WSABuff.buf = pszBuffer;

	for (;;)
	{
		// 注册FD_READ事件
		if( WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, FD_READ) == SOCKET_ERROR)
		{
			CloseHandle(hReadEvent);
			SetLastError(  WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		DWORD dwWaitResult = WSAWaitForMultipleEvents(1, &hReadEvent, TRUE,	dwTimeout, TRUE);

		if( dwWaitResult != WSA_WAIT_EVENT_0 )
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError());
			return (SOCKET_FAIL);
		}

		////////////////////////////////////////////////////////////// 
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hReadEvent,&NetEvent) == SOCKET_ERROR)
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		if(NetEvent.iErrorCode[FD_READ_BIT] !=0 )	// 发生错误
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);
			CloseHandle(hReadEvent);
			SetLastError(NetEvent.iErrorCode[FD_READ_BIT]);
			return (SOCKET_FAIL);
		}
		////////////////////////////////////////////////////////////////
		// 注销事件
		WSAEventSelect(hSocket, (WSAEVENT) hReadEvent, 0);

		int nAddrLen = nFromlen;
		if ( WSARecvFrom(hSocket, &WSABuff, 1, &dwRecvBytes, &dwFlags,pFrom, &nAddrLen, NULL, NULL) == SOCKET_SUCCESS )
			break;

		if ( WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CloseHandle(hReadEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		///////////////////////////////////////////////////////////////////////////
		//	睡眠一段时间
		//////////////////////////////////////////////////////////////////////////
		Sleep(DEFAULT_BLOCKED_SNDRCV_SLEEP);
	}
	CloseHandle(hReadEvent);
	return ((int) dwRecvBytes);

}


////////////////////////////////////////////////////////////////////////////////
//	函数名：int SendTo(
//				SOCKET hSocket,
//				const struct sockaddr * pTo,
//				int nToLen,
//				const char * pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout ) 
//	用  途：发送数据报
//	对全局变量的影响：无
//	参  数：
//		hSocket     : 套接字
//		pTo         : 数据报的目的地址
//		nToLen      : 地址长度
//		pszBuffer   : 缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 超时
//	返回值：int
////////////////////////////////////////////////////////////////////////////////
int CBufSocket::SendTo(SOCKET hSocket, const struct sockaddr * pTo,int nToLen,const char * pszBuffer, int nBufferSize, DWORD dwTimeout)
{
	HANDLE hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL)
	{
		SetLastError( (int)GetLastError() );
		return (SOCKET_FAIL);
	}

	DWORD	dwSendBytes = 0,
            dwFlags		= 0;
	WSABUF	WSABuff;

	ZeroMemory(&WSABuff,sizeof(WSABUF));
	WSABuff.len = nBufferSize;
	WSABuff.buf = (char *) pszBuffer;

	for (;;)
	{
		if (WSASendTo( hSocket, &WSABuff, 1, &dwSendBytes, dwFlags,pTo, nToLen, NULL, NULL) == SOCKET_SUCCESS)
			break;

		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CloseHandle(hWriteEvent);
			SetLastError(  WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		//////////////////////////////////////////////////////////////////////////
		//	睡眠一段时间
		/////////////////////////////////////////////////////////////////////////
		Sleep(DEFAULT_BLOCKED_SNDRCV_SLEEP);

		// 注册FD_WRITE事件  
		if( WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, FD_WRITE) == SOCKET_ERROR)
		{
			CloseHandle(hWriteEvent);
			SetLastError( WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		DWORD dwWaitResult = WSAWaitForMultipleEvents(1, &hWriteEvent, TRUE,dwTimeout, TRUE);
		
		if( dwWaitResult != WSA_WAIT_EVENT_0 )
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError(  WSAGetLastError() );
			return (SOCKET_FAIL);
		}

		////////////////////////////////////////////////////////////// 
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if(WSAEnumNetworkEvents(hSocket,(WSAEVENT)hWriteEvent,&NetEvent) == SOCKET_ERROR)
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError(  WSAGetLastError() );
			return (SOCKET_FAIL);
		}
		if(NetEvent.iErrorCode[FD_WRITE_BIT] !=0 )	// 发生错误
		{
			// 注销事件
			WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
			CloseHandle(hWriteEvent);
			SetLastError(NetEvent.iErrorCode[FD_WRITE_BIT]);
			return (SOCKET_FAIL);
		}
		////////////////////////////////////////////////////////////////
		// 注销事件
		WSAEventSelect(hSocket, (WSAEVENT) hWriteEvent, 0);
	}

	CloseHandle(hWriteEvent);
	return ((int) dwSendBytes);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL RecvFrom(
//				OUT char* pszFrom,
//				OUT int& nFromPort,
//				OUT char *pszBuffer,
//				IN int nBufferSize,
//				IN DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/ ) 
//	用  途：接收数据报
//	对全局变量的影响：无
//	参  数：
//		pszFrom     : 源地址
//		nFromPort   : 源地址端口
//		pszBuffer   : 缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::RecvFrom(OUT char* pszFrom,OUT int& nFromPort,OUT char *pszBuffer,IN int nBufferSize,IN DWORD dwTimeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	SOCKADDR_IN sockAddrFrom;
	ZeroMemory(&sockAddrFrom,sizeof(sockAddrFrom));

	// 假设缓冲区足够大,不考虑缓冲区不够的情形
	if( RecvFrom(m_hSocket, (SOCKADDR *)&sockAddrFrom, sizeof(sockAddrFrom),pszBuffer,nBufferSize,dwTimeout) <= 0 )
		return FALSE;

	// 保存源地址
	char* pFrom = inet_ntoa(sockAddrFrom.sin_addr);
	memcpy( pszFrom,pFrom,strlen(pFrom) );
	nFromPort = ntohs( sockAddrFrom.sin_port);
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendTo(
//				const char* pszTo,
//				int nToPort,
//				const char * pszBuffer,
//				int nBufferSize,
//				DWORD dwTimeout /*= DEFAULT_SEND_TIMEOUT*/ ) 
//	用  途：发送数据报
//	对全局变量的影响：无
//	参  数：
//		pszTo       : 目的地址
//		nToPort     : 目的端口
//		pszBuffer   : 数据缓冲区
//		nBufferSize : 缓冲区大小
//		dwTimeout   : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::SendTo(const char* pszTo,int nToPort,const char * pszBuffer, int nBufferSize, DWORD dwTimeout /*= DEFAULT_SEND_TIMEOUT*/)
{
	SOCKADDR_IN sockAddrTo;
	ZeroMemory(&sockAddrTo,sizeof(sockAddrTo));

    sockAddrTo.sin_family = AF_INET;
    sockAddrTo.sin_addr.s_addr = GetIP(pszTo,TRUE);
    sockAddrTo.sin_port = htons( (u_short) nToPort);
	
	if (sockAddrTo.sin_addr.s_addr == INADDR_NONE)
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
	}

	if( SendTo(m_hSocket,(SOCKADDR *)&sockAddrTo,sizeof(sockAddrTo),pszBuffer,nBufferSize,dwTimeout) < nBufferSize )
		return FALSE;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SetSockOpt(
//				int nOptionName,
//				const void* lpOptionValue,
//				int nOptionLen,
//				int nLevel /*= SOL_SOCKET*/ ) 
//	用  途：设置SOCKET的属性
//	对全局变量的影响：无
//	参  数：
//		nOptionName   : 属性名称
//		lpOptionValue : 属性值
//		nOptionLen    : 属性值的长度
//		nLevel        : 套接字属性级别
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::SetSockOpt( int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel /*= SOL_SOCKET*/ )
{
	if (setsockopt(m_hSocket, nLevel, nOptionName, (const char *) lpOptionValue,nOptionLen) == SOCKET_ERROR )
    {
		SetLastError( WSAGetLastError() );
        return FALSE;
    }
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL GetSockOpt(
//				int nOptionName,
//				void* lpOptionValue,
//				int* lpOptionLen,
//				int nLevel /*= SOL_SOCKET*/ ) 
//	用  途：获取SOCKET的属性
//	对全局变量的影响：无
//	参  数：
//		nOptionName   : 套接字属性名称
//		lpOptionValue : 保存套接字属性值
//		lpOptionLen   : 长度
//		nLevel        : 级别
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CBufSocket::GetSockOpt( int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel /*= SOL_SOCKET*/ )
{
	if( getsockopt(m_hSocket, nLevel, nOptionName, (char *)lpOptionValue, lpOptionLen) == SOCKET_ERROR )
	{
		SetLastError( WSAGetLastError() );
		return FALSE;
	}
	return TRUE;
}

