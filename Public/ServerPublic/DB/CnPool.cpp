#include "StdAfx.h"
#include "CnPool.h"

const tagCnPtr CnPool::InvalidPtr;

CnPool::CnPool(long lMaxCnNum)
:m_Num(0)
,m_bCanGet(false)
,m_lMaxCnNum(lMaxCnNum)
,m_bAbleRun(false)
{
	
}
CnPool::~CnPool(void)
{
	Release();
}

void CnPool::Init(const string &strCn)
{
	InitializeCriticalSection(&m_CriticalSection);
	m_strCn = strCn;
	m_bCanGet = true;
	m_bAbleRun = true;
}
void CnPool::Release(void)
{
	m_bCanGet = false;

	if(m_bAbleRun)
	{
		while (true)
		{
			EnterCriticalSection(&m_CriticalSection);
			if(m_Num == m_listCn.size())
			{
				LeaveCriticalSection(&m_CriticalSection);
				break;
			}

			LeaveCriticalSection(&m_CriticalSection);
			Sleep(512);
		}

		DeleteCriticalSection(&m_CriticalSection);

		for (list<tagCnPtr>::iterator ite = m_listCn.begin(); m_listCn.end() != ite; ++ite)
		{
			try
			{
				if(ite->ptr->State != adStateOpen)
					ite->ptr->Close();
				ite->ptr.Release();
			}
			catch (...)
			{

			}
		}
		DeleteCriticalSection(&m_CriticalSection);
	}
	m_bAbleRun = false;
}
bool CnPool::GetCnPtr(tagCnPtr &cnPtr)
{
	if(!m_bCanGet)
		return false;

	EnterCriticalSection(&m_CriticalSection);
	
	//! 返回一个空闲的连接
	if(0 != m_listCn.size())
	{
		cnPtr = *(m_listCn.begin());
		m_listCn.pop_front();

		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}

	//! 如果空闲连接已经用完，则判断是否新增连接
	if(m_lMaxCnNum <= m_Num)
	{
		LeaveCriticalSection(&m_CriticalSection);
		return false;
	}

	LeaveCriticalSection(&m_CriticalSection);

	bool bRe = RepairCnPtr(cnPtr);
	EnterCriticalSection(&m_CriticalSection);
	m_Num++;
	LeaveCriticalSection(&m_CriticalSection);
	return bRe;
}

bool CnPool::RepairCnPtr(tagCnPtr &cnPtr)
{
	if(!m_bCanGet)
		return false;

	try
	{
		if(cnPtr.ptr->State == adStateOpen)
			cnPtr.ptr->Close();
		cnPtr.ptr.Release();
	}
	catch (...)
	{
		
	}

	try
	{
		if(S_OK != CMyAdoBase::CreateCn(cnPtr.ptr))
		{
			return false;
		}

		if(S_OK != CMyAdoBase::OpenCn(cnPtr.ptr, m_strCn))
		{
			cnPtr.ptr.Release();
			return false;
		}
	}
	catch(_com_error &e)
	{
		string strErrInfo = m_strCn + " CnPool::RepairCnPtr ReConnection Failed";
		CMyAdoBase::PrintErr(strErrInfo.c_str(), e);
		try
		{
			cnPtr.ptr->Close();
			cnPtr.ptr.Release();
		}
		catch (_com_error)
		{	
		}

		return false;
	}

	return true;
}
void CnPool::ReturnCnPtr(tagCnPtr cnPtr)
{
	EnterCriticalSection(&m_CriticalSection);
	m_listCn.push_back(cnPtr);
	LeaveCriticalSection(&m_CriticalSection);
}