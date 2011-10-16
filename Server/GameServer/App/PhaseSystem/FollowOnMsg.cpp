/** 
 * @file   PhaseMsg.cpp  
 * @brief  跟随队长消息处理
 * @date   2010/03/18        
 */

#pragma  once

#include "StdAfx.h"
#include "../Player.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../../Server/Game.h"
#include "../../../../Public/Common/FollowDef.h"
#include "FollowManager.h"

/** 
 *   预留队长主动邀请队友设计接口处理
 */

void  OnFollowMessage( CMessage* pMsg)
{
    /// GS=GC的通信
    TEAM_FOLLOW_TYPE type = (TEAM_FOLLOW_TYPE)pMsg->GetLong();

    switch( type )
    {
        /// 队长邀请队友跟随队长
    case  TF_Team_Leader_Invite:
        {
            CPlayer* pLeader =  pMsg->GetPlayer();
            if ( pLeader != NULL && pLeader->GetTeamID() != NULL_GUID )
            {
                GSTeam *team =  GetOrganiCtrl()->GetGameTeamByID( pLeader->GetTeamID() );
                /// 如果是队长消息
                if ( team != NULL && team->GetLeader() == pLeader->GetExID() )
                {
                    /// 给队员发送邀请请求
                }
                else
                {
                    //// 非队长的非法消息
                }
            }

        }
        break;

    case  TF_Team_Return_TeamFollow:
        {
            CGUID  first,second;  
            ///> 邀请者
            pMsg->GetGUID( first ); 
            ///> 被邀请者
            pMsg->GetGUID( second );   
        }
        break;

    case  TF_Team_Member_Follow_Leader:
        {
            CPlayer* player =  pMsg->GetPlayer();
            if ( player != NULL && player->GetTeamID() != NULL_GUID )
            {
                GSTeam *team =  GetOrganiCtrl()->GetGameTeamByID( player->GetTeamID() );
                CPlayer* Leader = GetGame()->FindPlayer( team->GetLeader() );
                if( !team || !Leader || Leader==player ) 
                    return ;
                /// 如果不是队长消息 

                player->SetCurrentProgress( CPlayer::PROGRESS_FOLLOW_LEADER );          
                long  lRegionID = static_cast<CServerRegion*>(Leader->GetFather())->GetID();
                long  lPointx   = Leader->GetTileX();
                long  lPointy   = Leader->GetTileY();
                /// 给队员发送邀请请求
                CMessage  Msg( MSG_S2C_FOLLOW_Leader );
                Msg.Add( (long) TF_Team_Member_Follow_Leader);
                Msg.Add( (long) TF_Result_Follow_Success );
                Msg.Add( lRegionID );
                Msg.Add( lPointx );
                Msg.Add( lPointy );
                Msg.SendToPlayer( player->GetExID() );
            }
        }
        break;

    case  TF_Team_Stop_TeamFollow:
        {
            CPlayer* player =  pMsg->GetPlayer();
            if ( !player )  return ;
            
            GetInst(FollowManager).StopFollow( player );
        }
        break;

    case TF_Team_Member_ChangeRgn:
        {

        }
        break;
    }
}