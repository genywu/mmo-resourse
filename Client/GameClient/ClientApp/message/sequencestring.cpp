#include "StdAfx.h"
#include ".\sequencestring.h"
#include "../../../Public/Common/Public.h"
/*
* 	static for class
*/
long CSequenceString::m_sStartPos = 0;
SequenceString CSequenceString::m_sSequenceString;
unsigned long CSequenceString::m_pos = 0;
bool CSequenceString::m_bIsUsable = false;


void CSequenceString::Initialize(BYTE* pByte, long& pos)
{
	m_sStartPos = (UINT)_GetLongFromByteArray(pByte, pos);
	UINT count = (UINT)_GetLongFromByteArray(pByte, pos);

	for(UINT i=0; i<count; i++)
	{
		m_sSequenceString.push_back(_GetLongFromByteArray(pByte, pos));
	}

	m_pos = m_sStartPos;
	m_bIsUsable = true;
}

void CSequenceString::Uninitialize()
{
	m_sSequenceString.clear();
	m_bIsUsable = false;
	m_sStartPos = -1;
}



CSequenceString::CSequenceString()
{

}


CSequenceString::~CSequenceString()
{
}

// 循环获取一个String，发送到GameServer 
unsigned long CSequenceString::GetElement()
{
	if(m_sSequenceString.size() == 0 || m_pos == -1 ) return 0;

	if(m_pos >= (ulong)m_sSequenceString.size() )
		m_pos = 0;

	return  m_sSequenceString[m_pos++];
}

void CSequenceString::Clearup()
{
	m_sSequenceString.clear();
	m_bIsUsable = false;
	m_sStartPos = -1;
}