#include "StdAfx.h"
#include "RsBouns.h"

CRsBouns::CRsBouns(void)
{
	::InitializeCriticalSection(&m_csBouns);
}

CRsBouns::~CRsBouns(void)
{
	::DeleteCriticalSection(&m_csBouns);
}

// 加载奖励描述信息
HRESULT CRsBouns::LoadBounsDes(map<DWORD,tagBounsDes> &mapBD)
{
	
	_ConnectionPtr	cn;
	_RecordsetPtr	rs;

	try
	{
		CreateCn(cn);
		OpenCn(cn);	
	}
	catch(_com_error &e)
	{
		PrintErr("*** 数据库错误[LoadBounsDes] ***", e);
		return E_FAIL;
	}
	
	try
	{		
		CreateRs(rs);

		char sql[256];	
		sprintf(sql,"SELECT * FROM TBL_BOUNS_DES"); 
		OpenRs(sql, rs, cn);

		while( !rs->GetadoEOF() )
		{
			tagBounsDes tgBD;
			tgBD.dwBounsID	= rs->GetCollect("ID").lVal;
			tgBD.strTitle	= (_bstr_t)rs->GetCollect("Title");
			tgBD.strDes		= (_bstr_t)rs->GetCollect("Description");
			tgBD.bType		= rs->GetCollect("bType").bVal;
			tgBD.strName	= (_bstr_t)rs->GetCollect("Name");
			tgBD.dwValue	= rs->GetCollect("Value").lVal;

			mapBD[tgBD.dwBounsID] = tgBD;
			rs->MoveNext();			
		}
		CloseRs(rs);		
	}
	catch(_com_error &e)
	{
		CloseRs(rs);
		CloseCn(cn);
		PrintErr("*** 数据库错误[LoadBounsDes] ***", e);
		return E_FAIL;
	}

	CloseCn(cn);	
	return S_OK;

}

HRESULT CRsBouns::LoadBounsDesByID(_ConnectionPtr& cn, DWORD dwBounsID, tagBounsDes& tgBD)
{
	_RecordsetPtr rsPtr;
	CreateRs(rsPtr);
	try
	{
		char sql[256];	
		sprintf(sql,"SELECT * FROM TBL_BOUNS_DES WHERE ID='%d'", dwBounsID); 
		OpenRs(sql, rsPtr, cn);
	
		if(!rsPtr->GetadoEOF())
		{
			tgBD.dwBounsID	= rsPtr->GetCollect("ID").lVal;
			tgBD.strTitle	= (_bstr_t)rsPtr->GetCollect("Title");
			tgBD.strDes		= (_bstr_t)rsPtr->GetCollect("Description");
			tgBD.bType		= rsPtr->GetCollect("bType").bVal;
			tgBD.strName	= (_bstr_t)rsPtr->GetCollect("Name");
			tgBD.dwValue	= rsPtr->GetCollect("Value").lVal;
		}
		CloseRs(rsPtr);
		return S_OK;

	}
	catch(_com_error &e)
	{
		PrintErr("*** 数据库错误[LoadBounsDesByID] ***", e);
		return e.Error();
	}

	return E_FAIL;
}

HRESULT CRsBouns::LoadBounsInfo(_ConnectionPtr& cn, const char* strAccount, DWORD dwAreaServerID, DWORD dwMaxBounsNum, list<tagBounsInfo*>& listBounsInfo)
{
	try
	{
		::EnterCriticalSection(&m_csBouns);

		_CommandPtr		cmdPtr;
		_ParameterPtr	paramPtr[3];
		_RecordsetPtr	rsPtr;
		_bstr_t			strProc = L"PROC_LOAD_BOUNSINFO";
		
		cmdPtr.CreateInstance(__uuidof(Command));
		cmdPtr->ActiveConnection = cn;
		cmdPtr->CommandText = strProc;
		cmdPtr->CommandType = adCmdStoredProc;

		paramPtr[0] = cmdPtr->CreateParameter(L"Account", adVarChar, adParamInput, (ADO_LONGPTR)strlen(strAccount), _bstr_t(strAccount));
		cmdPtr->Parameters->Append(paramPtr[0]);

		paramPtr[1] = cmdPtr->CreateParameter(L"ServerID", adInteger, adParamInput, sizeof(int), dwAreaServerID);
		cmdPtr->Parameters->Append(paramPtr[1]);

		paramPtr[2] = cmdPtr->CreateParameter(L"BounsNum", adInteger, adParamInput, sizeof(int), dwMaxBounsNum);
		cmdPtr->Parameters->Append(paramPtr[2]);

		rsPtr = cmdPtr->Execute(NULL, NULL, adCmdStoredProc);
		cmdPtr.Release();

		while(!rsPtr->GetadoEOF())
		{
			tagBounsInfo *ptgBounsInfo = new tagBounsInfo();

			CGUID guidTemp(_bstr_t(rsPtr->GetCollect("guid")));
			ptgBounsInfo->guid			= guidTemp;
			ptgBounsInfo->strAccount	= _bstr_t(rsPtr->GetCollect("Account"));
			ptgBounsInfo->bStatus		= rsPtr->GetCollect("Status").bVal;
			ptgBounsInfo->dwBounsID		= rsPtr->GetCollect("BounsID").intVal;

			_variant_t var = rsPtr->GetCollect("ValidTime");
			SYSTEMTIME st;
			VariantTimeToSystemTime(var.date, &st);			
			ptgBounsInfo->tValidTime = SystemtimeToTime(st);
			
			listBounsInfo.push_back(ptgBounsInfo);

			rsPtr->MoveNext();
		}

		::LeaveCriticalSection(&m_csBouns);

		return S_OK;

	}
	catch(_com_error &e)
	{
		::LeaveCriticalSection(&m_csBouns);
		PrintErr("*** 数据库错误[GetBounsInfo] ***", e);
		return E_FAIL;
	}
}

// 冻结奖励信息，并返回对应的奖励ID
HRESULT CRsBouns::FreezeBouns(_ConnectionPtr& cn, const CGUID& guid, DWORD& dwBounsID, long &lRet)
{
	try
	{
		::EnterCriticalSection(&m_csBouns);

		_CommandPtr		cmdPtr;
		_ParameterPtr	paramPtr,retPtrID,retPtr;
		_bstr_t			strProc = L"PROC_FREEZE_BOUNS";

		cmdPtr.CreateInstance(__uuidof(Command));
		cmdPtr->ActiveConnection = cn;
		cmdPtr->CommandText = strProc;
		cmdPtr->CommandType = adCmdStoredProc;

		char szGUID[128];
		guid.tostring(szGUID);

		paramPtr = cmdPtr->CreateParameter(L"guid", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGUID), _bstr_t(szGUID));
		cmdPtr->Parameters->Append(paramPtr);

		retPtrID = cmdPtr->CreateParameter(L"ID", adInteger, adParamOutput, sizeof(long), 0);
		cmdPtr->Parameters->Append(retPtrID);

		retPtr = cmdPtr->CreateParameter(L"Ret", adInteger, adParamOutput, sizeof(long), 0);
		cmdPtr->Parameters->Append(retPtr);

		cmdPtr->Execute(NULL, NULL, adCmdStoredProc);
		dwBounsID = cmdPtr->Parameters->GetItem("ID")->GetValue().lVal;
		lRet = cmdPtr->Parameters->GetItem("Ret")->GetValue();
		cmdPtr.Release();

		::LeaveCriticalSection(&m_csBouns);

        return S_OK;

	}
	catch(_com_error &e)
	{
		::LeaveCriticalSection(&m_csBouns);
		PrintErr("*** 数据库错误[FreezeBouns] ***", e);
		return E_FAIL;
	}

	
}

// 领奖 
long CRsBouns::GetBouns(_ConnectionPtr &cn, const CGUID &guid, const CGUID& playerID, const char* strPlayerName, DWORD dwWSID, const char* strIP, long &lRet)
{
	try
	{
		::EnterCriticalSection(&m_csBouns);

		_CommandPtr		cmdPtr;
		_ParameterPtr	paramPtr[5],retPtr;
	//	_variant_t		varReturn;
		_bstr_t			strProc = L"PROC_OBTAIN_BOUNS";
		
		cmdPtr.CreateInstance(__uuidof(Command));
		cmdPtr->ActiveConnection = cn;
		cmdPtr->CommandText = strProc;
		cmdPtr->CommandType = adCmdStoredProc;

		char szGUID[128],szPlayerID[128];
		guid.tostring(szGUID);
		playerID.tostring(szPlayerID);

		paramPtr[0] = cmdPtr->CreateParameter(L"guid", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGUID), _bstr_t(szGUID));
		cmdPtr->Parameters->Append(paramPtr[0]);

		paramPtr[1] = cmdPtr->CreateParameter(L"PlayerID", adGUID, adParamInput, (ADO_LONGPTR)strlen(szPlayerID), _bstr_t(szPlayerID));
		cmdPtr->Parameters->Append(paramPtr[1]);

		paramPtr[2] = cmdPtr->CreateParameter(L"PlayerName", adVarChar, adParamInput, (ADO_LONGPTR)strlen(strPlayerName), _bstr_t(strPlayerName));
		cmdPtr->Parameters->Append(paramPtr[2]);

		paramPtr[3] = cmdPtr->CreateParameter(L"WSID", adInteger, adParamInput, sizeof(int), dwWSID);
		cmdPtr->Parameters->Append(paramPtr[3]);

		paramPtr[4] = cmdPtr->CreateParameter(L"PlayerIP", adVarChar, adParamInput, (ADO_LONGPTR)strlen(strIP), _bstr_t(strIP));
		cmdPtr->Parameters->Append(paramPtr[4]);

		retPtr = cmdPtr->CreateParameter(L"Ret", adInteger, adParamOutput, sizeof(long), 0);
		cmdPtr->Parameters->Append(retPtr);

		cmdPtr->Execute(NULL, NULL, adCmdStoredProc);
		lRet = cmdPtr->Parameters->GetItem("Ret")->GetValue();
		cmdPtr.Release();

		::LeaveCriticalSection(&m_csBouns);

		return S_OK;

	}
	catch(_com_error &e)
	{
		::LeaveCriticalSection(&m_csBouns);
		PrintErr("*** 数据库错误[GetBouns] ***", e);
		return E_FAIL;
	}

}

// 领奖失败回滚
HRESULT CRsBouns::BounsRollBack(_ConnectionPtr& cn, const CGUID& guid)
{
	try
	{
		::EnterCriticalSection(&m_csBouns);

		_CommandPtr		cmdPtr;
		_ParameterPtr	paramPtr,retPtr;
		_variant_t		varReturn;
		_bstr_t			strProc = L"PROC_FREEZE_BOUNS_ROLLBACK";

		cmdPtr.CreateInstance(__uuidof(Command));
		cmdPtr->ActiveConnection = cn;
		cmdPtr->CommandText = strProc;
		cmdPtr->CommandType = adCmdStoredProc;

		char szGUID[128];
		guid.tostring(szGUID);

		paramPtr = cmdPtr->CreateParameter(L"guid", adGUID, adParamInput, sizeof(szGUID), szGUID);
		cmdPtr->Parameters->Append(paramPtr);

		retPtr = cmdPtr->CreateParameter(L"Ret", adInteger, adParamOutput, sizeof(int), varReturn);
		cmdPtr->Parameters->Append(retPtr);

		cmdPtr->Execute(NULL, NULL, adCmdStoredProc);
		long nRet = cmdPtr->Parameters->GetItem("Ret")->Value;
		cmdPtr.Release();

		::LeaveCriticalSection(&m_csBouns);
		return S_OK;
	}
	catch (_com_error &e)
	{
		::LeaveCriticalSection(&m_csBouns);
		PrintErr("*** 数据库错误[BounsRollBack] ***", e);
		return E_FAIL;
	}
}

time_t CRsBouns::SystemtimeToTime(const SYSTEMTIME &st)
{
	struct tm stTime;
	memset(&stTime, 0, sizeof(tm));


	stTime.tm_year	= st.wYear-1900;
	stTime.tm_mon	= st.wMonth-1;
	stTime.tm_mday	= st.wDay;

	stTime.tm_hour	= st.wHour;
	stTime.tm_min	= st.wMinute;
	stTime.tm_sec	= st.wSecond;

	return mktime(&stTime);
}