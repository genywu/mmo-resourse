///
/// @file OrderListener.h
/// @author Kevin Lynx
///
#ifndef ___AH_ORDERLISTENER_H_
#define ___AH_ORDERLISTENER_H_

#include <list>

namespace AH
{
    class SubOrder;
    class AgentOrder;

    /// OrderListener listen SubOrder's status.
    class OrderListener
    {
    public:
        virtual ~OrderListener() { }

        /// Called after a SubOrder has been created.
        virtual void OnAdd( SubOrder *order )  { }

        /// Called before delete a SubOrder.
        virtual void OnRemove( SubOrder *order ) { }

        /// Called after a SubOrder has been changed.
        /// return true to indicate SubOrder should be deleted.
        virtual bool OnUpdate( SubOrder *order, int subCnt, AgentOrder *aorder ) { return false; }

        /// Do delete, return true means delete finished.
        virtual bool OnDelete( SubOrder *order ) {  return false; } 
    };

    /// Store a list of OrderListeners. This class only keep a reference to 
    /// the listener.
    class ListenerList
    {
    public:
        typedef std::list<OrderListener*> ListenerListT;
    public:
        /// Add a listener to the list.
        void Add( OrderListener *listener );

        /// Remove the listener from the list.
        void Remove( OrderListener *listener );
    public:
        ListenerListT m_Listeners;
    };
#define TRAVERSE_LISTENER( l, v, exp ) \
    for( ListenerList::ListenerListT::iterator it = l.m_Listeners.begin(); \
        it != l.m_Listeners.end(); ++ it ) \
    { OrderListener *v = *it; exp; } 
}

#endif
