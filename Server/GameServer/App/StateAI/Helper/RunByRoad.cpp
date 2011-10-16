///
/// @file RunByRoad.cpp
/// @brief 提供按路点移动的实现
///
#include "stdafx.h"
#include "RunByRoad.h"
#include "../MonsterAI.h"
#include "../../MoveShape.h"

namespace StateAI
{
    RunByRoad::RunByRoad( MonsterAI *ai )
    {
        m_AI = ai;
        m_Owner = ai->GetOwner();
        Reset();
    }

    void RunByRoad::Reset()
    {
        m_Road.clear();
        m_Direct = -1;
        m_Index = 0;
        m_LeaveRoadPos.x = m_LeaveRoadPos.y = 0;
    }

    void RunByRoad::SetRoad( const RoadType &road )
    {
        m_Road = road;
    }

    void RunByRoad::OnLeaveRoad()
    {
        m_LeaveRoadPos.x = m_Owner->GetTileX();
        m_LeaveRoadPos.y = m_Owner->GetTileY();
    }

    bool RunByRoad::CheckRun()
    {
        if( m_Road.size() == 0 )
        {
            return false;
        }

        if( m_Road.size() == 1 )
        {
            m_AI->Stand( OPER_FAILED_TIME );
            return true;
        }

        long destX, destY = 0;
        GetNextPoint( destX, destY );
        // 向目标点移动
        if( !m_AI->Move( destX, destY ) )
        {
            return false;
        }

        return true;
    }

    void RunByRoad::GetNextPoint( long &x, long &y )
    {
        int destIndex = m_Index ;
        assert( destIndex >= 0 && destIndex < (int)m_Road.size() );
        POINT &dest = m_Road[destIndex];

        long curX = m_Owner->GetTileX();
        long curY = m_Owner->GetTileY();

        // 已经到达目标点（配置点），使用下一个配置点作为目标点
        if( dest.x == curX && dest.y == curY )
        {
            // 首尾点需要考虑路径是否是环形
            if( destIndex == 0 || destIndex == (int)m_Road.size() - 1 )
            {
                POINT &start = m_Road[0];
                POINT &end = m_Road[m_Road.size() - 1];
                // 环形：绕环移动
                if( start.x == end.x && start.y == end.y )
                {
                    // if destIndex = 0 then destIndex = size() - 2 else destIndex = 1
                    destIndex = ( destIndex == 0 ? (int)m_Road.size() - 2 : 1 );
                }
                // 非环形：反向移动
                else
                {
                    m_Direct = -m_Direct;
                    destIndex += m_Direct;
                }
            }
            // 按之前方向取下一个目标点
            else
            {
                destIndex += m_Direct;
            }
        }

        m_Index = destIndex;
        x = m_Road[destIndex].x;
        y = m_Road[destIndex].y;
    }
}
