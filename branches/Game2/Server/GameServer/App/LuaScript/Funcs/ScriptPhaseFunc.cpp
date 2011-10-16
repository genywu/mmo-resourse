#include "StdAfx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../Monster.h"
#include "../../RgnManager.h"	
#include "../../PhaseSystem/PhaseManager.h"
#include "../../../../../Public/Common/PhaseDef.h"

bool  Script::CreatePhase(float size1 , float size2, long trigger , const char* scriptfile )
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() != NULL_GUID)
    {
        //PutoutLog(PHASE_LOG_FILE,LT_ERROR,"Script Error : name %s Player is Nulll or  ReCreatePhase ", player->GetName() );
        return false;
    }
    PHASEMAN.CreatePhase(player,size1,size2,(eTrigger)trigger,scriptfile);
    return true;
}

void  Script::ActivePhase(long etype,long lTimer,long lExpipires)
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return ;
    }
    PHASEMAN.ActivePhase(player->GetPhaseID(),(eTimerType)etype,lTimer, lExpipires);
}

long  Script::GetPhasePoint(const char* pName)
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return 0;
    }
    return PHASEMAN.GetPhasePoint(player->GetPhaseID() , pName );
}

long  Script::GetPhasePoint(const CGUID& id , const char* pName)
{
   return PHASEMAN.GetPhasePoint(id , pName );
}

void  Script::CreatePhaseMonster(const char* OrgName,int Amount,int x_point,
                                 int y_point,int Width,int Height,int rgnId,
                                 const char* scriptFileName,const char* talkFileName,
                                 int dir)
{
    if( !OrgName )   return ;
    if( Amount <= 0) return ;
    CServerRegion* region = NULL;
    if( rgnId != -1) 
        region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID( RGN_NORMAL , rgnId );
    else
        region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
    if ( m_RunEnv.goodsId == CGUID::GUID_INVALID )     return;
    if ( region != NULL )
    {
        long x , y;
        for ( int i = 0 ; i < Amount ; i++ )
        {
            region->GetRandomPosInRange( x , y , x_point , y_point , Width , Height );
            CMonster* monster = region->AddMonster(OrgName,x,y,dir);
            if (monster != NULL )
            {
                if ( scriptFileName) monster->SetScriptFile( scriptFileName );
                if ( talkFileName )  monster->SetTalkScriptFile( talkFileName );
                PHASEMAN.OnPhaseOper(m_RunEnv.goodsId,monster,eAddToPhase);
            }
        }
    }
}

long   Script::OnPhaseOper(long Oper)
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return 0;
    }
    return PHASEMAN.OnPhaseOper( player->GetPhaseID() , player , (ePhaseOper)Oper );
}

long   Script::OnPhaseEvent(long Event)
{
    CMoveShape* shape = dynamic_cast<CMoveShape*>(m_RunEnv.srcShape);
    if ( shape == NULL || shape->GetPhaseID() == NULL_GUID)
    {
        return 0;
    }
    return PHASEMAN.OnPhaseEvent( shape->GetPhaseID() , shape ,(ePhaseEvent)Event );
}

bool   Script::SetPhaseEnd()
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return false;
    }
    PHASEMAN.SetPhaseEnd( player->GetPhaseID() , player );
    return false;
}

void  Script::SetPhaseEndScriptFile(const char* filename)
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return ;
    }
    PHASEMAN.SetPhaseEndScriptName( player->GetPhaseID() , filename );
}

void  Script::ClosePhase(const CGUID&  PhaseId)
{
    PHASEMAN.ClosePhase( PhaseId );
}

void  Script::ClosePhase()
{
    CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if ( player == NULL || player->GetPhaseID() == NULL_GUID)
    {
        return ;
    }
    PHASEMAN.ClosePhase( player->GetPhaseID() );
}

const CGUID& Script::GetPhaseID()
{
    if ( m_RunEnv.goodsId == CGUID::GUID_INVALID )
    {
        return NULL_GUID;
    }
    return m_RunEnv.goodsId;
}

const CGUID& Script::GetShapePhaseID()
{
    CMoveShape* shape = dynamic_cast<CMoveShape*>(m_RunEnv.srcShape);
    if ( shape == NULL )
    {
        return NULL_GUID;
    }
    return  shape->GetPhaseID();
}

long  Script::GetCostTime(const CGUID& guid)
{
    return PHASEMAN.GetCostTime( guid );
}
    
void  Script::CancelPhaseTimer(const CGUID& guid)
{
    PHASEMAN.CancelPhaseTimer(guid);
}

void  Script::PostPhaseMsg(const CGUID& PhaseId,const char* pMsg,ulong dwColor,ulong dwBkColor,long ePos)
{
    PHASEMAN.PostPhaseMsg(PhaseId,pMsg,dwColor,dwColor,ePos);
}