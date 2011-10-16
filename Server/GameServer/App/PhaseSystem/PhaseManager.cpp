#include "StdAfx.h"
#include "PhaseManager.h"
#include "PhasePro.h"
#include "FilterPlayer.h"
#include "PhaseOtherDef.h"
#include "../Player.h"
#include "../Monster.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../../../../Public/Setup/PhaseSetup.h"

PhaseManager::PhaseManager()
{
    Release(); 
}

PhaseManager::~PhaseManager()
{
    Release();
}

void  PhaseManager::Release()
{
    PIter it = m_PhaseList.begin();
    for ( ; it!= m_PhaseList.end(); it++ )
    {
        OBJ_RELEASE( PhasePro , it->second );
    }
    m_PhaseList.clear();
    m_InviteList.clear();
}

void  PhaseManager::Release(const CGUID& GID)
{
    PIter it = m_PhaseList.find( GID );
    if ( it != m_PhaseList.end()  )
    {
        char  str[200];     
        sprintf_s( str, "相位%s,达到触发完成条件后结束相位功能" ,GID.tostring());
        PRINT_PHASE_TO_CLIENT(str,NULL);
        OBJ_RELEASE( PhasePro , it->second );
        m_PhaseList.erase( it );
    }
}

///  针对野战相位创建
void  PhaseManager::CreatePhase(CPlayer* player, 
                                float size1 ,
                                float size2 ,
                                eTrigger trigger,
                                const char* Scriptfile)
{
      PhaseOpen(RNULL);
  
      if (!player || player->GetPhaseID() != NULL_GUID )
      {
          Log4c::Error(ROOT_MODULE,"%-15s Player name %s ReCreatePhase",__FUNCTION__, player->GetName() );
          return ;
      }

      const CGUID& team_id = player->GetTeamID();
      GSTeam *team = GetOrganiCtrl()->GetGameTeamByID( team_id );

      /// 如果不是单独的一个人  而且 又不是队长不能创建相位
      if( team_id != NULL_GUID && (team == NULL || team->GetLeader()!= player->GetExID()) )
      {
          Log4c::Error(ROOT_MODULE,"%-15s eFight Phase Create Failed ! player % s is not team leader",__FUNCTION__, player->GetName() );
          return ;
      }

      if ( player->IsDied() ) return ;

      /// 相位初始化
      PhasePro* TempPro = OBJ_CREATE(PhasePro) ;// new PhasePro();
      if( TempPro != NULL )
      {   
          m_PhaseList[ TempPro->GetID() ] = TempPro;
          TempPro->Init(player,team, 0 , size1 , size2 , eFight,trigger);
          TempPro->RunScript(player, Scriptfile ); 

          Log4c::Trace(ROOT_MODULE, "eFight Phase { %s } Create Successed ! player % s is team leader" , 
              TempPro->GetID().tostring() , player->GetName() );
      }
}

/// 针对竞技相位创建
void  PhaseManager::CreatePhase(const CGUID& first,const CGUID& second,
                                float size1, float size2,
                                eTrigger trigger,
                                const char* Scriptfile )
{
      /// 针对竞技的创建规则，不明确，没有注明
      /// 传进来的first,second 应该都是队伍Ｌｅａｄｅｒ
      PhaseOpen(RNULL);

      CPlayer* fplayer = GetGame()->FindPlayer( first );
      CPlayer* splayer = GetGame()->FindPlayer( second);

      if ( fplayer->IsDied() || splayer->IsDied() ) return ;

      const CGUID& team_fid = fplayer->GetTeamID();
      const CGUID& team_sid = splayer->GetTeamID();

      GSTeam *teamf = GetOrganiCtrl()->GetGameTeamByID( team_fid );
      GSTeam *teams = GetOrganiCtrl()->GetGameTeamByID( team_sid );

      /// 相位初始化 new PhasePro();
      PhasePro* TempPro = OBJ_CREATE(PhasePro);
      if( TempPro != NULL )
      {   
          m_PhaseList[ TempPro->GetID() ] = TempPro;
          
          /// 建立2组队伍映射
          TempPro->Init(fplayer,teamf,2,size1,size2,ePk,trigger,splayer,teams);
          TempPro->RunScript(fplayer, Scriptfile );  

         Log4c::Trace(ROOT_MODULE, "Pk Phase { %s } Create Successed ! first leader % s , second leader %s", 
                    TempPro->GetID().tostring() , fplayer->GetName() , splayer->GetName() );
      }
}

void   PhaseManager::CreatePhaseMonster(const CGUID& GID, const char* OrgName, float x_point , float y_point )
{
    PhaseOpen(RNULL);

    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
       itr->second->CreateMonster(OrgName,x_point,y_point);
    }
}

long   PhaseManager::GetPhasePlayerNum(const CGUID& GID)
{
    PhaseOpen( 0 );

    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        return itr->second->GetPlayerNum();
    }
    return 0;
}

long   PhaseManager::ActivePhase(const CGUID& GID, eTimerType etype,
                                 long  lTimer , long lExipires)
{
    PhaseOpen( 0 );

    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
       itr->second->ActiveSelf( etype , lTimer , lExipires);
    return 0;
}

long    PhaseManager::OnPhaseOper(const CGUID& GID,CPlayer* player,ePhaseOper Oper)
{
    PhaseOpen( 0 );

    if(player == NULL ) return 0;
    PIter itr = m_PhaseList.find( GID );
    if ( itr == m_PhaseList.end() )
    {
        player->SetPhaseID( NULL_GUID );
        return 0;
    }
    itr->second->OnPhaseOper( player, Oper,player->GetPhaseRest() );

    if ( itr->second->GetPhaseStates() == eClose )
    {
        Release( itr->second->GetID() );
        Log4c::Trace(ROOT_MODULE, "Phase End , because All Player Leave Phase ");
    }
    return 1;
}

/**
 * 此函数的作用是为主动判断相位结束，然后运行结束脚本。
 *
 */
long    PhaseManager::OnPhaseOper(const CGUID& GID,CMonster* Monster, ePhaseMOper Oper )
{
    PhaseOpen( 0 );

    if( !Monster )  return 0;
    PIter itr = m_PhaseList.find( GID );
    if ( itr == m_PhaseList.end() )
    {
        Monster->SetPhaseID( NULL_GUID );
        return 0;
    }
    itr->second->OnPhaseOper( Monster, Oper );

    if ( itr->second->GetPhaseStates() == eEnd && itr->second->GetPhaseTrigger() == eActive )
    {
        itr->second->RunScript(itr->second->GetEndScriptFile());
        Log4c::Trace(ROOT_MODULE,"Phase End , because All Monster had been killed in Phase ");
    }
    return 1;
}

/** 
 *  相位的事件处理
 */
long    PhaseManager::OnPhaseEvent(const CGUID& GID,CMoveShape* shape,ePhaseEvent Event)
{
    PhaseOpen(0);
    
    if( !shape )  return 0;
    PIter itr = m_PhaseList.find( GID );
    if ( itr == m_PhaseList.end() )
    {
        shape->SetPhaseID( NULL_GUID );
        return 0;
    }
    
    itr->second->OnPhaseEvent( shape, Event );
    
    if ( itr->second->GetPhaseStates() == eClose)
    {
        Release( itr->second->GetID() );
        Log4c::Trace(ROOT_MODULE, "Phase Close , because All Player Leave Phase ");
        return 0;
    }
    if ( itr->second->GetPhaseStates() == eEnd && itr->second->GetPhaseTrigger() == eActive)
    {
        itr->second->RunScript(itr->second->GetEndScriptFile());
    }
    return 0;
}

bool     PhaseManager::PhasePlayerFilter(CMoveShape* player , CMoveShape* shape)
{
    PhaseOpen( false );

    if ( !player || !shape )
        return false;
    return    PhaseSystem::FilterPlayer::Filter( player , shape );
}

long   PhaseManager::GetPhasePoint(const CGUID& GID, const char* pName )
{
    PhaseOpen(0);

    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
         return itr->second->GetPointInfo( pName );
    }
    return 0;
}

bool    PhaseManager::SetPhaseEnd(const CGUID& GID, CPlayer* player )
{
    PhaseOpen(false);

    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        itr->second->SetPhaseStates( eEnd );
        itr->second->RunScript(player,itr->second->GetEndScriptFile());
    }

    Log4c::Trace(ROOT_MODULE,"Player %s  Execute Close Phase",player->GetName());
    return true;
}

/**
 *  阻挡判断，这里代价比较高,需要优化..主要进行相位内和相位外阻挡
 *  @Update 2010/04/10
 *  优化1：在每一个队伍增加一个相位标示，只要队伍存在，相位就存在，减少下面2层判断。
 *  优化2: 客户端处于阻挡
 */
bool    PhaseManager::IsPhaseBlock(CMoveShape* shape,long xNew,long yNew)
{
    PhaseOpen(false);

    if( !shape )    return false;

    /// 如果怪物处于非相位中，则不进行阻挡
    if( shape->GetType() == TYPE_MONSTER && shape->GetPhaseID() == NULL_GUID )
        return false;

    if( shape->GetPhaseID() != NULL_GUID )
    {
        /// 范围内阻挡
        PIter itr = m_PhaseList.find( shape->GetPhaseID() );
        if ( itr!= m_PhaseList.end() )
            return itr->second->IsPhaseBlock(shape,xNew,yNew);   
    }
    else 
    {
        /// 范围外阻挡 首先存在队伍
        /// 对于范围2的进行阻挡
        CPlayer* player = static_cast<CPlayer*>(shape);        
        if( player!= NULL && player->GetTeamID() != NULL_GUID )
        {
            const CGUID& team_id = player->GetTeamID();
            GSTeam *team = GetOrganiCtrl()->GetGameTeamByID( team_id );

            /// 如果队伍相位ID不为空,且玩家相位为空
            if( team !=NULL && team->GetPhaseID() != NULL_GUID)
            {
                PIter itr = m_PhaseList.find( team->GetPhaseID() );
                if( itr != m_PhaseList.end() )
                    return itr->second->IsPhaseBlock(shape,xNew,yNew,1);
            }
        }
    }
    return false;
}

void  PhaseManager::SetPhaseEndScriptName(const CGUID& GID,const char* FileName)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        itr->second->SetEndScriptFile( FileName );
    }
}

void  PhaseManager::ClosePhase(const CGUID& GID)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        Release( GID );
    }
}

long  PhaseManager::GetCostTime(const CGUID& GID)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        return itr->second->GetCostTime();
    }
    return 0;
}

void  PhaseManager::CancelPhaseTimer(const CGUID& GID)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr!= m_PhaseList.end() )
    {
        itr->second->CancelTimer();
    }
}

ePhaseType  PhaseManager::GetPhaseType(const CGUID& GID)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr != m_PhaseList.end() )
    {
        return itr->second->GetPhaseType();
    }
    return eInValid;    
}

void  PhaseManager::PostPhaseMsg(const CGUID& GID,const char* pMsg,ulong dwColor, ulong dwBkColor,long ePos)
{
    PIter itr = m_PhaseList.find( GID );
    if ( itr != m_PhaseList.end() )
    {
        itr->second->PostSelfMsg( pMsg ,dwColor,dwColor,ePos);
    }
}
     
bool  PhaseManager::AddPhaseInviteMapTable(const CGUID& first,const CGUID& second)
{
    /// 针对竞技的创建规则，不明确，没有注明
    /// 传进来的first,second 应该都是队伍leader或者不存在队伍
    PhaseOpen( false );

    CPlayer* fplayer = GetGame()->FindPlayer( first );
    CPlayer* splayer = GetGame()->FindPlayer( second);

    if ( !fplayer || !splayer ) return false;

    if ( fplayer->IsDied() || splayer->IsDied() ) return false;
   
    if (  fplayer->GetPhaseID() != NULL_GUID  || splayer->GetPhaseID() !=NULL_GUID )
    {
        Log4c::Error(ROOT_MODULE,"%-15s first Player name %s ReCreatePhase or second player name %s with in Phase",
            __FUNCTION__,fplayer->GetName() , splayer->GetName() );
        return false;
    }

    const CGUID& team_fid = fplayer->GetTeamID();
    const CGUID& team_sid = splayer->GetTeamID();
    if(team_fid != NULL_GUID &&  team_fid == team_sid )
    {
        Log4c::Error(ROOT_MODULE,"%-15s first Player name %s second Player name %s is the Same team",
            __FUNCTION__,fplayer->GetName() , splayer->GetName());
        return false;
    }

    GSTeam *teamf = GetOrganiCtrl()->GetGameTeamByID( team_fid );
    GSTeam *teams = GetOrganiCtrl()->GetGameTeamByID( team_sid );

    /// 对于2个队伍如果不是单独的一个人  而且 又不是队长不能创建相位
    if( (team_fid != NULL_GUID && (teamf == NULL || teamf->GetLeader()!= fplayer->GetExID()) )  ||  
        (team_sid != NULL_GUID && (teams == NULL || teams->GetLeader()!= splayer->GetExID()))  ||
        fplayer->GetFather() != splayer->GetFather() )
    {
        Log4c::Error(ROOT_MODULE,"%-15s first Player name %s second Player name %s ,触发条件不满足",
            __FUNCTION__, fplayer->GetName() , splayer->GetName() );
        return false;
    }
    AppItr Aitr = m_InviteList.find( first );
    if( Aitr != m_InviteList.end() )
    {
        /// 你已经发出相位邀请，请不要重复发送
        if ( Aitr->second.second )
        {
            CMessage msg( MSG_S2C_PHASE_OPER );
            msg.Add( (long) S2C_ReInvite );
            msg.SendToPlayer( first );
            return false;
        }
        else
        {
            m_InviteList.erase( Aitr );
        }
    }
    /** 
     *   关于2个玩家之间距离判断,太远通知邀请者S2C_TooFar
     */
    if ( Distance(fplayer->GetPosX(),fplayer->GetPosY(),splayer->GetPosX(),splayer->GetPosY()) >= 10 )
    {
        CMessage msg( MSG_S2C_PHASE_OPER );
        msg.Add( (long) S2C_TooFar );
        msg.SendToPlayer( first );
        return false;
    }

    return true;
}

void  PhaseManager::ClearTimeOut()
{
    
}
