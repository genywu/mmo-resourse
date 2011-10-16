#include "stdafx.h"
#include "..\AppServer\SMClient.h"
#include "..\public\InfoType.h"
void OnSMMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_SM2GAPP_SHUTDOWN:
		{
			g_bGameThreadExit = true;
		}
		break;
	case MSG_SM2GAPP_GET_MAINAITICK:
		{
			CSMClient::GetSM()->SendAITick();
		}
		break;
	case MSG_SM2GAPP_ACTION:
		{
			long SMID=pMsg->GetLong();
			long lGappID = pMsg->GetLong();
			switch(pMsg->GetLong())
			{
				//ÇëÇóÊý¾Ý 
			case GS_PALYER:
				{
					CMessage msg(MSG_GAPP2SM_ACTION);
					msg.Add(SMID);
					msg.Add(lGappID);
					msg.Add((long)GS_PALYER);
					msg.Add(GetGame()->GetPlayerCount());
					msg.SendToSM();
				}
				break;
			default:
				break;
			}
		}
	default:
		break;
	}
}