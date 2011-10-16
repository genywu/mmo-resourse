#include "stdafx.h"
#include "..\nets\Servers.h"
#include "..\entity\Account.h"
#include "..\Entity\EntityGroup.h"
#include "..\Entity\EntityManager.h"
#include "../../../../dbaccess/WorldDB/RsEntity.h"
#include "../Thread/DBAccessThread.h"
#include "../Thread/DBThread.h"
#include "../Thread/ThreadDataDef.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"

#include <mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern unsigned __stdcall SaveThreadFunc(void* pArguments);
extern HANDLE g_hSavingThread;

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2DB_SERVER_UNREGENTITY:
		{
			char szCdkey[128];
			pMsg->GetStr(szCdkey, sizeof(szCdkey));
			if(szCdkey)
			{
				Account* pAcc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
				if(pAcc)
				{
					long lCurTime = timeGetTime();
					pAcc->SetLastLoginTime(lCurTime);
				}
			}
		}
		break;
	case MSG_W2DB_SERVER_QUEST_RECONNECTION:
		{
			// 定时刷新数据
			lConnects++;

			long msgid = pMsg->GetLong(); // MSGID
			CMessage msg(MSG_DB2W_SERVER_QUEST_RECONNECTION);
			msg.Add(msgid);
			// 保存的factions和unions数据由各自的消息处理
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
		// 发送server信息给dbserver
	case MSG_W2DB_SERVER_SENDSERVERINFO:
		{
			// 定时刷新数据
			lConnects++;

			// 设置GameServer标识
			bool bReconnect = pMsg->GetChar()?true:false;
			DWORD dwPort = pMsg->GetLong();
			char strGameServerIP[256];
			pMsg->GetStr(strGameServerIP,256);

			CGame::tagWorldServer* pGameServer = GetGame()->GetWorldServer(strGameServerIP, dwPort);
			if (pGameServer)
			{
				pGameServer->bConnected = true;
				CMessage::MapWSIDSocket(pGameServer->dwIndex, pMsg->GetSocketID());

				GetGame()->SetWSSocket(pMsg->GetSocketID());
			}
			else
			{
				AddLogText(CStringReading::LoadString(IDS_DBS_SERVERMESSAGE, STR_DBS_SERVERMESSAGE_WSCONNECTFAIL), strGameServerIP, dwPort);
				break;
			}
			char str[256];
			sprintf(str, "%s [%d]", strGameServerIP, dwPort);
			SendMessage(g_hGameServerListBox, LB_INSERTSTRING,-1, (LPARAM)str);
			AddLogText(CStringReading::LoadString(IDS_DBS_SERVERMESSAGE, STR_DBS_SERVERMESSAGE_WSCONNECTOK), strGameServerIP, dwPort);

			// 发送数据库配置信息
			GetGame()->SendDBTableInfo(pMsg->GetSocketID());
		}
		break;
		// 收到WorldServer的返回
	case MSG_W2DB_SERVER_ANSWER_PING:
		
		break;
		// WS的脚本发送的消息,需要回应处理
	case MSG_W2DB_SERVER_PLAYER_PERSONALHOUSEGUID:
		{
			long scriptID = pMsg->GetLong();// Script ID

			// player name
			char playerName[128];
			pMsg->GetStr(playerName, 128);

			// PH GUID
			CGUID guid;

			char szPHGuid[128]; // 返回的PH GUID 变量名
			pMsg->GetStr(szPHGuid, 128);

			
			CMessage msg(MSG_W2DB_SERVER_PLAYER_PERSONALHOUSEGUID);
			msg.Add(scriptID);// Script ID
			msg.Add(playerName); // player name
			msg.Add(szPHGuid);// 返回的PH GUID 变量名
			msg.Add(guid);// // PH GUID
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
		// 接收从WS来的SaveData消息结束标志
	case MSG_W2DB_SERVER_SAVEDATAEND:
		{
			BYTE flag = pMsg->GetByte();
			if(flag == 0)
				AddLogText("接收WS定时保存消息结束(不代表线程处理完成)！");
			else if(flag == 1)
				AddLogText("接收WS下线保存玩家数据消息结束(不代表线程处理完成)！");
		}
		break;
	case MSG_W2DB_SERVER_QUEST_PING:
		{
			CMessage msg(MSG_DB2W_SERVER_QUEST_PING);
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
	}
}
