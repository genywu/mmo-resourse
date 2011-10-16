#include "stdafx.h"
#include "..\BillingServer\Game.h"
#include "..\Nets\NetBilling\ServerForGS.h"
#include "..\Nets\NetBilling\ClientForGS.h"
#include "..\Nets\NetBilling\Message.h"

// 响应Server消息
void OnServerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
		// Receive GS login info
	case CMessage::MSG_S2B_SERVER_SENDSERVERINFO:
		{
			long lID = pMsg->GetClientSocketID();
			AllowedClient *pClient = NULL;
			if( GetGame()->GetServerForGS()->GetClientCheck() && !(pClient = GetGame()->GetServerForGS()->GetClientByID(lID)) )
				return;

			GetGame()->GetServerForGS()->SetClientMapID(lID);

			// 可以添加区服信息
			char str[256];
			if( pClient )
			{
				sprintf( str, "%s:%lu", pClient->_ip, pClient->_port);
			}
			else
			{
				DWORD dwIP = pMsg->GetIP();
				sprintf( str, "%u.%u.%u.%u:0", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)) );
			}
			AddLogText("GameServer %s Connected OK", str);
			SendMessage(g_hWndGSList, LB_INSERTSTRING, -1,(LPARAM)str);
		}
		break;

		// GS lost
	case CMessage::MSG_B2B_SERVER_GAMESERVERLOST:
		{
			long lID = pMsg->GetLong();
			AllowedClient *pClient = GetGame()->GetServerForGS()->GetClientByID(lID);
			if( pClient )
			{
				char str[256] = "";
				if( GetGame()->GetServerForGS()->GetClientCheck() )
					sprintf( str, "%s:%lu", pClient->_ip, pClient->_port);
				else
					sprintf( str, "%s:0", pClient->_ip, 0);
				long lPos = (long)SendMessage(g_hWndGSList, LB_FINDSTRING, 0, (LPARAM)str);
				if( lPos != LB_ERR )
				{
					SendMessage( g_hWndGSList, LB_DELETESTRING, lPos, 0 );
				}

				AddLogText("GameServer %s Lost!", str);
			}
			else
			{
				AddLogText("!!!!!Unknown GameServer Lost[%d]!!!!!!!!!", lID);
			}
		}
		break;
	}
}
