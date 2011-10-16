///
///
///
#include "stdafx.h"
#include "Mover.h"
#include "MoveShape.h"
#include "../Utility/FindPath.h"


//浮点移动功能----------------------------------
CFloatMove::CFloatMove():bIsBeelineMove(true),bIsPreviousMoveByA_Star(false),
MINIMUS_OP_DISTANCE(0.5f),
MAX_LINE_DISTANCE(10.0f)
{
    
}

CFloatMove::~CFloatMove()
{

}

//直线移动时超过最大步长的拆分处理
void CFloatMove::UpdatelinearMoveList(const float fPosX,const float fPosY,
                          const float fDestX, const float fDestY)
{
    if (fDestX == 0 && fDestY == 0)
        return;
    //清除上一次路径点
    LineMoveList.clear();
    float lDistance = Distance(fPosX,fPosY,fDestX,fDestY);
    if(lDistance < MAX_LINE_DISTANCE)
        return;
    const float sinLine = (fDestY-fPosY) / lDistance;
    const float cosLine = (fDestX-fPosX) / lDistance;
    int i = 1;
    tagMovPos PrePos;
    PrePos.x = fPosX;
    PrePos.y = fPosY;
    while(lDistance >= MAX_LINE_DISTANCE )
    {
        tagMovPos newPos;
        newPos.x = fPosX + i* (MAX_LINE_DISTANCE-0.5f) * cosLine;
        newPos.y = fPosY + i* (MAX_LINE_DISTANCE-0.5f) * sinLine;
        lDistance = Distance(PrePos.x,PrePos.y,newPos.x,newPos.y);
        PrePos.x = newPos.x;
        PrePos.y = newPos.y;
        LineMoveList.push_back(newPos);
        
        i++;
        lDistance = Distance(newPos.x,newPos.y,fDestX,fDestY);
    }
}

//转换A*点路劲为 转向线段路径
void CFloatMove::UpdateA_MoveList(CFindPath *pFindPath,const float fPosX,
                                  const float fPosY,const float fDestX,
                                  const float fDestY)
{
    A_MoveList.clear();
    list<CFindPath::stPos> *pPathlist = pFindPath->GetPathList();
    if(pPathlist->empty())
    {
        if(abs(fPosX - fDestX) < MINIMUS_OP_DISTANCE &&
            abs(fPosY - fDestY) < MINIMUS_OP_DISTANCE)
            return;
        else
        {
            //一个格子内的移动
            if (abs(fPosX - fDestX) >= MINIMUS_OP_DISTANCE ||
                abs(fPosY - fDestY) >= MINIMUS_OP_DISTANCE)
            {
                tagMovPos movePos;
                movePos.x = fDestX;
                movePos.y = fDestY;
                A_MoveList.push_back(movePos);
                bIsBeelineMove = true;
            }
            return;
        }
    }

    CFindPath::stPos PreviousPos,CurPos;
    POINT FirstPos;
    long lTileX = (long)fPosX;
    long lTileY = (long)fPosY;

    list<CFindPath::stPos>::iterator pathIter = pPathlist->begin();
    FirstPos.x = pathIter->x;
    FirstPos.y = pathIter->y;

    //起始位置玩家当前站立点
    PreviousPos.x = lTileX;
    PreviousPos.y = lTileY;
    //初始方向
    long PreviousDir = GetLineDir(lTileX, lTileY, FirstPos.x, FirstPos.y);

    for (long i = 0;pathIter != pPathlist->end();++pathIter,i++)
    {
        CurPos.x = pathIter->x;
        CurPos.y = pathIter->y;
        //如果更上一个移动格子方向不一致 则记录转向点
        long CurDir = GetLineDir(PreviousPos.x, PreviousPos.y, CurPos.x, CurPos.y);
        tagMovPos movePos;
        if (PreviousDir != CurDir)
        {    
            movePos.x = PreviousPos.x + 0.5f;
            movePos.y = PreviousPos.y + 0.5f;
            A_MoveList.push_back(movePos);//这里记录每一个拐点
            PreviousDir = CurDir; 
        }
        if(i == pPathlist->size()-1)//最后目标点
        {
            movePos.x = fDestX;
            movePos.y = fDestY;
            A_MoveList.push_back(movePos);
        }
        PreviousPos.x = CurPos.x;
        PreviousPos.y = CurPos.y;
    }
    if (A_MoveList.size()>1)
        bIsBeelineMove = false;
    else
        bIsBeelineMove = true;
}

//////////////////////////////////////////////////////////////////////////////////////
Mover::Mover( CMoveShape *owner ) : m_owner( owner )
{
    Reset();
}

Mover::~Mover()
{
    Stop();
}

void Mover::Move( float destX, float destY )
{
    if( m_arriveTimer != -1 )
    {
        PutDebugString( "Invalid move request." );
        //return;
    }

    CClientRegion *region = (CClientRegion*) m_owner->GetFather();
    CFindPath *findPath = region->GetFindPath();
    findPath->AddPathPos( region->GetResourceID(), m_owner->GetTileX(), m_owner->GetTileY(),
        (long) destX, (long) destY );
    float curX = m_owner->GetPosX();
    float curY = m_owner->GetPosY();
    if( findPath->GetPathList()->size() > 0 )
    {
        findPath->GetPathList()->pop_front();
    }

    m_floatMove.UpdateA_MoveList( findPath, curX, curY, destX, destY );
    if( !m_floatMove.A_MoveList.empty() )
    {
        CFloatMove::tagMovPos firstStep;
        firstStep.x = m_floatMove.A_MoveList.begin()->x;
        firstStep.y = m_floatMove.A_MoveList.begin()->y;
        m_floatMove.bIsPreviousMoveByA_Star = true;
        m_floatMove.UpdatelinearMoveList( curX, curY, firstStep.x, firstStep.y );
        if( !m_floatMove.LineMoveList.empty() )
        {
           firstStep.x = m_floatMove.LineMoveList.begin()->x;
           firstStep.y = m_floatMove.LineMoveList.begin()->y;
           m_floatMove.LineMoveList.pop_front();
           m_floatMove.bIsPreviousMoveByA_Star = false;
        }
        MoveTo( firstStep.x, firstStep.y );
    }
}

#define TIME_ADJUST (500)

void Mover::MoveTo( float destX, float destY )
{
    float curX = m_owner->GetPosX();
    float curY = m_owner->GetPosY();
    float dir = GetRadianDir( curX, curY, destX, destY );
    m_owner->SetDirEx( dir );
    m_owner->SetDestXY( destX, destY );
    m_owner->SetIsArrive( false );
    float dis = Distance( curX, curY, destX, destY );
    float time = dis / m_owner->GetSpeed();
    m_offsetX = ( destX - curX ) / time;
    m_offsetY = ( destY - curY ) / time;
    m_lastUpdate = timeGetTime();
    m_endTime = m_lastUpdate + (ulong) time - TIME_ADJUST;

  CMessage msg( MSG_C2S_SHAPE_MOVE );
  msg.Add( curX );
  msg.Add( curY );
  msg.Add( (uchar)0 );
  msg.Add( destX );
  msg.Add( destY );
  msg.SendByPlayer( m_owner->GetExID() );

    // be sure the time is very correct
    assert( m_arriveTimer == -1 );
	m_arriveTimer = QueryTimer()->Schedule( m_owner, 0, m_lastUpdate + (ulong) time );
}

void Mover::OnTimeOut( ulong timerid )
{
    if( timerid == m_arriveTimer )
    {
        ulong elapsed = timeGetTime() - m_endTime;
        if( elapsed > TIME_ADJUST + 100 )
        {
            char log[256];
            sprintf( log, "Elapsed time: %u.", elapsed );
            PutDebugString( log );
        }

        OnArrived();
    }
}

void Mover::Stop()
{
    if( m_arriveTimer != -1 )
    {
        QueryTimer()->Cancel( m_arriveTimer );
        m_arriveTimer = -1;
    }
    m_endTime = timeGetTime();
    m_finished = true;
    m_floatMove.A_MoveList.clear();
    m_floatMove.LineMoveList.clear();
    m_owner->SetIsArrive( true );
}

void Mover::OnStop()
{
    Stop();

     CMessage msg( MSG_C2S_SHAPE_MOVE );
     msg.Add( m_owner->GetPosX() );
     msg.Add( m_owner->GetPosY() );
     msg.Add( (uchar)1 );
     msg.SendByPlayer( m_owner->GetExID() );
}

bool Mover::Update()
{
    if( m_finished && m_arriveTimer == -1 ) return true;
    ulong curTime = timeGetTime();
    m_lastUpdate = curTime;
    if( curTime >= m_endTime )
    {
        m_owner->SetPosXY( m_owner->GetDestX(), m_owner->GetDestY() );
        m_owner->SetIsArrive( true );
    }
    else
    {
        ulong elapsed = curTime - m_lastUpdate;
        float x = m_owner->GetPosX() + elapsed * m_offsetX;
        float y = m_owner->GetPosY() + elapsed * m_offsetY;
        m_owner->SetPosXY( x, y );
    }
    return false;
}

void Mover::OnArrived()
{
    m_owner->SetPosXY( m_owner->GetDestX(), m_owner->GetDestY() );
    m_owner->SetIsArrive( true );
    CFloatMove::tagMovPos CurDestPoint;
    if( !m_floatMove.LineMoveList.empty() )
    {
        CurDestPoint.x = m_floatMove.LineMoveList.begin()->x;
        CurDestPoint.y = m_floatMove.LineMoveList.begin()->y;

        m_floatMove.LineMoveList.pop_front();
        m_floatMove.bIsPreviousMoveByA_Star = false;

        MoveTo( CurDestPoint.x, CurDestPoint.y );    
    }
    else
    {
        if( m_floatMove.bIsPreviousMoveByA_Star &&
            m_floatMove.A_MoveList.size() > 0 )
            m_floatMove.A_MoveList.pop_front();
        if( !m_floatMove.A_MoveList.empty() )
        {
            CurDestPoint.x = m_floatMove.A_MoveList.begin()->x;
            CurDestPoint.y = m_floatMove.A_MoveList.begin()->y;
            m_floatMove.bIsPreviousMoveByA_Star = true;
            m_floatMove.UpdatelinearMoveList( m_owner->GetPosX(), m_owner->GetPosY(), 
                CurDestPoint.x, CurDestPoint.y );

            if( !m_floatMove.LineMoveList.empty() )
            {
                CurDestPoint.x = m_floatMove.LineMoveList.begin()->x;
                CurDestPoint.y = m_floatMove.LineMoveList.begin()->y;
                m_floatMove.LineMoveList.pop_front();
                m_floatMove.bIsPreviousMoveByA_Star = false;
            }
            MoveTo( CurDestPoint.x, CurDestPoint.y );    
        }
        else
        {
            OnStop();
        }
    }
}

void Mover::Reset()
{
    m_offsetX = m_offsetY = 0.0f;
    m_lastUpdate = 0;
    m_endTime = 0;
    m_arriveTimer = -1;
    m_finished = false;
}


