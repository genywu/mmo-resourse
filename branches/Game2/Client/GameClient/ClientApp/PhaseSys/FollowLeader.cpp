#include "StdAfx.h"
#include "FollowLeader.h"
#include "../Player.h"
#include "../ClientRegion.h"
#include "../Other/RegionList.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/GameControl.h"
#include "../../../Net/Message.h"

#define   FOLLOWRESULT(player,Type) \
    {\
        const char* name = player->GetName();\
        Log4c::Trace(ROOT_MODULE,"FollowLeader失败消息:错误ID:%6d\n",Type);\
    }
    

void  FollowLeaderManager::OnMessage(CMessage *pMsg)
{
     TEAM_FOLLOW_TYPE   Type = (TEAM_FOLLOW_TYPE)pMsg->GetLong();  
     TEAM_FOLLOW_RESULT result = (TEAM_FOLLOW_RESULT)pMsg->GetLong();

     CPlayer *pPlayer = GetGame()->GetMainPlayer();
     if( !pPlayer )  return ;

     if ( result != TF_Result_Follow_Success )
     {
         //FOLLOWRESULT(pPlayer,(long)result );
         return ;
     }

     switch( Type )
     {
     case TF_Team_Leader_Invite:
         {

         }
         break;

     case TF_Team_Return_TeamFollow:
         {

         }
         break;

     case TF_Team_Member_Follow_Leader:
         {
             long lRegionID = pMsg->GetLong();
             long lPointx   = pMsg->GetLong();
             long lPointy   = pMsg->GetLong();

             long  RegionId = pPlayer->GetRegionID();
             CGameControl *pGameCtl = GetGame()->GetGameControl();

             pGameCtl->SetFollowLeader( true );

             if(pGameCtl->GetMPActionMode() == CGameControl::MPAM_AutoAttack)
                 pGameCtl->AutoAttactStop();

             /// 自动寻路
             if ( lRegionID == RegionId )
             {
                 // 目的地无阻挡开始移动
                 if(GetGame()->GetRegion()->GetBlock(lPointx,lPointy) != CRegion::BLOCK_CANFLY &&
                     GetGame()->GetRegion()->GetBlock(lPointx,lPointy) != CRegion::BLOCK_UNFLY)
                 pGameCtl->AutoMoveTo((float)lPointx,(float)lPointy);
             }
             else 
             {
                 pGameCtl->ReSetFollowPoint();
                 CRegionList::FindRegionPath(RegionId,lRegionID);
                 pGameCtl->FindPathToOtherRegion(RegionId,pPlayer->GetTileX(),pPlayer->GetTileY(),lRegionID,lPointx,lPointy);
             }
         }     
         break;

     case TF_Team_Stop_TeamFollow:
         {
             pPlayer->SetAction( CShape::ACT_STAND );
             CGameControl *pGameCtl = GetGame()->GetGameControl();
             pGameCtl->SetFollowLeader();
             pGameCtl->AutoMoveStop();
             pGameCtl->StopMove();
         }
         break;
     case TF_Team_Member_ChangeRgn:
         {
             long lRegionID = pMsg->GetLong();
             long lPointx   = pMsg->GetLong();
             long lPointy   = pMsg->GetLong();

             long  RegionId = pPlayer->GetRegionID();
             CGameControl *pGameCtl = GetGame()->GetGameControl();

             if(pGameCtl->GetMPActionMode() == CGameControl::MPAM_AutoAttack)
                 pGameCtl->AutoAttactStop();

             /// 自动寻路
             if ( lRegionID == RegionId )
             {
                 // 目的地无阻挡开始移动
                 if(GetGame()->GetRegion()->GetBlock(lPointx,lPointy) != CRegion::BLOCK_CANFLY &&
                    GetGame()->GetRegion()->GetBlock(lPointx,lPointy) != CRegion::BLOCK_UNFLY)
                 pGameCtl->AutoMoveTo((float)lPointx,(float)lPointy);
             }
             else 
             {
                 pGameCtl->ReSetFollowPoint();
                 CRegionList::FindRegionPath(RegionId,lRegionID);
                 pGameCtl->FindPathToOtherRegion(RegionId,pPlayer->GetTileX(),pPlayer->GetTileY(),lRegionID,lPointx,lPointy);
             }
         }
         break;
     }
}

void  FollowLeaderManager::FollowOper(CPlayer* player,TEAM_FOLLOW_TYPE Oper)   
{
    CMessage  Msg( MSG_C2S_FOLLOW_Leader );
    Msg.Add( (long)Oper );
    Msg.Send();
}

void  FollowLeaderManager::Pro(TEAM_FOLLOW_RESULT  result,CMessage* pMsg)
{
    
}