#include "StdAfx.h"
#include ".\playerranks.h"

#include "Game.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPlayerRanks* CPlayerRanks::instance = NULL;											//该类唯一实例句柄


CPlayerRanks::CPlayerRanks(void)
{
}

CPlayerRanks::~CPlayerRanks(void)
{
}

//得到唯一实例句柄
CPlayerRanks* CPlayerRanks::getInstance()
{
	if(instance == NULL)
	{
		instance = new CPlayerRanks();
	}
	return instance;
}

//初始化
bool  CPlayerRanks::Initialize()
{
	return true;
}

void   CPlayerRanks::Release()
{
	if(instance != NULL)
	{
		SAFE_DELETE(instance);
	}
}


bool CPlayerRanks::DecordFromByteArray(BYTE* pByte, long& lPos)
{
	char str[20]="";
	m_PlayerRanks.clear();
	long lSize = _GetLongFromByteArray( pByte, lPos );
	for(int i=0;i<lSize;i++)
	{
		tagRank Rank;
		Rank.nPlayerID = _GetLongFromByteArray(pByte, lPos);
		_GetStringFromByteArray(pByte, lPos, str);
		Rank.strName = str;
		Rank.wOccupation = _GetWordFromByteArray(pByte, lPos);
		Rank.wLevel = _GetWordFromByteArray(pByte, lPos);
		_GetStringFromByteArray(pByte, lPos, str);
		Rank.strFacName = str;
		m_PlayerRanks.push_back(Rank);
	}
	return true;
}

CPlayerRanks* GetPlayerRanks()
{
	return CPlayerRanks::getInstance();
}
