#include "StdAfx.h"
#include "../RgnManager.h"
#include "../ServerRegion.h"
#include "../PhaseSystem/PhaseManager.h"
#include "../PhaseSystem/PhaseOtherDef.h"
#include "../Player.h"   
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../../../../Public/Setup/PhaseSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern  void OnFollowMessage( CMessage* pMsg);

void  PhaseMessage( CMessage* pMsg)
{
    switch( pMsg->GetType() )
    {
    case MSG_C2S_PHASE_OPER:
        GetInst( PhaseManager ).OnMessage( pMsg );
        break;

    case MSG_C2S_FOLLOW_Leader:
        {
              OnFollowMessage( pMsg );
        }
        break;
    }
}
