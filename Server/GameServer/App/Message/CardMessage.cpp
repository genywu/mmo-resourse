#include "StdAfx.h"
#include "../Player.h"
#include "../Goods/CGoodsFactory.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"
#include "../MessagePackage/CS2CContainerObjectAmountChange.h"

#include "../../../../Public/Setup/GlobalSetup.h"

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
		// 充值请求 
	case MSG_C2S_CARD_CHARGE_REQUEST:
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			
			char CardNumber[16];
			char CardPassword[16];
			char szAccount[32];

			pMsg->GetStr(CardNumber,16);
			pMsg->GetStr(CardPassword, 16);
			pMsg->GetStr(szAccount, 32);

			DWORD dwClientIP = pMsg->GetIP();

			if( strlen(CardNumber) != 12 )
			{
				CMessage msg(MSG_S2C_CARD_CHARGE_RESPONSE);
				msg.Add(long(200)); 
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			if( strlen(CardPassword) != 10 )
			{
				CMessage msg(MSG_S2C_CARD_CHARGE_RESPONSE);
				msg.Add(long(201)); 
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			if( strlen(szAccount) < 4 )
			{
				CMessage msg(MSG_S2C_CARD_CHARGE_RESPONSE);
				msg.Add(long(202)); 
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			// Player身上金钱操作指定数目
			if( pPlayer->GetMoney() >= 2000000000 )
			{
				CMessage msg(MSG_S2C_CARD_CHARGE_RESPONSE);
				msg.Add(long(100)); 
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			// Player身上金钱操作指定数目
			if( pPlayer->GetYuanbao() >= 2000000000 )
			{
				CMessage msg(MSG_S2C_CARD_CHARGE_RESPONSE);
				msg.Add(long(101)); 
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			// 成功，转发给ws

			CMessage msg(MSG_S2W_CARD_CHARGE_REQUEST);
			msg.Add(CardNumber);
			msg.Add(CardPassword);
			msg.Add(szAccount);
			msg.Add(pPlayer -> GetExID());
			msg.Add(dwClientIP);
			msg.Send();
		}
		break;

	case MSG_W2S_CARD_CHARGE_RESPONSE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			if(!pPlayer) return;

			pMsg->SetType(MSG_S2C_CARD_CHARGE_RESPONSE);
			pMsg->SendToPlayer(PlayerGuid);

		}
		break;
		//////
		//////
	case MSG_C2S_CARD_SELL_REQUEST:		// 客户端请求寄售点卡
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			char CardNumber[16];
			char CardPassword[16];
			DWORD CardSellPrice;
			BYTE CardTimeType;

			pMsg->GetStr(CardNumber,16);
			pMsg->GetStr(CardPassword, 16);
			CardSellPrice = pMsg->GetDWord();
			// 获取寄售类型 6 12 24小时
			CardTimeType= pMsg->GetByte();

			//// 仓库金额超过20乙，不能寄售
			//if( pPlayer->GetDepotMoney() >= GlobalSetup::GetSetup()->dwGoldCoinLimit )
			//{
			//	CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
			//	msg.Add(long(9001)); 
			//	msg.Add(CardNumber);
			//	msg.SendToPlayer(pPlayer -> GetExID());
			//	return;
			//}

			// 通过寄售类型 从全局配置中读取手续费
			switch(CardTimeType)
			{
			case 0: // 6 hours
				if( pPlayer->GetMoney() < GlobalSetup::GetSetup()->m_dwCard6HoursChargePrice ) {
					CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
					msg.Add(long(1001)); // 手续费不够，而失败 6小时的
					msg.Add(CardNumber);
					msg.SendToPlayer(pPlayer -> GetExID());
					return;
				}
				break;

			case 1: // 12 hours
				if( pPlayer->GetMoney() < GlobalSetup::GetSetup()->m_dwCard12HoursChargePrice ) {
					CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
					msg.Add(long(1002)); // 手续费不够，而失败 12小时的
					msg.Add(CardNumber);
					msg.SendToPlayer(pPlayer -> GetExID());
					return;
				}
				break;

			case 2:
			default: // 24 hours and other
				if( pPlayer->GetMoney() < GlobalSetup::GetSetup()->m_dwCard24HoursChargePrice ) {
					CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
					msg.Add((long)1003); // 手续费不够，而失败 24小时的
					msg.Add(CardNumber);
					msg.SendToPlayer(pPlayer -> GetExID());
					return;
				}
				//break;
			}

			// 手续费够，检查寄售金额是否在合法区间内
			if( CardSellPrice < GlobalSetup::GetSetup()->m_dwCardLowerLimitPrice ||
				CardSellPrice > GlobalSetup::GetSetup()->m_dwCardUpperLimitPrice )
			{
				CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
				msg.Add((long)1004); // 寄售金额非法
				msg.Add(CardNumber);
				msg.SendToPlayer(pPlayer -> GetExID());
				return;
			}

			// 手续费够，转发到WS
			pMsg->SetType(MSG_S2W_CARD_SELL_REQUEST);
			pMsg->Add(pPlayer -> GetExID());
			pMsg->Send();
		}
		break;

		//////
		//////
	case MSG_W2S_CARD_SELL_RESPONSE:
		{
			long res;

			char CardNumber[16];
			char CardType[16];
			DWORD ChargePrice;
			
			DWORD CardTime;
			DWORD Price;
			
			res = pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pMsg->GetStr(CardNumber, 16);

			if(res)
			{
				CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
				msg.Add((long)res); 
				msg.Add(CardNumber);
				//...
				msg.SendToPlayer(PlayerGuid);
				return;
			}

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			if(!pPlayer) return;
			

			pMsg->GetStr(CardType, 16);
			CardTime = pMsg->GetDWord();
			Price = pMsg->GetDWord();
			ChargePrice = pMsg->GetDWord();

			// 玩家在线，扣除手续费
			DWORD money = pPlayer->GetMoney();

			if( money < ChargePrice )
			{
				CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
				msg.Add((long)1005); // 寄售手续费不够了。
				msg.Add(CardNumber);
				msg.SendToPlayer(PlayerGuid);
				return;
			}

			// 扣了
			if(!pPlayer->SetMoney( money - ChargePrice,14))
			{
				CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
				msg.Add((long)1005); // 寄售手续费不够了。
				msg.Add(CardNumber);
				msg.SendToPlayer(PlayerGuid);
				return;
			}

			// 发消息到Client
			CMessage msg(MSG_S2C_CARD_SELL_RESPONSE);
			msg.Add((long)0); // 成功
			msg.Add((char*)CardNumber);
			msg.Add((char*)CardType);
			msg.Add(CardTime);
			msg.Add(Price);
			msg.SendToPlayer(PlayerGuid);

		}
		break;

		//////
		//////

		// 购买此卡
	case MSG_C2S_CARD_BUY_REQUEST:
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			char CardNumber[16];
			DWORD money;

			pMsg->GetStr(CardNumber, 16);
			const CGUID& PlayerID = pPlayer -> GetExID();
			money = pPlayer->GetMoney();

						
			pMsg->SetType(MSG_S2W_CARD_RESERVE_REQUEST);
			pMsg->Add(PlayerID);
			pMsg->Add(money);
			pMsg->Send();

		}
		break;

		/////
		/////
	case MSG_W2S_CARD_RESERVE_RESPONSE: // 预定完成后，扣除购买者的金额
		{
			long res;
			DWORD money;
			char CardNumber[16];

			res = pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			// 失败
			if(res)
			{
				// 直接发送到Client 购买失败
				// 发送购买消息到WorldServer
				CMessage msg(MSG_S2C_CARD_BUY_RESPONSE);
				msg.Add(res);
				msg.Add(CardNumber);
				msg.SendToPlayer(PlayerGuid);
				return;
			}
				
			pMsg->GetStr(CardNumber, 16);
			money = pMsg->GetDWord();

			CPlayer *pPlayer = GetGame()->FindPlayer( PlayerGuid );
			if( !pPlayer ) return;

			if( pPlayer->GetMoney() < money ) 
			{
				// 金额不够，购买失败。
				CMessage msg(MSG_S2C_CARD_BUY_RESPONSE);
				msg.Add(long(2001));
				msg.Add(CardNumber);
				msg.SendToPlayer(PlayerGuid);
				return;
			}


			// 扣钱
			if(!pPlayer->SetMoney( pPlayer->GetMoney() - money,15 ))
			{
				// 金额不够，购买失败。
				CMessage msg(MSG_S2C_CARD_BUY_RESPONSE);
				msg.Add(long(2001));
				msg.Add(CardNumber);
				msg.SendToPlayer(PlayerGuid);
				return;
			}

			// 发送购买消息到WorldServer
			CMessage msg(MSG_S2W_CARD_BUY_REQUEST);
			msg.Add(CardNumber);
			msg.Add(PlayerGuid);
			msg.Send();
		}
		break;


		//////
		//////
	case MSG_W2S_CARD_BUY_RESPONSE: // 返回购买请求
		{
			long res;
			char CardNumber[16];

			res = pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetStr(CardNumber,16);

			CMessage msg(MSG_S2C_CARD_BUY_RESPONSE);
			msg.Add(res);
			msg.Add(CardNumber);
			msg.SendToPlayer(PlayerGuid);

			// send to player
		}
		break;


		////////
		////////
	case MSG_C2S_CARD_CANCEL_REQUEST: // 玩家取消点卡的寄售
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			pMsg->SetType(MSG_S2W_CARD_CANCEL_REQUEST);
			pMsg->Add(pPlayer -> GetExID());
			pMsg->Send();
		}
		break;


		///////
		///////
	case MSG_W2S_CARD_CANCEL_RESPONSE:
		{
			long res;
			char CardNumber[16];

			res = pMsg->GetLong();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetStr(CardNumber, 16);

			CMessage msg(MSG_S2C_CARD_CANCEL_RESPONSE);
			msg.Add(res);
			msg.Add(CardNumber);
			msg.SendToPlayer(PlayerGuid);
		}
		break;

	case MSG_C2S_CARD_LIST_REQUEST:// 请求寄售列表
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			pMsg->SetType(MSG_S2W_CARD_LIST_REQUEST);
			pMsg->Add(pPlayer -> GetExID());
			pMsg->Send();
		}
		break;

	case MSG_W2S_CARD_LIST_RESPONSE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pMsg->SetType(MSG_S2C_CARD_LIST_RESPONSE);
			pMsg->SendToPlayer(PlayerGuid);
		}
		break;

	case MSG_C2S_CARD_SALECOSTS_REQUEST: // 请求费用。
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();
			if( !pPlayer ) return;

			CMessage msg(MSG_S2C_CARD_SALECOSTS_RESPONSE);
			msg.Add(GlobalSetup::GetSetup()->m_dwCard6HoursChargePrice);
			msg.Add(GlobalSetup::GetSetup()->m_dwCard12HoursChargePrice);
			msg.Add(GlobalSetup::GetSetup()->m_dwCard24HoursChargePrice);
			msg.SendToPlayer(pPlayer -> GetExID());
		}
		break;

	} // end switch;
} // end OnCardMessage