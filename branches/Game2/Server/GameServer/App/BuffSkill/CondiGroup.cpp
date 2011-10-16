#include "StdAfx.h"
#include "CondiGroup.h"
#include "Condition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCondiGroup::CCondiGroup(void)
{
}

CCondiGroup::~CCondiGroup(void)
{
	itCond it=m_Conditions.begin();
	for (;it!=m_Conditions.end();++it)
	{
		OBJ_RELEASE(CCondition, *it);
		
	}
	m_Conditions.clear();
}


//用&&组合所有条件判断结果
bool CCondiGroup::ResultByAnd(CMoveShape* pDest,stModuParam *pParam)
{
	bool bRet = true;
	itCond it = m_Conditions.begin();
	for(;it != m_Conditions.end();it++)
	{
		CCondition *pCondition = (*it);
		if (pCondition)
		{
			pCondition->SetParam(pParam);
			bRet &= pCondition->Result(pDest);
		}
		if(!bRet)
			break;
	}
	return bRet;
}
//用||组合所有条件判断结果
bool CCondiGroup::ResultByOr(CMoveShape* pDest,stModuParam *pParam)
{
	bool bRet = false;
	itCond it = m_Conditions.begin();
	for(;it != m_Conditions.end();it++)
	{
		CCondition *pCondition = (*it);
		if (pCondition)
		{
		  pCondition->SetParam(pParam);
		  bRet = pCondition->Result(pDest);
		}
	
		if(bRet)
			break;
	}
	return bRet;
}

bool CCondiGroup::Result(CMoveShape* pDest,stModuParam *pParam)
{
	if (m_Conditions.size()==0)
	{
		return true;
	}

	if(m_eMode == Mode_And)
	{
		return ResultByAnd(pDest,pParam);
	}
	else if(m_eMode == Mode_Or)
	{
		return ResultByOr(pDest,pParam);
	}
	return false;
}
