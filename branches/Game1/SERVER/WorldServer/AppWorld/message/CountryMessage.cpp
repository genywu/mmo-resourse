#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\..\worldserver\game.h"
#include "..\..\appworld\player.h"
#include "..\..\appworld\Country\CountryHandler.h"
#include "..\..\appworld\Country\Country.h"
#include "../../../setup/LogSystem.h"



void OnCountryMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_COUNTRY_CHANGE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			BYTE btCountry = pMsg->GetByte();
			CPlayer* pPlayer = GetGame() -> GetMapPlayer( PlayerGuid );
			if(!pPlayer) return;

			long ret = pPlayer->ChangeCountry(btCountry);

			CMessage msg(MSG_W2S_COUNTRY_CHANGE);
			msg.Add(PlayerGuid);
			msg.Add(ret);
			msg.SendGSID(pMsg->GetGSID());

		}
		break;


	case MSG_S2W_COUNTRY_POWER_CHANGE:
		{
			BYTE btCountryID = pMsg->GetByte();
			long lCountryPower = pMsg->GetLong();

			CCountry *pCountry = GetCountryHandler()->GetCountry(btCountryID);
			if(!pCountry) return;

			pCountry->SetPower( lCountryPower );

			CMessage msg(MSG_W2S_COUNTRY_POWER_CHANGE);
			msg.Add( btCountryID );
			msg.Add( lCountryPower );
			msg.SendAll();

		}
		break;

	case MSG_S2W_COUNTRY_TECHLEVEL_CHANGE:
		{
			BYTE btCountryID = pMsg->GetByte();
			long ln = pMsg->GetLong();

			CCountry *pCountry = GetCountryHandler()->GetCountry(btCountryID);
			if(!pCountry) return;

			pCountry->SetTechLevel( ln );

			CMessage msg(MSG_W2S_COUNTRY_TECHLEVEL_CHANGE);
			msg.Add( btCountryID );
			msg.Add( ln );
			msg.SendAll();

		}
		break;

	case MSG_S2W_COUNTRY_KING_CHANGE:
		{
			BYTE btCountryID = pMsg->GetByte();
			CGUID KingGuid;
			pMsg->GetGUID(KingGuid);

			CCountry *pCountry = GetCountryHandler()->GetCountry(btCountryID);
			if(!pCountry) return;

			pCountry->SetKing( KingGuid );

			CMessage msg(MSG_W2S_COUNTRY_KING_CHANGE);
			msg.Add( btCountryID );
			msg.Add( KingGuid );
			msg.SendAll();

		}
		break;
	}
}