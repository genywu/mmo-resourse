///
/// @file TauntHandler.cpp
/// @author Kevin Lynx
/// @brief Handle monster taunt stuff.
///
#include "stdafx.h"
#include "TauntHandler.h"
#include "TauntedList.h"
#include "../Monster.h"

static bool CompareItem( const TauntHandler::TauntItem &left, 
                        const TauntHandler::TauntItem &right )
{
    return left.value > right.value;
}

void TauntHandler::IncValue( const CGUID &id, long type, int inc )
{
    TauntItem *item = GetItem( id );
    if( item )
    {
        item->value += inc;
        size_t pos = GetItemPos( item );
        assert( pos != NPOS );
        if( pos != NPOS && pos > 0 ) 
        {
            Sort();
        }
    }
}

void TauntHandler::DecValue( const CGUID &id, long type, int inc )
{
    TauntItem *item = GetItem( id );
    if( item )
    {
        item->value -= inc;
        if( item->value <= 0 )
        {
            RemoveItem( id );
            return ;
        }
        Sort();
    }
}

void TauntHandler::SetValue( const CGUID &id, int value )
{
    TauntItem *item = GetItem( id );
    if( item && value > 0 )
    {
        item->value = value;
        Sort();
    }
}

void TauntHandler::Sort()
{
    // sort the items except the 1st item.
    if( Size() > 2 )
    {
        std::sort( ++m_taunts.begin(), m_taunts.end(), CompareItem );
    }
    // resort the whole list if necessary.
    if( CanReplaceFirst() )
    {
        std::sort( m_taunts.begin(), m_taunts.end(), CompareItem );
    }
}

bool TauntHandler::CanReplaceFirst()
{
    if( m_taunts.size() < 2 ) return false;
    int v1 = m_taunts[0].value;
    int v2 = m_taunts[1].value;
    assert( v1 > 0 );
    // TODO: modify 1.2
    return 1.0f * v2 / v1 > 1.2;
}

int TauntHandler::Value( const CGUID &id ) 
{
    const TauntItem *item = GetItem( id );
    return item ? item->value : -1;
}

int TauntHandler::Value( size_t index )
{
    return index < Size() ? m_taunts[index].value : -1;
}

bool TauntHandler::Exist( const CGUID &id )
{
    const TauntItem *item = GetItem( id );
    return item != NULL;
}

void TauntHandler::AddItem( const CGUID &id, long type, int initVal )
{
    if( Size() >= m_maxSize ) return ;

    assert( !Exist( id ) );
    TauntItem item = { { id, type }, initVal };
    m_taunts.push_back( item );
    Sort();
    // notify the target.
    TauntedList *tlist = QueryTauntedList( m_owner, id, type );
    if( tlist )
    {
        tlist->OnAdded( m_owner->GetExID() );
    }
}

void TauntHandler::RemoveItem( const CGUID &id )
{
    for( TauntListT::iterator it = m_taunts.begin();
        it != m_taunts.end(); ++ it )
    {
        if( (*it).target.id == id ) 
        {
            // notify the target.
            TauntedList *tlist = QueryTauntedList( m_owner, id, (*it).target.type );
            if( tlist )
            {
                tlist->OnRemoved( m_owner->GetExID() );
            }
            m_taunts.erase( it );
            break;
        }
    }
}

void TauntHandler::Clear()
{
    for( TauntListT::const_iterator it = m_taunts.begin();
        it != m_taunts.end(); ++ it )
    {
        // notify the target.
        const TauntItem &item = *it;
        TauntedList *tlist = QueryTauntedList( m_owner, item.target.id, item.target.type );
        if( tlist )
        {
            tlist->OnRemoved( m_owner->GetExID() );
        }
    }
    m_taunts.clear();
}

const TauntHandler::TauntItem *TauntHandler::GetBestItem() const
{
    if( Size() > 0 )
    {
        return &m_taunts[0];
    }
    return NULL;
}

bool TauntHandler::ForceToBest( const CGUID &id )
{
    TauntItem *item = GetItem( id );
    if( item == NULL ) return false;
    TauntItem *best = GetItem( 0 );
    if( best == item ) return false; // only 1 item
    item->value = best->value;
    // swap them
    TauntItem tmpItem = *item;
    *item = *best;
    *best = tmpItem;
    return true;
}

TauntHandler::TauntItem *TauntHandler::GetItem( const CGUID &id ) 
{
    for( TauntListT::iterator it = m_taunts.begin();
        it != m_taunts.end(); ++ it )
    {
        if( (*it).target.id == id ) return &(*it);
    }
    return NULL;
}

TauntHandler::TauntItem *TauntHandler::GetItem( size_t pos )
{
    return pos >= Size() ? NULL : &m_taunts[pos];
}

size_t TauntHandler::GetItemPos( const TauntItem *item )
{
    size_t i = 0;
    for( TauntListT::iterator it = m_taunts.begin();
        it != m_taunts.end(); ++ it, ++ i )
    {
        if( &(*it) == item ) return i;
    }
    return NPOS;
}
