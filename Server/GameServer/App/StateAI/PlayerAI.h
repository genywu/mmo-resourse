///
/// @file PlayerAI.h
/// @author Kevin Lynx
///
#ifndef ___PLAYERAI_H_
#define ___PLAYERAI_H_

#include "BaseAI.h"

class CPlayer;
class CMoveShape;

namespace StateAI
{
	class PlayerAI : public BaseAI
	{
	public:
		PlayerAI( CPlayer *owner );

		void Attack( long actID, long x, long y, CMoveShape *target );

		void SetReturnPeaceTimer( bool useSkill );

		bool IsInSkillActTime() const
		{
			return m_InSkillActTime;
		}

		void SetInSkillActTime( bool v )
		{
			m_InSkillActTime = v;
		}
	private:
        bool BeginSkill( long x, long y, CMoveShape *target );

		virtual void OnEvent( const AIEvent &event );
	private:
		bool m_InSkillActTime;
	};
}

#endif
