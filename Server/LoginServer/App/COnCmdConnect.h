/**
2009/07/08
张科智

简单命令连接类：
	-允许指定的IP进行多个一次性的短连接
	-响应短连接发送过来的字符串命令
*/
#pragma once
#include <winsock2.h>


struct tagGsInfo
{
	bool bGsWorking;
	LONG lLSID;
	LONG lWSID;
	LONG lGSID;
	LONG lGSPlayerNum;

	tagGsInfo(VOID){memset(this, 0, sizeof(tagGsInfo));}
};

class COnCmdConnect
{
public:
	COnCmdConnect(VOID);
	~COnCmdConnect(VOID);
public:
	//!					初始化
	BOOL				Init(VOID);
	//!					释放对象
	VOID				Release(VOID);
	//!					开始监视
	static	LPVOID		Watch(LPVOID pThis);
	//!					响应连接
	VOID				OnConnect(SOCKET sClient, sockaddr_in &client);

	//!
	VOID				ClearGsInfo(){m_Lock.Lock();m_vGsInfo.clear();m_Lock.UnLock();}
	//!
	VOID				AddGsInfo(tagGsInfo &GsInfo){m_Lock.Lock();m_vGsInfo.push_back(GsInfo);m_Lock.UnLock();}
private:
	//!					用于标识LS的外部编号
	string				m_strNumber;
	//!					监视的端口
	u_short				m_usPort;
	//!					最大连接数
	set<DWORD>			m_setIP;

	bool				m_bWorking;
	SOCKET				m_sListen;

	
	vector<tagGsInfo>	m_vGsInfo;
	LockOfWin32			m_Lock;
};