


#include "stdafx.h"
#include "DbOperator.h"
#include "../Setup/DbSetup.h"
#include "GmSystem.h"


AsynchronousExecute			DbOperator::c_AsynchronousExecute(TRUE);
CnPool						DbOperator::c_CnPool(20);

DbOperator::DbOperator(DWORD dwDOOT)
:m_dwDOOT(dwDOOT)
,m_pData(NULL)
{

}
DbOperator::~DbOperator(VOID)
{

}

//! 初始化对象
BOOL DbOperator::Init(VOID)
{

	if(!LoadSetup())
		return FALSE;

	const tagThreadSetup *pThreadSetup = DbSetup::GetInstance().GetThreadSetup();
	if (NULL == pThreadSetup)
		return FALSE;

	if(!CMyAdoBase::Initialize())
		return FALSE;

	c_AsynchronousExecute.SetWorkParam(
		pThreadSetup->dwMinDbOptThreadNum, 
		pThreadSetup->dwMaxDbOptThreadNum, 
		pThreadSetup->dwThreadNumAdjustSpace
		);
	c_AsynchronousExecute.BeginWork(pThreadSetup->dwMinDbOptThreadNum);

	c_CnPool.Init(DbSetup::GetInstance().GetConnectString(1));

	return TRUE;
}

//! 释放对象
VOID DbOperator::Release(VOID)
{

	c_AsynchronousExecute.Exit();
	CMyAdoBase::Uninitalize();

}

//! 读取配置
BOOL DbOperator::LoadSetup(VOID)
{

	//! 数据库配置
	if( !DbSetup::GetInstance().LoadSetup("Database.xml") )
	{
		LogTools::OutLog("load Database.xml lost!!!");
		return FALSE;
	}
	else
	{
		LogTools::OutLog("load Database.xml succeed!");
	}

	return TRUE;
}


//! 创建一个密码比较操作操作
DbOperator* DbOperator::CreatePwdCmpOperator(const char *pCdkey, const char *pMD5Pwd)
{

	if(NULL != pCdkey && NULL != pMD5Pwd)
	{
		LONG lLine = __LINE__;
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_PwdCmp);
			if(pOpt->CreateData())
			{
				tagUserInfo *pUserInfo = (tagUserInfo*)pOpt->m_pData;
				strcpy(pUserInfo->szCdkey, pCdkey);
				strcpy(pUserInfo->szPwd, pMD5Pwd);
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个密码修改操作
DbOperator* DbOperator::CreatePwdChangeOperator(const char *pCdkey, const char *pPwd)
{

	if(NULL != pCdkey && NULL != pPwd)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_PwdChange);
			if(pOpt->CreateData())
			{
				tagUserInfo *pUserInfo = (tagUserInfo*)pOpt->m_pData;
				strcpy(pUserInfo->szCdkey, pCdkey);
				strcpy(pUserInfo->szPwd, pPwd);
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个硬件信息组写入操作
DbOperator* DbOperator::CreateInHardOperator(DWORD dwSMID, DWORD dwNum, tagHardwareInfo *pHardwareInfo)
{

	if(NULL != pHardwareInfo)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_InHard);
			if(pOpt->CreateData())
			{
				tagHardInfoGroup *pHardInfoGroup = (tagHardInfoGroup*)pOpt->m_pData;
				pHardInfoGroup->dwSMID			= dwSMID;
				pHardInfoGroup->dwNum			= dwNum;
				pHardInfoGroup->pHardwareInfo	= pHardwareInfo;
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个写入DB的日志组写入操作
DbOperator* DbOperator::CreateIDBLogOperator(tagDbLog *pDbLog, DWORD dwNum)
{

	if(NULL != pDbLog)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_DBLogGroup);
			if(pOpt->CreateData())
			{
				tagDBLogGroup *pDBLogGroup = (tagDBLogGroup*)pOpt->m_pData;
				pDBLogGroup->pDbLog = pDbLog;
				pDBLogGroup->dwNum	= dwNum;
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个游戏信息组写入操作
DbOperator* DbOperator::CreateGameInfoOperator(DWORD dwSMID, DWORD dwGappID, tagDbGameInfo *pDbGameInfo, DWORD dwNum)
{
	if(NULL != pDbGameInfo)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_GameInfoGroup);
			if(pOpt->CreateData())
			{
				tagGameInfoGroup *pGameInfoGroup = (tagGameInfoGroup*)pOpt->m_pData;
				pGameInfoGroup->dwSMID		= dwSMID;
				pGameInfoGroup->dwGappID	= dwGappID;
				pGameInfoGroup->pDbGameInfo	= pDbGameInfo;
				pGameInfoGroup->dwNum		= dwNum;
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个游戏信息组读取操作
DbOperator* DbOperator::CreateLoadGameInfo(DWORD dwSMID, DWORD dwGappID, DWORD dwInfoType, tagDbGameInfo *pDbGameInfo, DWORD dwNum, DWORD dwBeginTime, DWORD dwEndTime, LONG lUserID)
{
	if(NULL != pDbGameInfo)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_LoadGameInfo);
			if(pOpt->CreateData())
			{
				tagLoadGameInfo *pLoadGameInfo = (tagLoadGameInfo*)pOpt->m_pData;
				pLoadGameInfo->dwSMID		= dwSMID;
				pLoadGameInfo->dwGappID		= dwGappID;
				pLoadGameInfo->pDbGameInfo	= pDbGameInfo;
				pLoadGameInfo->dwNum		= dwNum;
				pLoadGameInfo->dwInfoType	= dwInfoType;
				pLoadGameInfo->dwBeginTime	= dwBeginTime;
				pLoadGameInfo->dwEndTime	= dwEndTime;
				pLoadGameInfo->lUserID		= lUserID;
				return pOpt;
			}
			else
			{
				M_FREE(pOpt, sizeof(DbOperator));
			}
		}
	}

	return NULL;
}

//! 创建一个保存举报信息的操作
DbOperator* DbOperator::CreateSaveReport(VOID *pDataBlock)
{
	if(NULL != pDataBlock)
	{
		DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
		if(NULL != pOpt)
		{
			new(pOpt)DbOperator(eDOOT_SaveReport);
			pOpt->m_pData = pDataBlock;
			return pOpt;
		}
	}
	return NULL;
}

//! 创建一个读取举报列表的操作
DbOperator* DbOperator::CreateLoadReport(__int64 ServerKey, LONG UserID)
{
	DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
	if(NULL != pOpt)
	{
		new(pOpt)DbOperator(eDOOT_LoadReport);
		if(pOpt->CreateData())
		{
			tagLoadReport *pLoadReport = (tagLoadReport*)pOpt->m_pData;
			pLoadReport->ServerKey = ServerKey;
			pLoadReport->UserID = UserID;
			return pOpt;
		}
		else
		{
			M_FREE(pOpt, sizeof(DbOperator));
		}
	}

	return NULL;
}

//! 创建一个读取玩家处理结果的操作
DbOperator* DbOperator::CreateLoadResult(__int64 ServerKey, LONG UserID, DWORD BeginDat)
{
	DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
	if(NULL != pOpt)
	{
		new(pOpt)DbOperator(eDOOT_LoadResult);
		if(pOpt->CreateData())
		{
			tagLoadResult *pLoadReport = (tagLoadResult*)pOpt->m_pData;
			pLoadReport->ServerKey = ServerKey;
			pLoadReport->BeginDate = BeginDat;
			pLoadReport->UserID = UserID;
			return pOpt;
		}
		else
		{
			M_FREE(pOpt, sizeof(DbOperator));
		}
	}

	return NULL;
}

//! 创建一个保存玩家处理结果成功的操作
DbOperator* DbOperator::CreateSaveOptResult(const CGUID &OptGuid, DWORD OptSuccp)
{
	DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
	if(NULL != pOpt)
	{
		new(pOpt)DbOperator(eDOOT_SaveOptResult);
		if(pOpt->CreateData())
		{
			tagSaveOptResult *pSaveOptResult = (tagSaveOptResult*)pOpt->m_pData;
			pSaveOptResult->OptGuid = OptGuid;
			pSaveOptResult->OptSuccp = OptSuccp;
			return pOpt;
		}
		else
		{
			M_FREE(pOpt, sizeof(DbOperator));
		}
	}

	return NULL;
}

//! 创建一个执行SQL语句的操作
DbOperator* DbOperator::CreateRunSql(const char *pSql, DWORD SqlSize)
{
	DbOperator* pOpt = (DbOperator*)M_ALLOC(sizeof(DbOperator));
	if(NULL != pOpt)
	{
		new(pOpt)DbOperator(eDOOT_RunSql);
		if(pOpt->CreateData())
		{
			tagDbSql *pDbSql = (tagDbSql*)pOpt->m_pData;
			pDbSql->pSql = pSql;
			pDbSql->SqlSize = SqlSize;
		}
		else
		{
			M_FREE(pOpt, sizeof(DbOperator));
		}
	}

	return pOpt;
}

//! 销毁操作
VOID DbOperator::ReleaseOperator(DbOperator **ppOperator)
{

	if(NULL != ppOperator && NULL != (*ppOperator))
	{
		(*ppOperator)->ReleaseData();
		M_FREE((*ppOperator), sizeof(DbOperator));
		*ppOperator = NULL;
	}
}

BOOL DbOperator::CreateData(VOID)
{

	LONG lSize = 0;
	switch(m_dwDOOT)
	{
	case eDOOT_PwdCmp:
	case eDOOT_PwdChange:
		lSize = sizeof(tagUserInfo);
		break;
	case eDOOT_InHard:
		lSize = sizeof(tagHardInfoGroup);
		break;
	case eDOOT_DBLogGroup:
		lSize = sizeof(tagDBLogGroup);
		break;
	case eDOOT_GameInfoGroup:
		lSize = sizeof(tagGameInfoGroup);
		break;
	case eDOOT_LoadGameInfo:
		lSize = sizeof(tagLoadGameInfo);
		break;
	case eDOOT_SaveReport:
		lSize = 0;
		break;
	case eDOOT_LoadReport:
		lSize = sizeof(tagLoadReport);
		break;
	case eDOOT_LoadResult:
		lSize = sizeof(tagLoadResult);
		break;
	case eDOOT_SaveOptResult:
		lSize = sizeof(tagSaveOptResult);
		break;
	case eDOOT_RunSql:
		lSize = sizeof(tagDbSql);
		break;
	default:
	    break;
	}

	if(0 < lSize)
	{
		m_pData = M_ALLOC(lSize);
		if(NULL != m_pData)
		{
			memset(m_pData, 0, lSize);
			return TRUE;
		}
	}

	return FALSE;
}
VOID DbOperator::ReleaseData(VOID)
{

	LONG lSize = 0;
	switch(m_dwDOOT)
	{
	case eDOOT_PwdCmp:
	case eDOOT_PwdChange:
		lSize = sizeof(tagUserInfo);
		break;
	case eDOOT_InHard:
		lSize = sizeof(tagHardInfoGroup);
		break;
	case eDOOT_DBLogGroup:
		lSize = sizeof(tagDBLogGroup);
		break;
	case eDOOT_GameInfoGroup:
		lSize = sizeof(tagGameInfoGroup);
		break;
	case eDOOT_LoadGameInfo:
		lSize = sizeof(tagLoadGameInfo);
		break;
	case eDOOT_SaveReport:
		return;
	case eDOOT_LoadReport:
		lSize = sizeof(tagLoadReport);
		break;
	case eDOOT_LoadResult:
		lSize = sizeof(tagLoadResult);
		break;
	case eDOOT_SaveOptResult:
		lSize = sizeof(tagSaveOptResult);
		break;
	case eDOOT_RunSql:
		lSize = sizeof(tagDbSql);
		break;
	default:
		break;
	}

	if(0 < lSize)
		M_FREE(m_pData, lSize);

}


//! 执行操作
BOOL DbOperator::Begin(VOID)
{

	if (!c_AsynchronousExecute.IsRunning())
		return FALSE;

	switch(m_dwDOOT)
	{
	case eDOOT_PwdCmp:
		{
			c_AsynchronousExecute.Execute(PwdCmp, this);
		}
		break;
	case eDOOT_PwdChange:
		{
			c_AsynchronousExecute.Execute(PwdChange, this);
		}
		break;
	case eDOOT_InHard:
		{
			c_AsynchronousExecute.Execute(InHard, this);
		}
		break;
	case eDOOT_DBLogGroup:
		{
			c_AsynchronousExecute.Execute(DbLog, this);
		}
		break;
	case eDOOT_GameInfoGroup:
		{
			c_AsynchronousExecute.Execute(GameInfo, this);
		}
		break;
	case eDOOT_LoadGameInfo:
		{
			c_AsynchronousExecute.Execute(LoadGameInfo, this);
		}
		break;
	case eDOOT_SaveReport:
		{
			c_AsynchronousExecute.Execute(SaveReport, this);
		}
		break;
	case eDOOT_LoadReport:
		{
			c_AsynchronousExecute.Execute(LoadReport, this);
		}
		break;
	case eDOOT_SaveOptResult:
		{
			c_AsynchronousExecute.Execute(SaveOptResult, this);
		}
		break;
	case eDOOT_LoadResult:
		{
			c_AsynchronousExecute.Execute(LoadResult, this);
		}
		break;
	case eDOOT_RunSql:
		{
			c_AsynchronousExecute.Execute(RunSql, this);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


//! 密码比较操作
LPVOID DbOperator::PwdCmp(LPVOID pThis)
{

	tagUserInfo *pUserInfo = (tagUserInfo*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			_CommandPtr		CmdPtr;
			_ParameterPtr	paraReturn, paraAccount, paraPwd, paraOutPwd;     
			_bstr_t			strProc = L"sp_user_pwd_cmp"; 
			_variant_t		varReturn, varRePwd;

			CmdPtr.CreateInstance(__uuidof(Command));
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->CommandText = strProc;
			CmdPtr->CommandType = adCmdStoredProc;

			//! 参数
			paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(LONG), varReturn);
			CmdPtr->Parameters->Append(paraReturn);

			paraAccount = CmdPtr->CreateParameter(L"SenderName", adVarChar, adParamInput, MAX_USER_NAME_SZIE, pUserInfo->szCdkey);
			CmdPtr->Parameters->Append(paraAccount);

			paraPwd = CmdPtr->CreateParameter(L"SenderPwd", adVarWChar, adParamInput, BASE_PASSWORD_SIZE, pUserInfo->szPwd);
			CmdPtr->Parameters->Append(paraPwd);

			//! Out参数
			paraOutPwd = CmdPtr->CreateParameter(L"ReturnPwd", adVarWChar, adParamReturnValue, BASE_PASSWORD_SIZE, varRePwd);
			CmdPtr->Parameters->Append(paraOutPwd);

			//! 执行
			CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 

			LONG lRunResult = CmdPtr->Parameters->GetItem("Return")->Value;

			_bstr_t BstrVar = CmdPtr->Parameters->GetItem("ReturnPwd")->Value;
			const char *pRePwd = (const char *)BstrVar;
			if(NULL == pRePwd)
				pRePwd = "";
			//! 把结果以消息形式返回主线程
			AinMessage *pAinMsg = AinMessage::Create(USER_PWD_CMP_RESULT);
			if(NULL != pAinMsg)
			{
				pAinMsg->AddDword(lRunResult);
				pAinMsg->AddStr(pUserInfo->szCdkey);
				pAinMsg->AddStr(pRePwd);
				ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
			}

			if(NULL != CmdPtr)
				CmdPtr.Release();
			c_CnPool.ReturnCnPtr(cnPtr);
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::PwdCmp error!!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);
	
	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 密码修改操作
LPVOID DbOperator::PwdChange(LPVOID pThis)
{

	tagUserInfo *pUserInfo = (tagUserInfo*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		if(!c_CnPool.GetCnPtr(cnPtr))
			break;

		_CommandPtr		CmdPtr;
		_ParameterPtr	paraReturn, paraAccount, paraPwd, paraOutPwd;     
		_bstr_t			strProc = L"sp_user_pwd_change"; 
		_variant_t		varReturn;

		CmdPtr.CreateInstance(__uuidof(Command));
		
		CmdPtr->CommandText = strProc;
		CmdPtr->CommandType = adCmdStoredProc;

		//! 参数
		paraReturn = CmdPtr->CreateParameter(L"Return", adInteger, adParamReturnValue, sizeof(LONG), varReturn);
		CmdPtr->Parameters->Append(paraReturn);

		paraAccount = CmdPtr->CreateParameter(L"UserName", adVarChar, adParamInput, MAX_USER_NAME_SZIE, pUserInfo->szCdkey);
		CmdPtr->Parameters->Append(paraAccount);

		paraPwd = CmdPtr->CreateParameter(L"NewPwd", adVarWChar, adParamInput, BASE_PASSWORD_SIZE, pUserInfo->szPwd);
		CmdPtr->Parameters->Append(paraPwd);


		try
		{
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::PwdChange error, restoring...", e);

			if(c_CnPool.RepairCnPtr(cnPtr))
			{
				try
				{
					CmdPtr->ActiveConnection = cnPtr.ptr;
					CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 
				}	
				catch(_com_error &e2)
				{
					CMyAdoBase::PrintErr("DbOperator::PwdChange error!!!", e2);
					if(NULL != CmdPtr)
						CmdPtr.Release();
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}	
		}

		LONG lRunResult = CmdPtr->Parameters->GetItem("Return")->Value;

		//! 把结果以消息形式返回主线程
		AinMessage *pAinMsg = AinMessage::Create(USER_PWD_CHANGE_RESULT);
		if(NULL != pAinMsg)
		{
			pAinMsg->AddDword(lRunResult);
			pAinMsg->AddStr(pUserInfo->szCdkey);
			pAinMsg->AddStr(pUserInfo->szPwd);
			ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
		}

		if(NULL != CmdPtr)
			CmdPtr.Release();
		c_CnPool.ReturnCnPtr(cnPtr);
		
	} while(false);

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}


//! 硬件信息组写入操作
LPVOID DbOperator::InHard(LPVOID pThis)
{

	tagHardInfoGroup *pHardInfoGroup = (tagHardInfoGroup*)((DbOperator*)pThis)->m_pData;

	BOOL bExecuteOK = FALSE;
	tagCnPtr cnPtr;
	do 
	{
		if(!c_CnPool.GetCnPtr(cnPtr))
			break;

		string strSQL;
		for (DWORD i = 0; i < pHardInfoGroup->dwNum; ++i)
		{
			tagHardwareInfo &HardwareInfo = pHardInfoGroup->pHardwareInfo[i];
			char szTmp[2048] = {0};
			sprintf(szTmp, "INSERT INTO sm_hardware_info ( \
							get_time, sm_id, \
							mem_Length, mem_MemoryLoad, mem_TotalPhys, \
							mem_AvailPhys, mem_TotalPageFile, mem_AvailPageFile, \
							mem_TotalVirtual, mem_AvailVirtual, \
							cpu_Usage, \
							cpu_SubUsage1, cpu_SubUsage2, cpu_SubUsage3, cpu_SubUsage4, \
							cpu_SubUsage5, cpu_SubUsage6, cpu_SubUsage7, cpu_SubUsage8, \
							cpu_SubUsage9, cpu_SubUsage10, cpu_SubUsage11, cpu_SubUsage12, \
							cpu_SubUsage13, cpu_SubUsage14, cpu_SubUsage15, cpu_SubUsage16 )\
							VALUES(\
							%u,%u,\
							%u,%u,%u,%u,%u,%u,%u,%u,\
							%u,\
							%u,%u,%u,%u,%u,%u,%u,%u,\
							%u,%u,%u,%u,%u,%u,%u,%u\
							);",
							HardwareInfo.dwTime,				pHardInfoGroup->dwSMID,
							HardwareInfo.stMS.dwLength,			HardwareInfo.stMS.dwMemoryLoad,		HardwareInfo.stMS.dwTotalPhys, 
							HardwareInfo.stMS.dwAvailPhys,		HardwareInfo.stMS.dwTotalPageFile,	HardwareInfo.stMS.dwAvailPageFile, 
							HardwareInfo.stMS.dwTotalVirtual,	HardwareInfo.stMS.dwAvailVirtual,
							HardwareInfo.cUsage,
							HardwareInfo.cSubUsage[0], HardwareInfo.cSubUsage[1], HardwareInfo.cSubUsage[2], HardwareInfo.cSubUsage[3],
							HardwareInfo.cSubUsage[4], HardwareInfo.cSubUsage[5], HardwareInfo.cSubUsage[6], HardwareInfo.cSubUsage[7],
							HardwareInfo.cSubUsage[8], HardwareInfo.cSubUsage[9], HardwareInfo.cSubUsage[10], HardwareInfo.cSubUsage[11],
							HardwareInfo.cSubUsage[12], HardwareInfo.cSubUsage[13], HardwareInfo.cSubUsage[14], HardwareInfo.cSubUsage[15]
						   );

				
			strSQL.append(szTmp);
		}

		try
		{
			//! 执行
			cnPtr.ptr->Execute(strSQL.c_str(), NULL, adCmdText); 
			bExecuteOK = TRUE;	
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::InHard error, restoring...", e);
			if(c_CnPool.RepairCnPtr(cnPtr))
			{
				try
				{
					//! 执行
					cnPtr.ptr->Execute(strSQL.c_str(), NULL, adCmdText); 
					bExecuteOK = TRUE;	
				}	
				catch(_com_error &e)
				{
					CMyAdoBase::PrintErr("DbOperator::InHard error!!!", e);
				}
			}
		}

		c_CnPool.ReturnCnPtr(cnPtr);
	} while(false);

	//! 把结果以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(WAITE_HARDWARE_FINISH);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword((DWORD)bExecuteOK);
		pAinMsg->AddDword((DWORD)pHardInfoGroup->pHardwareInfo);
		pAinMsg->AddDword(pHardInfoGroup->dwNum);
		pAinMsg->AddDword(pHardInfoGroup->dwSMID);
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

 
//! 日志组写入操作
LPVOID DbOperator::DbLog(LPVOID pThis)
{
	tagDBLogGroup *pDBLogGroup = (tagDBLogGroup*)((DbOperator*)pThis)->m_pData;

	BOOL bExecuteOK = FALSE;
	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			string strSQL;
			for (DWORD i = 0; i < pDBLogGroup->dwNum; ++i)
			{
				char szTmp[2048] = {0};
				sprintf(szTmp, "INSERT INTO action_log ( \
							   write_time, \
							   sm_id, gapp_id, user_name, \
							   log_txt \
							   )\
							   VALUES(\
							   %u, %d, %d, N'%s', N'%s' \
							   );",
							   pDBLogGroup->pDbLog[i].dwTime, 
							   pDBLogGroup->pDbLog[i].dwSmID, pDBLogGroup->pDbLog[i].dwGappID, pDBLogGroup->pDbLog[i].szUserName,
							   pDBLogGroup->pDbLog[i].szInfo
				);


				strSQL.append(szTmp);
			}

			//! 执行
			cnPtr.ptr->Execute(strSQL.c_str(), NULL, adCmdText); 

			c_CnPool.ReturnCnPtr(cnPtr);
			bExecuteOK = TRUE;
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::DbLog error!!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	//! 把结果以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(WAITE_DBLOG_FINISH);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword((DWORD)bExecuteOK);
		pAinMsg->AddDword((DWORD)pDBLogGroup->pDbLog);
		pAinMsg->AddDword(pDBLogGroup->dwNum);
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 游戏信息写入操作
LPVOID DbOperator::GameInfo(LPVOID pThis)
{
	tagGameInfoGroup *pGameInfoGroup = (tagGameInfoGroup*)((DbOperator*)pThis)->m_pData;

	BOOL bExecuteOK = FALSE;
	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			string strSQL;
			for (DWORD i = 0; i < pGameInfoGroup->dwNum; ++i)
			{
				char szTmp[2048] = {0};
				sprintf(szTmp, "INSERT INTO game_info ( \
							   log_time, \
							   sm_id, gapp_id, info_type,\
							   value1, value2\
							   )\
							   VALUES(\
							   %u, %d, %d, %d, %d, %d\
							   );",
							   pGameInfoGroup->pDbGameInfo[i]._tTime,
							   pGameInfoGroup->dwSMID, pGameInfoGroup->dwGappID, pGameInfoGroup->pDbGameInfo[i].dwInfoType, 
							   pGameInfoGroup->pDbGameInfo[i]._dwData1, pGameInfoGroup->pDbGameInfo[i]._dwData2
							   );


				strSQL.append(szTmp);
			}

			//! 执行
			cnPtr.ptr->Execute(strSQL.c_str(), NULL, adCmdText); 

			c_CnPool.ReturnCnPtr(cnPtr);
			bExecuteOK = TRUE;
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::GameInfo error !!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	//! 把结果以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(WAITE_GAME_INFO_FINISH);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword((DWORD)bExecuteOK);
		pAinMsg->AddDword((DWORD)pGameInfoGroup->pDbGameInfo);
		pAinMsg->AddDword(pGameInfoGroup->dwNum);
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 日志读取操作
LPVOID DbOperator::LoadGameInfo(LPVOID pThis)
{
	tagLoadGameInfo *pLoadGameInfo = (tagLoadGameInfo*)((DbOperator*)pThis)->m_pData;

	LONG lDbInfoNum = 0;
	vector<BYTE> vReData;
	BOOL bExecuteOK = FALSE;
	do 
	{
		tagCnPtr cnPtr;
		try
		{
			_RecordsetPtr RsPtr;
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			char szTmp[2048] = {0};
			sprintf(szTmp,"SELECT * FROM game_info WHERE sm_id=%d AND gapp_id=%d AND info_type=%d AND log_time>=%d AND log_time<=%d",
				pLoadGameInfo->dwSMID,
				pLoadGameInfo->dwGappID,
				pLoadGameInfo->dwInfoType,
				pLoadGameInfo->dwBeginTime,
				pLoadGameInfo->dwEndTime);

			//! 执行
			RsPtr = cnPtr.ptr->Execute(szTmp, NULL, adCmdText);

			while (!RsPtr->GetadoEOF())
			{
				DWORD dwTime	= (DWORD)RsPtr->GetCollect("log_time");
				DWORD dwValue1	= (DWORD)RsPtr->GetCollect("value1");
				DWORD dwValue2	= (DWORD)RsPtr->GetCollect("value2");

				_AddToByteArray(&vReData, dwTime);
				_AddToByteArray(&vReData, dwValue1);
				_AddToByteArray(&vReData, dwValue2);
				++lDbInfoNum;
				RsPtr->MoveNext();
			}

			c_CnPool.ReturnCnPtr(cnPtr);
			bExecuteOK = TRUE;			
		}
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::LoadGameInfo error!!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);
	
	AinMessage *pAinMsg = AinMessage::Create(LOAD_GAME_INFO_FINISH);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword(bExecuteOK);
		pAinMsg->AddDword(pLoadGameInfo->lUserID);
		pAinMsg->AddDword((DWORD)pLoadGameInfo->pDbGameInfo);
		pAinMsg->AddDword(pLoadGameInfo->dwSMID);
		pAinMsg->AddDword(pLoadGameInfo->dwGappID);
		pAinMsg->AddDword(pLoadGameInfo->dwInfoType);
		if(bExecuteOK)
		{
			for (DWORD i = 0; i < pLoadGameInfo->dwNum; ++i)
			{
				_AddToByteArray(&vReData, (DWORD)pLoadGameInfo->pDbGameInfo[i]._tTime);
				_AddToByteArray(&vReData, pLoadGameInfo->pDbGameInfo[i]._dwData1);
				_AddToByteArray(&vReData, pLoadGameInfo->pDbGameInfo[i]._dwData2);
			}
			pAinMsg->AddDword((DWORD)vReData.size());
			if(0 != vReData.size())
				pAinMsg->AddEx(&vReData[0], (LONG)vReData.size());
		}
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);
	return NULL;
}

//! 保存举报信息的操作
LPVOID DbOperator::SaveReport(LPVOID pThis)
{
	const DataCache<tagReportInfo>::tagDataBlock *pDataBlock = (const DataCache<tagReportInfo>::tagDataBlock*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			for (DWORD i = 0; i < pDataBlock->DataNum; ++i)
			{
				_CommandPtr		CmdPtr;    
				_bstr_t			strProc = L"sp_save_report"; 

				CmdPtr.CreateInstance(__uuidof(Command));
				CmdPtr->ActiveConnection = cnPtr.ptr;
				CmdPtr->CommandText = strProc;
				CmdPtr->CommandType = adCmdStoredProc;

				_ParameterPtr paraIndicteeName, paraReportName, paraReportType, paraReportTime, paraServerKey; 

				paraIndicteeName = CmdPtr->CreateParameter(L"IndicteeName", adVarChar, adParamInput, GAPP_NAME_SZIE, pDataBlock->pData[i]._IndicteeName);
				CmdPtr->Parameters->Append(paraIndicteeName);

				paraReportName = CmdPtr->CreateParameter(L"ReportName", adVarChar, adParamInput, GAPP_NAME_SZIE, pDataBlock->pData[i]._ReportName);
				CmdPtr->Parameters->Append(paraReportName);

				paraReportType = CmdPtr->CreateParameter(L"ReportType", adInteger, adParamInput, sizeof(DWORD), pDataBlock->pData[i]._ReportType);
				CmdPtr->Parameters->Append(paraReportType);

				__int64 Time64 = pDataBlock->pData[i]._ReportTime;
				paraReportTime = CmdPtr->CreateParameter(L"ReportTime", adBigInt, adParamInput, sizeof(__int64), _variant_t(Time64));
				CmdPtr->Parameters->Append(paraReportTime);

				GmSystem::tagKey ServerKey(pDataBlock->pData[i]._WsSmId, pDataBlock->pData[i]._WsGappId);
				paraServerKey = CmdPtr->CreateParameter(L"ServerKey", adBigInt, adParamInput, sizeof(__int64), _variant_t(ServerKey._64Key));
				CmdPtr->Parameters->Append(paraServerKey);

				try
				{
					//! 执行
					CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 

					if(NULL != CmdPtr)
						CmdPtr.Release();
				}
				catch (_com_error &e)
				{
					CMyAdoBase::PrintErr("DbOperator::SaveReport error 1 !!!", e);
					continue;
				}
			}
			
			c_CnPool.ReturnCnPtr(cnPtr);
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::SaveReport error 2 !!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	//! 把使用完的数据指针以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(RETURN_REPORT_DATA);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword((DWORD)pDataBlock);
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 读取举报信息
LPVOID DbOperator::LoadReport(LPVOID pThis)
{
	tagLoadReport *pLoadReport = (tagLoadReport*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			_bstr_t			strProc = L"sp_load_report"; 
			_RecordsetPtr	RsPtr;
			_CommandPtr		CmdPtr;
			RsPtr.CreateInstance(__uuidof(Recordset));
			CmdPtr.CreateInstance(__uuidof(Command));
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->CommandText = strProc;
			CmdPtr->CommandType = adCmdStoredProc;

			_ParameterPtr paraServerKey; 

			paraServerKey = CmdPtr->CreateParameter(L"ServerKey", adBigInt, adParamInput, sizeof(__int64), _variant_t(pLoadReport->ServerKey));
			CmdPtr->Parameters->Append(paraServerKey);

			//! 执行
			RsPtr = CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 

			string strCurrIndicteeName = "";
			vector<BYTE> vData;

			while (!RsPtr->GetadoEOF())
			{
				DWORD ReportNum				= (DWORD)RsPtr->GetCollect("report_num");
				
				tagBaseReportInfo BaseReportInfo;

				_bstr_t bstrIndicteeName = (_bstr_t)RsPtr->GetCollect("indictee_name");
				const char *pIndicteeName	= (const char*)bstrIndicteeName;
				strcpy_s(BaseReportInfo._IndicteeName, GAPP_NAME_SZIE, pIndicteeName);

				_bstr_t bstrReportName = (_bstr_t)RsPtr->GetCollect("report_name");
				const char *pReportName		= (const char*)(_bstr_t)bstrReportName;
				strcpy_s(BaseReportInfo._ReportName, GAPP_NAME_SZIE, pReportName);

				BaseReportInfo._ReportType	= (DWORD)RsPtr->GetCollect("report_type");
				BaseReportInfo._ReportTime	= (DWORD)(__int64)RsPtr->GetCollect("report_time");

				if (0 != stricmp(strCurrIndicteeName.c_str(), pIndicteeName))
				{
					strCurrIndicteeName = pIndicteeName;
					_AddToByteArray(&vData, ReportNum);
				}

				_AddToByteArray(&vData, &BaseReportInfo, sizeof(tagBaseReportInfo));

				RsPtr->MoveNext();
			}

			CMessage msg(MSG_SCC2SUC_MANAGE_Notify_ReportList);
			msg.Add((DWORD)vData.size());
			if(0 != vData.size())
				msg.AddEx(&vData[0], (LONG)vData.size());
			msg.SendToSuc(pLoadReport->UserID);
			

			CmdPtr.Release();
				
			c_CnPool.ReturnCnPtr(cnPtr);
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::LoadReport error!!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 读取处理结果信息
LPVOID DbOperator::LoadResult(LPVOID pThis)
{
	tagLoadResult *pLoadResult = (tagLoadResult*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			_bstr_t			strProc = L"sp_load_result"; 
			_RecordsetPtr	RsPtr;
			_CommandPtr		CmdPtr;
			RsPtr.CreateInstance(__uuidof(Recordset));
			CmdPtr.CreateInstance(__uuidof(Command));
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->CommandText = strProc;
			CmdPtr->CommandType = adCmdStoredProc;

			_ParameterPtr paraServerKey, paraBeginTime;

			paraServerKey = CmdPtr->CreateParameter(L"ServerKey", adBigInt, adParamInput, sizeof(__int64), _variant_t(pLoadResult->ServerKey));
			CmdPtr->Parameters->Append(paraServerKey);
			paraBeginTime = CmdPtr->CreateParameter(L"ServerKey", adBigInt, adParamInput, sizeof(__int64), _variant_t((__int64)pLoadResult->BeginDate));
			CmdPtr->Parameters->Append(paraBeginTime);

			//! 执行
			RsPtr = CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 

			vector<BYTE> vData;
			_AddToByteArray(&vData, 0L);
			DWORD DataNum = 0;

			while (!RsPtr->GetadoEOF())
			{
				tagBaseResultInfo BaseResultInfo;

				_bstr_t bstrIndicteeName = (_bstr_t)RsPtr->GetCollect("indictee_name");
				const char *pIndicteeName	= (const char*)bstrIndicteeName;
				strcpy_s(BaseResultInfo._IndicteeName, GAPP_NAME_SZIE, pIndicteeName);

				_bstr_t bstrGmName = (_bstr_t)RsPtr->GetCollect("gm_name");
				const char *pGmName	= (const char*)bstrGmName;
				strcpy_s(BaseResultInfo._szGmName, GAPP_NAME_SZIE, pGmName);

				BaseResultInfo._OptTime = (DWORD)RsPtr->GetCollect("log_time");
				BaseResultInfo._OptResult = (DWORD)RsPtr->GetCollect("opt_type");
				BaseResultInfo._dwCause = (DWORD)RsPtr->GetCollect("opt_cause");
				BaseResultInfo._lBanTime = (DWORD)RsPtr->GetCollect("opt_value");

				_bstr_t bstrWhys = (_bstr_t)RsPtr->GetCollect("whys");
				const char *pWhys	= (const char*)bstrWhys;

				_bstr_t bstrLog = (_bstr_t)RsPtr->GetCollect("opt_log");
				const char *pLog	= (const char*)bstrLog;

				DWORD OptSccd = (DWORD)RsPtr->GetCollect("opt_succeed");

				_AddToByteArray(&vData, &BaseResultInfo, sizeof(tagBaseResultInfo));
				_AddToByteArray(&vData, pWhys);
				_AddToByteArray(&vData, pLog);
				_AddToByteArray(&vData, OptSccd);

				++DataNum;
				RsPtr->MoveNext();
			}

			*((LONG*)&vData[0]) = DataNum;
			CMessage msg(MSG_SCC2SUC_MANAGE_Notify_ResultList);
			msg.Add((DWORD)vData.size());
			if(0 != vData.size())
				msg.AddEx(&vData[0], (LONG)vData.size());
			msg.SendToSuc(pLoadResult->UserID);

			CmdPtr.Release();

			c_CnPool.ReturnCnPtr(cnPtr);
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::LoadResult error !!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 保存玩家处理结果成功的操作
LPVOID DbOperator::SaveOptResult(LPVOID pThis)
{
	tagSaveOptResult *pSaveOptResult = (tagSaveOptResult*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		try
		{
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			_CommandPtr		CmdPtr;    
			_bstr_t			strProc = L"sp_report_execute"; 

			CmdPtr.CreateInstance(__uuidof(Command));
			CmdPtr->ActiveConnection = cnPtr.ptr;
			CmdPtr->CommandText = strProc;
			CmdPtr->CommandType = adCmdStoredProc;

			_ParameterPtr paraOptGuid, paraOptSuccp; 

			char szGUID[64] = {0};
			pSaveOptResult->OptGuid.tostring(szGUID);
			paraOptGuid = CmdPtr->CreateParameter(L"opt_id", adGUID, adParamInput, (ADO_LONGPTR)strlen(szGUID), szGUID);
			CmdPtr->Parameters->Append(paraOptGuid);

			paraOptSuccp = CmdPtr->CreateParameter(L"opt_succeed", adInteger, adParamInput, sizeof(DWORD), pSaveOptResult->OptSuccp);
			CmdPtr->Parameters->Append(paraOptSuccp);


			//! 执行
			CmdPtr->Execute(NULL,NULL,adCmdStoredProc); 

			if(NULL != CmdPtr)
				CmdPtr.Release();

			c_CnPool.ReturnCnPtr(cnPtr);
		}	
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::SaveOptResult error!!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}

//! 执行一段无返回值的SQL语句
LPVOID DbOperator::RunSql(LPVOID pThis)
{
	tagDbSql *pDbSql = (tagDbSql*)((DbOperator*)pThis)->m_pData;

	tagCnPtr cnPtr;
	do 
	{
		tagCnPtr cnPtr;
		try
		{
			_RecordsetPtr RsPtr;
			if(!c_CnPool.GetCnPtr(cnPtr))
				break;
			if(cnPtr.ptr->State != adStateOpen)
			{
				if(!c_CnPool.RepairCnPtr(cnPtr))
				{
					c_CnPool.ReturnCnPtr(cnPtr);
					break;
				}
			}

			//! 执行
			RsPtr = cnPtr.ptr->Execute(pDbSql->pSql, NULL, adCmdText);
			c_CnPool.ReturnCnPtr(cnPtr);		
		}
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("DbOperator::RunSql error !!!", e);
			c_CnPool.ReturnCnPtr(cnPtr);
		}
	} while(false);

	//! 把使用完的数据指针以消息形式返回主线程
	AinMessage *pAinMsg = AinMessage::Create(SQL_FINISH);
	if(NULL != pAinMsg)
	{
		pAinMsg->AddDword((DWORD)pDbSql->pSql);
		pAinMsg->AddDword((DWORD)pDbSql->SqlSize);
		ServiceNet::GetInstance().GetMsgQueue().PushMessage(pAinMsg);
	}

	ReleaseOperator((DbOperator**)&pThis);

	return NULL;
}