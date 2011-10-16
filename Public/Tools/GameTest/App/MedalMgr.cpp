


#include "stdafx.h"
#include "MedalMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMedalMgr::CMedalMgr()
{

}

CMedalMgr::~CMedalMgr()
{
	Release();
}

void CMedalMgr::Release()
{

}

void CMedalMgr::DecodeFromDataBlock(DBReadSet& dbRead)
{
	m_vecMedal.clear();

	long lNum = dbRead.GetLongFromByteArray();

	for(int i=0;i<lNum;i++)
	{
		DWORD dwMedalIndex = dbRead.GetLongFromByteArray();
		m_vecMedal.push_back(dwMedalIndex);
	}
}