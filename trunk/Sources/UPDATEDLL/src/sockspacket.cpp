
////////////////////////////////////////////////////////////////////////////////
// SocksPacket.cpp: implementation of the CSocksPacket class.
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SocksPacket.h"

//////////////////////////////////////////////////////////////////////
//			构造函数
//////////////////////////////////////////////////////////////////////
CSocksPacket::CSocksPacket( CBufSocket* pBufSocket )
{
	m_pBufSocket		= pBufSocket;
	m_nRepPacketType	= 0;
}

//////////////////////////////////////////////////////////////////////
//			析构函数
//////////////////////////////////////////////////////////////////////
CSocksPacket::~CSocksPacket()
{

}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL Pack(
//				OUT char *pszDest,
//				PSOCKSREQPACKET pPacket,
//				int nPacketType ) 
//	用  途：将数据包打包成字符串
//	对全局变量的影响：无
//	参  数：
//		pszDest     : 字符串指针,用于保存打包数据(必须预先分配足够的内存空间)
//		pPacket     : 数据包指针(待打包的数据包)
//		nPacketType : 包类型
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::Pack(OUT char *pszDest,PSOCKSREQPACKET pPacket,int nPacketType)
{

	int n,nIndex=0;
	WORD  wData;
	DWORD dwData;

	switch( nPacketType )
	{
	case PACKET_SOCKS4REQ:
		pszDest[nIndex++] = (char)pPacket->socks4Req.byVer;
		pszDest[nIndex++] = (char)pPacket->socks4Req.byCmd;
		
		// 端口
		wData = htons(pPacket->socks4Req.wDestPort);
		pszDest[nIndex++] = (char)( wData & 0x00ff );
		pszDest[nIndex++] = (char)( (wData>>8) & 0x00ff);
		
		// IP地址(不需要再进行htonl转换,由inet_addr出来的IP地址已经是网络字节序)
		dwData = pPacket->socks4Req.dwDestIP;
		pszDest[nIndex++] = (char)( (dwData    ) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>8 ) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>16) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>24) &0x000000ff); 
		
		// UserID
		for( n = 0;n < (int)strlen(pPacket->socks4Req.pszUserID) ; n++ )
			pszDest[nIndex++] = (pPacket->socks4Req.pszUserID)[n];
		
		// NULL BYTE
		pszDest[nIndex++]	= (char)pPacket->socks4Req.byNULL;
		pszDest[nIndex]		= '\0';
		break;
	case PACKET_SOCKS4AREQ:
		pszDest[nIndex++] = (char)pPacket->socks4aReq.byVer;
		pszDest[nIndex++] = (char)pPacket->socks4aReq.byCmd;
	
		// 端口
		wData = htons(pPacket->socks4aReq.wDestPort);
		pszDest[nIndex++] = (char)( wData & 0x00ff );
		pszDest[nIndex++] = (char)( (wData>>8) & 0x00ff);
		
		// IP地址(不需要再进行htonl转换,由inet_addr出来的IP地址已经是网络字节序)
		dwData = pPacket->socks4aReq.dwDestIP;
		pszDest[nIndex++] = (char)( (dwData    ) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>8 ) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>16) &0x000000ff); 
		pszDest[nIndex++] = (char)( (dwData>>24) &0x000000ff); 

		// UserID
		for( n = 0;n < (int)strlen(pPacket->socks4aReq.pszUserID) ; n++ )
			pszDest[nIndex++] = (pPacket->socks4aReq.pszUserID)[n];
		
		// NULL1 BYTE
		pszDest[nIndex++]	= (char)pPacket->socks4aReq.byNULL1;

		// DestHostName
		for( n = 0;n < (int)strlen(pPacket->socks4aReq.pszDestHostName); n++ )
			pszDest[nIndex++] = (pPacket->socks4aReq.pszDestHostName)[n];
		
		// NULL2 BYTE
		pszDest[nIndex++]	= (char)pPacket->socks4aReq.byNULL2;
		pszDest[nIndex]		= '\0';
		break;
	case PACKET_SOCKS5AUTHREQ:
		pszDest[nIndex++]	= (char)pPacket->socks5AuthReq.byVer;
		pszDest[nIndex++]	= (char)pPacket->socks5AuthReq.byNAuth;
		for( n = 0; n < pPacket->socks5AuthReq.byNAuth;n++ )
			pszDest[nIndex++] = (pPacket->socks5AuthReq.pszAuth)[n];
		pszDest[nIndex]		= '\0';
		break;
	case PACKET_SOCKS5AUTHPASSWDREQ:
		pszDest[nIndex++]	= (char)pPacket->socks5AuthPasswdReq.byVer;
		pszDest[nIndex++]	= (char)pPacket->socks5AuthPasswdReq.byUserNameLen;

		for( n = 0 ; n < (int)strlen( pPacket->socks5AuthPasswdReq.pszUserName ); n++ )
			pszDest[nIndex++] = (pPacket->socks5AuthPasswdReq.pszUserName)[n];

		pszDest[nIndex++]	= (char)pPacket->socks5AuthPasswdReq.byPasswdLen;

		for( n = 0; n < (int)strlen(pPacket->socks5AuthPasswdReq.pszPasswd); n++ )
			pszDest[nIndex++] = (pPacket->socks5AuthPasswdReq.pszPasswd)[n];
		pszDest[nIndex]		= '\0';
		break;
	case PACKET_SOCKS5REQ:
		pszDest[nIndex++]	= (char)pPacket->socks5Req.byVer;
		pszDest[nIndex++]	= (char)pPacket->socks5Req.byCmd;
		pszDest[nIndex++]	= (char)pPacket->socks5Req.byRsv;
		pszDest[nIndex++]	= (char)pPacket->socks5Req.byAtyp;

		int nAddrLen;
		switch(pPacket->socks5Req.byAtyp)
		{
		case ATYP_IPV4ADDR:
			nAddrLen = 4;
		/*
			如果目标地址IP = "123.222.111.222"
			则传进来的参数socks5Req.pszDestAddr的形式应该是这样的:
			DWORD dwIP = inet_addr("123.222.111.222");
			char*  szAddr = new char[5];
			szAddr[0] = (char)( dwIP      & 0x000000ff);
			szAddr[1] = (char)((dwIP>>8 ) & 0x000000ff);
			szAddr[2] = (char)((dwIP>>16) & 0x000000ff);
			szAddr[3] = (char)((dwIp>>24) & 0x000000ff);
			szAddr[4] = '\0';
			
			socks5Req.pszDestAddr = szAddr;
		*/
			break;
		case ATYP_IPV6ADDR:
			nAddrLen = 16;
			break;
		case ATYP_HOSTNAME:
			nAddrLen =  1;
			nAddrLen += (BYTE) (pPacket->socks5Req.pszDestAddr[0]);
		/*
			如果目标地址DM = "www.yahoo.com"
			则传进来的参数socks5Req.pszDestAddr的形式应该是这样的:
			int nLen = strlen("www.yahoo.com");
			char* szAddr = new char[nLen+2];
			szAddr[0] = (char)nLen;			// 第一个字节保存域名的长度
			for( int i =0;i<nLen;i++)		// 从第二个字节开始才是真正的域名
				szAddr[i+1] = DM[i];
			szAddr[nLen+1] = '\0';

			socks5Req.pszDestAddr = szAddr;	
		*/
			break;
		default:
			nAddrLen = (int)strlen(pPacket->socks5Req.pszDestAddr);
			break;
		}

		for( n = 0; n <nAddrLen; n++ )
			pszDest[nIndex++] = (pPacket->socks5Req.pszDestAddr)[n];

		wData = htons(pPacket->socks5Req.wDestPort);
		pszDest[nIndex++] = (char)( wData & 0x00ff );
		pszDest[nIndex++] = (char)( (wData>>8) & 0x00ff);
		pszDest[nIndex]		= '\0';
		break;
	}

	//打包成功
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：int PacketSize(
//				PSOCKSREQPACKET pPacket,
//				int nPacketType ) 
//	用  途：计算包的尺寸
//	对全局变量的影响：无
//	参  数：
//		pPacket     : 数据包指针
//		nPacketType : PACKET类型
//	返回值：int，数据包长度
////////////////////////////////////////////////////////////////////////////////
int CSocksPacket::PacketSize(PSOCKSREQPACKET pPacket,int nPacketType)
{
	int nLen = 0;
	switch( nPacketType )
	{
	case PACKET_SOCKS4REQ:
		nLen = 9 + strlen(pPacket->socks4Req.pszUserID);
		break;
	case PACKET_SOCKS4AREQ:
		nLen = 10 + strlen(pPacket->socks4aReq.pszUserID) + strlen(pPacket->socks4aReq.pszDestHostName);
		break;
	case PACKET_SOCKS5AUTHREQ:
		nLen = 2 + (BYTE)pPacket->socks5AuthReq.byNAuth;
		break;
	case PACKET_SOCKS5AUTHPASSWDREQ:
		nLen = 3 + strlen(pPacket->socks5AuthPasswdReq.pszUserName) + strlen(pPacket->socks5AuthPasswdReq.pszPasswd);
		break;
	case PACKET_SOCKS5REQ:
		nLen = 6;
		switch( pPacket->socks5Req.byAtyp )
		{
		case ATYP_IPV4ADDR:
			nLen += 4;
			break;
		case ATYP_IPV6ADDR:
			nLen += 16;
			break;
		case ATYP_HOSTNAME:
			nLen += ((BYTE)pPacket->socks5Req.pszDestAddr[0]);
			nLen += 1;
			break;
		}
		break;
	}
	return nLen;
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendPacket(
//				PSOCKSREQPACKET pPacket,
//				int nPacketType,
//				DWORD dwTimeout/* = SOP_SEND_TIMEOUT*/ ) 
//	用  途：发送包
//	对全局变量的影响：无
//	参  数：
//		pPacket     : 数据包指针(发送数据包)
//		nPacketType : 数据包类型
//		dwTimeout   :  发送超时(缺省为SOP_SEND_TIMEOUT)
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::SendPacket(PSOCKSREQPACKET pPacket,int nPacketType,DWORD dwTimeout/* = SOP_SEND_TIMEOUT*/)
{
	// 计算包大小
	int		nSize		= PacketSize(pPacket,nPacketType);
	char*	pSendBuf	= (char *)malloc(nSize+1);

	if(pSendBuf == NULL)
		return FALSE;

	int nSend;
	BOOL bRet = FALSE;
	
	// 打包并发送
	if( Pack(pSendBuf,pPacket,nPacketType) )
	{
		nSend = m_pBufSocket->Send(pSendBuf,nSize,dwTimeout);
		bRet  = (nSend == nSize)? TRUE:FALSE;
	}

	free(pSendBuf);
	return (bRet);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendSocks4Req(
//				BYTE byCmd,
//				WORD wDestPort,
//				DWORD dwDestIP,
//				const char * pszUserID /* = NULL */,
//				DWORD dwTimeout /* = SOP_SEND_TIMEOUT */ ) 
//	用  途：发送Socks4请求
//	对全局变量的影响：无
//	参  数：
//		byCmd     : 命令
//		wDestPort : 目标端口
//		dwDestIP  : 目标IP
//		pszUserID : 用户ID
//		dwTimeout : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::SendSocks4Req(BYTE byCmd,WORD wDestPort,DWORD dwDestIP,const char * pszUserID /* = NULL */,DWORD dwTimeout /* = SOP_SEND_TIMEOUT */)
{
	SOCKSREQPACKET pack;
	pack.socks4Req.byVer		= VER_SOCKS4;
	pack.socks4Req.byCmd		= byCmd;
	pack.socks4Req.wDestPort	= wDestPort;
	pack.socks4Req.dwDestIP		= dwDestIP;
	pack.socks4Req.pszUserID	= pszUserID;
	pack.socks4Req.byNULL		= BYTE_NULL;
	
	return SendPacket(&pack,PACKET_SOCKS4REQ,dwTimeout);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendSocks4aReq(
//				BYTE byCmd,
//				WORD wDestPort,
//				const char* pszDestHostName,
//				const char * pszUserID /* = NULL */,
//				DWORD dwTimeout /* = SOP_SEND_TIMEOUT */ ) 
//	用  途：发送Socks4a请求
//	对全局变量的影响：无
//	参  数：
//		byCmd           : 命令
//		wDestPort       : 端口
//		pszDestHostName : 主机名
//		pszUserID       : 用户ID
//		dwTimeout       : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL  CSocksPacket::SendSocks4aReq(BYTE byCmd,WORD wDestPort,const char* pszDestHostName,const char * pszUserID /* = NULL */,DWORD dwTimeout /* = SOP_SEND_TIMEOUT */)
{
	SOCKSREQPACKET pack;
	pack.socks4aReq.byVer			= VER_SOCKS4A;
	pack.socks4aReq.byCmd			= byCmd;
	pack.socks4aReq.wDestPort		= wDestPort;
	pack.socks4aReq.dwDestIP		= UNKNOWN_IP;
	pack.socks4aReq.pszUserID		= pszUserID;
	pack.socks4aReq.byNULL1			= BYTE_NULL;
	pack.socks4aReq.pszDestHostName = pszDestHostName;
	pack.socks4aReq.byNULL2			= BYTE_NULL;

	return SendPacket(&pack,PACKET_SOCKS4AREQ,dwTimeout);
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendSocks5AuthReq(
//				BYTE byNAuth,
//				const char* pszAuth,
//				DWORD dwTimeout /* = SOP_SEND_TIMEOUT */ ) 
//	用  途：发送SOCKS5认证请求 
//	对全局变量的影响：无
//	参  数：
//		byNAuth   : 认证方式数量
//		pszAuth   : 认证方式
//		dwTimeout : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL  CSocksPacket::SendSocks5AuthReq(BYTE byNAuth,const char* pszAuth,DWORD dwTimeout /* = SOP_SEND_TIMEOUT */)
{
	SOCKSREQPACKET pack;
	pack.socks5AuthReq.byVer		= VER_SOCKS5;
	pack.socks5AuthReq.byNAuth		= byNAuth;
	pack.socks5AuthReq.pszAuth		= pszAuth;

	return SendPacket(&pack,PACKET_SOCKS5AUTHREQ,dwTimeout);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendSocks5AuthPasswdReq(
//				const char* pszUserName,
//				const char * pszPassword,
//				DWORD dwTimeout /* = SOP_SEND_TIMEOUT */ ) 
//	用  途：发送SOCKS5 USERNAME/PASSWORD验证请求
//	对全局变量的影响：无
//	参  数：
//		pszUserName : 用户名
//		pszPassword : 口令
//		dwTimeout   : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL  CSocksPacket::SendSocks5AuthPasswdReq(const char* pszUserName,const char * pszPassword,DWORD dwTimeout /* = SOP_SEND_TIMEOUT */)
{
	SOCKSREQPACKET pack;
	pack.socks5AuthPasswdReq.byVer			= 0x01;
	pack.socks5AuthPasswdReq.byUserNameLen	= (BYTE)( (pszUserName == NULL)?0:strlen(pszUserName) );
	pack.socks5AuthPasswdReq.pszUserName	= pszUserName;
	pack.socks5AuthPasswdReq.byPasswdLen	= (BYTE)( (pszPassword == NULL)?0:strlen(pszPassword) );
	pack.socks5AuthPasswdReq.pszPasswd		= pszPassword;

	return SendPacket(&pack,PACKET_SOCKS5AUTHPASSWDREQ,dwTimeout);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL SendSocks5Req(
//				BYTE byCmd,
//				BYTE byAtyp,
//				const char * pszDestAddr,
//				WORD wDestPort,
//				DWORD dwTimeout /* = SOP_SEND_TIMEOUT */ ) 
//	用  途：发送SOCKS5请求
//	对全局变量的影响：无
//	参  数：
//		byCmd       : 命令
//		byAtyp      : 地址类型
//			ATYP_IPV4ADDR	pszDestAddr = IPV4_Address (4 Octects)
//			ATYP_IPV6ADDR	pszDestAddr = IPV6_Address (16 Octects) 	
//			ATYP_HOSTNAME	pszDestAddr = [1BYTE:(HostName)Length]+[HostName]	
//		pszDestAddr : 目标地址
//		wDestPort   : 目标端口
//		dwTimeout   : 超时
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL  CSocksPacket::SendSocks5Req(BYTE byCmd,BYTE byAtyp,const char * pszDestAddr,WORD wDestPort,DWORD dwTimeout /* = SOP_SEND_TIMEOUT */)
{
	SOCKSREQPACKET pack;
	pack.socks5Req.byVer		= VER_SOCKS5;
	pack.socks5Req.byCmd		= byCmd;
	pack.socks5Req.byRsv		= BYTE_NULL;
	pack.socks5Req.byAtyp		= byAtyp;
	pack.socks5Req.pszDestAddr	= pszDestAddr;
	pack.socks5Req.wDestPort	= wDestPort;
	return SendPacket(&pack,PACKET_SOCKS5REQ,dwTimeout);
}


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL IsSocksOK(
//				PSOCKSREPPACKET pPacket,
//				int nPacketType ) 
//	用  途：检查SOCKS的返回值是否正确
//	对全局变量的影响：无
//	参  数：
//		pPacket     : 数据包
//		nPacketType : 数据包类型
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::IsSocksOK( PSOCKSREPPACKET pPacket,int nPacketType )
{
	switch ( nPacketType )
	{
	case PACKET_SOCKS4REP:
		return ( pPacket->socks4Rep.byRep == REP4_SUCCESS );
		break;
	case PACKET_SOCKS4AREP:
		return ( pPacket->socks4aRep.byRep == REP4_SUCCESS );
		break;
	case PACKET_SOCKS5AUTHREP:
		return ( pPacket->socks5AuthRep.byAuth == AUTH_NONE	 ||
				 pPacket->socks5AuthRep.byAuth == AUTH_GSSAPI ||
				 pPacket->socks5AuthRep.byAuth == AUTH_PASSWD ||
				 pPacket->socks5AuthRep.byAuth == AUTH_CHAP ) ;
		break;
	case PACKET_SOCKS5AUTHPASSWDREP:
		return ( pPacket->socks5AuthPasswdRep.byStatus == AUTH_OK );
		break;
	case PACKET_SOCKS5REP:
		return ( pPacket->socks5Rep.byRep == REP5_SUCCESS );
		break;
	default:
		return FALSE;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL RecvPacket(
//				OUT PSOCKSREPPACKET pPacket,
//				int nPacketType,
//				DWORD dwTimeout /*= SOP_RECV_TIMEOUT*/ ) 
//	用  途：接收并解包
//	对全局变量的影响：无
//	参  数：
//		pPacket     : 数据包指针(用来保存接收数据包)
//		nPacketType : 包类型
//		dwTimeout   : 接收超时(缺省为SOP_RECV_TIMEOUT)
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::RecvPacket( OUT PSOCKSREPPACKET pPacket,int nPacketType,DWORD dwTimeout /*= SOP_RECV_TIMEOUT*/)
{
	int		nRecv,n,nCount;
	WORD	wData	= 0;
	DWORD	dwData	= 0;
	char*	pszData	= NULL;

	// 保存类型
	m_nRepPacketType = nPacketType;

	//接收数据
	switch( nPacketType )
	{
	case PACKET_SOCKS4REP:
		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks4Rep.byVer = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks4Rep.byRep = (BYTE)nRecv;
		
		wData = 0;
		for( n = 0; n < 2; n++)
		{
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;
		
			wData |= ( ((WORD)nRecv) << (8*n) );
		}
		pPacket->socks4Rep.wDestPort =ntohs(wData);

		dwData = 0;
		for( n = 0; n < 4; n++)
		{
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;

			dwData |= ( ((DWORD)nRecv)<<(8*n) );
		}
		pPacket->socks4Rep.dwDestIP = ntohl(dwData);
		break;
	case PACKET_SOCKS4AREP:
		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks4aRep.byVer = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks4aRep.byRep = (BYTE)nRecv;
		
		wData = 0;
		for( n = 0; n < 2; n++)
		{
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;
		
			wData |= ( ((WORD)nRecv) << (8*(n)) );
		}
		pPacket->socks4aRep.wDestPort = ntohs(wData);

		dwData = 0;
		for( n = 0; n < 4; n++)
		{
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;

			dwData |= ( ((DWORD)nRecv)<<(8*(n)) );
		}
		pPacket->socks4aRep.dwDestIP = ntohl(dwData);
		break;
	case PACKET_SOCKS5AUTHREP:
		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5AuthRep.byVer = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		
		pPacket->socks5AuthRep.byAuth = (BYTE)nRecv;
		break;
	case PACKET_SOCKS5AUTHPASSWDREP:
		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5AuthPasswdRep.byVer = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		
		pPacket->socks5AuthPasswdRep.byStatus = (BYTE)nRecv;
		break;
	case PACKET_SOCKS5REP:
		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5Rep.byVer = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5Rep.byRep = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5Rep.byRsv = (BYTE)nRecv;

		nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
		if( nRecv == INVALID_CHAR )
			return FALSE;
		pPacket->socks5Rep.byAtyp = (BYTE)nRecv;

		if( pPacket->socks5Rep.byRep != REP5_SUCCESS )
		{
			pPacket->socks5Rep.pszBindAddr = NULL;
			pPacket->socks5Rep.wBindPort   = WORD_NULL;
			break;
		}
		
		switch( pPacket->socks5Rep.byAtyp )
		{
		case ATYP_IPV4ADDR:
			pszData = (char *)malloc(5);
			if(pszData == NULL)
				return FALSE;
			for( n = 0; n < 4; n++)
			{
				nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
				if( nRecv == INVALID_CHAR )
					return FALSE;

				pszData[n] = (char)nRecv;
			}
			pszData[n] = '\0';
			pPacket->socks5Rep.pszBindAddr = pszData;
			break;
		case ATYP_IPV6ADDR:
			
			pszData = (char *)malloc(17);
			if(pszData == NULL)
				return FALSE;
			for( n = 0; n < 16; n++)
			{
				nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
				if( nRecv == INVALID_CHAR )
					return FALSE;

				pszData[n] = (char)nRecv;
			}
			pszData[n] = '\0';
			pPacket->socks5Rep.pszBindAddr = pszData;
			break;
		case ATYP_HOSTNAME:
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;
			
			nCount = nRecv;
			pszData = (char *)malloc(nCount+1 );
			if(pszData == NULL)
				return FALSE;
			for( n = 0; n < nCount; n++)
			{
				nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
				if( nRecv == INVALID_CHAR )
					return FALSE;

				pszData[n] = (char)nRecv;
			}
			pszData[n] = '\0';
			pPacket->socks5Rep.pszBindAddr = pszData;
			break;
		default:
			pPacket->socks5Rep.pszBindAddr = NULL;
			break;
		}

		wData = 0;
		for( n = 0; n < 2; n++)
		{
			nRecv = m_pBufSocket->BSDGetChar(dwTimeout);
			if( nRecv == INVALID_CHAR )
				return FALSE;
		
			wData |= ( ((WORD)nRecv) << (8*n) );
		}
		pPacket->socks5Rep.wBindPort = ntohs(wData);
		break;
	default:
		return FALSE;
		break;
	}

	return TRUE;
}		


////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL RecvPacket(
//				int nPacketType,
//				DWORD dwTimeout /*= SOP_RECV_TIMEOUT*/ ) 
//	用  途：接收并解包
//	对全局变量的影响：无
//	参  数：
//		nPacketType : 包类型
//		dwTimeout   : 接收超时(缺省为SOP_RECV_TIMEOUT)
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::RecvPacket( int nPacketType,DWORD dwTimeout /*= SOP_RECV_TIMEOUT*/)
{
	ZeroMemory( &m_stRepPacket,sizeof(SOCKSREPPACKET) );
	return RecvPacket( &m_stRepPacket,nPacketType,dwTimeout );
}

////////////////////////////////////////////////////////////////////////////////
//	函数名：BOOL IsSocksOK()
//	用  途：检查SOCKS的返回值是否正确
//	对全局变量的影响：无
//	参  数：无
//	返回值：BOOL
////////////////////////////////////////////////////////////////////////////////
BOOL CSocksPacket::IsSocksOK( )
{
	return IsSocksOK(&m_stRepPacket,m_nRepPacketType);
}