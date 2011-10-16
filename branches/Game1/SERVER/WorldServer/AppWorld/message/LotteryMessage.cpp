#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\goods\cgoods.h"
#include "..\..\worldserver\game.h"
#include "..\player.h"
#include "../Linkman/LinkmanSystem.h"
#include "..\Lottery\CLotterySystem.h"



void OnLotteryMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_LOTTERY_CHANGEWORLDCREDIT:
		{
			long lWorldCredit = pMsg->GetLong();
			CLotterySystem::GetInstance().SetWorldCredit(lWorldCredit);
            CLotterySystem::GetInstance().SetIsSendWorldCredit(true);
		}
		break;
	case MSG_S2W_LOTTERY_CHANGEPRECEDE:
		{
			long lCreditHighest = pMsg->GetLong();
			char szName[128];
			pMsg->GetStr(szName, 128);
			CLotterySystem::GetInstance().SetCreditHighest(lCreditHighest);
			CLotterySystem::GetInstance().SetCreditHighestName(szName);
			//向GS转发世界最高个人积分的数据
            CLotterySystem::GetInstance().SendCreditHighest();
		}
		break;
	case MSG_S2W_LOTTERY_WORLDINFO:
		{
			char szText[1024];
			pMsg->GetStr(szText, 1024);
			CMessage msg(MSG_W2S_LOTTREY_SENDWorldInfo);
	        msg.Add((char*)szText);
	        msg.SendAll();
		}
		break;
	}
}
