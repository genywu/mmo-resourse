// testIOCPServer.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>

//[ Include in .\..\IPCPServer
#include "IOCPServer\Exception.h"
#include "IOCPServer\Utils.h"
#include "IOCPServer\ManualResetEvent.h"

#include "IOCPServer\SocketServer.h"
#include "IOCPServer\tstring.h"
//]

#include "GameServer.h"

using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::CException;
using OnlineGameLib::Win32::Output;
using OnlineGameLib::Win32::OutPutInfo;
using OnlineGameLib::Win32::CManualResetEvent;
using OnlineGameLib::Win32::CCriticalSection;
using OnlineGameLib::Win32::ToString;
using OnlineGameLib::Win32::GetTimeStamp;

#include "stack"
#include "map"
using namespace std;

#if _MSC_VER > 1020			// if VC++ version is > 4.2
	using namespace std;	// std c++ libs implemented in std
#endif

/*
 * Manager of client
 */
#define MAX_NUMOF_CLIENT	1000

typedef map< DWORD, DWORD > INDEX2CLIENT;

INDEX2CLIENT g_lClientSocket;

typedef stack< DWORD > STACK;

STACK g_vFreeClient;

CCriticalSection g_csClient;

void Init();
void DeInit();

bool AddClient( OnlineGameLib::Win32::CSocketServer::Socket *pSocket );
bool DelClient( OnlineGameLib::Win32::CSocketServer::Socket *pSocket );

void SendDataToAllClient( const char *pData, size_t dataLength );

/*
 * Performance
 */
LARGE_INTEGER g_theFrequency = {0};

/*
 * Entrance of the application
 */
int main(int argc, char* argv[])
{
	::QueryPerformanceFrequency( &g_theFrequency );			

	Init();

	try
	{
		CGameServer server(
			"Welcome to jx.online server! What are you doing now?",
			INADDR_ANY,                   // address to listen on
			5001,                         // port to listen on
			10,                           // max number of sockets to keep in the pool
			10,                           // max number of buffers to keep in the pool
			1024);                        // buffer size 
		
		server.Start();
		
		server.StartAcceptingConnections();
	
		char szBuffer[1000] = {0};
		
		memset( szBuffer, '.', 1000 );
		memcpy( szBuffer, "BEGIN", strlen("BEGIN") );
		memcpy( szBuffer+1000-strlen("END"), "END", 3 );

		LARGE_INTEGER thePerformanceCount_Begin = {0};
		LARGE_INTEGER thePerformanceCount_End = {0};

		while ( true )
		{
			::QueryPerformanceCounter( &thePerformanceCount_Begin );					

			/*
			 * Send some data to all client
			 */
			SendDataToAllClient( szBuffer, 1000 );

			::QueryPerformanceCounter( &thePerformanceCount_End );
			
			/*
			 * 20 frame percent one second
			 */
			LONGLONG l64nInterval = thePerformanceCount_End.QuadPart - thePerformanceCount_Begin.QuadPart;
			LONGLONG l64nLeave = g_theFrequency.QuadPart / 50 - l64nInterval;

			while ( l64nLeave >= 0 )
			{				
				::QueryPerformanceCounter( &thePerformanceCount_Begin );
				l64nInterval = thePerformanceCount_Begin.QuadPart - thePerformanceCount_End.QuadPart;
				
				l64nLeave -= l64nInterval;
				thePerformanceCount_End.QuadPart = thePerformanceCount_Begin.QuadPart;
				
				::Sleep( 1 );
			}			
		}

		CManualResetEvent shutdownEvent(_T("OnlineGameServerShutdown"), false);
		CManualResetEvent pauseResumeEvent(_T("OnlineGameServerPauseResume"), false);
		
		HANDLE handlesToWaitFor[2];
		
		handlesToWaitFor[0] = shutdownEvent.GetEvent();
		handlesToWaitFor[1] = pauseResumeEvent.GetEvent();
		
		bool accepting = true;
		bool done = false;
		
		while (!done)
		{
			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

			if (waitResult == WAIT_OBJECT_0)
			{
				done = true;
			}
			else if (waitResult == WAIT_OBJECT_0 + 1)
			{
				if (accepting)
				{
					server.StopAcceptingConnections();
				}
				else
				{
					server.StartAcceptingConnections();
				}
				
				accepting = !accepting;
			}
			else
			{
				Output(_T("Unexpected result from WaitForMultipleObjects - exiting"));
				done = true;
			}
		}

		server.WaitForShutdownToComplete();
	}
	catch(const CException &e)
	{
		Output(_T("Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
	}
	catch(...)
	{
		Output(_T("Unexpected exception"));
	}

	DeInit();
	
	return 0;
}

bool AddClient( OnlineGameLib::Win32::CSocketServer::Socket *pSocket )
{
	CCriticalSection::Owner lock( g_csClient );

	if ( !g_vFreeClient.empty() )
	{
		DWORD dwIndex = g_vFreeClient.top();
		g_vFreeClient.pop();

		pSocket->SetUserData( dwIndex );
		g_lClientSocket.insert( INDEX2CLIENT::value_type( dwIndex, reinterpret_cast<DWORD>(pSocket) ) );
			
		OutPutInfo( GetTimeStamp() + _T("=> Add a client [current total:") + 
			ToString( g_lClientSocket.size() ) +
			_T( "]" ) );

		return true;
	}

	return false;
}

bool DelClient( OnlineGameLib::Win32::CSocketServer::Socket *pSocket )
{
	CCriticalSection::Owner lock( g_csClient );

	if ( !g_lClientSocket.empty() )
	{
		DWORD index = pSocket->GetUserData();
		g_lClientSocket.erase( index );
		
		g_vFreeClient.push( index );
			
		OutPutInfo( GetTimeStamp() + _T("=> Del a client [current total:") + 
			ToString( g_lClientSocket.size() ) + 
			_T( "]" ) );
		
		return true;
	}

	return false;
}

void SendDataToAllClient( const char *pData, size_t dataLength )
{
	CCriticalSection::Owner lock( g_csClient );

	for( INDEX2CLIENT::iterator index = g_lClientSocket.begin();
		index != g_lClientSocket.end();
		index ++ )
	{
		OnlineGameLib::Win32::CSocketServer::Socket *pSocket = 
			reinterpret_cast<OnlineGameLib::Win32::CSocketServer::Socket *>((*index).second);
		if ( pSocket )
		{
			pSocket->Write( pData, dataLength );
		}
	}
}

void Init()
{
	for ( DWORD i = 0; i<MAX_NUMOF_CLIENT; i++ )
	{
		g_vFreeClient.push( i );
	}
}

void DeInit()
{
	while ( !g_vFreeClient.empty() )
	{
		g_vFreeClient.pop();
	}

	g_lClientSocket.erase( g_lClientSocket.begin(), g_lClientSocket.end() );
}