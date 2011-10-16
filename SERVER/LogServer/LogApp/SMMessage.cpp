#include "stdafx.h"
#include "..\LogApp\SMClient.h"
#include "..\nets\netlog\Message.h"
#include "..\public\ManageToolMsgType.h"
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
				//ÇëÇóÊı¾İ 
			case LOGS_CREATETABLE:
				GetGame()->CreateTableInDB();
			default:
				break;
			}
		}
	default:
		break;
	}
}