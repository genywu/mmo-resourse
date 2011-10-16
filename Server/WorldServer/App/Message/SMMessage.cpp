#include "StdAfx.h"
#include "../SMClient.h"
#include "../../../../Public/Common/ManageToolMsgType.h"
#include "../../../../Public/ServerPublic/Server/BaseDef/InfoType.h"

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
	/*case MSG_SM2GAPP_WS_SENDTOPNEWS:
		{
			CMessage msg(MSG_W2S_OTHER_NEWS_TOP_WORLD);
			pMsg->SetType(MSG_W2S_OTHER_NEWS_TOP_WORLD);
			pMsg->SendToAllGS();
		}
		break;*/
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
					msg.Add((long)GetGame().GetMapPlayerCount());
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
					msg.SendToAllGS();
				}
				break;
			case WS_RELOADFILE:
				{
					CMessage msg(MSG_GAPP2SM_ACTION);
					msg.Add(SMID);
					msg.Add(lGappID);
					msg.Add((long)WS_RELOADFILE);
				//	char szFile[MAX_PATH]={0};
				//	pMsg->GetStr(szFile,MAX_PATH);
					long lFlag = pMsg->GetLong();
					GetGame().LoadServerResource();
					GetGame().LoadEx(lFlag);
					BOOL ret= false; 
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
					GetGame().KickAllPlayer();
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
	default:
		break;
	}
}