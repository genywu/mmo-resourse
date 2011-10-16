/**
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-16
*/
#pragma once
#include "../../../Public/ServerPublic/db/CnPool.h"
#include "../../../Public/Common/AccountServerDef.h"

class LoaclAccountValidate
{
public:
	virtual ~LoaclAccountValidate(VOID);

private:
	LoaclAccountValidate(VOID);
	LoaclAccountValidate(tagAccountInfo_ToAS &AccountInfo_ToAS);
	LoaclAccountValidate(vector<tagAccountInfo_FromAS> *pVSaveData);

public:
	static		BOOL					Init(string strConnectionString);
	static		VOID					Release(VOID);
	static		LoaclAccountValidate*	CreateValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS);
	static		LoaclAccountValidate*	CreateSaveOperator(VOID);
	static		VOID					ReleasValidateOperator(LoaclAccountValidate **ppRVO);


public:
	//!									开始执行
	BOOL								BeginValidate(VOID);
	//!									保存
	BOOL								SaveResult(VOID);

	BOOL								AddSaveItem(tagAccountInfo_FromAS &AccountInfo_FromAS, const tagSecurityData *pCardData);


private:
	//!									提交在数据库中验证账号的请求
	static		LPVOID					ValidateAccountFromDB(LPVOID pThis);
	//!									提交在数据库中验证账号的请求
	static		LPVOID					SaveASResult(LPVOID pThis);

private:
	//!									对象能否执行
	static		BOOL					c_bAbleWork;

	static		CnPool					*c_pCnPool;
	//!
	static		MultiObjPool<LoaclAccountValidate, LockOfWin32>		c_MultiObjPool;

private:
	struct tagSaveData
	{
		tagAccountInfo_FromAS	_AccountData;
		tagSecurityData			_CardData;

		tagSaveData(VOID){memset(this, 0, sizeof(tagSaveData));}
		explicit tagSaveData(tagAccountInfo_ToAS toAS):_AccountData(toAS){memset(&_CardData, 0, sizeof(tagSecurityData));}
	};

	tagSaveData							m_ValidateData;
	vector<tagSaveData>					m_vSaveData;




};