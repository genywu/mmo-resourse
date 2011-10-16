#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ÏìÓ¦LOGÏûÏ¢
void OnLogMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{

	case MSG_O2A_LOGIN_SendOnlineInfo:
		{
			DBReadSet dbReadSet;
			pMsg->GetDBReadSet(dbReadSet);

			GetGame()->GetClientManager().OnlineInfoRevise(dbReadSet, pMsg->GetSocketID());
		}
		break;
	case MSG_O2A_LOGIN_Affirm_Request:
		{
			DWORD dwClientNo = pMsg->GetDWord();
			DWORD dwLoginRe = 0;
	
			if(GetGame()->GetClientManager().OnClientLogin(pMsg->GetIP(), dwClientNo, pMsg->GetSocketID(), dwLoginRe))
			{
				CMessage reMsg(MSG_A2O_LOGIN_Affirm_Re);
				reMsg.Add(dwLoginRe);
				reMsg.SendToClient(pMsg->GetSocketID());
			}
			else
			{
				GetGame()->DisConnClient(pMsg->GetSocketID());
			}
			
		}
		break;
	default:
		break;
	}
}

void OnSocketLost(LONG lSockte)
{
	GetGame()->GetClientManager().OnClientLogout(lSockte);
}