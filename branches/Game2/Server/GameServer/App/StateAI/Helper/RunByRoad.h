///
/// @file RunByRoad.h
/// @brief 提供按路点移动的实现
///
#ifndef ___RUN_BY_ROAD_H_
#define ___RUN_BY_ROAD_H_

#include <vector>

class CMoveShape;
namespace StateAI
{
    class MonsterAI;

    class RunByRoad
    {
    public:
        typedef std::vector<POINT> RoadType;

    public:
        RunByRoad( MonsterAI *ai );

        void Reset();

        bool CheckRun();

        void OnLeaveRoad();

        void SetRoad( const RoadType &road );

    private:
        void GetNextPoint( long &x, long &y );
    public:
        MonsterAI *m_AI;
        CMoveShape *m_Owner;
        RoadType m_Road;
        /// 1, -1
        int m_Direct; 
        /// point index in the road
        int m_Index;
        POINT m_LeaveRoadPos;
    };
}

#endif
