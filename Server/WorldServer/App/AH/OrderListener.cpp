///
/// @file OrderListener.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "OrderListener.h"

namespace AH
{
    void ListenerList::Add( OrderListener *listener )
    {
        m_Listeners.push_back( listener );
    }
    
    void ListenerList::Remove( OrderListener *listener )
    {
        m_Listeners.remove( listener );
    }
}
