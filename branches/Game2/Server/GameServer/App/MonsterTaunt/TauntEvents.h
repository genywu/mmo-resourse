///
/// @file TauntEvents.h
/// @author Kevin Lynx
///
#ifndef ___TAUNT_EVENTS_H_
#define ___TAUNT_EVENTS_H_

class TauntHandler;

///
/// Handle taunt value related events.
///
class TauntEvents
{
public:
    TauntEvents( TauntHandler &handler ) : m_handler( handler )
    {
    }

    /// A gladiator ai monster find some enemies.
    void Searched( const CGUID &target, long type );

    /// The monster is attacked by someone.
    void Attacked( const CGUID &attacker, long type, long hurt );

    /// Someone rescue other players.
    void Rescue( const CGUID &rescuer, long type, long validHp );
    
    /// Someone help other players.
    void Help( const CGUID &helper, long type, int value );
    
    /// The attacker target died.
    void TargetDied( const CGUID &target, long type );

    /// Can not find the target anymore.
    void TargetLost( const CGUID &target, long type );

private:
    /// Add the teammates.
    void AddTeammate( const CGUID &player );
private:
    TauntHandler &m_handler;
};

#endif

