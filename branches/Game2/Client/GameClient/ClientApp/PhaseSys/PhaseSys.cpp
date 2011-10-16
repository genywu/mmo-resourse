#include "stdafx.h"
#include "PhaseSys.h"
#include "../Player.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../GameClient/Game.h"
#include "../../../Net/Message.h"

namespace  PhaseSysBox
{
    struct  tagPhaseBox
    {
        CGUID  first;
        CGUID  second;
        CGUID  SessionID;
    };
}

void  InviteeJoinPhase(MsgEvent& event,eMsgResult UserId)
{
    /// 是否加入相位
    BYTE  ret = ( UserId == S_Ok ) ? 1 : 0;
    PhaseSysBox::tagPhaseBox* PhaseBox = (PhaseSysBox::tagPhaseBox*)event.pData; 
    if ( !PhaseBox ) return ;
   
    CMessage  Msg( MSG_C2S_PHASE_OPER );
    Msg.Add( (long) C2S_Invitee );
    Msg.Add( PhaseBox->first );
    Msg.Add( PhaseBox->second );
    Msg.Add( PhaseBox->SessionID);
    Msg.Add( (BYTE) ret );
    Msg.Send();

    SAFE_DELETE( PhaseBox );
}

void   PhaseSys::PhaseMsgPro(CMessage* pMsg)
{
    S2CPhaseOperType  Type = (S2CPhaseOperType)pMsg->GetLong();
    
    switch( Type )
    {   
    case S2C_ChangeState:
        {
            /// 针对周围玩家 
            CGUID  PhaseID,PlayerID;
            pMsg->GetGUID( PhaseID );
            pMsg->GetGUID( PlayerID);
            CPlayer* player = GetGame()->GetMainPlayer();
            /// 如果是自己
            if ( player->GetExID() == PlayerID )
            {
                player->SetPhaseID(PhaseID);
            }
            else 
            {
                /// 自己UI和特效改变
                /// 同步更新周围Shape
            } 
        }
        break;

    case S2C_EnterPhase:
        {
            CGUID  PhaseID;
            /// 得到相位ID
            pMsg->GetGUID( PhaseID ); 
            CPlayer* player = GetGame()->GetMainPlayer();
            player->SetPhaseID(PhaseID);
            {
                /** 
                 * /// 自己UI和特效改变
                 * /// 同步更新自身周围Shape
                 */
                float  xpoint,ypoint,asize,bsize;
                xpoint = pMsg->GetFloat();
                ypoint = pMsg->GetFloat();
                asize = pMsg->GetFloat();
                bsize = pMsg->GetFloat();
                CPlayer::PhasePlace  place;
                place.x_point = xpoint;
                place.y_point = ypoint;
                place.a_size  = asize;
                place.b_size  = bsize;
                player->SetPhasePlace( place );
                GetInst(MsgEventManager).PushEvent(Msg_Ok,"进入相位");
            }
        }
        break;

    case S2C_LeavePhase:
        {
            CPlayer* player = GetGame()->GetMainPlayer();
            player->SetPhaseID(NULL_GUID);
            {
                /// 自己UI和特效改变
                /// 同步更新周围Shape
            }
            GetInst(MsgEventManager).PushEvent(Msg_Ok,"离开相位");
        }
        break;

    case S2C_InVitee:
        {
            CGUID  first,second,SessionID;
            pMsg->GetGUID( first ); ///> 邀请者
            pMsg->GetGUID( second );///> 被邀请者
            pMsg->GetGUID( SessionID);/// SessionID

            CPlayer *pPlayer = GetGame()->GetMainPlayer();

            char name[256];
            pMsg->GetStr(name,256);
            strcat(name," 邀请是否加入相位? ");

            PhaseSysBox::tagPhaseBox* PhaseBox = new PhaseSysBox::tagPhaseBox;
            PhaseBox->first = first;
            PhaseBox->second= second;
            PhaseBox->SessionID= SessionID;
            GetInst(MsgEventManager).PushEvent(Msg_YesNo,name,InviteeJoinPhase,PhaseBox);
        }
        break;

    case S2C_JoinTimeOut:
        {
            GetInst(MsgEventManager).PushEvent(Msg_Ok,"加入相位超时!");
        }
        break;
    case S2C_RefuseJoin:
        {
            GetInst(MsgEventManager).PushEvent(Msg_Ok,"拒绝加入相位!");
        }
        break;
    }
}

void   PhaseSys::PhaseOper(CPlayer* player,C2SPhaseOperType Oper)
{
    CMessage  Msg( MSG_C2S_PHASE_OPER );
    Msg.Add( (long) Oper );
    Msg.Add( player->GetExID() );
    Msg.Send();
}

void   PhaseSys::PhaseOper(const CGUID& first,const CGUID& second,C2SPhaseOperType Oper)
{
     CMessage  Msg( MSG_C2S_PHASE_OPER );
     Msg.Add( (long) Oper );
     Msg.Add( first );
     Msg.Add( second);
     Msg.Send();
}