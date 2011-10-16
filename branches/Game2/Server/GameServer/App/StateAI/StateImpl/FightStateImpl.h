///
/// @file FightStateImpl.h
///
#ifndef ___FIGHT_STATE_IMPL_H_
#define ___FIGHT_STATE_IMPL_H_

#include "../AIStateDefs.h"

class CMonster;

namespace StateAI
{
	class SOFightNormal;

    class FightStateBase : public AIState
    {
    public:
        FightStateBase( MachineType *machine ) : AIState( machine, ST_FIGHT )
        {
        }

    protected:
        bool SearchEnemy( BaseType::EntityType *entity );

        bool CheckCycleState( BaseType::EntityType *entity );
    };

	class FightStateNormal : public FightStateBase
	{
		struct SkillInfo
		{
			long id;
			long level;
			long index;
			long type;
		};
	public:
		FightStateNormal( MachineType *machine ) ;//: AIState( machine, ST_FIGHT )

		~FightStateNormal();

		void Enter( BaseType::EntityType *entity, const AITransition &tran );

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( FightStateNormal, AIState );
	private:
        bool CheckSkill( CMonster *monster, int id, int lvl, int type, long dis );

		bool SearchSkill( CMonster *monster, long dis );

        bool SearchSkill( int type, CMonster *monster, long dis );

		void Attack( BaseType::EntityType *entity, CMoveShape *target );

		void ClearSelSkill();

		void OnSkillUsed( CMonster *monster );
	private:
		SOFightNormal *m_FightShareObj;
		SkillInfo m_SelSkill;
        ulong m_NormalSkillTimeStamp;
	};
}

#endif
