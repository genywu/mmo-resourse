///
/// @file OpPosEx.cpp
/// @author Kevin Lynx
/// @brief Position operator
///
#include "stdafx.h"
#include "OpPosEx.h"
#include "../../../../Public/Module/GraphUtils.h"

#define CHECK_RETURN( s, v ) if( s == #v ) return v

OpPositionEx::OpPositionEx( stModuParam *param ) : 
    COperator( param )
{
}

OpPositionEx::OpPositionEx( void *p ) : 
    COperator( (stModuParam*) p )
{
}

OpPositionEx::~OpPositionEx()
{
}

void OpPositionEx::InitBaseData( eObj obj, eObj refObj, const std::string &dir,
        const std::string &pos, const std::string &speed, float dis )
{
    m_Obj = obj;
    m_refObj = refObj;
    m_dirType = GetDirType( dir );
    m_posType = GetPosType( pos );
    m_speed = (float) atof( speed.c_str() );
    m_dis = dis;
}

void OpPositionEx::Execute( CMoveShape *shape )
{
    float refX = 0.0f, refY = 0.0f;

    if( m_refObj == Obj_Mouse )
    {
        refX = (float) m_pParam->nMouseX;
        refY = (float) m_pParam->nMouseY;
    }
    else if( shape )
    {
        refX = shape->GetPosX();
        refY = shape->GetPosY();
    }

    if( Graph::FloatEq( refX, 0.0f ) && Graph::FloatEq( refY, 0.0f ) )
    {
        return;
    }

    if( m_Obj == Obj_Self )
    {
        shape = m_pParam->pUser;
    }

    float dir = GetMoveDir( m_pParam->pUser->GetDirEx() );
    float dis = m_dis;
    // adjust distance
    if( shape != m_pParam->pUser && m_dirType == DIR_FRONT && m_dis < 0 )
    {
        dis = shape->RealDistanceF( m_pParam->pUser );
    }
    Graph::Point destPos = Graph::GetPosOnLine( shape->GetPosX(), shape->GetPosY(),
        dir, dis );

    if( !shape->CanMove() )
    {
        return;
    }
    if( m_posType == POS_SIMPLE )
    {
        shape->SetPosition( destPos.x, destPos.y );
    }
    else if( m_posType == POS_SLIP )
    {
        shape->SetSlipPos( destPos.x, destPos.y, m_speed, m_pParam->pUser->GetDirEx() );
    }
}

int OpPositionEx::GetDirType( const std::string &dir )
{
    CHECK_RETURN( dir, DIR_FRONT );
    CHECK_RETURN( dir, DIR_BACK );
    CHECK_RETURN( dir, DIR_LEFT );
    CHECK_RETURN( dir, DIR_RIGHT );
    return DIR_FRONT;
}

int OpPositionEx::GetPosType( const std::string &pos )
{
    CHECK_RETURN( pos, POS_SIMPLE );
    CHECK_RETURN( pos, POS_SLIP );
    return POS_SIMPLE;
}

float OpPositionEx::GetMoveDir( float faceDir )
{
    if( m_dirType == DIR_BACK )
    {
        return faceDir + Graph::PI;
    }
    if( m_dirType == DIR_LEFT )
    {
        return faceDir - Graph::PI / 2;
    }
    if( m_dirType == DIR_RIGHT )
    {
        return faceDir + Graph::PI / 2;
    }
    return faceDir;
}
