#pragma once
#include "stdafx.h"

#include "Message.h"
#include "Player.h"
#include "ClientRegion.h"
#include "../GameClient/Game.h"
#include "../PhaseSys/PhaseSys.h"
#include "../PhaseSys/FollowLeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void   PhaseMessage( CMessage* pMsg)
{
    CPlayer* pPlayer = pMsg -> GetPlayer();
    CClientRegion *pRegion = (CClientRegion *)pMsg->GetRegion();
    if( pPlayer == NULL )
    {
        return;
    }
    switch( pMsg -> GetType() )
    {                         
    case  MSG_S2C_PHASE_OPER:
        {
            GetInst(PhaseSys).PhaseMsgPro( pMsg );
        }
        break;

    case  MSG_S2C_FOLLOW_Leader:
        {
           GetInst(FollowLeaderManager).OnMessage( pMsg );
        }
        break;
    }
}