///
/// @file CycleStateImpl.h
///
#ifndef ___CYCLE_STATE_IMPL_H_
#define ___CYCLE_STATE_IMPL_H_

#include "../StateImplDef.h"
#include "../../AIStateDefs.h"
#include "../../EventImpl/CycleEvent.h"

namespace StateAI
{
	class CycleStateBase : public AIState
	{
	public:
		CycleStateBase( MachineType *machine ) : AIState( machine, ST_CYCLE )
		{
		}

		virtual bool ExecuteWithReturn( BaseType::EntityType *entity ) = 0;

		void Execute( BaseType::EntityType *entity ) { }
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	class MoveToCellCycle : public CycleStateBase
	{
	public:
		MoveToCellCycle( MachineType *machine ) : CycleStateBase( machine )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		bool ExecuteWithReturn( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( MoveToCellCycle, AIState );
    private:
        CEMoveToCell m_Data;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	class MoveByStepCycle : public CycleStateBase
	{
	public:
		MoveByStepCycle( MachineType *machine ) : CycleStateBase( machine )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		bool ExecuteWithReturn( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( MoveByStepCycle, AIState );
	private:
		CEMoveByStep m_Data;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	class FollowTargetCycle : public CycleStateBase
	{
	public:
		FollowTargetCycle( MachineType *machine ) : CycleStateBase( machine )
		{
		}

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		bool ExecuteWithReturn( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( FollowTargetCycle, AIState );
	private:
		CEFollowTarget m_Data;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	class CycleState : public CycleStateBase
	{
	public:
		CycleState( MachineType *machine ) ;

		~CycleState();

		void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

		bool ExecuteWithReturn( BaseType::EntityType *entity );

		DECL_LIFE_POLICY_FUNC( CycleState, AIState );

	private:

		void SetState( long state );

		bool HasState() const
		{
			return m_CurState != CSI_INVALID;
		}

	private:
		CycleStateBase *m_States[CSI_SUB_STATE_COUNT];
		long m_CurState;
	};

	/// Execute a cycle state, not necessary actually.
	class CycleStateExecutor
	{
	public:
		bool Execute( AIState *state, AIState::EntityType *entity );
	};
}

#endif
