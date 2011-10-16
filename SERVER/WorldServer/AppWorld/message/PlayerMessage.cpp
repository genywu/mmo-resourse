#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\goods\cgoods.h"
#include "..\..\worldserver\game.h"
#include "..\player.h"
#include "../Linkman/LinkmanSystem.h"
#include "..\Organizingsystem\OrganizingParam.h"



void OnPlayerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_PLAYER_ADDSKILL://gm跨服务器给玩家添加一个技能,该消息转发给下面连接的gameserver
		{
			DWORD dwGSIndex=pMsg->GetGSID();
			pMsg->SetType(MSG_W2S_OTHER_ADDSKILL);
			pMsg->Add(dwGSIndex);
			pMsg->SendAll();			
		}
		break;
	case MSG_S2W_PLAYER_DELGOODS://gm跨服务器删除一个玩家的物品,该消息转发给下面连接的gameserver
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
	case MSG_S2W_PLAYER_OCCU_CHANGE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			LONG lNewOccu = pMsg->GetLong();
			CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			if (NULL != pPlayer)
			{
				pPlayer->SetOccupation(lNewOccu);
				LinkmanSystem::GetInstance().OnPlayerOccuChange(pPlayer);
				GetOrganiCtrl()->OnPlayerOccuChange(pPlayer, lNewOccu);
			}
			
		}
		break;
	}
}
