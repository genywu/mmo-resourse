///
/// @file ScriptStateImpl.h
///
#ifndef ___SCRIPT_STATE_IMPL_H_
#define ___SCRIPT_STATE_IMPL_H_

#include <string>
#include "PeaceStateImpl.h"
#include "FightStateImpl.h"
#include "../EventImpl/Event.h"
#include "../Helper/ScriptCaller.h"

namespace StateAI
{
    template <int stateType>
    struct AdapterType : public AIState
    {
        AdapterType( MachineType *machine ) : AIState( machine, stateType )
        {
        }
    };

    template <typename Base>
    class ScriptState : public Base
    {
    public:
        typedef ScriptState<Base> SelfType;
        typedef typename Base::BaseType BaseType;
    public:
        ScriptState( MachineType *machine ) : Base( machine )
        {
        }

        void ReceiveEvent( typename BaseType::EntityType *entity, const typename BaseType::EventType &ev )
        {
            if( ev.Type() == ET_INIT )
            {
                const InitEvent &iev = (const InitEvent&) ev;
                m_File = (const char*) iev.arg;
            }
        }

        virtual bool PreExecute( typename BaseType::EntityType *entity )
        {
            return false;
        }

        void Execute( typename BaseType::EntityType *entity )
        {
            if( PreExecute( entity ) )
            {
                return;
            }
            ScriptCaller caller( entity->GetOwner() );
            caller.Call( m_File.c_str() );
        }

    private:
        std::string m_File;
    };

    /// PeaceState
    class PeaceStateScript : public ScriptState<PeaceStateBase>
    {
    public:
        PeaceStateScript( MachineType *machine ) : SelfType( machine )
        {
        }

        bool PreExecute( BaseType::EntityType *entity );

        DECL_LIFE_POLICY_FUNC( PeaceStateScript, AIState );
    };

    /// SkillTracer, used by FightStateScript
    class SkillTracer
    {
    public:
        enum { Okay, TIMEOUT, FAILED, RUNNING };
    public:
        SkillTracer();

        void Init( long id, long lvl, const CGUID &target, ulong traceTime );

        int Run( BaseAI *ai );

        void Reset();

    public:
        long m_SkillID;
        long m_SkillLvl;
        float m_SkillMaxDis;
        float m_SkillMinDis;
        CGUID m_TargetID;
        ulong m_EndTime;
    };

    /// FightState
    class FightStateScript : public ScriptState<FightStateBase>
    {
    public:
        enum { NORMAL, TRACING };
    public:
        FightStateScript( MachineType *machine ) : SelfType( machine )
        {
            m_State = NORMAL;
        }

        void ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev );

        bool PreExecute( BaseType::EntityType *entity );

        void Reset();

        DECL_LIFE_POLICY_FUNC( FightStateScript, AIState );
    private:
        bool CheckTracer( BaseType::EntityType *entity );

        void OnUseSkillEnd( long ret );
    private:
        CGUID m_skillScriptID;
        SkillTracer m_SkillTracer;
        int m_State;
    };

    /// EscapeState
    class EscapeStateScript : public ScriptState<AdapterType<ST_ESCAPE> >
    {
    public:
        EscapeStateScript( MachineType *machine ) : SelfType( machine )
        {
        }

        DECL_LIFE_POLICY_FUNC( EscapeStateScript, AIState );
    };

    /// SearchState
    class SearchStateScript : public ScriptState<AdapterType<ST_SEARCH> >
    {
    public:
        SearchStateScript( MachineType *machine ) : SelfType( machine )
        {
        }

        DECL_LIFE_POLICY_FUNC( SearchStateScript, AIState );
    };

}

#endif
