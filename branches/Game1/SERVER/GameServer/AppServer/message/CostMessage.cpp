#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\player.h"
#include "..\..\gameserver\game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnCostMessage(CMessage* pMsg)
{
	// 传2不需要发计费信息 bugs 2006.06.05
	switch(pMsg->GetType())
	{
	case MSG_W2S_COST_NOTENOUGHCOST:
		//{
		//	LONG lPlayerID		= pMsg -> GetLong();
		//  CPlayer* pPlayer	= GetGame() -> FindPlayer( lPlayerID );
		//  char szText[256];
		//	pMsg->GetStr(szText,255);
		//	if( pPlayer )
		//	{
		//		//GetGame()->KickPlayer(pPlayer->GetID());
		//		//##首先发送消息给该玩家。				
		//		CMessage msg( MSG_S2C_COST_NOTENOUGHCOST);
		//		//CMessage msg( MSG_S2C_OTHER_ADDINFO );
		//		//msg.Add( static_cast<DWORD>(0xffffffff) );
		//		//msg.Add( static_cast<DWORD>(0xffff0000) );
		//		msg.Add(szText);
		//		msg.SendToPlayer( pPlayer -> GetID() );
		//		GetGame()->KickPlayer(pPlayer->GetID());
		//		//##然后准备延迟1秒钟后断掉该Socket。
		//		//pPlayer -> Disconnect( 5000 );
		//	}
		//}
		break;
	case MSG_W2S_COST_INFORMATION:
		//{
		//	LONG lPlayerID		= pMsg -> GetLong();
		//  CPlayer* pPlayer	= GetGame() -> FindPlayer( lPlayerID );
		// char szText[256];
		//	pMsg->GetStr(szText,255);
		//	if( pPlayer )
		//	{				
		//		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		//		msg.Add( static_cast<DWORD>(0xffffffff) );
		//		msg.Add( static_cast<DWORD>(0xffff0000) );
		//		msg.Add(szText);
		//		msg.SendToPlayer( pPlayer -> GetID() );								
		//	}
		//}
		break;
	}
}