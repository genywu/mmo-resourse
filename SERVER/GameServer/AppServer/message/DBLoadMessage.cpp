
#include "stdafx.h"
#include "..\nets\netserver\message.h"
//#include "..\nets\netserver\MyServerClient.h"
//#include "..\nets\netserver\MyNetServer.h"
//#include "..\nets\netserver\MyServerClient.h"
#include "..\goods\cgoods.h"
#include "..\..\gameserver\game.h"
#include "..\player.h"

#include "..\OrganizingSystem\GameFaction.h"
#include "..\OrganizingSystem\GameUnion.h"
#include "..\OrganizingSystem\GameOrganizingCtrl.h"
#include "..\..\..\..\public\Date.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDBLoadMessage(CMessage* pMsg)
{
	BYTE Data[102400];
	pMsg->GetEx(Data,102400);

	switch(pMsg->GetType())
	{
	case MSG_DB2S_LOAD_PLAYER://保存玩家数据
		{
			BYTE buf[102400];
			pMsg->GetEx(buf, 102400);
			long pos = 0;
			long msgTimerID = _GetLongFromByteArray(buf, pos);
			CGUID id;
			_GetBufferFromByteArray(buf, pos, id);	// playerID
			CPlayer* player = GetGame()->FindPlayer(id);
			if(player)
			{
				player->DecordFromByteArray(buf, pos);
			}
			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);
			AddLogText("从DBS读取Player数据消息...OK！");
		}
		break;
	case MSG_DB2S_LOAD_CONFEDERATION:
		{
			long msgid = pMsg->GetLong();

			//该联盟的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//联盟名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//盟主帮会ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);	

			GameUnion* pConf = new GameUnion(lID, lMastterID, string(strName));

			pConf->InitByWSMsg(pMsg);

			GetOrganiCtrl()->AddConfederationOrganizing(lID, pConf);

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			char str[128];
			sprintf(str,"成功读取同盟 ID[%d]", lID);
			AddLogText(str);
		}
		break;
	case MSG_DB2S_LOAD_FACTION:
		{
			long msgid = pMsg->GetLong();

			//该帮的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//帮会名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//帮主ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);	

			//该帮会成立时间32
			tagTime EstablishedTime;
			pMsg->GetEx(&EstablishedTime, sizeof(tagTime));

			GameFaction* pFaction = new GameFaction(lID, lMastterID,EstablishedTime,string(strName));

			pFaction->InitByWSMsg(pMsg);

			GetOrganiCtrl()->AddConfederationOrganizing(lID, pFaction);

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			char str[128];
			sprintf(str,"成功读取帮会 ID[%d]", lID);
			AddLogText(str);
		}
		break;
	case MSG_S2DB_LOAD_PLAYER_GOOD://保存玩家物品
		{
			pMsg->SetType(MSG_W2S_OTHER_DELGOODS);
			pMsg->SendAll();
		}
		break;
	case MSG_S2W_PLAYER_DELSKILL://gm跨服务器删除一个玩家的技能,该消息转发给下面连接的gameserver
		{
			pMsg->SetType(MSG_W2S_OTHER_DELSKILL);
			pMsg->SendAll();
		}
		break;
	case MSG_S2W_PLAYER_SETLEVEL://gm跨服务器设置一个玩家的等级,该消息转发给下面连接的gameserver
		{
			pMsg->SetType(MSG_W2S_OTHER_SETLEVEL);
			pMsg->SendAll();
		}
		break;	

	}
}
