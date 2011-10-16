#include "StdAfx.h"

#include "../Country/Country.h"
#include "../Country/CountryHandler.h"
#include "../Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnCountryMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_COUNTRY_CHANGE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			BYTE btCountry = pMsg->GetByte();
			CPlayer* pPlayer = GetGame().GetMapPlayer( PlayerGuid );
			if(!pPlayer) return;
			/***********************************************************************/
			/*  zhaohang 2010-11-10 fix*/
			/***********************************************************************/
			//long ret = pPlayer->ChangeCountry(btCountry);
			BYTE ret = pPlayer->ChangeCountry(btCountry);
			CMessage msg(MSG_W2S_COUNTRY_CHANGE);
			msg.Add(PlayerGuid);
			msg.Add(ret);
			msg.SendToGS(pMsg->GetGSID());

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
			msg.SendToAllGS();

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
			msg.SendToAllGS();

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
			msg.SendToAllGS();

		}
		break;
	}
}