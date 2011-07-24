////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   IpSocket.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-16  16:41:53
//  Comment     :   Tcp/ip socket class for internet header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPSOCKET_H__EFE2FDC0_7D71_49C3_A26E_43A90A48389D__INCLUDED_)
#define AFX_IPSOCKET_H__EFE2FDC0_7D71_49C3_A26E_43A90A48389D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>

//Socket datatype redefinition
typedef SOCKET                              HSOCKET;
typedef struct sockaddr_in                  INET_ADDRESS, *PINET_ADDRESS;

//Socket Types same as SOCK_STREAM/SOCK_DGRAM/SOCK_RAW
#define SOCK_UNKNOWN                        0


class CIPSocket
{
public:
    CIPSocket();
    virtual ~CIPSocket();
    
public:
    virtual int Create();
    virtual void Destroy();
    
    virtual int Attach(HSOCKET hInetSocket);
    virtual HSOCKET Detach();
    
    int IsSocketOK();
    HSOCKET GetSafeHandle();
    
public:
    int GetLastError();
    int SetLastError(int nErrorNum);
    
    int GetProtocol();
    int SetProtocol(int nProtocol = IPPROTO_IP);
    
    int GetWorkFlags();
    int SetWorkFlags(int nSocketFlags = 0);
    
    int GetLocalAddress(INET_ADDRESS *psaAddrLocal);
    int GetRemoteAddress(INET_ADDRESS *psaAddrRemote);
    
public:
    int SetBlockMode(int nIsBlock = false);
    int Select(int nReadReady, int nWriteReady, int nErrorReady, int nTimeOut = 0);
    
    int Bind(const INET_ADDRESS *pAddrLocal);
    
    int Send(int nBufSize, const unsigned char *pbyBuffer);
    int Receive(int nBufSize, unsigned char *pbyBuffer);
    
    int SendTo(int nBufSize, const unsigned char *pbyBuffer, const INET_ADDRESS *pAddrTo);
    int ReceiveFrom(int nBufSize,  unsigned char *pbyBuffer, const INET_ADDRESS *pAddrFrom);
    
    int ShutDown(int nModeHow);
    
public:
    static int GetSocketAddress(const char *pcszAddr, INET_ADDRESS *pSocketAddr);
    
    static int GetLocalAddress(HSOCKET hInetSocket, INET_ADDRESS *pAddrLocal);
    static int GetRemoteAddress(HSOCKET hInetSocket, INET_ADDRESS *pAddrRemote);
    
protected:
    int CheckError(int nRetValue);
    
protected:
    virtual void OnSendHandler(int nBufSize, const unsigned char *pbySendBuf);
    virtual void OnReceiveHandler(int nBufSize, const unsigned char *pbyReceiveBuf);
    
    virtual void OnErrorHandler();
    
protected:
    int m_nLastError;
    int m_nLastResult;
    
    int m_nBlockMode;
    
    int m_nSckType;
    int m_nSckProto;
    int m_nSckFlags;
    
    HSOCKET m_hSckHandle;
};

#endif // !defined(AFX_IPSOCKET_H__EFE2FDC0_7D71_49C3_A26E_43A90A48389D__INCLUDED_)
