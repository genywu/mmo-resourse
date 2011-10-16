
/*
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-21
*/

#include "ValidateOperator.h"

#pragma once

//! 执行防沉迷查询的方法
enum eLoginAndLogoutType
{
	eLALT_Logout	= 0,	//! 登陆账号
	eLALT_Login		= 1,	//! 退出账号
	eLALT_GetTime	= 2,	//! 无变化账号，只是重新计算时间
};


class OtherOperator
{
public:
	static		VOID					Release(VOID);
public:
	//!									创建一个封号操作
	static		OtherOperator*			CreateRejectOperator(const char* pCdKey, LONG lEndTime);

#ifdef ENABLE_FCM
	//!									创建一个用户上下线操作
	static		OtherOperator*			CreateLoginAndLogout(const char* pCdKey, LONG lLSID, eLoginAndLogoutType eLogin);
#endif
	//!									销毁操作
	static		VOID					ReleaseOperator(OtherOperator **ppOperator);

	///////////////////////////////////////////////////////////
	//! 异步数据库操作
public:
	//!									执行封号操作
	VOID								RejectAccount(VOID);
#ifdef ENABLE_FCM
	//!									记录上下线，获得防沉迷时间（秒）
	VOID								LoginAndLogout(VOID);
#endif

	///////////////////////////////////////////////////////////
	//! 同步数据库操作
public:


	//!									执行安全验证操作
	static		BOOL					UserSecurityOperate(const char* pCdKey, DWORD dwIp, LONG &lWorkFlag, char arrSecurityCard[]);
#ifdef ENABLE_FCM
	//!									用户上下线处理
	static		LONG					UserLoginAndLogout(const char* pCdKey, LONG lLSID, DWORD dwLogin, set<DWORD> &setLSID);
	//!									从DB中获得一个LS上的在线用户
	static		BOOL					GetLsOnlineAccount(LONG lLSID, set<string> &setAccount);
#endif

	//! UserLoginAndLogout（）的返回值定义
	enum eUserLoginAndLogoutRe
	{
		//! 存储过程定义的返回值
		eULALR_RejoinLs = -20,	//! 错误，多次登陆或退出同一个LS
		eULALR_NoLimit = -10,	//! 没有时间限制
		
		//! 程序自定义的值
		eULALR_RunLose = -9999,
	};

	
private:
	//!									封号的数据操作
	static		LPVOID					RejectAccountFromDB(LPVOID pThis);
#ifdef ENABLE_FCM
	//!									用户上下线
	static		LPVOID					LoginAndLogoutFromDB(LPVOID pThis);
#endif

private:
	//! 封号操作的数据
	struct tagRejectData
	{
		char		_szCdKey[CDKEY_SIZE];
		LONG		_lEndTime;
	};

	//! 上下线的数据
	struct tagLoginAndlogout
	{
		char		_szCdKey[CDKEY_SIZE];
		LONG		_lLSID;
		DWORD		_dwSecond;
		DWORD		_dwLogin;
		set<DWORD>	_setLSID;
	};


	enum eDateType
	{
		eDT_NoData,
		eDT_RejectData,
		eDT_LoginAndlogout,
	};

private:
	//!									数据指针
	LPVOID								m_pData;
	//!									数据类型
	DWORD								m_dwDataType;

//! 不允许构造和拷贝
private:
	OtherOperator(void);
	OtherOperator(OtherOperator&);
	~OtherOperator(void);
};