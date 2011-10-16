


#include "stdafx.h"
#include "TeamMgr.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTeamMgr::CTeamMgr()
{

}


CTeamMgr::~CTeamMgr()
{
	Release();
}

void CTeamMgr::Release()
{

}

void CTeamMgr::DecodeFromDataBlock(DBReadSet& dbRead)
{
	long lFlag = dbRead.GetByteFromByteArray();
	if( lFlag == 0 )
		SetRecruitingTeam(false);
	else
	{
		long lMemNum = dbRead.GetByteFromByteArray();
		SetTeammateAmount(lMemNum);

		char strTeamName[20] = {};
//		memset(strTeamName,0,sizeof(strTeamName));
		dbRead.GetStringFromByteArray(strTeamName,20);

		SetRecruitingTeam(true);
		SetTeamname(strTeamName);

		long lHasPass = dbRead.GetByteFromByteArray();
		SetTeamHasPassword((lHasPass)==0?false:true);
	}
}