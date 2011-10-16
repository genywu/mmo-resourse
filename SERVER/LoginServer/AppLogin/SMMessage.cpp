#include "stdafx.h"
#include "..\AppLogin\SMClient.h"
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
	default:
		break;
	}
}