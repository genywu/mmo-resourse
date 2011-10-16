#include "StdAfx.h"
#include "RegionList.h"

CRegionList::CRegionList(void)
{
	m_uID			= 0; 
	m_uResourceID	= 0;
	m_uExpScale		= 0;
	m_uType			= 0;
	m_strName		= _T("");
	m_uGS_ID		= 0;
	m_uCountry		= 0;
	m_uNotify		= 0;
}

CRegionList::~CRegionList(void)
{
}

// 读场景列表文件 "regionlist.ini"
ifstream & operator>>(ifstream &rstream, CRegionList &regionlist)
{
	rstream >> regionlist.m_uID >> regionlist.m_uResourceID 
		>> regionlist.m_uExpScale >> regionlist.m_uType
		>> regionlist.m_uNoPK >> regionlist.m_uContribution
		>> regionlist.m_strName >> regionlist.m_uGS_ID 
		>> regionlist.m_uCountry >> regionlist.m_uNotify;

	return rstream;
}
