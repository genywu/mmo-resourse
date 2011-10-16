#include "StdAfx.h"
#include "organizing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COrganizing::COrganizing(void)
{
}

COrganizing::~COrganizing(void)
{
}

BOOL COrganizing::AddToByteArray(vector<BYTE>& ByteArray)
{
	_AddToByteArray(&ByteArray, m_guid);
	_AddToByteArray(&ByteArray, m_szName, 16);
	return TRUE;
}

BOOL COrganizing::Decode(BYTE* pData, LONG &lPos)
{
	_GetBufferFromByteArray(pData, lPos, m_guid);
	_GetBufferFromByteArray(pData, lPos, m_szName, 16);
	return TRUE;
}

const CGUID&	COrganizing::GetExID()
{ 
	return m_guid; 
}
const CGUID&	COrganizing::SetExID(const CGUID& guid) 
{ 
	return m_guid = guid; 
}
const char* COrganizing::GetName()const
{
	return m_szName;
}
void COrganizing::SetName(const char* pName)
{
	if (NULL != pName)
	{
		LONG lSize = strlen(pName);
		lSize = (15 >= lSize ) ? lSize : 15;
		memmove(m_szName, pName, lSize);
	}
}
