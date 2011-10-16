///
/// @file kl_state.h
/// @author Kevin Lynx
/// @date 12.21.2009
/// @brief A simple FSM(finite state machine) pattern implemention.
///
#ifndef ___KL_STATE_H_
#define ___KL_STATE_H_

namespace kl_common
{
	///
	/// Inherit this class to add more event data. Event is used to 
	/// transform states. It's not necessary to implement this class
	/// because this state machine only depends on your concrete class.
	///
	struct Event
	{
	};	

	template <typename StateWrapper>
	class State;

	///
	/// Transition between two states. Also an optional util for you.
	///
	template <typename ConcreteType>
	struct SimpleTransition
	{
		typedef ConcreteType StateType;
		SimpleTransition( StateType *from, StateType *to )
		{
			this->from = from; 
			this->to = to;
		}

		StateType *from, *to;
	};
		
	template <typename StateWrapper>
	class StateMachine;

	///
	/// Base state class, you should inherite this class to implement your
	/// concrete state.
	/// To use this state library, you'd better to define a wrapper class to 
	/// typedef many require typedefs. This class will be used in base state 
	/// class, to make these functions' parameter type safe.
	/// These typedefs should be defined:
	///  - concrete_type: concrete state type, usually derived from state<Type>.
	///  - event_type: event sent to states.
	///  - transition_type: transition between two states.
	///  - entity_type: state owner type.
	///
	template <typename StateWrapper>
	class State
	{
	public:
		typedef StateWrapper WrapperType;
		typedef StateMachine<WrapperType> MachineType;
		typedef State<WrapperType> SelfType;
		typedef typename WrapperType::ConcreteType ConcreteType;
		typedef typename WrapperType::EventType EventType;
		typedef typename WrapperType::TransitionType TransitionType;
		typedef typename WrapperType::EntityType EntityType;
	public:
		State( MachineType *machine ) : m_Machine( machine )
		{
		}

		virtual ~State() { }

		/// Receive an event to transform to other states.
		virtual void ReceiveEvent( EntityType*, const EventType &ev ) { }

		/// Execute when enter this state.
		virtual void Enter( EntityType*, const TransitionType& ) { }

		/// Execute when leave this state.
		virtual void Leave( EntityType*, const TransitionType& ) { }

		/// Execute when in this state.
		virtual void Execute( EntityType* ) = 0;

		/// Compare two states. It's not type safe actually.
		virtual bool operator == ( const ConcreteType &other ) const = 0;

		bool operator != ( const ConcreteType &other ) const
		{
			return !( *this == other );
		}

		/// Query the machine.
		MachineType *GetMachine() const
		{
			return m_Machine;
		}
	protected:
		MachineType *m_Machine;
	};

	///
	/// Usage: state_machine<Player> m_smachine;
	///
	/// @see state
	///
	template <typename StateWrapper>
	class StateMachine
	{
	public:
		typedef StateWrapper WrapperType;
		typedef State<WrapperType> StateType;
		typedef typename WrapperType::ConcreteType ConcreteType;
		typedef typename WrapperType::EventType EventType;
		typedef typename WrapperType::TransitionType TransitionType;
		typedef typename WrapperType::EntityType EntityType;
	public:
		StateMachine( EntityType *owner ) :
			m_Owner( owner )
		{
			Clear();
		}

		~StateMachine()
		{
		}

		/// Clear the current state.
		void Clear()
		{
			m_CurState = 0;
			m_PreState = 0;
		}

		/// Return true if the machine has a state.
		bool HasState() const
		{
			return m_CurState != 0;
		}

		/// Get the current state.
		ConcreteType *CurState()
		{
			return m_CurState;
		}

		/// Check whether is in the specified state.
		bool IsState( const ConcreteType &other ) const
		{
			if( !HasState() )
			{
				return false;
			}
			return other == *m_CurState;
		}

		/// Execute the current state.
		void Execute() const
		{
			if( HasState() )
			{
				m_CurState->Execute( m_Owner );
			}
		}

		/// Change state.
		void Change( ConcreteType *other, const TransitionType &tran )
		{
			if( HasState() )
			{
				m_CurState->Leave( m_Owner, tran );
			}
			other->Enter( m_Owner, tran );
			m_PreState = m_CurState;
			m_CurState = other;
		}

		/// Revert the previous state.
		void Revert( const TransitionType &tran )
		{
			if( m_PreState != 0 )
			{
				Change( m_PreState, tran );
			}
		}

		/// Get the entity object.
		EntityType *GetEntity() const
		{
			return m_Owner;
		}
	private:
		ConcreteType *m_CurState;
		ConcreteType *m_PreState;
		EntityType *m_Owner;
	};
}

#endif // __KL_STATE_H_
