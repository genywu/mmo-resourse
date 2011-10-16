#include "StdAfx.h"
#include "../OrganizingSystem/WSTeam.h"
#include "../Player.h"


void  OnQuestMessage( CMessage* pMsg)
{
    switch( pMsg->GetType() )
    {
    case MSG_S2W_QUEST_OPER:
        BYTE   oper= pMsg->GetByte();
        BYTE   ret = pMsg->GetByte();
        char   str[64];
        pMsg->GetStr( str,64);
        CGUID  guid;
        pMsg->GetGUID( guid );
        long lGameServerID = GetGame().GetGameServerNumber_ByPlayerID(guid);

        pMsg->SetType( MSG_W2S_QUEST_OPER);
        pMsg->SendToGS(lGameServerID);
        break;
    }
}