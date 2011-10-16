#include "stdafx.h"
#include "ValidateOperator.h"
#include "OtherOperator.h"

#ifdef APEX_TEST

LONG ValidateOperator::c_lValidateNum = 0;

#endif

MultiObjPool<ValidateOperator, LockOfWin32>		ValidateOperator::c_MultiObjPool(128);
CnPool*											ValidateOperator::c_pCnPool = NULL;
CnPool*											ValidateOperator::c_pSecurityPool	= NULL;
CnPool*											ValidateOperator::c_pFcmCnPool = NULL;
LONG											ValidateOperator::c_lASID = 0;

//--------------------------------------------------------------------------------
ValidateOperator::ValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwSocket, DWORD dwLSID)
:m_AccountInfo_FromAS(AccountInfo_ToAS), m_dwSocket(dwSocket), m_dwLSID(dwLSID)
{	
}

//--------------------------------------------------------------------------------
ValidateOperator::~ValidateOperator(VOID)
{
}

BOOL ValidateOperator::Init(LONG lASID)
{
	c_pCnPool = new CnPool(32);
	c_pCnPool->Init(GetGame()->GetConnectionString());

	c_pSecurityPool = new CnPool(32);
	c_pSecurityPool->Init(GetGame()->GetSecurityConnectionString());

	c_pFcmCnPool = new CnPool(32);
	c_pFcmCnPool->Init(GetGame()->GetFcmConnectionString());


	c_lASID = lASID;
	return TRUE;
}
VOID ValidateOperator::Release(VOID)
{
    SAFE_DELETE(c_pFcmCnPool);
    SAFE_DELETE(c_pSecurityPool);
	SAFE_DELETE(c_pCnPool);
}

ValidateOperator* ValidateOperator::CreateValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwSocket, DWORD dwLSID)
{
	//return new ValidateOperator(AccountInfo_ToAS, dwSocket);

	ValidateOperator* pRe = c_MultiObjPool.Multi_Alloc(_FILE_AND_LINE_);
	if(NULL != pRe)
		new(pRe)ValidateOperator(AccountInfo_ToAS, dwSocket, dwLSID);
	return pRe;
}
VOID ValidateOperator::ReleasValidateOperator(ValidateOperator **ppRVO)
{
	//delete *ppRVO;
	//*ppRVO = NULL;

	c_MultiObjPool.Multi_Free(*ppRVO);
	*ppRVO = NULL;
}

//! 开始执行
BOOL ValidateOperator::BeginValidate(VOID)
{
	GetGame()->GetAsynchronousExecute().Execute(ValidateAccountFromDB, this, OnValidateFinish);
	return TRUE;
}

//! 发送操作结果
VOID ValidateOperator::SendResult(VOID)
{
	CMessage msg(MSG_A2O_ACCOUNT_Validate_Re);
	msg.AddEx(&m_AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
	msg.SendToClient(m_dwSocket);
}

//! 发送安全数据
VOID ValidateOperator::SendResult(tagSecurityData &SecurityData)
{
	CMessage msg(MSG_A20_SECURITY_DATA);
	msg.AddEx(&m_AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
	msg.AddEx(&SecurityData, sizeof(tagSecurityData));
	msg.SendToClient(m_dwSocket);
}


//! 在数据库中验证账号
LPVOID ValidateOperator::ValidateAccountFromDB(LPVOID pThis)
{
	ValidateOperator *pVO = (ValidateOperator*)pThis;

	tagCnPtr cnPtr;

	if(!c_pCnPool->GetCnPtr(cnPtr))
	{
		Log4c::Error(ROOT_MODULE,"Get CnPtr failed, end!");
		pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_DB_COM_ERROR;
		return pThis;
	}

	_CommandPtr		CmdPtr;
	_ParameterPtr	paraReturn, paraAccount, paraPwd, parCurTime;     
	_bstr_t			strProc; 
	_variant_t		varReturn;

	if(0 != (eVT_Not_Pwd & pVO->m_AccountInfo_FromAS._cValidateType))
		strProc = "sp_user_is_rejected";
	else
		strProc = L"sp_user_validate"; 

	CmdPtr.CreateInstance(__uuidof(Command));
	CmdPtr->CommandText = strProc;
	CmdPtr->CommandType = adCmdStoredProc;

	//! 设置参数
	paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, 32, varReturn);
	CmdPtr->Parameters->Append(paraReturn);

	paraAccount = CmdPtr->CreateParameter(L"SenderName", adVarWChar, adParamInput, 32, pVO->m_AccountInfo_FromAS._szCdkey);
	CmdPtr->Parameters->Append(paraAccount);

	if(0 == (eVT_Not_Pwd & pVO->m_AccountInfo_FromAS._cValidateType))
	{
		paraPwd = CmdPtr->CreateParameter(L"SenderPwd", adVarWChar, adParamInput, 32, pVO->m_AccountInfo_FromAS._szPwd);
		CmdPtr->Parameters->Append(paraPwd);
	}
	
	parCurTime = CmdPtr->CreateParameter(L"curr_time", adInteger, adParamInput, 32, (LONG)time(NULL));
	CmdPtr->Parameters->Append(parCurTime);

	try
	{
		//! 执行
		CmdPtr->ActiveConnection = cnPtr.ptr;
		CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = CmdPtr->Parameters->GetItem("Return")->Value;
	
#ifdef APEX_TEST
		InterlockedIncrement(&c_lValidateNum);
#endif;
	}	
	catch(_com_error &e)
	{	
		CMyAdoBase::PrintErr("Validate player account error, go on！", e);
		//! 修复连接
		if(c_pCnPool->RepairCnPtr(cnPtr))
		{
			try
			{
				//! 再次执行
				CmdPtr->ActiveConnection = cnPtr.ptr;
				CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
				pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = CmdPtr->Parameters->GetItem("Return")->Value;
			}
			catch (_com_error &err)
			{
				CMyAdoBase::PrintErr("Validate player account again error, end！", err);
				pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_DB_OPT_FAILED;
			}
		}
		else ;
			Log4c::Error(ROOT_MODULE,"Repair CnPtr failed, end!");
	}

	CmdPtr.Release();
	c_pCnPool->ReturnCnPtr(cnPtr);
	return pThis;
}

//! 处理验证结果
LPVOID ValidateOperator::OnValidateFinish(LPVOID pThis)
{
	tagSecurityData SecurityData;
	BOOL bHasSecurityData = FALSE;

	ValidateOperator *pVO = (ValidateOperator*)pThis;
	switch(pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode)
	{
		//! 通过验证
	case LOGIN_OK:
		{
			//! 开始安全验证
			if(0 != (eVT_Not_Safe & pVO->m_AccountInfo_FromAS._cValidateType) || OtherOperator::UserSecurityOperate(pVO->m_AccountInfo_FromAS._szCdkey, pVO->m_AccountInfo_FromAS._dwIp, SecurityData._lWorkFlag, SecurityData._arrSecurityCard))
			{
				if(2 == SecurityData._lWorkFlag || 3 == SecurityData._lWorkFlag)
					bHasSecurityData = TRUE;

				pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Normal;

				if(0 == (eVT_Not_Fcm & pVO->m_AccountInfo_FromAS._cValidateType))
				{
					set<DWORD> setLSID;
					LONG lHoldSecond = OtherOperator::UserLoginAndLogout(pVO->m_AccountInfo_FromAS._szCdkey, pVO->m_dwLSID, TRUE, setLSID);

					//! 有防沉迷时间
					if(0 < lHoldSecond)
					{
						//! 提前在时间通知前发送验证成功通知
						if(bHasSecurityData)
							pVO->SendResult(SecurityData);
						else
							pVO->SendResult();

						CMessage msg(MSG_A2O_ACCOUNT_FcmTime_Notify);
						msg.Add(pVO->m_AccountInfo_FromAS._szCdkey);
						msg.Add(lHoldSecond);

						for (set<DWORD>::iterator ite = setLSID.begin(); setLSID.end() != ite; ++ite)
						{
							LONG lSocketID = GetGame()->GetClientManager().MultiGetClientSocket(*ite);
							if(0 != lSocketID)
								msg.SendToClient(lSocketID);
						}

						//! 删除对象
						ReleasValidateOperator(&pVO);
						return NULL;
					}
					else if (0 == lHoldSecond)
					{
						pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
						pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_NOTENOUGH_HEALTHY_TIME;
					}
					//else if(OtherOperator::eULALR_RejoinLs == lHoldSecond)
					//{
					//	pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
					//	pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_PLAYER_ALREAD_IN_GAME;
					//}
					else if (OtherOperator::eULALR_RunLose == lHoldSecond)
					{
						pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
						pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_DB_OPT_FAILED;
					}
				}
				
			}
			else
			{
				//! 安全验证失败
				pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
				pVO->m_AccountInfo_FromAS._LoginRerult._InfoCode = SECURITY_OPERATE_LOST;
			}	
		}
		break;
		//! 帐号未激活
	case LOGIN_ACCOUNT_NOT_ACTIVE:
		//! 没找到用户所对应的记录
	case LOGIN_NOTCDKEY:
		//! 用户被锁定
	case LOGIN_ACCOUNT_IS_LOCKED:
		//! 密码错误
	case LOGIN_PASSWORDERROR:
		//! 账号被封
	case LOGIN_REJECTED:

		//! 程序定义的错误代码
		///////////////////////////////
		//! 验证对象没有准备好
	case LOGIN_WAIT_INIT_FINISH:
		//! 数据库操作对象错误
	case LOGIN_DB_COM_ERROR:
		//! 不能连接到DB
	case LOGIN_CANT_CONN_DB:
		{
			pVO->m_AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
		}
	default:
		break;
	}

	if(bHasSecurityData)
		pVO->SendResult(SecurityData);
	else
	pVO->SendResult();
	//! 删除对象
	ReleasValidateOperator(&pVO);
	return NULL;
}
