#include "stdafx.h"
#include "../../../GameClient/ClientApp/ClientRegion.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../Net/Message.h"
#include "../../../GameClient/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnCountryMessage(CMessage* pMsg)
{
	CPlayer* pPlayer=GetGame()->GetMainPlayer();
	switch(pMsg->GetType())
	{
	case MSG_S2C_COUNTRY_CHANGE:
		{
			BYTE ret = pMsg->GetByte();

			// 更改者
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
	
			if( pPlayer->GetExID() == PlayerID ) // 本人
			{
				switch(ret)
				{
				case -1:
					
					break;
				case -2:
					
					break;
				case -3:
					
					break;
				case -4:			// 失败
					
					break;
				case -5:			// 失败
					
					break;

				case 1:			// 提示更换国籍成功
				case 2:
				case 3:
				case 4:
					pPlayer->SetCountry(ret);
					
					break;
				default:			// 失败
					
					break;
				}
			}
			else // 其他人
			{
				CPlayer *pShowPlayer =(CPlayer*) GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER, PlayerID);

				if(pShowPlayer)
				{
					if(ret >= 1 && ret <= 4)
					{
						pShowPlayer->SetCountry(ret);
					}
				}
			}
		}
		break;
	}
}
	