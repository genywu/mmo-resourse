#include "StdAfx.h" 
#include "FollowManager.h"
#include "../Player.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../../Server/Game.h"
#include "../../../../Public/Common/FollowDef.h"

FollowManager::FollowManager()
{
    m_ltimer.clear();
    m_InviteTable.clear();
}

FollowManager::~FollowManager()
{
    m_ltimer.clear();
    m_InviteTable.clear();
}

void  FollowManager::StopFollow( CPlayer* player )
{
    if( !player )       return ;
    player->SetCurrentProgress( CPlayer::PROGRESS_NONE );
    /// 给队员发送邀请请求
    CMessage  Msg( MSG_S2C_FOLLOW_Leader );
    Msg.Add( (long) TF_Team_Stop_TeamFollow );
    Msg.Add( (long) TF_Result_Follow_Success );
    Msg.SendToPlayer( player->GetExID() );
}

long  FollowManager::OnTimeOut(ulong timerid,ulong curtime,const void* var)
{

    return 0;
}

void  FollowManager::OnTimerCancel(DWORD timerid,const void *var)
{
    tagTimerVar *pTimervar = (tagTimerVar*)var;
    OBJ_RELEASE(tagTimerVar, pTimervar );
    return ;
}

void  FollowManager::CancelTimer( )
{
    /// Cancel ALl Timer
    return ;
}

bool  FollowManager::IsTimeOut(const CGUID& ID)
{

    return false;
}