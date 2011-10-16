//////////////////////////////////////////////////////////////////////////
//worldvillageregion.cpp
//村战战地图类，负责村战的数据处理
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////



#include "StdAfx.h"
#include ".\worldvillageregion.h"
#include "Organizingsystem\VillageWarSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWorldVillageRegion::CWorldVillageRegion(void)
{
	m_lSymbolTotalNum=1;
	m_lWinVicSymbolNum=1;
	m_lVicSymbolNum=1;
}

CWorldVillageRegion::~CWorldVillageRegion(void)
{
}


// 读取
BOOL CWorldVillageRegion::Load()
{
	CWorldRegion::Load();

	return TRUE;
}

void CWorldVillageRegion::SetEnterPosXY(CPlayer *pPlayer)
{
}
