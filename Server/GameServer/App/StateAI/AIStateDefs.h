///
/// @file AIStateDefs.h
/// @author Kevin Lynx
/// @brief StateAI basic definitions.
///
#ifndef ___AI_STATE_DEFS_H_
#define ___AI_STATE_DEFS_H_

#include "GlobalDef.h"
#include "TypeObject.h"
#include "../../../Public/ServerPublic/Server/BaseDef/AIStateType.h"

namespace StateAI
{
	/// The AI event type.
	enum EventType
	{
		ET_WAKEUP = 0,
		ET_HURT,
		ET_KILL,
		ET_CYCLE,
		ET_USESKILLEND,
		ET_RETURN,
		ET_USESKILL,
        ET_BORN,
        ET_REBORN,
        ET_TIMERCYCLE,
        ET_INIT,
        ET_SCRIPT_USESKILL,
        ET_SCRIPT_SKILLEND,
        ET_AGONAL,
        ET_LEAVE_PHASE,
        ET_RELIVE,
		ET_INVALID
	};

	/// The basic AI event object type.
	typedef TypeObject<long> AIEvent;

	class AIState;
	class BaseAI;

	/// The AI state transition object type.
	typedef kl_common::SimpleTransition<AIState> AITransition;
	typedef AITransition SimpleTransition;

	/// typedefs used for kl_common::state.
	struct TypeWrapper
	{
		typedef BaseAI EntityType;
		typedef AIEvent EventType;
		typedef AIState ConcreteType;
		typedef AITransition TransitionType;
	};

	/// Object shared by several AI states, not implemented yet.
	typedef TypeObject<long> AIShareObject;

	/// Transition with event.
	struct EventTransition : public AITransition
	{
		EventTransition( AIState *from, AIState *to, const AIEvent &ev ) :
			AITransition( from, to ), event( ev )
		{
		}
		const AIEvent &event;
	};

	/// The state machine type.
	typedef kl_common::StateMachine<TypeWrapper> MachineType;

	/// The basic AI state class, inherited by these concrete AI states.
	/// An AI state is also an object with type, see TypeObject for details.
	class AIState : public kl_common::State<TypeWrapper>, public TypeObject<long>
	{
	public:
		typedef kl_common::State<TypeWrapper> BaseType;
	public:
		///
		/// @param type AI state type
		/// @see StateType
		///
		AIState( MachineType *machine, long type ) : TypeObject( type ), BaseType( machine )
		{
		}

		bool operator== ( const BaseType::ConcreteType &other ) const
		{
			return other.Type() == Type();
		}

		virtual void Execute( BaseType::EntityType *entity )
		{
		}

		/// Reset this AI state.
		virtual void Reset() {}

		/// A helper function to get the entity(BaseAI) object.
		BaseType::EntityType *GetAI() const
		{
			MachineType *machine = GetMachine();
			return machine->GetEntity();
		}
	};

	/// Expand a state create/destroy declaration.
#define DECL_LIFE_POLICY_FUNC( className, baseClassName ) \
	static baseClassName *Create( MachineType *machine ); \
	static void Destroy( baseClassName *p )

	/// Expand implementation for create/destroy.
#define IMPL_LIFE_POLICY_FUNC( className, baseClassName ) \
	baseClassName *className::Create( MachineType *machine ) { return new className( machine ); } \
	void className::Destroy( baseClassName *p ) { delete p; } 

#define AI_ASSERT( exp ) assert( exp )
}

#endif
