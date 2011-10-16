///
/// @file MoverSpecific.cpp
/// @author Kevin lynx
/// @brief specific implementation for Mover
///
#include "stdafx.h"
#include "Mover.h"
#include "MoverInternal.h"
#include "Player.h"
#include "Pet.h"

typedef std::pair<float, float> Position;

static bool IsBlock( CMoveShape *shape, long x, long y )
{
    CServerRegion *region = (CServerRegion*) shape->GetFather();
    if( region == NULL )
    {
        return true;
    }
    uchar b = region->GetBlock((int)x, (int)y);
    return b != CRegion::BLOCK_NO && b != CRegion::BLOCK_AIM;
}

static Position CalLeftPos( CMoveShape *shape, float fsx, float fsy )
{
    long sx = (long) fsx;
    long sy = (long) fsy;
    long x = sx, y = sy;
    // left
    long dir = shape->GetLeftDir();
    shape->GetDirPos( dir, x, y );
    // block ?
    if( !IsBlock( shape, x, y ) )
    {
        return Position( x+0.5f, y+0.5f );
    }
    // back
    x = sx;
    y = sy;
    dir = shape->GetRearDir();
    shape->GetDirPos( dir, x, y );
    if( !IsBlock( shape, x, y ) )
    {
        return Position( x+0.5f, y+0.5f );
    }
    // center
    x = sx;
    y = sy;
    assert( !IsBlock( shape, x, y ) );
    return Position( x+0.5f, y+0.5f );
}

void Mover::KickOut()
{
    long t = m_owner->GetType();
    if( t == TYPE_PLAYER )
    {
        GetGame()->KickPlayer( m_owner->GetExID() );
    }
    // else if( t == TYPE_PET ) ignore
}

void Mover::OnStart()
{
    long t = m_owner->GetType();
    if( t == TYPE_PLAYER )
    {
        CPlayer *player = (CPlayer*) m_owner;
        CPet *pet = player->GetActivePet();
        if( !pet || pet->GetState() != CShape::STATE_PEACE ) return;
        // drive his follow dogs(Pets) to move
        Position destPos = CalLeftPos( m_owner, m_destX, m_destY );
        Position srcPos;
        Mover &petMover = pet->GetMover();
        if( FLOATEQ( petMover.m_destX, 0.0f ) && FLOATEQ( petMover.m_destY, 0.0f ) )
        {
            srcPos.first = pet->GetPosX();
            srcPos.second = pet->GetPosY();
        }
        else
        {
            srcPos.first = petMover.m_destX;
            srcPos.second = petMover.m_destY;
        }
        // start to move
        pet->GetMover().OnMoveNoCheck( srcPos.first, srcPos.second, destPos.first, destPos.second );
    }
}

void Mover::OnStop()
{
    long t = m_owner->GetType();
    if( t == TYPE_PLAYER )
    {
        // Do NOT sync
    }
}

void Mover::OnArrive()
{
    long t = m_owner->GetType();
    if( t == TYPE_PET )
    {
        m_owner->OnStopMove();
    }
}
