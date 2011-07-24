////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   GetProxySetting.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002.01.21  15:40
//  Comment     :   Kingsoft Windows Proxy Getting source file
//  
////////////////////////////////////////////////////////////////////////////////

#ifndef _GETPROXYSET_H
#define _GETPROXYSET_H  1


#define IEPROXY_NONE                        0x0000
#define IEPROXY_HTTP                        0x0001
#define IEPROXY_SOCKS                       0x0002
#define IEPROXY_FTP                         0x0004


int GetIEProxyValid();

int GetIEProxyHttp(int nSvrNameSize, char *pszProxySvrName, int *pnProxyPort);

int GetIEProxySocks5(int nSvrNameSize, char *pszProxySvrName, int *pnProxyPort);

int IsIEProxyEnable();


#endif  // #ifndef _GETPROXYSET_H

