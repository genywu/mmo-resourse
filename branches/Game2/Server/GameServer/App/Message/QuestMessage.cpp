#include "StdAfx.h"

#include "../RgnManager.h"
#include "../Player.h"
#include "../OrganizingSystem/GameFaction.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GameUnion.h"
#include "../Organizingsystem/GSTeam.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../ServerRegion.h"
#include "../../../../Public/Common/QuestDef.h"
#include "../../../../Public/Setup/QuestSystem.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnQuestMessage(CMessage* pMsg)
{
    switch(pMsg->GetType())
    {
    case MSG_C2S_QUEST_OPER:
        {
            CGUID  Sendid;
            BYTE rep = pMsg->GetByte();
            switch ( rep )
            {
            case QS::QS_SHARE:
                {
                    BYTE ret = pMsg->GetByte();
                    long QuestID = pMsg->GetLong();
                    pMsg->GetGUID( Sendid );
                    CPlayer* pPlayer = pMsg->GetPlayer();
                    if ( ret == 1)
                    {
                        pPlayer->AddQuest( QuestID );
                    }
                    CMessage  msg(MSG_S2W_QUEST_OPER);
                    msg.Add((BYTE)QS::QS_SHARE_Ret);
                    msg.Add((BYTE)ret);
                    msg.Add(pPlayer->GetName());
                    msg.Add(Sendid);
                    msg.Send();
                }
                break;
            }  
        }
        break;

    case MSG_W2S_QUEST_OPER:
        {
            BYTE   oper= pMsg->GetByte();
            BYTE   ret = pMsg->GetByte();
            char   str[64];
            pMsg->GetStr( str,64);
            CGUID  guid;
            pMsg->GetGUID( guid );
            pMsg->SetType( MSG_S2C_QUEST );
            pMsg->SendToPlayer( guid );
        }
        break;
    }
}
