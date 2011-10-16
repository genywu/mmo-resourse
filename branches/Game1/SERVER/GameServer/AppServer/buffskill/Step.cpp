#include "StdAfx.h"
#include "Step.h"
#include "Do.h"
#include "..\..\..\..\public\SkillRelated.h"




CStep::CStep(void)
{
}

CStep::~CStep(void)
{	
	itDo it=m_DoSome.begin();
	for (;it!=m_DoSome.end();++it)
	{
		MP_DELETE(*it);
	}
	m_DoSome.clear();
}

//初始化基本数据
void CStep::InitBaseData(CDo *pDo)
{
	m_DoSome.push_back(pDo);
}

//处理所有的操作
eModuleRet CStep::DoSome(stModuParam *pParam)
{
	
	eModuleRet eRet = MR_Proc_End;
	itDo it = m_DoSome.begin();
	for(;it != m_DoSome.end();it++)
	{ 
		eRet = (*it)->Execute(pParam);

		//如果是结束,就返回
		if(eRet != MR_Continue)
			break;
	}
	return eRet;
}
