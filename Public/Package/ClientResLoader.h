///
/// @file ClientResLoader.h
/// @author Kevin Lynx
/// @brief Concrete resource loader for client, only for DEBUGing purporse now.
///
#ifndef ___CLIENT_RES_LOADER_H_
#define ___CLIENT_RES_LOADER_H_

#include "AsyncLoader.h"

#ifdef ENABLE_ALOADER 

class CRFile;
CRFile *rfOpenLocal( const char *file );

/// Replace 'rfOpen' function, donot bother the resource package.
#define rfOpen rfOpenLocal
#define ALOADER_WAITALL() GetInst( LoaderTable ).WaitAll()
#define ALOADER_WAIT(n) GetInst( LoaderTable ).Wait( (const char*)n )
/// Load tag can be : Skill, Quest
#define ALOADER_LOAD( tag, u, exp ) void ALoad##tag( const char *, void * ); \
    ALoad##tag( #tag, (void*) u )
#define ALOADER_TIME_STAT(tag, exp) { \
    DWORD stime = timeGetTime(); \
    exp; \
    DWORD etime = timeGetTime(); \
    char s[256]; \
    sprintf(s, "Load Time:%s %u\n", (const char*)tag, etime > stime ? etime-stime:0); \
    OutputDebugString(s); \
}
#define ALOADER_STARTUP(n) GetInst( LoaderTable ).Startup( (int)(n) )
#define ALOADER_SHUTDOWN() GetInst( LoaderTable ).Shutdown()
#else
#define ALOADER_WAITALL()
#define ALOADER_WAIT(n) (void) n
#define ALOADER_LOAD( tag, u, exp ) exp
#define ALOADER_TIME_STAT( tag, exp ) exp
#define ALOADER_STARTUP(n)
#define ALOADER_SHUTDOWN()
#endif

#endif
