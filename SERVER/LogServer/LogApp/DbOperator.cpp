

#include "stdafx.h"
#include "DbOperator.h"



map<LONG, DbOperator::tagExecuteData> DbOperator::c_mapExecuteData;

#ifdef APEX_TEST
LONG DbOperator::c_lLostNum = 0;
LONG DbOperator::c_lFinshNum = 0;
#endif

VOID DbOperator::Release(VOID)
{
	map<LONG, tagExecuteData>::iterator ite = c_mapExecuteData.begin();
	for (; c_mapExecuteData.end() != ite; ++ite)
	{
		SAFE_DELETE(ite->second.pCnPool);
	}
	c_mapExecuteData.clear();
}

//! 添加一个执行数据信息
BOOL DbOperator::AddExecuteData(LONG lDbID, string &strDbConn, string &strErrorLogFileName, string &strBusyLogFileName)
{
	assert(c_mapExecuteData.end() == c_mapExecuteData.find(lDbID));
	if(c_mapExecuteData.end() != c_mapExecuteData.find(lDbID))
		return FALSE;
	tagExecuteData ExecuteData;
	ExecuteData.pCnPool = new CnPool(10L);
	ExecuteData.pCnPool->Init(strDbConn);
	ExecuteData._strErrorLogFileName	= strErrorLogFileName;
	ExecuteData._strBusyLogFileName		= strBusyLogFileName;
	InitializeCriticalSection(&ExecuteData._FileCriticalSection);
	c_mapExecuteData[lDbID] = ExecuteData;
	return TRUE;
}

//! 释放所有执行数据信息
VOID DbOperator::ReleaseExecuteData(VOID)
{
	map<LONG, tagExecuteData>::iterator ite = c_mapExecuteData.begin();
	for ( ; ite != c_mapExecuteData.end(); ++ite)
	{
		DeleteCriticalSection(&(ite->second._FileCriticalSection));
	}
	c_mapExecuteData.clear();
}

//! 创建一个操作对象
DbOperator* DbOperator::CreateDbOperator(LONG lDbID)
{
	map<LONG, tagExecuteData>::iterator ite = c_mapExecuteData.find(lDbID);
	assert(c_mapExecuteData.end() != ite);
	if(c_mapExecuteData.end() == ite)
		return NULL;
	DbOperator *pDbOperator = (DbOperator*)M_ALLOC(sizeof(DbOperator));
	new(pDbOperator)DbOperator();

	pDbOperator->m_pExecuteData = &(ite->second);
	return pDbOperator;
}

//! 释放对象
VOID DbOperator::ReleaseDbOperator(DbOperator **ppDbOperator)
{
	if(NULL != ppDbOperator)
	{
		(*ppDbOperator)->~DbOperator();
		M_FREE(*ppDbOperator, sizeof(DbOperator));
	}
}

//! 执行
VOID DbOperator::ExecuteSql(VOID)
{
	GetGame()->GetAsynchronousExecute().Execute(ExecuteThread, this, OnExecuteFinish);
}


//! 执行线程
LPVOID DbOperator::ExecuteThread(LPVOID pThis)
{
	DbOperator *pDbOperator = (DbOperator*)pThis;

	tagCnPtr cnPtr;
	if(!pDbOperator->m_pExecuteData->pCnPool->GetCnPtr(cnPtr))
	{
		return pThis;
	}

	try
	{
		cnPtr.ptr->Execute(pDbOperator->m_strSql.c_str(), NULL, adCmdText);
	}
	catch(_com_error &e)
	{
		CMyAdoBase::PrintErr("DbOperator::ExecuteThread failed, go on!", e);

		if (pDbOperator->m_pExecuteData->pCnPool->RepairCnPtr(cnPtr))
		{
			try
			{	
				cnPtr.ptr->Execute(pDbOperator->m_strSql.c_str(), NULL, adCmdText);
			}
			catch(_com_error &err)
			{
				CMyAdoBase::PrintErr("DbOperator::ExecuteThread again failed, end!", err);
				pDbOperator->m_pExecuteData->pCnPool->ReturnCnPtr(cnPtr);
				return pThis;
			}
		}
		else
		{
			AddErrorLogText("Repair CnPtr failed, end!");
			pDbOperator->m_pExecuteData->pCnPool->ReturnCnPtr(cnPtr);
			return pThis;
		}	
	}

	pDbOperator->m_pExecuteData->pCnPool->ReturnCnPtr(cnPtr);
	ReleaseDbOperator(&pDbOperator);
	return NULL;
}

//! DB失败处理
LPVOID DbOperator::OnExecuteFinish(LPVOID pThis)
{
	if(NULL != pThis)
	{
		DbOperator *pDbOperator = (DbOperator*)pThis;

		char szDate[64] = {0};
		time_t currTime = time(NULL);

		ofstream ErrorLogFile;

		EnterCriticalSection(&(pDbOperator->m_pExecuteData->_FileCriticalSection));
		tm *pTm = localtime(&currTime);
		sprintf(szDate, "log/%04d-%02d-%02d ", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday + 1);
		string strFileName = szDate + pDbOperator->m_pExecuteData->_strErrorLogFileName;
		ErrorLogFile.open(strFileName.c_str(), ios::app);
		ErrorLogFile <<pDbOperator->m_strSql << endl;
		ErrorLogFile.close();
		LeaveCriticalSection(&(pDbOperator->m_pExecuteData->_FileCriticalSection));

		ReleaseDbOperator(&pDbOperator);
	}
	return NULL;
}