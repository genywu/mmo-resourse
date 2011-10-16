


#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\organizingsystem\SentaiSystem.h"



void OnSentaiMessage(CMessage * pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_SENTAI_REQUEST_QUEUE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			bool IsJoin = (0 != pMsg->GetLong());
			GetInst(CSentaiSystem).OnPlayerQueue(PlayerGuid, IsJoin);
		}
		break;
	case MSG_S2W_SENTAI_REQUEST_CHANGE_POINT:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			long ChangePoints = pMsg->GetLong();

			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				pPlayer->SetSentaiPoints(pPlayer->GetSentaiPoints() + ChangePoints, true);
			}
		}
		break;
	case MSG_S2W_SENTAI_END_MATCH:
		{
#ifdef _DEBUG_SENTAI_
			char DebugInfo[128] = {0};
			_snprintf(DebugInfo, 128, "MSG_S2W_SENTAI_END_MATCH gsid:%d flag:%d", pMsg->GetGSID(), pMsg->GetLong());
			PutStringToFile("SentaiS2W", DebugInfo);
#endif
			GetInst(CSentaiSystem).OnWarFieldFree(pMsg->GetLong());
		}
		break;
	case MSG_S2W_SENTAI_END_TEAM_MATCH:
		{
#ifdef _DEBUG_SENTAI_
			char DebugInfo[128] = {0};
			_snprintf(DebugInfo, 128, "MSG_S2W_SENTAI_END_TEAM_MATCH gsid:%d flag:%d", pMsg->GetGSID(), pMsg->GetLong());
			PutStringToFile("SentaiS2W", DebugInfo);
#endif
			long RegionID = pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				GetInst(CSentaiSystem).EndTeamWar(pPlayer->GetTeamID(), RegionID);				
			}
		}
		break;
	case MSG_S2W_SENTAI_REQUEST_PLAYER_STATE:
		{
			CGUID PlayerGuid, ScriptGuid;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(ScriptGuid);
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			long ReValue = 0;
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				if(GetInst(CSentaiSystem).IsQueuing(pPlayer->GetTeamID()) || GetInst(CSentaiSystem).IsBattle(pPlayer->GetTeamID()))
				{
					ReValue = 1;
				}
			}

			CMessage msg(MSG_W2S_SENTAI_RE_PLAYER_STATE);
			msg.Add(PlayerGuid);
			msg.Add(ScriptGuid);
			msg.Add(ReValue);
			msg.Add(szRet);
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
	}
}