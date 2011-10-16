#include "StdAfx.h"
#include ".\playerranks.h"
#include "..\nets\networld\message.h"

#include "..\AppWorld\Organizingsystem\OrganizingCtrl.h"
#include "..\AppWorld\Organizingsystem\Faction.h"
#include "..\AppWorld\Organizingsystem\OrganizingParam.h"

#include "Game.h"
//#include "..\..\..\dbaccess\worlddb\rsplayer.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPlayerRanks* CPlayerRanks::instance = NULL;											//该类唯一实例句柄


CPlayerRanks::CPlayerRanks(void)
{
	m_bStat = false;
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
	/*tagTime TaxTime(GetOrganizingParam()->GetStatPlayerRanksTime());
	m_nMaxNum = GetOrganizingParam()->GetPlayerRanksNum();

	m_StatTime = TaxTime;

	tagTime CurTime;
	TaxTime.tmTime.tm_year	= CurTime.tmTime.tm_year;
	TaxTime.tmTime.tm_mon	= CurTime.tmTime.tm_mon;
	TaxTime.tmTime.tm_wday	= CurTime.tmTime.tm_wday;
	if(TaxTime < CurTime)
		TaxTime.AddDay(1);

	m_bStat = false;
	m_lStatRanksEventID = GetTimer()->SetTimeEvent(TaxTime,(CallBackFun)CPlayerRanks::OnStatRanks,0);
	*/return true;
}

void   CPlayerRanks::Release()
{
	GetTimer()->KillTimeEvent(m_lStatRanksEventID);

	if(instance != NULL)
	{
		SAFE_DELETE(instance);
	}
}


// 添加到CByteArray
bool CPlayerRanks::AddToByteArray(vector<BYTE>& ByteArray)
{
	_AddToByteArray(&ByteArray, (long)m_PlayerRanks.size());
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
		_GetBufferFromByteArray(pByte, lPos, Rank.PlayerID);
		_GetStringFromByteArray(pByte, lPos, str);
		Rank.strName = str;
		Rank.wOccupation = _GetWordFromByteArray(pByte, lPos);
		Rank.wLevel = _GetWordFromByteArray(pByte, lPos);
		_GetStringFromByteArray(pByte, lPos, str);
		Rank.strFacName = str;
	}
	return true;
}

void CPlayerRanks::StatPlayerRanks()
{
	//Clear();
	//AddLogText("统计玩家排行榜开始...");
	//DWORD dwStatTime= timeGetTime();
	////	GetGame()->GetRsPlayer()->StatRanks( GetPlayerRanks() );
	//DWORD dwEndTime= timeGetTime();
	//char str[100];
	//sprintf(str,"统计玩家排行榜结束。(耗时:%d毫秒)",dwEndTime-dwStatTime);
	//AddLogText(str);
}

void CPlayerRanks::AddRank(const CGUID& PlayerID, string& strName,WORD wOccu,WORD wLevl)
{
	string strFacName("");
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerID);
	if( NULL == pPlayer) return;
	const CGUID& FactionID = pPlayer->GetFactionID();
	CFaction* pFaction = GetOrganiCtrl()->GetFactionOrganizing(FactionID);
	if(pFaction != NULL)
	{
		strFacName = pFaction->GetName();
	}
	tagRank Rank = { PlayerID,strName,wOccu,wLevl,strFacName };
	m_PlayerRanks.push_back(Rank);
}

void CPlayerRanks::UpdateRanksToGameServer()
{
	CMessage msg(MSG_W2S_SERVER_SENDSETUP);
	msg.Add((long)SI_PLAYERRANKS);
	vector<BYTE> ByteData;
	AddToByteArray(ByteData);
	if(ByteData.size() > 0)
		msg.AddEx(&ByteData[0],ByteData.size());
	msg.SendAll();
}

//统计排行
void CALLBACK CPlayerRanks::OnStatRanks(LPARAM lparam)
{
	//触发统计排行榜
	if(GetPlayerRanks() == NULL)	 return;

	GetPlayerRanks()->StatPlayerRanks();
	GetPlayerRanks()->UpdateRanksToGameServer();

	tagTime TaxTime(GetPlayerRanks()->GetStatTime());
	tagTime CurTime;
	TaxTime.tmTime.tm_year	= CurTime.tmTime.tm_year;
	TaxTime.tmTime.tm_mon	= CurTime.tmTime.tm_mon;
	TaxTime.tmTime.tm_wday	= CurTime.tmTime.tm_wday;
	TaxTime.AddDay(1);

	ULONG lStatRanksEventID = GetTimer()->SetTimeEvent(TaxTime,(CallBackFun)CPlayerRanks::OnStatRanks,0);
	GetPlayerRanks()->SetStatRanksEventID(lStatRanksEventID);

	GetPlayerRanks()->SetIsStat(true);
}

CPlayerRanks* GetPlayerRanks()
{
	return CPlayerRanks::getInstance();
}
