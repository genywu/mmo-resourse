
#include "stdafx.h"
#include "LoaclAccountValidate.h"
#include "..\nets\netlogin\message.h"

const ULONG SAVE_CACHE_NUM = 1U;

//!													对象能否执行
BOOL												LoaclAccountValidate::c_bAbleWork = FALSE;
//!													DB连接字符串
CnPool*												LoaclAccountValidate::c_pCnPool = NULL;

LoaclAccountValidate::LoaclAccountValidate(VOID)
{
}
//--------------------------------------------------------------------------------
LoaclAccountValidate::LoaclAccountValidate(tagAccountInfo_ToAS &AccountInfo_ToAS)
:m_ValidateData(AccountInfo_ToAS)
{	
}

//--------------------------------------------------------------------------------
LoaclAccountValidate::~LoaclAccountValidate(VOID)
{
}

BOOL LoaclAccountValidate::Init(string strConnectionString)
{
	if(c_bAbleWork) return TRUE;
	c_pCnPool = (CnPool*)M_ALLOC(sizeof(CnPool));
	if (NULL == c_pCnPool)
		return FALSE;
	new(c_pCnPool)CnPool(32);
	c_pCnPool->Init(strConnectionString);

	c_bAbleWork = TRUE;
	return c_bAbleWork;
}
VOID LoaclAccountValidate::Release(VOID)
{
	if(!c_bAbleWork) return;

	c_bAbleWork = FALSE;
	if(NULL != c_pCnPool)
	{
		c_pCnPool->~CnPool();
		M_FREE(c_pCnPool, sizeof(CnPool));
	}
}

LoaclAccountValidate* LoaclAccountValidate::CreateValidateOperator(tagAccountInfo_ToAS &AccountInfo_ToAS)
{
	LoaclAccountValidate* pRe = (LoaclAccountValidate*)M_ALLOC(sizeof(LoaclAccountValidate));
	if(NULL != pRe)
		new(pRe)LoaclAccountValidate(AccountInfo_ToAS);
	return pRe;
}

LoaclAccountValidate* LoaclAccountValidate::CreateSaveOperator(VOID)
{
	LoaclAccountValidate* pRe = (LoaclAccountValidate*)M_ALLOC(sizeof(LoaclAccountValidate));
	if(NULL != pRe)
	{
		new(pRe)LoaclAccountValidate();
		pRe->m_vSaveData.reserve(SAVE_CACHE_NUM);
	}
	return pRe;
}

VOID LoaclAccountValidate::ReleasValidateOperator(LoaclAccountValidate **ppRVO)
{
	if(NULL != ppRVO)
		M_FREE(*ppRVO, sizeof(LoaclAccountValidate));
}


//! 开始执行
BOOL LoaclAccountValidate::BeginValidate(VOID)
{
	if(!c_bAbleWork) return FALSE;
	GetGame()->GetAsynchronousExecute().Execute(ValidateAccountFromDB, this);
	return TRUE;
}

//! 保存
BOOL LoaclAccountValidate::SaveResult(VOID)
{
	if(!c_bAbleWork) return FALSE;
	GetGame()->GetAsynchronousExecute().Execute(SaveASResult, this);
	return TRUE;
}

BOOL LoaclAccountValidate::AddSaveItem(tagAccountInfo_FromAS &AccountInfo_FromAS, const tagSecurityData *pCardData)
{
	if(SAVE_CACHE_NUM <= m_vSaveData.size())
		return FALSE;

	tagSaveData SaveData;
	memcpy(&SaveData._AccountData, &AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
	if(NULL != pCardData)
		memcpy(&SaveData._CardData, pCardData, sizeof(tagSecurityData));
	m_vSaveData.push_back(SaveData);
	return TRUE;
}

//! 在数据库中验证账号
LPVOID LoaclAccountValidate::ValidateAccountFromDB(LPVOID pThis)
{
	LoaclAccountValidate *pVO = (LoaclAccountValidate*)pThis;

	tagCnPtr cnPtr;

	if(!c_pCnPool->GetCnPtr(cnPtr))
	{
		AddLogText("Get CnPtr failed, end!");
		ReleasValidateOperator(&pVO);
		return NULL;
	}


	_CommandPtr		CmdPtr;
	_ParameterPtr	paraReturn, paraAccount, paraPwd, paraCard;     
	_bstr_t			strProc = L"sp_new_user_validate"; 
	_variant_t		varReturn, varCard;

	CmdPtr.CreateInstance(__uuidof(Command));
	
	CmdPtr->CommandText = strProc;
	CmdPtr->CommandType = adCmdStoredProc;

	//! 设置参数
	paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(LONG), varReturn);
	CmdPtr->Parameters->Append(paraReturn);

	paraAccount = CmdPtr->CreateParameter(L"SenderName", adVarWChar, adParamInput, CDKEY_SIZE, pVO->m_ValidateData._AccountData._szCdkey);
	CmdPtr->Parameters->Append(paraAccount);

	paraPwd = CmdPtr->CreateParameter(L"SenderPwd", adVarWChar, adParamInput, PWD_SIZE, pVO->m_ValidateData._AccountData._szPwd);
	CmdPtr->Parameters->Append(paraPwd);

	paraCard = CmdPtr->CreateParameter(L"pwd_card", adBinary, adParamOutput, SECURITY_CARD_SIZE, varCard);
	CmdPtr->Parameters->Append(paraCard);

	try
	{
		//! 执行
		CmdPtr->ActiveConnection = cnPtr.ptr;
		CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("loacl validate error, go on！", e);

		//! 修复连接
		if (c_pCnPool->RepairCnPtr(cnPtr))
		{
			try
			{
				//! 再次执行
				CmdPtr->ActiveConnection = cnPtr.ptr;
				CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 
				pVO->m_ValidateData._AccountData._LoginRerult._dwLoginResult	= CmdPtr->Parameters->GetItem("Return")->Value;	
			}	
			catch(_com_error &e)
			{
				CMyAdoBase::PrintErr("loacl validate again error, end！", e);
				pVO->m_ValidateData._AccountData._LoginRerult._InfoCode = LOGIN_DB_OPT_FAILED;
			}
		}
		else
			AddLogText("Repair CnPtr failed, end!");
	}
	
	//! 结果处理
	try
	{
		pVO->m_ValidateData._AccountData._LoginRerult._dwLoginResult = CmdPtr->Parameters->GetItem("Return")->Value;

		if(0 >= (LONG)pVO->m_ValidateData._AccountData._LoginRerult._dwLoginResult)
		{
			pVO->m_ValidateData._AccountData._LoginRerult._InfoCode = pVO->m_ValidateData._AccountData._LoginRerult._dwLoginResult;
			pVO->m_ValidateData._AccountData._LoginRerult._dwLoginResult = ePLR_Lost;
		}

		_variant_t varOutCard = CmdPtr->Parameters->GetItem("pwd_card")->Value;
		if (varOutCard.vt == (VT_ARRAY | VT_UI1))
		{
			if(varOutCard.parray->rgsabound->cElements == SECURITY_CARD_SIZE)
			{
				BYTE *pBuf = NULL;
				SafeArrayAccessData(varOutCard.parray, (void**)&pBuf);
				memcpy(pVO->m_ValidateData._CardData._arrSecurityCard, pBuf, SECURITY_CARD_SIZE);
				SafeArrayUnaccessData(varOutCard.parray);

				pVO->m_ValidateData._CardData._lWorkFlag = 2;
			}
			else
			{
				pVO->m_ValidateData._AccountData._LoginRerult._InfoCode = LOGIN_DB_OPT_FAILED;
				AddLogText("user[%s]'s card data is bad!!!", pVO->m_ValidateData._AccountData._szCdkey);
			}
		}
	}	
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("UserLoginAndLogout error, get db result lost!", e);
	}

	//! 把结果以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(AIN_MSG_ACCOUNT_Validate_Re);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddEx(&(pVO->m_ValidateData._AccountData), sizeof(tagAccountInfo_FromAS));
		pAinMsg->AddEx(&(pVO->m_ValidateData._CardData), sizeof(tagSecurityData));
		GetGame()->SendToSelf(pAinMsg);
	}

	CmdPtr.Release();
	c_pCnPool->ReturnCnPtr(cnPtr);
	ReleasValidateOperator(&pVO);
	return NULL;
}

//! 提交在数据库中验证账号的请求
LPVOID LoaclAccountValidate::SaveASResult(LPVOID pThis)
{
	LoaclAccountValidate *pVO = (LoaclAccountValidate*)pThis;

	tagCnPtr cnPtr;

	if(!c_pCnPool->GetCnPtr(cnPtr))
	{
		AddLogText("Get CnPtr failed, end!");
		pVO->m_ValidateData._AccountData._LoginRerult._InfoCode = LOGIN_DB_COM_ERROR;
		ReleasValidateOperator(&pVO);
		return NULL;
	}

	vector<tagSaveData> &vSaveData = pVO->m_vSaveData;
	for (LONG i = 0; i < vSaveData.size(); ++i)
	{
		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraAccount, paraPwd, paraCard, paraValidateResult, paraValidateInfo;     
		_bstr_t			strProc = L"sp_new_user_save"; 
		_variant_t		varReturn, varCard;

		SAFEARRAY *psa = 0;
		SAFEARRAYBOUND rgsabound[1];

		CmdPtr.CreateInstance(__uuidof(Command));
		
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		//! 设置参数
		paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(LONG), varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		paraAccount = CmdPtr->CreateParameter(L"SenderName", adVarWChar, adParamInput, CDKEY_SIZE, vSaveData[i]._AccountData._szCdkey);
		CmdPtr->Parameters->Append(paraAccount);

		paraPwd = CmdPtr->CreateParameter(L"crypt_password", adVarWChar, adParamInput, PWD_SIZE, vSaveData[i]._AccountData._szPwd);
		CmdPtr->Parameters->Append(paraPwd);

		if(0 != (vSaveData[i]._CardData._lWorkFlag & 0x2))
		{
			char *pBuf = vSaveData[i]._CardData._arrSecurityCard;
			rgsabound[0].lLbound = 0;
			rgsabound->cElements = SECURITY_CARD_SIZE;
			psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
			for(long i = 0; i < SECURITY_CARD_SIZE; ++i)
				SafeArrayPutElement(psa, &i, pBuf++);
			varCard.vt = VT_ARRAY | VT_UI1;
			varCard.parray = psa;
		}
		else
		{
			varCard.ChangeType(VT_NULL);
		}
		
		paraCard = CmdPtr->CreateParameter(L"pwd_card", adBinary, adParamInput, SECURITY_CARD_SIZE, varCard);
		CmdPtr->Parameters->Append(paraCard);

		paraValidateResult = CmdPtr->CreateParameter(L"ValidateResult", adInteger, adParamInput, sizeof(LONG), vSaveData[i]._AccountData._LoginRerult._dwLoginResult);
		CmdPtr->Parameters->Append(paraValidateResult);

		try
		{
			//! 执行
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
		}
		catch (_com_error &e)
		{
			CMyAdoBase::PrintErr("save player login_info error, go on！", e);

			if(c_pCnPool->RepairCnPtr(cnPtr))
			{
				try
				{
					//! 再次执行
					CmdPtr->ActiveConnection = cnPtr.ptr;
					CmdPtr->Execute(NULL,NULL,adCmdStoredProc);
				}
				catch (_com_error &err)
				{
					CMyAdoBase::PrintErr("save player login_info again error, loop continue!", err);
					CmdPtr.Release();
					continue;
				}
			}
			else
			{
				AddLogText("Repair CnPtr failed, loop continue!");
				CmdPtr.Release();
				continue;
			}
		}

		CmdPtr.Release();
	}

	c_pCnPool->ReturnCnPtr(cnPtr);
	//! 删除对象
	ReleasValidateOperator(&pVO);
	return NULL;
}
