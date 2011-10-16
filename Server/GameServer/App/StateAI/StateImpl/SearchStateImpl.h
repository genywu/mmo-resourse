///
/// @file SearchStateImpl.h
///
#ifndef ___SEARCH_STATE_IMPL_H_
#define ___SEARCH_STATE_IMPL_H_

#include <list>
#include "../AIStateDefs.h"
#include "../GlobalDef.h"

namespace StateAI
{
	class SearchStateNormal : public AIState
	{
	public:
		SearchStateNormal( MachineType *machine ) : AIState( machine, ST_SEARCH )
		{
		}

		void Execute( BaseType::EntityType *entity );

        /// Notify targets found in the search range.
        virtual void OnSearched( BaseType::EntityType *entity, const CGUID &id, long type ) { }
	};

	class DHFSearchState : public SearchStateNormal
	{
	public:
		typedef std::list<CGUID> AttackerListT;
	public:
		DHFSearchState( MachineType *machine ) : SearchStateNormal( machine )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		void Execute( BaseType::EntityType *entity );

        void Reset();
	protected:
		AttackerListT m_Attackers;	
	};

	class DHFPassiveGladiatorState : public DHFSearchState
	{
	public:
		DHFPassiveGladiatorState( MachineType *machine ) : DHFSearchState( machine )
		{
		}

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( DHFPassiveGladiatorState, AIState );
	};

	class DHFGladiatorState : public DHFSearchState
	{
	public:
		DHFGladiatorState( MachineType *machine ) : DHFSearchState( machine )
		{
		}

		void Execute( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( DHFGladiatorState, AIState );
	};

    class TauntPassiveGladiatorState : public SearchStateNormal
    {
    public:
        TauntPassiveGladiatorState( MachineType *machine ) : SearchStateNormal( machine )
        {
        }

        void Execute( BaseType::EntityType *entity );

        DECL_LIFE_POLICY_FUNC( TauntPassiveGladiatorState, AIState );
    };

    class TauntGladiatorState : public TauntPassiveGladiatorState
    {
    public:
        TauntGladiatorState( MachineType *machine ) : TauntPassiveGladiatorState( machine )
        {
        }

        void Execute( BaseType::EntityType *entity );

        void OnSearched( BaseType::EntityType *entity, const CGUID &id, long type );

        DECL_LIFE_POLICY_FUNC( TauntGladiatorState, AIState );
    };
}

#endif
