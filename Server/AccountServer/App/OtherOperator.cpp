#include "stdafx.h"
#include "OtherOperator.h"

MultiMemPool<LockOfWin32>	OtherOperator::c_MultiMemPool;

VOID OtherOperator::Release(VOID)
{
}

//! 创建一个封号操作
OtherOperator* OtherOperator::CreateRejectOperator(const char* pCdKey, LONG lEndTime)
{
	if(NULL == pCdKey) return NULL;
	LONG lLen = (LONG)strlen(pCdKey);
	if(0 == lLen || CDKEY_SIZE <= lLen) return NULL;

	OtherOperator* pRe = (OtherOperator*)c_MultiMemPool.Multi_Alloc(sizeof(OtherOperator), _FILE_AND_LINE_);
	if(NULL != pRe)
	{
		pRe->m_dwDataType = eDT_RejectData;
		pRe->m_pData = c_MultiMemPool.Multi_Alloc(sizeof(tagRejectData), _FILE_AND_LINE_);
		if(NULL == pRe->m_pData)
		{
			c_MultiMemPool.Multi_Free(pRe, sizeof(OtherOperator));
			return NULL;
		}
		
		tagRejectData *pRejectData = (tagRejectData*)pRe->m_pData;
		strcpy(pRejectData->_szCdKey, pCdKey);
		pRejectData->_lEndTime = lEndTime;

		return pRe;
	}

	return NULL;
}

//! 创建一个用户上下线操作
OtherOperator* OtherOperator::CreateLoginAndLogout(const char* pCdKey, LONG lLSID, eLoginAndLogoutType eLogin)
{
	if(NULL == pCdKey) return NULL;
	LONG lLen = (LONG)strlen(pCdKey);
	if(0 == lLen || CDKEY_SIZE <= lLen) return NULL;

	OtherOperator* pRe = (OtherOperator*)c_MultiMemPool.Multi_Alloc(sizeof(OtherOperator), _FILE_AND_LINE_);
	if(NULL != pRe)
	{
		pRe->m_dwDataType = eDT_LoginAndlogout;
		pRe->m_pData = c_MultiMemPool.Multi_Alloc(sizeof(tagLoginAndlogout), _FILE_AND_LINE_);
		if(NULL == pRe->m_pData)
		{
			c_MultiMemPool.Multi_Free(pRe, sizeof(OtherOperator));
			return NULL;
		}
		new(pRe->m_pData)tagLoginAndlogout();
		tagLoginAndlogout *pLoginAndlogout = (tagLoginAndlogout*)pRe->m_pData;
		strcpy(pLoginAndlogout->_szCdKey, pCdKey);
		pLoginAndlogout->_lLSID = lLSID;
		pLoginAndlogout->_dwLogin = eLogin;
		return pRe;
	}
	return NULL;
}

//! 销毁操作
VOID OtherOperator::ReleaseOperator(OtherOperator **ppOperator)
{
	if(NULL == ppOperator || NULL == *ppOperator) return;
	switch((*ppOperator)->m_dwDataType)
	{
	case eDT_NoData:
		break;
	case eDT_RejectData:
		{
			c_MultiMemPool.Multi_Free((*ppOperator)->m_pData, sizeof(tagRejectData));
			(*ppOperator)->m_pData = NULL;
		}
		break;
	case eDT_LoginAndlogout:
		{
			tagLoginAndlogout *pLoginAndlogout = (tagLoginAndlogout*)(*ppOperator)->m_pData;
			pLoginAndlogout->~tagLoginAndlogout();
			c_MultiMemPool.Multi_Free((*ppOperator)->m_pData, sizeof(tagLoginAndlogout));
			(*ppOperator)->m_pData = NULL;
		}
	    break;
	default:
	    return;
	}
	
	(*ppOperator)->m_dwDataType = eDT_NoData;
	c_MultiMemPool.Multi_Free(*ppOperator, sizeof(OtherOperator));
	*ppOperator = NULL;
}


//! 执行封号操作
VOID OtherOperator::RejectAccount(VOID)
{
	GetGame()->GetAsynchronousExecute().Execute(RejectAccountFromDB, this);
}

//! 记录上下线，获得防沉迷时间（秒）
VOID OtherOperator::LoginAndLogout(VOID)
{
	GetGame()->GetAsynchronousExecute().Execute(LoginAndLogoutFromDB, this);
}



//! 用户上下线
LPVOID OtherOperator::LoginAndLogoutFromDB(LPVOID pThis)
{
	OtherOperator *pThisObj = (OtherOperator*)pThis;
	tagLoginAndlogout *pLoginAndlogout = (tagLoginAndlogout*)pThisObj->m_pData;

	pLoginAndlogout->_setLSID.clear();
	pLoginAndlogout->_dwSecond = UserLoginAndLogout(pLoginAndlogout->_szCdKey, pLoginAndlogout->_lLSID, pLoginAndlogout->_dwLogin, pLoginAndlogout->_setLSID);

	if(0 <= pLoginAndlogout->_dwSecond)
	{
		CMessage msg(MSG_A2O_ACCOUNT_FcmTime_Notify);
		msg.Add(pLoginAndlogout->_szCdKey);
		msg.Add(pLoginAndlogout->_dwSecond);

		for (set<DWORD>::iterator ite = pLoginAndlogout->_setLSID.begin(); pLoginAndlogout->_setLSID.end() != ite; ++ite)
		{
			msg.SendToClient(GetGame()->GetClientManager().GetClientSocket(*ite));
		}
	}

	ReleaseOperator(&pThisObj);
	return NULL;
}

//! 从DB中获得一个LS上的在线用户
BOOL OtherOperator::GetLsOnlineAccount(LONG lLSID, set<string> &setAccount)
{
	setAccount.clear();

	_RecordsetPtr RsPtr;
	RsPtr.CreateInstance(__uuidof(Recordset));

	tagCnPtr cnPtr;
	if(!ValidateOperator::GetFcmCnPool()->GetCnPtr(cnPtr))
	{
		Log4c::Error(ROOT_MODULE,"Get CnPtr failed, end!");
		return FALSE;
	}

	char szSQL[512] = {0};
	sprintf(szSQL, "SELECT user_account FROM user_login_info WHERE login_server_id=%d;", lLSID);

	try
	{
		RsPtr->Open(szSQL, cnPtr.ptr.GetInterfacePtr(), adOpenStatic, adLockReadOnly, adCmdText);
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("GetLsOnlineAccount error, go on！", e);
		if(ValidateOperator::GetFcmCnPool()->RepairCnPtr(cnPtr))
		{
			try
			{
				RsPtr->Open(szSQL, cnPtr.ptr.GetInterfacePtr(), adOpenStatic, adLockReadOnly, adCmdText);
			}	
			catch(_com_error &e)
			{
				CMyAdoBase::PrintErr("GetLsOnlineAccount again error, end！", e);
				ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
				return FALSE;
			}
		}
		else
		{
			CMyAdoBase::PrintErr("GetLsOnlineAccount RepairCnPtr error, end！", e);
			ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
			return FALSE;
		}
	}

	ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
	
	while (!RsPtr->GetadoEOF())
	{
		_bstr_t var = (_bstr_t)RsPtr->GetCollect("user_account");
		const char *pAccount = (const char *)var;
		if(NULL != pAccount)
			setAccount.insert(string(pAccount));
		RsPtr->MoveNext();
	}

	return TRUE;
}

//! 执行安全验证操作
BOOL OtherOperator::UserSecurityOperate(const char* pCdKey, DWORD dwIp, LONG &lWorkFlag, char arrSecurityCard[])
{
	tagCnPtr cnPtr;

	if(!ValidateOperator::GetSecurityCnPool()->GetCnPtr(cnPtr))
	{
		return FALSE;
	}

	_CommandPtr		CmdPtr;
	_ParameterPtr	paraAccount, paraIP, paraCard, paraFlag, paraReturn;     
	_bstr_t			strProc = L"sp_security_validate"; 
	_variant_t		varFlag, varCard, varReturn;


	CmdPtr.CreateInstance(__uuidof(Command));
	CmdPtr->CommandText = strProc;
	CmdPtr->CommandType = adCmdStoredProc;

	paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, 32, varReturn);
	CmdPtr->Parameters->Append(paraReturn);

	paraAccount = CmdPtr->CreateParameter(L"user_account", adVarWChar, adParamInput, (ADO_LONGPTR)strlen(pCdKey) + 1, pCdKey);
	CmdPtr->Parameters->Append(paraAccount);

	//! 将网络字节序的IP改为主机字节序
	DWORD	dwMasterIp	= 0;
	BYTE	*pIP		= (BYTE*)&dwIp;
	BYTE	*pMasterIP	= (BYTE*)&dwMasterIp;
	pMasterIP[0] = pIP[3];
	pMasterIP[1] = pIP[2];
	pMasterIP[2] = pIP[1];
	pMasterIP[3] = pIP[0];

	paraIP = CmdPtr->CreateParameter(L"int_ip", adBigInt, adParamInput, 32, dwMasterIp);
	CmdPtr->Parameters->Append(paraIP);

	paraCard = CmdPtr->CreateParameter(L"card_password", adBinary, adParamOutput, SECURITY_CARD_SIZE, varCard);
	CmdPtr->Parameters->Append(paraCard);

	paraFlag = CmdPtr->CreateParameter(L"operation_flag", adInteger, adParamOutput, 32, varFlag);
	CmdPtr->Parameters->Append(paraFlag);

	CmdPtr->ActiveConnection = cnPtr.ptr;

	try
	{
		//! 执行
		CmdPtr->Execute(NULL, NULL, adCmdStoredProc);
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserSecurityOperator error, go on！", e);

		//! 修复连接
		if(ValidateOperator::GetSecurityCnPool()->RepairCnPtr(cnPtr))
		{
			try
			{
				//! 再次执行
				CmdPtr->ActiveConnection = cnPtr.ptr;
				CmdPtr->Execute(NULL, NULL, adCmdStoredProc);
			}	
			catch(_com_error &err)
			{
				CMyAdoBase::PrintErr("UserSecurityOperator again error, end!", err);
				ValidateOperator::GetSecurityCnPool()->ReturnCnPtr(cnPtr);
				return FALSE;
			}
		}
		else
		{
			Log4c::Error(ROOT_MODULE,"UserSecurityOperator Repair CnPtr failed, end!");
			ValidateOperator::GetSecurityCnPool()->ReturnCnPtr(cnPtr);
			return FALSE;
		}
	}

	ValidateOperator::GetSecurityCnPool()->ReturnCnPtr(cnPtr);

	BOOL bRe = FALSE;
	try
	{
		LONG lDbRe = CmdPtr->Parameters->GetItem("Return")->Value;
		if(0 == lDbRe)
		{
			bRe = TRUE;
			lWorkFlag = CmdPtr->Parameters->GetItem("operation_flag")->Value;
			if(2 == lWorkFlag || 3 == lWorkFlag)
			{
				_variant_t varOutCard = CmdPtr->Parameters->GetItem("card_password")->Value;
				if (varOutCard.vt == (VT_ARRAY | VT_UI1))
				{
					if(varOutCard.parray->rgsabound->cElements == SECURITY_CARD_SIZE)
					{
						BYTE *pBuf = NULL;
						SafeArrayAccessData(varOutCard.parray, (void**)&pBuf);
						memcpy(arrSecurityCard, pBuf, SECURITY_CARD_SIZE);
						SafeArrayUnaccessData(varOutCard.parray);
					}
					else
					{
						Log4c::Error(ROOT_MODULE,"user[%s]'s card data is bad!!!", pCdKey);
						bRe = FALSE;
					}
				}
				else
					bRe = FALSE;
			}
		}
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserLoginAndLogout error, get db result lost!", e);
	}

	CmdPtr.Release();
	return bRe;
}

//! 用户上下线处理
LONG OtherOperator::UserLoginAndLogout(const char* pCdKey, LONG lLSID, DWORD dwLogin, set<DWORD> &setLSID)
{
	tagCnPtr cnPtr;

	if(!ValidateOperator::GetFcmCnPool()->GetCnPtr(cnPtr))
	{
		return eULALR_RunLose;
	}

	_CommandPtr		CmdPtr;
	_ParameterPtr	paraReturn, paraAccount, paraLSID, paraCurrTime, paraIsLogin;     
	_bstr_t			strProc = L"sp_login_logout"; 
	_variant_t		varReturn;
	_RecordsetPtr	RsPtr = NULL;

	try
	{
		CmdPtr.CreateInstance(__uuidof(Command));
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		paraAccount = CmdPtr->CreateParameter(L"account", adVarWChar, adParamInput, (ADO_LONGPTR)strlen(pCdKey) + 1, pCdKey);
		CmdPtr->Parameters->Append(paraAccount);

		paraLSID = CmdPtr->CreateParameter(L"login_server_id", adInteger, adParamInput, 32, lLSID);
		CmdPtr->Parameters->Append(paraLSID);

		long lCurrTime = static_cast<long>(time(NULL));
		paraCurrTime = CmdPtr->CreateParameter(L"curr_time", adInteger, adParamInput, 32, lCurrTime);
		CmdPtr->Parameters->Append(paraCurrTime);

		paraIsLogin = CmdPtr->CreateParameter(L"in_or_out", adBigInt, adParamInput, 32, dwLogin);
		CmdPtr->Parameters->Append(paraIsLogin);

		paraReturn = CmdPtr->CreateParameter(L"re", adInteger, adParamOutput, 32, varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		CmdPtr->ActiveConnection = cnPtr.ptr;
	}
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserLoginAndLogout create _ParameterPtr error!", e);
		ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
		return eULALR_RunLose;
	}

	try
	{
		//! 执行
		RsPtr = CmdPtr->Execute(NULL, NULL, adCmdStoredProc);
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserLoginAndLogout error, go on！", e);

		//! 修复连接
		if(ValidateOperator::GetFcmCnPool()->RepairCnPtr(cnPtr))
		{
			try
			{
				//! 再次执行
				CmdPtr->ActiveConnection = cnPtr.ptr;
				RsPtr = CmdPtr->Execute(NULL, NULL, adCmdStoredProc);
			}	
			catch(_com_error &err)
			{
				CMyAdoBase::PrintErr("UserLoginAndLogout again error, end!", err);
				ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
				return eULALR_RunLose;
			}
		}
		else
		{
			Log4c::Error(ROOT_MODULE,"UserLoginAndLogout Repair CnPtr failed, end!");
			ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);
			return eULALR_RunLose;
		}
	}
	ValidateOperator::GetFcmCnPool()->ReturnCnPtr(cnPtr);

	//! 结果处理
	LONG lRe = eULALR_RunLose;
	try
	{
	if(adStateOpen == RsPtr->State)
	{
		setLSID.clear();
		while (!RsPtr->GetadoEOF())
		{
			DWORD lTmpLSID = (DWORD)RsPtr->GetCollect("login_server_id");
			setLSID.insert(lTmpLSID);
			RsPtr->MoveNext();
		}
		RsPtr->Close();
		RsPtr.Release();
	}
	
		lRe = CmdPtr->Parameters->GetItem("re")->Value;
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserLoginAndLogout error, get db result lost!", e);
	}
	
	CmdPtr.Release();

	return lRe;
}

//! 封号的数据操作
LPVOID OtherOperator::RejectAccountFromDB(LPVOID pThis)
{
	OtherOperator *pThisObj = (OtherOperator*)pThis;
	tagRejectData *pRejectData = (tagRejectData*)pThisObj->m_pData;
	
	tagCnPtr cnPtr;

	if(!ValidateOperator::GetCnPool()->GetCnPtr(cnPtr))
	{
		Log4c::Error(ROOT_MODULE,"Get CnPtr failed, end!");
		ValidateOperator::GetCnPool()->ReturnCnPtr(cnPtr);
		OtherOperator::ReleaseOperator(&pThisObj);
		return NULL;
	}

	_CommandPtr		CmdPtr;
	_ParameterPtr	paraReturn, paraAccount, paraEndTime;     
	_bstr_t			strProc = L"sp_reject_account"; 
	_variant_t		varReturn;

	CmdPtr.CreateInstance(__uuidof(Command));
	CmdPtr->CommandText = strProc;
	CmdPtr->CommandType = adCmdStoredProc;

	//! 设置参数
	//paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, 32, varReturn);
	//CmdPtr->Parameters->Append(paraReturn);

	paraAccount = CmdPtr->CreateParameter(L"SenderName", adVarWChar, adParamInput, 32, pRejectData->_szCdKey);
	CmdPtr->Parameters->Append(paraAccount);

	paraEndTime = CmdPtr->CreateParameter(L"end_time", adInteger, adParamInput, 32, pRejectData->_lEndTime);
	CmdPtr->Parameters->Append(paraEndTime);

	try
	{
		//! 执行
		CmdPtr->ActiveConnection = cnPtr.ptr;
		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("Validate player account error, go on！", e);

		//! 修复连接
		if(ValidateOperator::GetCnPool()->RepairCnPtr(cnPtr))
		{
			try
			{
				//! 再次执行
				CmdPtr->ActiveConnection = cnPtr.ptr;
				CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
			}	
			catch(_com_error &err)
			{
				CMyAdoBase::PrintErr("Validate player account again error, end!", err);
			}
		}
		else ;
			Log4c::Error(ROOT_MODULE,"Repair CnPtr failed, end!");
	}
	
	CmdPtr.Release();
	ValidateOperator::GetCnPool()->ReturnCnPtr(cnPtr);
	OtherOperator::ReleaseOperator(&pThisObj);
	return NULL;
}