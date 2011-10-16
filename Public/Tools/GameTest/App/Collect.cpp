


#include "stdafx.h"
#include "Collect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPlayerCollect::CPlayerCollect()
{

}

CPlayerCollect::~CPlayerCollect()
{
	Release();
}

void CPlayerCollect::Release()
{

}

void CPlayerCollect::DecodeFromDataBlock(DBReadSet& dbRead)
{
	m_dwCollectExp		= dbRead.GetLongFromByteArray();
	m_dwCollectLevel	= dbRead.GetLongFromByteArray();
	m_dwFactureExp		= dbRead.GetLongFromByteArray();
	m_dwFactureLevel	= dbRead.GetLongFromByteArray();

	long lNum = dbRead.GetLongFromByteArray();
	for (int i = 0; i < lNum; ++i )
	{
		DWORD dwFactureID = dbRead.GetLongFromByteArray();
		m_vecFactureEntrys.push_back(dwFactureID);
	}
}