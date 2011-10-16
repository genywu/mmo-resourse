
/*
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-16
*/


#include "..\..\..\dbaccess\CnPool.h"

#pragma once




class ValidateOperator
{
public:
	ValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwSocket, DWORD dwLSID);
	virtual ~ValidateOperator(VOID);

public:
	static		BOOL				Init(LONG lASID);
	static		VOID				Release(VOID);
	static		ValidateOperator*	CreateValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwSocket, DWORD dwLSID);
	static		VOID				ReleasValidateOperator(ValidateOperator **ppRVO);

	static		CnPool*				GetCnPool(VOID){return c_pCnPool;}
	static		CnPool*				GetSecurityCnPool(VOID){return c_pSecurityPool;}
#ifdef ENABLE_FCM
	static		CnPool*				GetFcmCnPool(VOID){return c_pFcmCnPool;}
#endif
	
public:
	//!								开始执行
	BOOL							BeginValidate(VOID);
	//!								发送操作结果
	VOID							SendResult(VOID);
	//!								发送带安全数据的操作结果
	VOID							SendResult(tagSecurityData &SecurityData);

private:
	//!								提交在数据库中验证账号的请求
	static		LPVOID				ValidateAccountFromDB(LPVOID pThis);	
	//!								处理验证结果
	static		LPVOID				OnValidateFinish(LPVOID pThis);

private:
	//!															账号数据库连接
	static		CnPool											*c_pCnPool;
	//!															安全数据库连接
	static		CnPool											*c_pSecurityPool;
	//!															防沉迷数据库连接
	static		CnPool											*c_pFcmCnPool;

	static		LONG											c_lASID;

	///////////////////////////////////////////////////////////////////
#ifdef APEX_TEST

public:
	static	DWORD					GetValidateNum(VOID){return c_lValidateNum;}

private:
	static	LONG					c_lValidateNum;

#endif
	///////////////////////////////////////////////////////////////////


private:
	tagAccountInfo_FromAS			m_AccountInfo_FromAS;
	DWORD							m_dwSocket;
	DWORD							m_dwLSID;



	

};