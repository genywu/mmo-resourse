///
/// @file Mover.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "Mover.h"
#include "MoverInternal.h"
#include "MoveShape.h"
#include "ServerRegion.h"

Mover::Mover()
{
    m_owner = NULL;
}

Mover::~Mover()
{
    Release();
}

void Mover::Init( CMoveShape *owner )
{
    m_owner = owner;
    m_offsetX = m_offsetY = 0.0f;
    m_updateCnt = 0;
    m_destX = m_destY = 0.0f;
    m_timerID = INVALID_TIMER;
    m_msgState = WAIT_MOVE;
    ResetChkData();
#ifdef MOVER_TEST
    m_logFile[0] = '\0';
#endif
}

void Mover::Release()
{
    Stop();
}

bool Mover::IsMoving() const
{
    return m_timerID != INVALID_TIMER;
}

void Mover::OnTimeOut()
{
    if( m_updateCnt-- > 0 ) return;

    float curX = m_owner->GetPosX();
    float curY = m_owner->GetPosY();
    float x = curX + m_offsetX;
    float y = curY + m_offsetY;

    if( IsDestPos( x, y ) )
    {
        x = m_destX;
        y = m_destY;
        Log4c::Trace(ROOT_MODULE, "Arrive dest pos (%f, %f).", x, y );
        Stop();
        OnArrive();
    }

    while( IsBlock( x, y ) && !IsDestPos( x, y ) )
    {
        m_updateCnt ++;
        x += m_offsetX;
        y += m_offsetY;
    }
    Log4c::Warn(ROOT_MODULE,"Skip %d update frame.", m_updateCnt );

    m_owner->SetPosXY( x, y );

    Log4c::Trace(ROOT_MODULE, "(%f, %f) -> (%f, %f).", curX, curY, x, y );
}

void Mover::OnMove( float clientX, float clientY, float destX, float destY )
{
#ifdef MOVER_TEST
    if( m_logFile[0] == '\0' )
    {
        sprintf( m_logFile, "MoveTest_%s", m_owner->GetName() );
        Log4c::Trace(ROOT_MODULE, "Initial" );
    }
#endif
   Log4c::Trace(ROOT_MODULE, "Received move request (%f, %f) -> (%f, %f).",
        clientX, clientY, destX, destY );
    if( FLOATEQ(clientX, destX) && FLOATEQ(clientY, destY) )
    {
        OnFailed();
        return;
    }
    if( !m_owner->CanMove() )
    {
        OnFailed();
        return;
    }
    if( !CheckBlock( clientX, clientY, destX, destY ) )
    {
        Log4c::Warn(ROOT_MODULE, "Invalid path" );
        OnFailed();
        return;
    }
    if( IsFar( clientX, clientY ) )
    {
        Log4c::Warn(ROOT_MODULE, "Too far." );
        OnFailed();
        return;
    }

    float reqDis = Distance( clientX, clientY, destX, destY );
    if( reqDis > REQ_MAXDIS )
    {
        Log4c::Warn(ROOT_MODULE, "Request distance %f is too long.", reqDis );
        OnFailed();
        return;
    }

    m_msgState = WAIT_STOP;
    
    AddErrRecord( clientX, clientY );

    if( m_requestCnt > REQ_MAX &&
        Check() )
    {
        Log4c::Warn(ROOT_MODULE, "Check failed." );
        return;
    }
    OnMoveNoCheck( clientX, clientY, destX, destY );
}

void Mover::OnMoveNoCheck( float clientX, float clientY, float destX, float destY )
{
    m_destX = destX;
    m_destY = destY;

#ifdef MOVER_TEST // special for PET
    if( m_logFile[0] == '\0' && m_owner->GetType() == TYPE_PET )
    {
        sprintf( m_logFile, "MoveTest_Pet_%s", m_owner->GetName() );
        Log4c::Trace(ROOT_MODULE,"Initial" );
    }
#endif
   Log4c::Trace(ROOT_MODULE, "Sync position (%f, %f) -> (%f, %f).", 
        m_owner->GetPosX(), m_owner->GetPosY(), clientX, clientY );
    m_owner->SetPosXY( clientX, clientY );
    float dir = GetRadianDir( clientX, clientY, destX, destY );
    m_owner->SetDirEx( dir );
    m_owner->OnMove( destX, destY );

    Start();
    OnStart();
}

void Mover::OnStopMove( float clientX, float clientY )
{
   Log4c::Trace(ROOT_MODULE,"Received stop moving request (%f, %f).",clientX, clientY );
    if( m_msgState == WAIT_MOVE ) 
    {
        // invalid msg sequence
        Log4c::Warn(ROOT_MODULE, "Invalid msg sequence (%f, %f).",clientX, clientY );
        return;
    }
    m_msgState = WAIT_MOVE;
    if( IsMoving() )
    {
        Stop();
    }

    if( IsFar( clientX, clientY ) ||
        IsBlock( clientX, clientY ) )
    {
       Log4c::Warn(ROOT_MODULE, "Too far or block." );
        OnFailed();
        return;
    }

    AddErrRecord( clientX, clientY );

    if( m_requestCnt > REQ_MAX &&
        Check() )
    {
        Log4c::Warn(ROOT_MODULE, "Check failed." );
        return;
    }
    SetChkStart( 0.0f, 0.0f );
    OnStopMoveNoCheck( clientX, clientY );
}

void Mover::OnSetPos( float x, float y )
{
    m_msgState = WAIT_MOVE;
    if( IsMoving() )
    {
        Stop();
    }
    SetChkStart( 0.0f, 0.0f );
}

void Mover::OnStopMoveNoCheck( float clientX, float clientY )
{
   Log4c::Trace(ROOT_MODULE, "Sync position (%f, %f) -> (%f, %f).", 
        m_owner->GetPosX(), m_owner->GetPosY(), clientX, clientY );
    m_owner->SetPosXY( clientX, clientY );
    m_owner->OnStopMove();
    OnStop();
}

bool Mover::CheckBlock( float srcX, float srcY, float destX, float destY )
{
    int offsetX = (int)destX - (int)srcX;
    int offsetY = (int)destY - (int)srcY;
    if( offsetX != 0 && abs(offsetX) == abs(offsetY) )
    {
        // A*
        Log4c::Trace(ROOT_MODULE, "A* request (%f, %f) -> (%f, %f)",
            srcX, srcY, destX, destY );
        return CheckBlockAStar( srcX, srcY, destX, destY );
    }
    else
    {
        // direct
        Log4c::Trace(ROOT_MODULE, "direct request (%f, %f) -> (%f, %f)",
            srcX, srcY, destX, destY );
        return CheckBlockDirect( srcX, srcY, destX, destY );
    }
}

bool Mover::CheckBlockAStar( float srcX, float srcY, float destX, float destY )
{
    int offsetX = (int)destX - (int)srcX;
    int offsetY = (int)destY - (int)srcY;
    int stepX = offsetX / abs(offsetX);
    int stepY = offsetY / abs(offsetY);
    int x, y;
    for( x = (int)srcX, y = (int)srcY; x != (int)destX && y != (int)destY; 
        x += stepX, y += stepY )
    {
        if( IsBlock( x, y ) ) 
        {
            Log4c::Warn(ROOT_MODULE, "(%d, %d) is block.", x, y );
            return false;
        }
    }
    return true;
}

bool Mover::CheckBlockDirect( float srcX, float srcY, float destX, float destY )
{
    long dis = Distance( (long)srcX, (long)srcY, (long)destX, (long)destY );
    typedef std::list<POINT> PointsType;
    PointsType points;
    GetLinePointToDest( points, (long)srcX, (long)srcY, (long)destX, (long)destY, dis );
    for( PointsType::iterator it = points.begin();
        it != points.end(); ++ it )
    {
        if( IsBlock( it->x, it->y ) ) 
        {
            Log4c::Warn(ROOT_MODULE, "(%d, %d) is block.", it->x, it->y );
            return false; 
        }
    }
    return true;
}

void Mover::OnFailed()
{
    Log4c::Warn(ROOT_MODULE, "Move failed." );
    m_owner->OnCannotMove();
    SetChkStart( 0.0f, 0.0f );
}

void Mover::Start()
{
    if( IsMoving() )
    {
        Stop();
    }
    float srcX = m_owner->GetPosX();
    float srcY = m_owner->GetPosY();
    float dis = Distance( srcX, srcY, m_destX, m_destY );
    // TODO
    if( FLOATEQ(dis, 0.0f) ) return ;
    assert( dis > 0.0f );
    float step = dis / DIS_PERSTEP;
    float timeInterval = DIS_PERSTEP / m_owner->GetSpeed();
    float lateTime = timeInterval / 2.0f;
    
    m_offsetX = ( m_destX - srcX ) / step;
    m_offsetY = ( m_destY - srcY ) / step;

    tagTimerVar *var = OBJ_CREATE( tagTimerVar );
    memset( var, 0, sizeof(*var) );
	var->TimerType = ChildObjType_ShapeMove;
    m_timerID = GameManager::GetInstance()->Schedule( m_owner, var, 
        timeGetTime() + (long)lateTime, (long)timeInterval );
    Log4c::Trace(ROOT_MODULE, "Start timer, lateTime=%f, interval=%f.",
        lateTime, timeInterval );
}

void Mover::Stop()
{
    GameManager::GetInstance()->Cancel( m_timerID );
    m_timerID = INVALID_TIMER;
}

bool Mover::Check()
{
    Log4c::Trace(ROOT_MODULE,"Start to check, relErrDis=%f.", m_relErrDis );
    if( m_relErrDis >= MAX_ERRRELDIS )
    {
        Log4c::Warn(ROOT_MODULE, "relErrDis>=MAX, kick player." );
        KickOut();
        return true;
    }
    if( m_relErrDis >= MIN_ERRRELDIS ) // adjust 
    {
        Log4c::Warn(ROOT_MODULE,"relErrDis>MIN, adjust position." );
        OnFailed();
        ResetChkData();
        return true;
    }
    // ignore
    ResetChkData();
    return false;
}

void Mover::ResetChkData()
{
    m_requestCnt = 0;
    m_relErrDis = 0.0f;
    SetChkStart( 0.0f, 0.0f );
}

bool Mover::IsFar( float clientX, float clientY )
{
    float offsetX = abs(m_owner->GetPosX() - clientX);
    float offsetY = abs(m_owner->GetPosY() - clientY);
    float maxOffset = max(offsetX, offsetY);
    Log4c::Trace(ROOT_MODULE, "offsetX=%f, offsetY=%f, maxOffset=%f.",
        offsetX, offsetY, maxOffset );
    return maxOffset > DIS_MAX;
}

bool Mover::IsBlock( float x, float y )
{
    CServerRegion *region = (CServerRegion*) m_owner->GetFather();
    if( region == NULL )
    {
        return true;
    }
    uchar b = region->GetBlock((int)x, (int)y);
    return b != CRegion::BLOCK_NO && b != CRegion::BLOCK_AIM;
}

bool Mover::IsDestPos( float x, float y )
{
    if( m_offsetX <= 0 && x <= m_destX &&
        m_offsetY <= 0 && y <= m_destY )
    {
        return true;
    }
    if( m_offsetX >= 0 && x >= m_destX &&
        m_offsetY <= 0 && y <= m_destY )
    {
        return true;
    }
    if( m_offsetX <= 0 && x <= m_destX &&
        m_offsetY >= 0 && y >= m_destY )
    {
        return true;
    }
    if( m_offsetX >= 0 && x >= m_destX &&
        m_offsetY >= 0 && y >= m_destY )
    {
        return true;
    }
    return false;
}

void Mover::SetChkStart( float clientX, float clientY )
{
    m_chkStartX = clientX;
    m_chkStartY = clientY;
    m_chkStartTime = timeGetTime();
}

void Mover::AddErrRecord( float clientX, float clientY )
{
    m_requestCnt++;
    if( FLOATEQ(m_chkStartX, 0.0f) && FLOATEQ(m_chkStartY, 0.0f) )
    {
        Log4c::Trace(ROOT_MODULE, "Init checking errRecord reqCnt=%d.", 
            m_requestCnt );
        SetChkStart( clientX, clientY );
        return;
    }
    float cdis = Distance( m_chkStartX, m_chkStartY, clientX, clientY );
    ulong elapsedT = timeGetTime() - m_chkStartTime;
    elapsedT = elapsedT < 0 ? 0 : elapsedT;
    float sdis = m_owner->GetSpeed() * elapsedT;
    m_relErrDis += abs( sdis - cdis );
    SetChkStart( clientX, clientY );
   Log4c::Trace(ROOT_MODULE, "reqCnt=%d, cdis=%f, sdis=%f, elapsedT=%u, relErrDis=%f.",
        m_requestCnt, cdis, sdis, elapsedT, m_relErrDis );
}
