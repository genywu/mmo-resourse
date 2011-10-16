///
/// @AsyncLoader.h
/// @brief Implement an asynchronous data loader, load data by a thread.
/// @author Kevin Lynx
/// 
#ifndef ___ASYNCLOADER_H_
#define ___ASYNCLOADER_H_

#include <map>
#include <string>
#include <vector>

typedef int (*LoadFuncT) (void *u);
struct LoadRequest;

class LoaderTable
{
public:
    typedef std::vector<LoadRequest*> RequestQueueT;
    typedef void* ThreadH;
    typedef void* EventH;
    typedef std::map<std::string, EventH> EventTableT;
public:
    LoaderTable();

    ~LoaderTable();

    bool Startup( int threadCount );

    void Shutdown();

    bool Request( const char *name, LoadFuncT func, void *u );

    void Wait( const char *name );

    void WaitAll();

private:
    static unsigned int __stdcall AdaptLoader( void *arg );

    LoadRequest *PopRequest();

    void PushRequest( LoadRequest *r );

private:
    RequestQueueT m_Requests;
    ThreadH *m_Threads;
    EventTableT m_Events;
};

int GetCPUNum();

#endif
