

/*
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-15
*/

#include "..\nets\netAccount\message.h"

#pragma once

//#define APEX_TEST


//! 前置申明
///////////////////////////////////////////////////////////////////////////////////////////////////
const ULONG	CLIENT_NAME_SIZE = 256;

//!	客户端信息结构
struct tagClientInfo
{
	//!		指定IP
	DWORD	_dwIP;
	//!		指定编号
	DWORD	_dwNo;
	//!		客户端类型
	DWORD	_dwType;
	//!		名称
	CHAR	_szName[CLIENT_NAME_SIZE];

	//!		登陆状态
	DWORD	_dwLoginState;
	//!		当前socket
	DWORD	_dwCurrSocket;

	BOOL	_bAbleWork;

	//////
	tagClientInfo(VOID){memset(this, 0, sizeof(tagClientInfo));}
};



//! 客户端管理类
//!		身份验证、登陆管理、信息管理
///////////////////////////////////////////////////////////////////////////////////////////////////

class ClientManager
{
public:
	ClientManager(VOID);
	~ClientManager(VOID);


public:
	//!						初始化对象
	BOOL					Init(const char *pFilename);
	//!						重读客户端配置
	VOID					ReLoadClientSetup(const char *pFilename);
	//!						释放对象
	VOID					Release(VOID);


public:
	//!						验证一个客户端是否合法
	//!							成功时，dwRetrunInfo返回客户端类型
	//!							失败时，dwRetrunInfo返回登陆错误类型
	BOOL					OnClientLogin(DWORD dwIP, DWORD dwNo, DWORD dwSocket, DWORD &dwRetrunInfo);
	//!						响应客户端退出
	BOOL					OnClientLogout(DWORD dwSocket);

	//!						该客户端能否进行操作，返回客户端ID
	DWORD					ClientAbleWork(LONG lSocket);

#ifdef ENABLE_FCM
	//!						在线信息修正
	VOID					OnlineInfoRevise(DBReadSet &dbReadSet, LONG lSocket);
#endif

	//!						得到客户端的Socket
	LONG					GetClientSocket(DWORD dwNo);
	//!						得到客户端的Socket
	LONG					MultiGetClientSocket(DWORD dwNo);
	//!						得到客户端的类型
	DWORD					GetClientType(DWORD dwNo);


private:
	//!						读取客户端配置
	BOOL					LoadClientSetup(const char *pFilename);
	//!						重新检测当前在线客户端是否合法
	VOID					CheckOnlineClient(VOID);


private:
	typedef		map<DWORD, tagClientInfo>				mapClientInfo;
	typedef		map<DWORD, tagClientInfo>::iterator		iteClientInfo;

	typedef		map<DWORD, DWORD>						mapOnlineClient;
	typedef		map<DWORD, DWORD>::iterator				iteOnlineClient;

	//!						客户端信息列表（key：ClienteNo）
	mapClientInfo			m_mapClientInfo;
	//!						用于线程的客户端信息列表（key：ClienteNo）
	mapClientInfo			m_mapMultiClientInfo;
	//!						m_mapClientInfo的锁
	AsyncLock				m_LockForClientInfo;

	//!						在线客户端列表（key：Socket，value：ClienteNo）
	mapOnlineClient			m_mapOnlineClient;

	
	

};








