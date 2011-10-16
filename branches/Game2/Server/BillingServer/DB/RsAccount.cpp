#include "StdAfx.h"
#include "RsAccount.h"

#include "../Server/Game.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CRITICAL_SECTION CRsAccount::m_LockCS;

CRsAccount::CRsAccount(void)
{
	InitializeCriticalSection(&m_CreateAccountCS);
	InitializeCriticalSection(&m_LockCS);
	InitializeCriticalSection(&m_UnlockCS);
	InitializeCriticalSection(&m_RollbackCS);
}

CRsAccount::~CRsAccount(void)
{
	DeleteCriticalSection(&m_CreateAccountCS);
	DeleteCriticalSection(&m_LockCS);
	DeleteCriticalSection(&m_UnlockCS);
	DeleteCriticalSection(&m_RollbackCS);
}

BOOL CRsAccount::CreateNewAccount(_ConnectionPtr& cn,const char* cdkey,int nBalance,int nFrozen)
{
	if(nBalance<0)
		nBalance=0;
	if(nFrozen<0)
		nFrozen=0;

	//_ConnectionPtr cn;
	char sql[256];
	//if(!CreateCn(cn))
		//return FALSE;
	try
	{
		//TESTHR( OpenCn(cn) ); // 打开目标数据库失败
		EnterCriticalSection(&m_CreateAccountCS);
		/*
		sprintf( sql,"INSERT INTO account(cdkey,balance,frozen) VALUES('%s',%d,%d)",
			cdkey, nBalance,nFrozen );*/
		sprintf( sql,"INSERT INTO d_billing_info(user_account,charge_money,charge_value,consume_value,balance_value,lock_value,game_id) VALUES('%s',%d,%d,%d,%d,%d,%d)",
			cdkey,2000000,2000000,0,2000000,0,1);
		TESTHR( ExecuteCn(sql, cn) );
		LeaveCriticalSection(&m_CreateAccountCS);
		//cn->Close();		
		return TRUE;
	}
	catch(_com_error &e)
	{		
		LeaveCriticalSection(&m_CreateAccountCS);
		PrintErr("*** 数据库错误[创建新的账号时] ***", e);
		return FALSE;
	}	
	return FALSE;
}

DWORD CRsAccount::GetBalance(_ConnectionPtr& cn,const char* cdkey)
{
	char sql[256];	

	_RecordsetPtr rs;
	
	CreateRs(rs);
	try
	{		
		//sprintf(sql,"SELECT cdkey,balance,frozen FROM account WHERE cdkey='%s'", cdkey); //cdkey is PK
		sprintf(sql,"SELECT balance_value,lock_value FROM d_billing_info WHERE user_account='%s'", cdkey); 
		OpenRs(sql, rs, cn);

		if( !rs->GetadoEOF() )
		{			
			int nBalance=(int)rs->GetCollect("balance_value");
			int nFrozen=(int)rs->GetCollect("lock_value");
			nBalance-=nFrozen;
			if(nBalance<0)
				nBalance=0;
			CloseRs(rs);			
			return (DWORD)nBalance;
		}
		else
		{
			CloseRs(rs);
			
			//创建新账号
			CreateNewAccount(cn,cdkey,200000);
			return 2000000;
		}
		
	}
	catch (_com_error &e)
	{
		PrintErr("*** 数据库错误[获取账户余额时] ***", e);
		return 0;
	}
	return 0;
}

eAccountLockRetVal CRsAccount::AccountLock(_ConnectionPtr& cn,const CGUID& lockGuid,const char* cdkey,long lNum)
{
	if(lNum<0)
		return ACCOUNT_LOCK_UNKNOWN_ERROR;
	long lTransNumber=0;
	try
	{
		EnterCriticalSection(&m_LockCS);
		string strMsg="";
		char sql[256];	
		_RecordsetPtr rs;
		CreateRs(rs);
		sprintf(sql,"SELECT cdkey,balance,frozen FROM account WHERE cdkey='%s'", cdkey); //cdkey is PK

		OpenRs(sql, rs, cn,adOpenForwardOnly,adLockOptimistic);
		int nBalance=0;
		
		if( !rs->GetadoEOF() )
		{		
			//当前余额
			nBalance=(int)rs->GetCollect("balance");
			int nLock=(int)rs->GetCollect("frozen");
			CloseRs(rs);
			//当前余额不能小于需要冻结的数额
			if(nBalance<=0 && nBalance<lNum)
			{
				//余额不够预冻
				CloseRs(rs);				
				LeaveCriticalSection(&m_LockCS);
				return ACCOUNT_LOCK_NOT_ENOUGH;
			}
			else
			{
				//开始事务
				lTransNumber=cn->BeginTrans();
				ZeroMemory(sql,256);
				nBalance-=lNum;
				nLock+=lNum;
				sprintf(sql,"UPDATE account SET balance=%d,frozen=%d WHERE cdkey='%s'",nBalance,nLock,cdkey);
				ExecuteCn(sql,cn);				


				ZeroMemory(sql,256);
				char strGuid[128];
				lockGuid.tostring(strGuid);
				sprintf( sql,"INSERT INTO lock(lock_guid,cdkey,lock_num,lock_time) VALUES('%s','%s',%d,getdate())",
					strGuid, cdkey,lNum );				
				ExecuteCn(sql, cn);
				cn->CommitTrans();
				LeaveCriticalSection(&m_LockCS);
				return ACCOUNT_LOCK_OK;
			}			
		}
		else
		{
			//错误的CDKEY 没有记录
			CloseRs(rs);			
			LeaveCriticalSection(&m_LockCS);
			return ACCOUNT_LOCK_ERROR_CDKEY;
		}
	}
	catch (_com_error &e)
	{
		if(lTransNumber)
		{
			cn->RollbackTrans();
		}
		

		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户预冻时] ***", e);
		return ACCOUNT_LOCK_UNKNOWN_ERROR;
	}
	return ACCOUNT_LOCK_UNKNOWN_ERROR;
}

BOOL CRsAccount::AccountUnlockRollBack(_ConnectionPtr& cn,const CGUID& operGuid,const char* cdkey,long lLockNum)
{
	if(lLockNum<0)
		return FALSE;
	long lTransNumber=0;
	try
	{
		EnterCriticalSection(&m_LockCS);
		
		char sql[256];

		_RecordsetPtr rs;

		CreateRs(rs);
		//先从LOCK表中找operGuid对应的记录
		char strGuid[128];
		operGuid.tostring(strGuid);
		sprintf(sql,"SELECT * FROM lock WHERE lock_guid='%s'", strGuid); 
		
		OpenRs(sql, rs, cn,adOpenForwardOnly,adLockOptimistic);

		if( !rs->GetadoEOF() )
		{	
			int nFrozen=(int)rs->GetCollect("lock_num");			
			CloseRs(rs);

			lTransNumber=cn->BeginTrans();
			
			ZeroMemory(sql,256);
			sprintf(sql,"UPDATE account SET balance=balance+%d,frozen=frozen-%d WHERE cdkey='%s'",nFrozen,nFrozen,cdkey);
			ExecuteCn(sql,cn);

			ZeroMemory(sql,256);
			sprintf(sql,"DELETE FROM lock WHERE lock_guid='%s'",strGuid);
			ExecuteCn(sql,cn);

			cn->CommitTrans();

			LeaveCriticalSection(&m_LockCS);
			return TRUE;
		}
		else
		{
			//错误的CDKEY 没有记录
			CloseRs(rs);
			
			LeaveCriticalSection(&m_LockCS);
			return FALSE;
		}
	}
	catch (_com_error &e)
	{
		if(lTransNumber)
		{
			cn->RollbackTrans();
		}
		
		
		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户冻结回滚时] ***", e);
		return FALSE;
	}
	return FALSE;
}

BOOL CRsAccount::AccountUnlock(_ConnectionPtr& cn,const CGUID& operGuid,const char* cdkey,long lLockNum)
{	
	if(lLockNum<0)
		return FALSE;
	long lTransNumber=0;
	try
	{
		EnterCriticalSection(&m_LockCS);

		char sql[256];

		_RecordsetPtr rs;

		CreateRs(rs);
		//先从LOCK表中找operGuid对应的记录
		char strGuid[128];
		operGuid.tostring(strGuid);
		sprintf(sql,"SELECT * FROM lock WHERE lock_guid='%s'", strGuid); 

		OpenRs(sql, rs, cn,adOpenForwardOnly,adLockOptimistic);

		if( !rs->GetadoEOF() )
		{	
			int nFrozen=(int)rs->GetCollect("lock_num");			
			CloseRs(rs);

			cn->BeginTrans();

			ZeroMemory(sql,256);
			sprintf(sql,"UPDATE account SET frozen=frozen-%d WHERE cdkey='%s'",nFrozen,cdkey);
			ExecuteCn(sql,cn);

			ZeroMemory(sql,256);
			sprintf(sql,"DELETE FROM lock WHERE lock_guid='%s'",strGuid);
			ExecuteCn(sql,cn);

			cn->CommitTrans();

			LeaveCriticalSection(&m_LockCS);
			return TRUE;
		}
		else
		{
			//错误的CDKEY 没有记录
			CloseRs(rs);

			LeaveCriticalSection(&m_LockCS);
			return FALSE;
		}
	}
	catch (_com_error &e)
	{
		if(lTransNumber)
		{
			cn->RollbackTrans();
		}
		

		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户解冻时] ***", e);
		return FALSE;
	}
	return FALSE;
}

int CRsAccount::RunGetBalanceProc(_ConnectionPtr& cn,const char* cdkey)
{
	try
	{
		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraAccount;//,paraAccount,paraAmount,paraGameId;     
		_bstr_t			strProc = L"usp_get_balance"; 
		_variant_t		varReturn;
		int nRet=0;

		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->ActiveConnection = cn;
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		paraAccount = CmdPtr->CreateParameter(L"account", adVarChar, adParamInput, (ADO_LONGPTR)strlen(cdkey),_bstr_t(cdkey));
		CmdPtr->Parameters->Append(paraAccount);

		//! 设置参数
		paraReturn = CmdPtr->CreateParameter(L"balance_value", adInteger, adParamOutput, (ADO_LONGPTR)sizeof(int), varReturn);
		CmdPtr->Parameters->Append(paraReturn);
		

		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		nRet=CmdPtr->Parameters->GetItem("balance_value")->GetValue();
		CmdPtr.Release();		
		return nRet;
	}
	catch (_com_error &e)
	{
		PrintErr("*** 数据库错误[获取账户余额时] ***", e);
		return 0;
	}
	return 0;
}

eAccountLockRetVal CRsAccount::RunConsumeProc(_ConnectionPtr& cn,const CGUID& lockGuid,const char* cdkey,long lNum)
{
	try
	{
		
		EnterCriticalSection(&m_LockCS);
		
		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraGuid,paraAccount,paraAmount,paraGameId;     
		_bstr_t			strProc = L"usp_player_consume"; 
		_variant_t		varReturn;

		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->ActiveConnection = cn;
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		char szGuid[128];
		lockGuid.tostring(szGuid);

		//! 设置参数
		paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(int), varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		paraGuid = CmdPtr->CreateParameter(L"guid", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGuid),szGuid);
		CmdPtr->Parameters->Append(paraGuid);

		paraAccount = CmdPtr->CreateParameter(L"account", adVarChar, adParamInput, (ADO_LONGPTR)strlen(cdkey),_bstr_t(cdkey));
		CmdPtr->Parameters->Append(paraAccount);

		paraAmount = CmdPtr->CreateParameter(L"amount", adInteger, adParamInput, sizeof(int), lNum);
		CmdPtr->Parameters->Append(paraAmount);

		paraGameId = CmdPtr->CreateParameter(L"game_id", adTinyInt, adParamInput, sizeof(BYTE),(BYTE)1);
		CmdPtr->Parameters->Append(paraGameId);

		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		int nRet=CmdPtr->Parameters->GetItem("Return")->Value;
		CmdPtr.Release();
		/*
		enum eAccountLockRetVal
		{
		ACCOUNT_LOCK_OK=1,
		ACCOUNT_LOCK_ERROR_CDKEY,
		ACCOUNT_LOCK_NOT_ENOUGH,
		ACCOUNT_LOCK_UNKNOWN_ERROR
		};
		*/
		

		LeaveCriticalSection(&m_LockCS);
		if(nRet==0)
		{
			return ACCOUNT_LOCK_OK;
		}
		else if(nRet==-2)
		{
			return ACCOUNT_LOCK_ERROR_CDKEY;
		}
		else if(nRet==-3)
		{
			return ACCOUNT_LOCK_NOT_ENOUGH;
		}

		return ACCOUNT_LOCK_UNKNOWN_ERROR;
	}
	catch (_com_error &e)
	{
		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户预冻时] ***", e);
		return ACCOUNT_LOCK_UNKNOWN_ERROR;
	}	
	return ACCOUNT_LOCK_UNKNOWN_ERROR;
}

BOOL CRsAccount::RunConsumeSuccessProc(_ConnectionPtr& cn,const CGUID& lockGuid)
{
	try
	{
		EnterCriticalSection(&m_LockCS);
	
		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraGuid;//,paraAccount,paraAmount,paraGameId;     
		_bstr_t			strProc = L"usp_consume_success"; 
		_variant_t		varReturn;

		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->ActiveConnection = cn;
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		char szGuid[128];
		lockGuid.tostring(szGuid);

		//! 设置参数
		paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(int), varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		paraGuid = CmdPtr->CreateParameter(L"guid", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGuid),szGuid);
		CmdPtr->Parameters->Append(paraGuid);

		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		int nRet=CmdPtr->Parameters->GetItem("Return")->Value;
		CmdPtr.Release();


		LeaveCriticalSection(&m_LockCS);
		return TRUE;
	}
	catch (_com_error &e)
	{
		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户解冻时] ***", e);
		return FALSE;
	}	
	return FALSE;
}



BOOL CRsAccount::RunConsumeFail(_ConnectionPtr& cn,const CGUID& lockGuid)
{
	try
	{
		EnterCriticalSection(&m_LockCS);

		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraGuid;
		_bstr_t			strProc = L"usp_consume_fail"; 
		_variant_t		varReturn;

		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->ActiveConnection = cn;
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		char szGuid[128];
		lockGuid.tostring(szGuid);

		//! 设置参数
		paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(int), varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		paraGuid = CmdPtr->CreateParameter(L"guid", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGuid),szGuid);
		CmdPtr->Parameters->Append(paraGuid);

		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		int nRet=CmdPtr->Parameters->GetItem("Return")->Value;
		CmdPtr.Release();


		LeaveCriticalSection(&m_LockCS);
		return TRUE;
	}
	catch (_com_error &e)
	{
		LeaveCriticalSection(&m_LockCS);
		PrintErr("*** 数据库错误[账户回滚时] ***", e);
		return FALSE;
	}	
	return FALSE;
}


void CRsAccount::SystemRollback()
{
	_ConnectionPtr CnPtr;
	HRESULT lCreateRet=-1;
	HRESULT lOpenRet=-1;
	try
	{	
		EnterCriticalSection(&m_LockCS);
		lCreateRet=CreateCn(CnPtr);
		lOpenRet=OpenCn(CnPtr);

		_CommandPtr		CmdPtr;		
		_bstr_t			strProc = L"usp_auto_rollback"; 				

		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->ActiveConnection = CnPtr;
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);		
		CmdPtr.Release();
		ReleaseCn(CnPtr);
		LeaveCriticalSection(&m_LockCS);
	}
	catch (_com_error &e)
	{
		PrintErr("*** 数据库错误[系统回滚时] ***", e);
		if(SUCCEEDED(lCreateRet) && SUCCEEDED(lOpenRet))
		{
			ReleaseCn(CnPtr);		
		}		
		LeaveCriticalSection(&m_LockCS);
	}	
}
