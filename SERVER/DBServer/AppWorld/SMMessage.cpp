#include "stdafx.h"
#include "..\AppWorld\SMClient.h"
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
			//请求数据 
			case DBS_SAVEENTITY:
				{
					CMessage msg(MSG_GAPP2SM_ACTION);
					msg.Add(SMID);
					msg.Add(lGappID);
					msg.Add((long)DBS_SAVEENTITY);
					msg.Add((long)lOperingSaveEntitys);
					msg.SendToSM();
				}
				break;
				//保存所有数据
			case DBS_SAVE_ALL:
				{
					extern bool g_bSaveNowData;
					g_bSaveNowData = true;
				}
			}
		}
		break;
	default:
		break;
	}
}