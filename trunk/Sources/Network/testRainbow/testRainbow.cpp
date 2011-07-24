// testRainbow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
#include <winsock2.h>
#include <conio.h>
#include <process.h>

#include "..\rainbow\interface\IClient.h"
#include "..\rainbow\interface\RainbowInterface.h"

typedef HRESULT ( __stdcall * pfnQueryClientInterface )(
			REFIID	riid,
			void	**ppv
		);

void __stdcall ServerEventNotify(
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case SERVER_CONNECT_CREATE:
		
		break;
	case SERVER_CONNECT_CLOSE:
		
		break;
	}
}

unsigned _stdcall ThreadFunction( void *pParam )
{
	IClient *pClient = reinterpret_cast< IClient * >( pParam );

	while ( pClient )
	{
		Sleep( 1 );
		
		size_t datalength = 0;

		const void *pData = pClient->GetPackFromServer( datalength );

		if ( !pData || 0 == datalength )
		{
			continue;
		}

		//cprintf( "get a package from client\n" );
	}

	if ( pClient )
	{
		pClient->Release();
		pClient = NULL;
	}

	return 0L;
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	HMODULE hModule = LoadLibrary( "..\\Rainbow\\Debug\\Rainbow.dll" );
#else
	HMODULE hModule = LoadLibrary( "..\\Rainbow\\Release\\Rainbow.dll" );
#endif

	pfnQueryClientInterface pfn = ( pfnQueryClientInterface )GetProcAddress( hModule, "QueryClientInterface" );
	
	IClient *pClient = NULL;

	if ( FAILED( pfn( IID_IESClient, reinterpret_cast< void ** >( &pClient ) ) ) )
	{
		printf( "Query client interface failed!" );
		exit( 0 );
	}

	pClient->Startup();

	pClient->RegisterMsgFilter( ServerEventNotify );

	pClient->ConnectTo( "127.0.0.1", 5001 );

	IClient *pClonClient = NULL;
	pClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pClonClient ) );

	unsigned int threadid = 0;
	HANDLE handle = (HANDLE)::_beginthreadex( 0, 0, ThreadFunction, reinterpret_cast< void * >( pClonClient ), 0, &threadid );

	CloseHandle( handle );

	while ( pClient )
	{
		pClient->SendPackToServer( "to server", strlen( "to server" ) );
		
		Sleep( 1 );
	}

	pClient->Shutdown();

	pClient->Cleanup();

	if ( pClient )
	{
		pClient->Release();
		pClient = NULL;
	}	
	
	return 0;
}

