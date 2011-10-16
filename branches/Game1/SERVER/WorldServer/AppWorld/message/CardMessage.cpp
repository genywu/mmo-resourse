#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "../../Server/setup/GlobeSetup.h"
#include "..\..\worldserver\game.h"
#include "..\..\appworld\CardsDealingSystem\CardsHandler.h"
#include "..\..\appworld\CardsDealingSystem\Card.h"

#include "../../../setup/LogSystem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应寄售卡消息
void OnCardMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_CARD_CHARGE_REQUEST:
		
		break;

	case MSG_COST2W_CARD_CHARGE_RESPONSE:
		
		break;

	case MSG_S2W_CARD_SELL_REQUEST:				// 转发到cost进行点卡验证
		
		break;

	case MSG_COST2W_CARD_VALIDATE_RESPONSE:		// 
		
		break;

	case MSG_COST2W_CARD_SELL_RESPONSE:
		
		break;

	case MSG_S2W_CARD_RESERVE_REQUEST: // 预定卡
		
		break;

		// CostServer 返回 预定卡消息
	case MSG_COST2W_CARD_RESERVE_RESPONSE:
		break;

	case MSG_S2W_CARD_BUY_REQUEST: // 预定成功后GameServer发出购买消息
		
		break;

	case MSG_COST2W_CARD_BUY_RESPONSE: // Cost Server回音购买请求
		
		break;

	case MSG_S2W_CARD_CANCEL_REQUEST: // Gameserver发送来的取消点卡
		
		break;

	case MSG_COST2W_CARD_CANCEL_RESPONSE:
		
		break;

	case MSG_S2W_CARD_LIST_REQUEST: // 请求寄售列表
		
		break;
	} // end switch
} // end fuction
