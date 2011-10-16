#include "stdafx.h"

#include "../Entity/Account.h"
#include "../Entity/EntityGroup.h"
#include "../Entity/EntityManager.h"
#include "../Thread/DBAccessThread.h"
#include "../Thread/DBThread.h"
#include "../Thread/DBRankThread.h"
#include "../Thread/ThreadDataDef.h"
#include "../../DB/RsEntity.h"

#include "../../../../Public/AppFrame/TextRes.h"
using namespace AppFrame;

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
			WORD dwPort = pMsg->GetWord();
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
				Log4c::Trace(ROOT_MODULE,"%s",FormatText("DB_SVRMSG_1", strGameServerIP, dwPort));//WorldServer %s [%d] 连接...失败!
				break;
			}
			char str[256];
			sprintf(str, "%s [%d]", strGameServerIP, dwPort);
            AppFrame::AddToList("list", str);
			Log4c::Trace(ROOT_MODULE,"%s",FormatText("DB_SVRMSG_0", strGameServerIP, dwPort));//WorldServer %s [%d] 连接...成功!

			// 发送数据库配置信息
			GetGame()->SendDBTableInfo(pMsg->GetSocketID());
            if(GetGame()->GetSetup()->bOpenRanks)
            {
                GetGame()->GetEntityManager()->GetRankThread()->SendRanksToWS();
            }            
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

			// 先在内存中查找
			//CDBEntityManager::DBPlayerMapItr pItr;
			//for(pItr = GetGame()->GetDBEntityManager()->GetDBPlayerMap().begin(); 
			//	pItr != GetGame()->GetDBEntityManager()->GetDBPlayerMap().end(); pItr++)
			//{
			//	CEntityProperty* ep = pItr->second->GetDataEntityManager().GetEntityProperty(string("Name"));
			//	if(ep)
			//	{
			//		if(strcmp(playerName, ep->GetStringAttr()) == 0) // 相同名字
			//		{
			//			ep = pItr->second->GetDataEntityManager().GetEntityProperty(string("PHGUID"));
			//			if(ep)
			//			{
			//				ep->GetGuidAttr( guid);	
			//				break;
			//			}
			//		}
			//	}
			//}

			//if(pItr == GetGame()->GetDBEntityManager()->GetDBPlayerMap().end()) // 未找到该玩家,到数据库查找
			//{
			//	// 查询数据库
			//	GetGame()->GetRsDupRgn()->QuestPHGuidByPlayerID(playerName, guid);
			//}

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
				Log4c::Trace(ROOT_MODULE,GetText("DB_SVRMSG_2"));//接收WS定时保存消息结束(不代表线程处理完成)！
			else if(flag == 1)
				Log4c::Trace(ROOT_MODULE,GetText("DB_SRVMSG_3"));//接收WS下线保存玩家数据消息结束(不代表线程处理完成)！
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
