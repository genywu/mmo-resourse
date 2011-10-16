#include "stdafx.h"
#include "..\nets\netlogin\message.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2L_SERVER_SENDWORLDINFO:
		{
			char str[256];
			long lWorldNumber = pMsg->GetLong();
			pMsg->GetStr(str,256);						
			//给WorldClient映射ID
			CMessage::MapWorldIDSocket(lWorldNumber,pMsg->GetSocketID());
			long lPos = GetGame()->AddWorld(lWorldNumber,str);			
			AddLogText("WorldServer [%s] connected...ok!",str);

			BOOL bIsReady = pMsg->GetLong();

			if( lWorldNumber )
			{
				long lCdkeyNum = pMsg->GetLong();
				GetGame()->DelWsCdKey(lWorldNumber);
				char str[256];
				for (int i=0; i<lCdkeyNum; i++)
				{
					pMsg->GetStr(str,256);
					GetGame()->AddCdkey(str, lWorldNumber);
					GetGame()->GetCSdoaFcm().UserOnline(str, 0);
				}
				GetGame()->SetWsReadyState(lWorldNumber, bIsReady);
			}

			if(bIsReady)
			{
				GetGame()->SendOnlineInfoToAs();
			}
		}
		break;

	case MSG_W2L_SERVER_CLEARCDKEYS:
		{
			long lNum = pMsg->GetLong();
			char szCdkey[256];
			if(lNum > 0)
			{
				vector<string> vCdkey;
				for(int i = 0;i < lNum; ++i)
				{
					pMsg->GetStr(szCdkey,256);
					GetGame()->ClearCDKey(szCdkey);

					if(0 == CMessage::GetSocketByCdkey(szCdkey))
					{
						GetGame()->GetLoginManager().DelFcmCdkey(szCdkey);
						vCdkey.push_back(string(szCdkey));
					}
				}

				if(GetGame()->UseAsFcm())
				{
					CMessage msg(MSG_O2A_ACCOUNT_ListLogout);
					msg.Add((LONG)vCdkey.size());
					for (size_t i = 0; i < vCdkey.size(); ++i)
					{
						msg.Add(vCdkey[i].c_str());
					}
					msg.SendToAS();
				}
				else
				{
					for (size_t i = 0; i < vCdkey.size(); ++i)
						GetGame()->GetCSdoaFcm().UserOffline(vCdkey[i].c_str());
				}
			}
		}
		break;

	case MSG_W2L_SERVER_ANSWER_PING:
		{
			char strIP[256];
			CGame::tagPingWorldServerInfo stPingWorld;
			long flag = pMsg->GetLong();
			DWORD dwIP = pMsg->GetIP();
			sprintf(strIP, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));
			stPingWorld.strIP = strIP;

			LONG lWorldID = pMsg->GetWorldID();
			LPCSTR pName = GetGame()->GetWorldNameByID(lWorldID);
			if(NULL != pName)
			{
				stPingWorld.strName = pName;
				stPingWorld.dwPlayerNum = GetGame()->GetLoginWorldPlayerNumByWorldID(lWorldID);
			}
			else
			{
				stPingWorld.dwPlayerNum = 0;
			}
			GetGame()->AddPingWorldServerInfo(stPingWorld);
		}
		break;
	case MSG_W2L_SERVER_ANSWER_GsPlayerNum:
		{
			GetGame()->GetOnCmdConnect().ClearGsInfo();

			LONG lGsNum = pMsg->GetLong();
			if(100 > lGsNum)
			{
				for (LONG i = 0; i < lGsNum; ++i)
				{
					tagGsInfo GsInfo;
					GsInfo.bGsWorking = (1 == pMsg->GetByte());
					GsInfo.lLSID = GetGame()->GetSetup()->dwAccountClientNo;
					GsInfo.lWSID = pMsg->GetWorldID();
					GsInfo.lGSID = pMsg->GetLong();
					GsInfo.lGSPlayerNum = pMsg->GetLong();
					GetGame()->GetOnCmdConnect().AddGsInfo(GsInfo);
				}
			}
		}
		break;
	case MSG_W2L_SERVER_QUEST_RECONNECTION:

		break;
	}
}
