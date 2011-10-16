///
/// @file TauntHandler.h
/// @author Kevin Lynx
/// @brief Handle monster taunt stuff.
///
#ifndef ___TAUNT_HANDLER_H_
#define ___TAUNT_HANDLER_H_

#include <vector>

class CMonster;

class TauntHandler
{
public:
    /// Identify a shape.
    struct ShapeAttr
    {
        CGUID id;
        long type;
    };

    struct TauntItem
    {
        ShapeAttr target;
        int value;
    };

    enum { NPOS = (size_t)(-1) };

    typedef std::vector<TauntItem> TauntListT;

public:
    TauntHandler( CMonster *owner ) : m_owner( owner )
    {
        m_maxSize = 1;
    }

    ///
    /// Increase taunt value, if the item does NOT exist, add it first.
    ///
    void IncValue( const CGUID &id, long type, int inc );

    ///
    /// Decrease taunt value, if the result value <= 0, remove the item.
    ///
    void DecValue( const CGUID &id, long type, int dec );

    ///
    /// Set taunt value directly.
    ///
    void SetValue( const CGUID &id, int value );

    ///
    /// Sort taunt items in the list.
    ///
    void Sort();

    ///
    /// Query the taunt value.
    ///
    int Value( const CGUID &id );
    int Value( size_t index );

    ///
    /// Check whether a taunt item exists.
    ///
    bool Exist( const CGUID &id );

    ///
    /// Add a taunt item.
    ///
    void AddItem( const CGUID &id, long type, int initVal );

    ///
    /// Remove a taunt item.
    ///
    void RemoveItem( const CGUID &id );

    ///
    /// Clear all items.
    ///
    void Clear();

    ///
    /// Get the highest taunt value item.
    ///
    const TauntItem *GetBestItem() const;

    ///
    /// Force an object to the best item.
    ///
    bool ForceToBest( const CGUID &id );

    /// 
    /// Set max size.
    ///
    void SetMaxSize( size_t i ) { m_maxSize = i; }

    size_t Size() const { return m_taunts.size(); }

    CMonster *GetOwner() { return m_owner; }

    const TauntListT &QueryList() const { return m_taunts; }

private:
    TauntItem *GetItem( const CGUID &id ) ;

    TauntItem *GetItem( size_t pos );

    size_t GetItemPos( const TauntItem *item );

    bool CanReplaceFirst();

private:
    TauntListT m_taunts;
    CMonster *m_owner;
    size_t m_maxSize;
};

#endif
