
#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\goods\cgoods.h"
#include "..\..\worldserver\game.h"
#include "..\player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDBSaveMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_DB2W_SAVE_CREATE_PLAYER:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存Create Player数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_COUNTRY:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存country数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_CARDS_LARGESS:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存cards largess数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_ALL_CITY_WAR_PARAM:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存cities war param数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_ALL_VILLAGE_WAR_PARAM:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存billages war param数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_ALL_ENEMY_FACTIONS:
		{
			//取消定时器

			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存enemy factions数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_REGION_PARAM:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存region param数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_CONFEDERATION:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存union数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_FACTION:
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存faction数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_DB2W_SAVE_PLAYER: // 保存玩家数据
		{
			//取消定时器
			long MsgTimerID = pMsg->GetLong();
			GetGame()->DelMsgTimer(MsgTimerID);
			AddLogText("从DBS反馈保存Player数据消息 [%d]OK！", MsgTimerID);
		}
		break;
	case MSG_S2DB_SAVE_PLAYER://保存玩家数据
		{
			DWORD dwGSIndex=pMsg->GetGSID();
			pMsg->SetType(MSG_W2S_OTHER_ADDSKILL);
			pMsg->Add(dwGSIndex);
			pMsg->SendAll();			
		}
		break;
	case MSG_S2DB_SAVE_PLAYER_GOOD://保存玩家物品
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
