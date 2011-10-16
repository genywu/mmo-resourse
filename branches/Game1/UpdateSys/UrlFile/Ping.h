#pragma once
//////////////////////////////////////////////////////////////////////////
//Ping.h
//Fun:用ICMP协议来完成Ping功能
//Author:Joe
//Create Time:2005.2.5
#include <winsock2.h>
#include <ws2tcpip.h>

#define IP_RECORD_ROUTE  0x7
#define DEF_PACKET_SIZE  16 
#define MAX_PACKET       1024      // Max ICMP packet size
#define MAX_IP_HDR_SIZE  60        // Max IP header size w/options
#define ICMP_ECHO        8
#define ICMP_ECHOREPLY   0
#define ICMP_MIN         8 // Minimum 8-byte ICMP packet (header)

typedef struct _iphdr 
{
	unsigned int   h_len:4;        // Length of the header
	unsigned int   version:4;      // Version of IP
	unsigned char  tos;            // Type of service
	unsigned short total_len;      // Total length of the packet
	unsigned short ident;          // Unique identifier
	unsigned short frag_and_flags; // Flags
	unsigned char  ttl;            // Time to live
	unsigned char  proto;          // Protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum

	unsigned int   sourceIP;
	unsigned int   destIP;
} IpHeader;

typedef struct _icmphdr 
{
	BYTE   i_type;
	BYTE   i_code;                 // Type sub code
	USHORT i_cksum;
	USHORT i_id;
	// This is not the standard header, but we reserve space for time
	ULONG  timestamp;
} IcmpHeader;



class CPing
{
public:
	CPing(void);
	~CPing(void);
	void Initial();
	void Release();
	long Ping(char *strDest,int nCount = 2, int timeout =1000,int nDataSize = DEF_PACKET_SIZE);
	SOCKET m_hSocket;
	SOCKADDR_IN m_addrDest;
	SOCKADDR_IN m_addrFrom;
	char              *icmp_data;
	char              *recvbuf;
	int   datasize;

private:
	long DecodeICMPHeader(char *buf, int bytes, SOCKADDR_IN* from);
	USHORT checksum(USHORT *buffer, int size);
	void FillICMPData(char *icmp_data, int datasize);

};
