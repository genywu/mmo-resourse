#include "StdAfx.h"
#include ".\playerranks.h"
#include "Game.h"
#include "../public/public.h"
#include "..\nets\netserver\message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPlayerRanks* CPlayerRanks::instance = NULL;											//该类唯一实例句柄


CPlayerRanks::CPlayerRanks(void)
{
	m_PlayerGetTime.clear();
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


// 添加到CByteArray
bool CPlayerRanks::AddToByteArray(vector<BYTE>& ByteArray,int nAddNum)
{
	long lSize = min(nAddNum,m_PlayerRanks.size());
	_AddToByteArray(&ByteArray, lSize);
	list<tagRank>::iterator it = m_PlayerRanks.begin();
	for(;it!=m_PlayerRanks.end();it++)
	{		
		_AddToByteArray(&ByteArray, (*it).PlayerID);
		_AddToByteArray(&ByteArray, (char*)(*it).strName.c_str());
		_AddToByteArray(&ByteArray, (*it).wOccupation);
		_AddToByteArray(&ByteArray, (*it).wLevel);
		_AddToByteArray(&ByteArray, (char*)(*it).strFacName.c_str() );
	}
	return true;
}

bool CPlayerRanks::DecordFromByteArray(BYTE* pByte, long& lPos)
{
	char str[20]="";
	m_PlayerRanks.clear();
	long lSize = _GetLongFromByteArray( pByte, lPos );
	for(int i=0;i<lSize;i++)
	{
		tagRank Rank;
		_GetBufferFromByteArray(pByte, lPos,Rank.PlayerID);
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

void CPlayerRanks::OnPlayerGetRanks(const CGUID& PlayerID,long lMaxNum)
{
	bool bFlag = true;

	DWORD dwCurTime = timeGetTime();
	map<CGUID,DWORD>::iterator it = m_PlayerGetTime.begin();
	for(;it != m_PlayerGetTime.end();)
	{
		if( (*it).second > dwCurTime )
		{
			if( (*it).first == PlayerID )
			{
				bFlag = false;
			}
			it++;
		}
		else
		{
			it = m_PlayerGetTime.erase(it++);
		}
	}

	if(bFlag)
	{
		CMessage msg(MSG_S2C_ANSWER_PlayerRanks);
		msg.Add(lMaxNum);
		msg.SendToPlayer(PlayerID);

		m_PlayerGetTime[PlayerID] =  dwCurTime+2000;
	}
}


CPlayerRanks* GetPlayerRanks()
{
	return CPlayerRanks::getInstance();
}
