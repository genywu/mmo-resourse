

#include "stdafx.h"
#include "..\player.h"
#include "..\OrganizingSystem\GameOrganizingCtrl.h"
#include "..\OrganizingSystem\GSTeam.h"
#include "..\script\VariableList.h"



void OnWsSentaiMessage(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{

	case MSG_W2S_SENTAI_RE_QUEUE:
		{
			BYTE IsTeamAim = pMsg->GetByte();
			CGUID AimGuid;
			pMsg->GetGUID(AimGuid);
			DWORD Succeed = pMsg->GetDWord();
			DWORD Value2 = pMsg->GetDWord();
			
			CMessage msg(MSG_S2C_SENTAI_RE_QUEUE);
			msg.Add(Succeed);
			msg.Add(Value2);

			if (0 == Succeed)
			{
				msg.SendToPlayer(AimGuid);
			}
			else
			{
				msg.Add(pMsg->GetDWord());

				if (IsTeamAim)
				{
					GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(AimGuid)));
					if(NULL != pTeam)
					{
						pTeam->SendToAllMember(&msg);
					}
				}
				else
				{
					msg.SendToPlayer(AimGuid);
				}
				
			}
		}
		break;
	case MSG_W2S_SENTAI_UPDATE_POINT:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			long NewPoints = pMsg->GetLong();

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				pPlayer->SetSentaiPoints(NewPoints);
			}
		}
		break;
	case MSG_W2S_SENTAI_INTO_READY:
		{
			DWORD WaitSec = pMsg->GetDWord();
			long TeamNum = pMsg->GetLong();

			CMessage msg(MSG_S2C_SENTAI_NOTIFY_READY);
			msg.Add(WaitSec);

			for (long i = 0; i < TeamNum; ++i)
			{
				CGUID TeamGuid;
				pMsg->GetGUID(TeamGuid);
				GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamGuid)));
				if(NULL != pTeam)
				{
					pTeam->SendToAllMember(&msg);
				}
			}
		}
		break;
	case MSG_W2S_SENTAI_INTO_FIELD:
		{
			long RegionID = pMsg->GetLong();
			long TeamNum = pMsg->GetLong();

			for (long i = 0; i < TeamNum; ++i)
			{
				CGUID TeamGuid;
				pMsg->GetGUID(TeamGuid);
				long x = pMsg->GetLong();
				long y = pMsg->GetLong();
				GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamGuid)));
				if(NULL != pTeam)
					pTeam->ChangeRegion(RegionID, x, y);
			}
		}
		break;
	case MSG_W2S_SENTAI_END_MACTH:
		{
#ifdef _DEBUG_SENTAI_
			char DebugInfo[128] = {0};
			_snprintf(DebugInfo, 128, "MSG_W2S_SENTAI_END_MACTH flag:%d", pMsg->GetLong());
			PutStringToFile("SentaiW2S", DebugInfo);
#endif
			CGUID TeamGuid;
			pMsg->GetGUID(TeamGuid);
			GSTeam* pTeam = ((GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(TeamGuid)));
			if(NULL != pTeam)
			{
				CMessage msg(MSG_S2C_SENTAI_RE_QUEUE);
				msg.Add((DWORD)1);//! 成功
				msg.Add((DWORD)0);//! 退出
				msg.Add((DWORD)1);//! 显示对话框
				pTeam->SendToAllMember(&msg);
			}
		}
		break;
	case MSG_W2S_SENTAI_RE_PLAYER_STATE:
		{
			CGUID PlayerGuid, ScriptGuid;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(ScriptGuid);
			long ReValue = pMsg->GetLong();
			char szRet[128] = {0};
			pMsg->GetStr(szRet, 128);

			//取消挂起的脚本
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddVar(szRet, ReValue);
			}
			ContinueRunScript(ScriptGuid, stackVarList);
		}
		break;
	}
}