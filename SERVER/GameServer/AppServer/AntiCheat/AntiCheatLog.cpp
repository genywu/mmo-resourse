#include "stdafx.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../../LogClient/gs/LogicLogInterface.h"
#include "AntiCheatCommon.h"

namespace AntiCheat
{
	static LogInfo s_LogInfo;

	const LogInfo *GetLogInfo()
	{
		return &s_LogInfo;
	}

	unsigned long AddAccWrong()
	{
		return ++s_LogInfo.AccWrong;
	}

	unsigned long AddAccRight()
	{
		return ++s_LogInfo.AccRight;
	}

	unsigned long AddAccQues()
	{
		return ++s_LogInfo.AccQues;
	}

	unsigned long AddAccTimeOut()
	{
		return ++s_LogInfo.AccTimeOut; 
	}

	unsigned long AddAccSysError()
	{
		return ++s_LogInfo.AccSysErr;
	}

	void ResetLogInfo()
	{
		memset( &s_LogInfo, 0, sizeof( s_LogInfo ) );
	}
}

bool LogicLogInterface::logT900_anticheat_player_log(CPlayer * player, const char *log_type, 
													 int acc_wrong, int cur_wrong, int acc_bonus)
{
	if ( 0 == GetGame()->GetSetup()->lUseLogServer )
	{
		return true;
	}
	if ( NULL == m_pLogClient )
	{
		return false;
	}
	if ( NULL == player )
	{
		return false;
	}
	CRegion *pRegion = static_cast<CRegion*>( GetGame()->FindRegion(player->GetRegionGUID()) );
	if (pRegion == NULL)
	{
		return false;
	}

	string strTime;
	GetCurrTimeString(strTime);
	
	float fPosX = player->GetPosX();
	float fPosY = player->GetPosY();
	return m_pLogClient->SendLogToServer(
		900,strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&fPosX,&fPosY,
		&player->GetExID(),player->GetName(),
		log_type,
		acc_wrong, cur_wrong, acc_bonus );
}

bool LogicLogInterface::logT901_anticheat_answer_log( CPlayer *player, const char *log_type, 
		int acc_wrong, int cur_wrong, int cur_right, int acc_bonus, int pk, bool RHAnswered )
{
	if ( 0 == GetGame()->GetSetup()->lUseLogServer )
	{
		return true;
	}
	if ( NULL == m_pLogClient )
	{
		return false;
	}
	if ( NULL == player )
	{
		return false;
	}
	CRegion *pRegion = static_cast<CRegion*>( GetGame()->FindRegion(player->GetRegionGUID()) );
	if (pRegion == NULL)
	{
		return false;
	}

	string strTime;
	GetCurrTimeString(strTime);
	
	float fPosX = player->GetPosX();
	float fPosY = player->GetPosY();

	return m_pLogClient->SendLogToServer(
		901, strTime.c_str(),
		pRegion->GetID(),pRegion->GetName(),&fPosX,&fPosY,
		&player->GetExID(),player->GetName(),
		log_type,
		acc_wrong, cur_wrong, cur_right, acc_bonus, pk, ( RHAnswered ? 1 : 0 ) );
}
