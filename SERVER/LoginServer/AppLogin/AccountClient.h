
/*
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-15
*/

#include "..\..\public\AccountServerDef.h"
#include "LoaclAccountValidate.h"

#pragma once


class AccountClient
{
public:
	//!	ValidateAccount()接口的返回值
	enum eValidateReturn
	{
		//! 等待AS验证
		eVR_WaitASValidate,
		//! 本地验证通过
		eVR_LocalPass,
		//! 本地验证失败
		eVR_LocalLose,
	};

public:
	AccountClient(VOID);
	~AccountClient(VOID);

public:
	//!						初始化
	BOOL					Init();

	//!						设置AS套接字
	VOID					SetASSocket(LONG lASSocket)		{m_lASSocket = lASSocket;};
	//!						得到AS套接字
	LONG					GetASSocket(VOID)				{return m_lASSocket;};
	
	//!						验证一个账号
	BOOL					ValidateAccount(tagAccountInfo_ToAS &AccountInfo);

	//!						在本地保存一个通过验证的账号
	VOID					SaveAccount(tagAccountInfo_FromAS &AccountInfo_FromAS, const tagSecurityData *pSecurityData);

	//!						封号
	VOID					RejectAccount(const char *pCdKey, LONG lEndTime);

#ifdef ENABLE_LOCAL_VALIDATE
	//!						本地验证
	BOOL					LocalValidate(tagAccountInfo_ToAS &AccountInfo);
#endif


private:
	
	//!								AS服务器套接字
	LONG							m_lASSocket;

#ifdef ENABLE_LOCAL_VALIDATE
	//!								保存数据缓冲
	LoaclAccountValidate			*m_pLoaclAccountValidate;
#endif

};