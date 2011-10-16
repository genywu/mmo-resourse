#include "stdafx.h"
#include ".\ping.h"

//Ping.h
//Fun:用ICMP协议来完成Ping功能
//Author:Joe
//Create Time:2005.2.5

CPing::CPing(void)
{
	Initial();
}

CPing::~CPing(void)
{
	Release();
}

//初始化
void CPing::Initial()
{
	icmp_data = NULL;
	recvbuf = NULL;
	datasize = DEF_PACKET_SIZE;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup() failed: %d\n", GetLastError());
		return ;
	}

	m_hSocket = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,
		WSA_FLAG_OVERLAPPED);
	if(m_hSocket == SOCKET_ERROR) 
	{
		char strJoe[300]="";
		sprintf(strJoe,"Error:%d.",WSAGetLastError());
	}
}

void CPing::Release()
{
	if (m_hSocket != INVALID_SOCKET) 
		closesocket(m_hSocket);
	HeapFree(GetProcessHeap(), 0, recvbuf);
	HeapFree(GetProcessHeap(), 0, icmp_data);

	WSACleanup();
}

long CPing::Ping(char *strDest,int nCount,int timeout,int nDataSize)
{   
	long lRet = -1;
	if (m_hSocket == INVALID_SOCKET) 
	{
		return lRet;
	}

	datasize = nDataSize;
	// 设置接受和发送的超时值
	//
	int bread = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, 
		(char*)&timeout, sizeof(timeout));
	if(bread == SOCKET_ERROR) 
	{
		return lRet;
	}
	
	bread = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, 
		(char*)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR) 
	{
		return lRet;
	}

	memset(&m_addrDest, 0, sizeof(m_addrDest));
	//
	//转换
	//
	m_addrDest.sin_family = AF_INET;
	if ((m_addrDest.sin_addr.s_addr = inet_addr(strDest)) == INADDR_NONE)
	{   
		struct hostent *hp = NULL;

		if ((hp = gethostbyname(strDest)) != NULL)
		{
			memcpy(&(m_addrDest.sin_addr), hp->h_addr, hp->h_length);
			m_addrDest.sin_family = hp->h_addrtype;
		}
		else
		{
			return lRet;
		}
	}        

	// 
	// 创建一个ICMP数据报
	//       
	datasize += sizeof(IcmpHeader);  

	icmp_data =(char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		MAX_PACKET);
	recvbuf =(char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		MAX_PACKET);
	if (!icmp_data) 
	{
		return lRet;
	}
	memset(icmp_data,0,MAX_PACKET);
	FillICMPData(icmp_data,datasize);
	//
	// 开始接受和发送数据报
	//

	for(int i=0;i < nCount; i++) 
	{
		((IcmpHeader*)icmp_data)->i_cksum = 0;
		((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
		((IcmpHeader*)icmp_data)->i_cksum = 
			checksum((USHORT*)icmp_data, datasize);

		int bwrote = sendto(m_hSocket, icmp_data, datasize, 0, 
			(struct sockaddr*)&m_addrDest, sizeof(m_addrDest));
		if (bwrote == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT) 
			{
				continue;
			}
			return lRet;
		}
		
		int fromlen = sizeof(m_addrFrom);
		bread = recvfrom(m_hSocket, recvbuf, MAX_PACKET, 0, 
			(struct sockaddr*)&m_addrFrom, &fromlen);
		if (bread == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT) 
			{
				continue;
			}
			return lRet;
		}
		lRet = DecodeICMPHeader(recvbuf, bread, &m_addrFrom);
		if( lRet != -1)
		{
			return lRet;
		}
	}
	return lRet;
}

//填充CMPData结构
void CPing::FillICMPData(char *icmp_data, int datasize)
{
	IcmpHeader *icmp_hdr = NULL;
	char       *datapart = NULL;

	icmp_hdr = (IcmpHeader*)icmp_data;
	icmp_hdr->i_type = ICMP_ECHO;        // 请求一个 ICMP echo
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
	icmp_hdr->i_cksum = 0;
	datapart = icmp_data + sizeof(IcmpHeader);
}


USHORT CPing::checksum(USHORT *buffer, int size)
{
	unsigned long cksum=0;

	while (size > 1) 
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size) 
	{
		cksum += *(UCHAR*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}

//解压返回的ICMP头数居
long CPing::DecodeICMPHeader(char *buf, int bytes, SOCKADDR_IN *from)
{
	long lRet = -1;
	IpHeader       *iphdr = NULL;
	IcmpHeader     *icmphdr = NULL;
	unsigned short  iphdrlen;
	DWORD           tick;
	
	iphdr = (IpHeader *)buf;
	// Number of 32-bit words * 4 = bytes
	iphdrlen = iphdr->h_len * 4;
	tick = GetTickCount();


	if (bytes  < iphdrlen + ICMP_MIN) 
	{
		printf("Too few bytes from %s\n", 
			inet_ntoa(from->sin_addr));
	}

	icmphdr = (IcmpHeader*)(buf + iphdrlen);

	if (icmphdr->i_type != ICMP_ECHOREPLY) 
	{
		return lRet;
	}
	// 保证恢复的是是我们发的信息
	//
	if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) 
	{
		printf("someone else's packet!\n");
		return lRet; ;
	}

	lRet = tick - icmphdr->timestamp;
	return lRet;;
}