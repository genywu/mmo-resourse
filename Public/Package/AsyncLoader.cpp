////
/// @AsyncLoader.cpp
/// @brief Implement an asynchronous data loader, load data by a thread.
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "AsyncLoader.h"
#include <assert.h>
#include <windows.h>
#include <process.h>

struct LoadRequest
{
    enum { N_LEN = 32 };
    char name[N_LEN];
    void *u;
    LoadFuncT func;
    HANDLE ev;
};

static CRITICAL_SECTION s_requestsMutex;
static HANDLE s_emptyEvent; 

#define LOCK EnterCriticalSection( &s_requestsMutex )
#define UNLOCK LeaveCriticalSection( &s_requestsMutex )

static LoadRequest *NewRequest( const char *name, LoadFuncT f, void *u )
{
    assert(strlen(name) < LoadRequest::N_LEN);
    LoadRequest *r = new LoadRequest();
    r->func = f;
    r->u = u;
    r->ev = CreateEvent( NULL, FALSE, FALSE, NULL );
    strcpy( r->name, name );
    return r;
}

static void FreeRequest( LoadRequest *r )
{
    delete r;
}

unsigned int __stdcall LoaderTable::AdaptLoader( void *arg )
{
    LoaderTable *lt = (LoaderTable*) arg;
    while( 1 )
    {
        LoadRequest *r = lt->PopRequest();
        if( !r ) break;
        r->func( r->u );
        SetEvent( r->ev );
        FreeRequest( r );
    }
    _endthreadex( 0 );
    return 0;
}

static void WaitRequest( HANDLE ev )
{
    WaitForSingleObject( ev, INFINITE );
    CloseHandle( ev );
}

LoaderTable::LoaderTable()
{
    m_Threads = NULL;
}

LoaderTable::~LoaderTable()
{
    Shutdown();
}

bool LoaderTable::Startup( int tc )
{
    s_emptyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    InitializeCriticalSection( &s_requestsMutex );

    m_Threads = new ThreadH[tc+1];
    m_Threads[tc] = NULL;
    for( int i = 0; i < tc; ++ i )
    {
        m_Threads[i] = (ThreadH) _beginthreadex( NULL, 0, AdaptLoader, this, 0, NULL );
    }
    return true;
}

void LoaderTable::Shutdown()
{
    if( m_Threads == NULL ) return ;
    for( int i = 0; m_Threads[i]; ++ i )
    {
        PushRequest( NULL );
        WaitForSingleObject( (HANDLE)m_Threads[i], INFINITE );
        CloseHandle( (HANDLE)m_Threads[i] );
    }
    delete [] m_Threads;
    m_Threads = NULL;
    for( RequestQueueT::iterator it = m_Requests.begin();
        it != m_Requests.end(); ++ it )
    {
        FreeRequest( *it );
    }
    m_Requests.clear();
    for( EventTableT::iterator it = m_Events.begin();
        it != m_Events.end(); ++ it )
    {
        CloseHandle( (HANDLE) it->second );
    }
    m_Events.clear();
    DeleteCriticalSection( &s_requestsMutex );
    CloseHandle( s_emptyEvent );
}

bool LoaderTable::Request( const char *name, LoadFuncT func, void *u )
{
    LoadRequest *r = NewRequest( name, func, u );
    assert( r );
    m_Events[r->name] = (EventH) r->ev;
    PushRequest( r );
    return true;
}

void LoaderTable::PushRequest( LoadRequest *r )
{
    LOCK;
    size_t s = m_Requests.size();
    m_Requests.push_back( r );
    UNLOCK;
    if( s == 0 )
    {
        SetEvent( s_emptyEvent );
    }
}

LoadRequest *LoaderTable::PopRequest()
{
    LOCK;
    while( m_Requests.size() == 0 ) // while is VERY necessary here.
    {
        UNLOCK;
        WaitForSingleObject( s_emptyEvent, INFINITE );
        LOCK;
    }
    LoadRequest *r = m_Requests.front();
    m_Requests.erase( m_Requests.begin() );
    UNLOCK;
    return r;
}


void LoaderTable::Wait( const char *name )
{
    EventTableT::iterator it = m_Events.find( name );
    if( it != m_Events.end() )
    {
        WaitRequest( (HANDLE) it->second );
        m_Events.erase( it );
    }
}

void LoaderTable::WaitAll()
{
    for( EventTableT::iterator it = m_Events.begin();
        it != m_Events.end(); ++ it )
    {
        WaitRequest( (HANDLE) it->second );
    }
    m_Events.clear();
}

int GetCPUNum()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo( &sysInfo );
    return sysInfo.dwNumberOfProcessors;
}
