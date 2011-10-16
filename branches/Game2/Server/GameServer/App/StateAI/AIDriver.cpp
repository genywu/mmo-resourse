///
/// @file AIDriver.cpp
/// @author Kevin Lynx
/// @brief Wrap the facility to drive the AI.
///
#include "stdafx.h"
#include "AIDriver.h"
#include "BaseAI.h"
#include "../MoveShape.h"

namespace StateAI
{
	bool AIDriver::Start( ulong trigerTime, long type, ulong interTime )
	{
		if( IsRunning( type ) )
		{
			return false;
		}
		tagTimerVar *timerVar = OBJ_CREATE( tagTimerVar );
		timerVar->TimerType = ChildObjType_AI;
		timerVar->lvar = type;
		m_TimerTable[type] = GameManager::GetInstance()->Schedule( m_Owner, timerVar, 
            timeGetTime() + trigerTime, interTime );

		return true;
	}

	void AIDriver::Stop( long type )
	{
		if( IsRunning( type ) )
		{
            long &timerID = m_TimerTable[type];
			GameManager::GetInstance()->Cancel( timerID );
            timerID = 0;
		}
	}

    void AIDriver::StopAll()
    {
        for( TimerTableT::iterator it = m_TimerTable.begin(); 
            it != m_TimerTable.end(); ++ it )
        {
            long &timerID = it->second;
            if( timerID > 0 )
            {
                GameManager::GetInstance()->Cancel( timerID );
                timerID = 0;
            }
        }
    }

    void AIDriver::Release()
    {
        StopAll();
        m_TimerTable.clear();
    }

	bool AIDriver::IsRunning( long type ) 
	{
		return m_TimerTable[type] > 0;
	}

	void AIDriver::OnTimeOut( long type )
	{
        m_TimerTable[type] = 0;
	}
}
