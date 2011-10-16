#include "StdAfx.h"
#include "Step.h"
#include "Do.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStep::CStep(void)
{
}

CStep::~CStep(void)
{	
	itDo it=m_DoSome.begin();
	for (;it!=m_DoSome.end();++it)
	{
		OBJ_RELEASE(CDo, *it);
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
//     if (Check && pParam && m_DoSome.size()>0)
//     {
//         size_t size = m_DoSome.size();
//         m_DoSome[size-1]->AnnexGeniusStates( pParam );
//     }
	return eRet;
}
