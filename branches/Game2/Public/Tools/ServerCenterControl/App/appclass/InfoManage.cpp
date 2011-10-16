


#include "stdafx.h"
#include "InfoManage.h"

InfoManage InfoManage::c_InfoManage;

InfoManage::InfoManage(VOID)
{

}

InfoManage::~InfoManage(VOID)
{
}

InfoManage&	InfoManage::GetInstance()
{
	return c_InfoManage;
}


BOOL InfoManage::Init(VOID)
{
	m_mapHypotaxisTable.clear();

	LONG lLoopCount = sizeof(g_arrHypotaxisTable) / sizeof(tagHypotaxis);
	for (LONG i = 0; lLoopCount > i; ++i)
	{
		m_mapHypotaxisTable[g_arrHypotaxisTable[i].dwSubType] = g_arrHypotaxisTable[i].dwParentType;
	}

	return TRUE;
}

VOID InfoManage::Release(VOID)
{

}

//! 得到一个类型的父类型
DWORD InfoManage::GetParentType(DWORD dwInfoType)
{
	map<DWORD, DWORD>::iterator ite = m_mapHypotaxisTable.find(dwInfoType);
	if (m_mapHypotaxisTable.end() != ite)
	{
		return ite->second;
	}
	return eIT_Invalid;
}