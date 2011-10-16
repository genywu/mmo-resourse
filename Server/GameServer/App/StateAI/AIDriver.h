///
/// @file AIDriver.h
/// @author Kevin Lynx
/// @brief Wrap the facility to drive the AI.
///
#ifndef ___AIDRIVER_H_
#define ___AIDRIVER_H_

#include <map>

class CMoveShape;
namespace StateAI
{
	/// This class mainly manage a timer for StateAI.
	class AIDriver
	{
    public:
        /// <type, timerID>
        typedef std::map<long, long> TimerTableT;
        enum Type
        {
            AI_EVENT,
            CYCLE_EVENT,
            SCRIPT_EVENT,
            RETPEACE_EVENT,
        };
	public:
		AIDriver( CMoveShape *owner )
		{
            m_Owner = owner;
		}

		bool IsRunning( long type );

		void Stop( long type );

        void StopAll();

        void Release();

		/// Register a timer.
		bool Start( ulong trigerTime, long type, ulong interTime = 0 );

		void OnTimeOut( long type );
	private:
		TimerTableT m_TimerTable;
		CMoveShape *m_Owner;
	};
}

#endif
