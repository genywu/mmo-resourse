///
/// @file TauntedList.h
/// @author Kevin Lynx
///
#ifndef ___TAUNTED_LIST_H_
#define ___TAUNTED_LIST_H_

class CMoveShape;
class CMonster;

///
/// Manage the taunt monster list on a player.
/// TODO: if all the kinds of shapes own this, add this as a memeber of MoveShape.
///
class TauntedList
{
public:
    typedef std::vector<CGUID> MonsterListT;

public:
    TauntedList( CMoveShape *owner ) : m_owner( owner )
    {
    }

    /// The owner is added to some monster taunt table.
    void OnAdded( const CGUID &id );

    /// The owner is removed from some monster taunt table.
    void OnRemoved( const CGUID &id );

    /// Rescued by someone, and SOMEONE will be added in the taunt list.
    void OnRescued( const CGUID &rescuer, long type, long hp );

    /// Different processed.
    void OnHelped( const CGUID &helper, long type, int value );

private:
    bool Exist( const CGUID &id );
private:
    MonsterListT m_monsters;
    CMoveShape *m_owner;
};

TauntedList *QueryTauntedList( CMonster *self, const CGUID &id, long type );

#endif
