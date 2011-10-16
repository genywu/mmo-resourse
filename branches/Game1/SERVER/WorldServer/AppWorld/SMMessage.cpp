#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\AppWorld\SMClient.h"
#include "..\..\public\InfoType.h"
void OnSMMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_SM2GAPP_GET_MAINAITICK:
		{
			
			CSMClient::GetSM()->SendAITick();
		}
		break;
	case MSG_SM2GAPP_SHUTDOWN:
		{
			g_bGameThreadExit = true; 
		}
		break;
	case MSG_SM2GAPP_WS_SENDTOPNEWS:
		{
			DWORD gappID=pMsg->GetDWord();
			char text[1024]={};
			pMsg->GetStr(text,1024);
			CMessage msg(MSG_W2S_OTHER_NEWS_TOP_WORLD);
			msg.Add((long)1);
			msg.Add(text);
			msg.Add((long)1);
			msg.SendAll();
		}
		break;
	case MSG_SM2GAPP_ACTION:
		{
			long SMID=pMsg->GetLong();
			long lGappID = pMsg->GetLong();
			switch(pMsg->GetLong())
			{
			case WS_PLAYER:
				{
					CMessage msg(MSG_GAPP2SM_ACTION);
					msg.Add(SMID);
					msg.Add(lGappID);
					msg.Add((long)WS_PLAYER);
					msg.Add((long)GetGame()->GetMapPlayerCount());
					msg.SendToSM();
				}
				break;
			case WS_TOPNEWS:
				{
					char szNews[1024]={0};
					CMessage msg(MSG_S2W_OTHER_NEWS_TOP_WORLD);
					msg.Add(pMsg->GetLong());
					msg.Add(pMsg->GetStr(szNews,1024));
					msg.Add(pMsg->GetLong());
					msg.SendAll();
				}
				break;
			case WS_RELOADFILE:
				{
					CMessage msg(MSG_GAPP2SM_ACTION);
					msg.Add(SMID);
					msg.Add(lGappID);
					msg.Add((long)WS_RELOADFILE);
					char szFile[MAX_PATH]={0};
					pMsg->GetStr(szFile,MAX_PATH);
					GetGame()->LoadServerResource();
					msg.AddEx(szFile,MAX_PATH);
					BOOL ret=GetGame()->ReLoad(szFile,true,true);
					if (ret)
					{
						msg.Add((BYTE)1);
					}
					else
						msg.Add((BYTE)0);
					msg.SendToSM();
				}
				break;
				//踢所有人
			case WS_KICK_ALL:
				{
					extern bool g_bKickAllPlayer;
					g_bKickAllPlayer = true;
				}
				break;
				//立即保存当前数据
			case WS_SAVE_ALL:
				{
					extern bool g_bSaveNowData;
					extern bool g_bSendSaveMsgNow;
					g_bSendSaveMsgNow = true;
					g_bSaveNowData = true;
				}
				break;
				//立刻存盘
			case WS_SAVE_ALL_NOW:
				{
					extern bool g_bSendSaveMsgNow;
					g_bSendSaveMsgNow = true;
				}
				break;
			}
		}
	case MSG_SM2GAPP_WATCH:
		{
			DWORD bDel=pMsg->GetDWord();
			char szName[32]={0};
			pMsg->GetStr(szName,32);
			CPlayer* pPlayer = GetGame()->GetMapPlayer(szName);
			
			CMessage Replymsg(MSG_GAPP2SM_WATCH_INFO_Reply);
			Replymsg.Add(GetGame()->GetSetup()->dwGappID);
			Replymsg.Add((BYTE)((0 == bDel || NULL == pPlayer) ? 0 : 1));
			Replymsg.SendToSM();

			CMessage msg(MSG_W2S_OTHER_WATCH);
			msg.Add(bDel);

			//发送到该玩家所在的GS
			if (0 == bDel)
			{
				const char *pPlayerName = CSMClient::GetSM()->GetWatchedName();
				if (NULL == pPlayerName)
					break;
				msg.Add(pPlayerName);
				msg.SendToSocket(CSMClient::GetSM()->GetWatchedGsSocket());

				CSMClient::GetSM()->ClearWatch();
			}
			else if(NULL != pPlayer)
			{
				msg.Add(szName);
				msg.SendToSocket(pPlayer->GetGsSocket());
				long GsSocket = pPlayer->GetGsSocket();
				CSMClient::GetSM()->SetWatchedPlayer(pPlayer->GetExID(), szName, GsSocket);
			}
			else
				CSMClient::GetSM()->ClearWatch();
		}
		break;
		//封号,解封等,玩家不在线也可操作
	case MSG_SM2GAPP_REPORT_Result:
		{
			DWORD GappID=pMsg->GetDWord();
			tagBaseResultInfo tgInfo;
			pMsg->GetEx(&tgInfo,sizeof(tgInfo));
			//操作GUID
			CGUID guid;
			pMsg->GetGUID(guid);
			switch(tgInfo._OptResult)
			{
			case eGOT_Ban:
				//封停帐号
				CSMClient::GetSM()->BanPlayer(tgInfo,guid);
				break;
			case eGOT_Unban:
				//封停帐号
				tgInfo._lBanTime=-1;
				CSMClient::GetSM()->BanPlayer(tgInfo,guid);
				break;
			case eGOT_ShutUp:
				break;
			case eGOT_OpenUp:
				break;
			}
		}
		break;
		//GM对在线玩家的操作
	case MSG_SM2GAPP_MANAGE_Request_Gm_Command:
		{
			const char* name=CSMClient::GetSM()->GetWatchedName();
			CPlayer* pPlayer=GetGame()->GetMapPlayer(name);
			//玩家不在线
			if (pPlayer==NULL)
			{
				CMessage msg(MSG_GAPP2SM_MANAGE_Request_Gm_Command);
				msg.Add(GetGame()->GetSetup()->dwGappID);
				msg.Add((DWORD)eGGC_GmChat);
				char szInfo[256]={};
				sprintf(szInfo,CStringReading::LoadString(IDS_WS_REPORT_INFO,eTipOther),
					name);
				msg.Add(szInfo);
				msg.SendToSM();
				break;
			}
			DWORD dwGappID=pMsg->GetDWord();
			DWORD cmdType=pMsg->GetDWord();
			CGUID guid=CSMClient::GetSM()->GetWatchedGuid();
			char szText[1024]={};
			pMsg->GetStr(szText,1024);
			switch(cmdType)
			{
				//GM密聊
			case eGGC_GmChat:
				{
					CMessage msg(MSG_W2S_OTHER_GM_CHAT);
					msg.Add(guid);
					msg.Add(szText);

					msg.SendToSocket(CSMClient::GetSM()->GetWatchedGsSocket());
				}
				break;
			}
		}
	default:
		break;
	}
}