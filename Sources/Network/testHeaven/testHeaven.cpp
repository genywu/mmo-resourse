// testHeaven.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
#include <winsock2.h>
#include <conio.h>
#include <process.h>

#include "..\heaven\interface\IServer.h"
#include "..\heaven\interface\HeavenInterface.h"

typedef HRESULT ( __stdcall * pfnQueryServerInterface )(
			REFIID	riid,
			void	**ppv
		);

void __stdcall ServerEventNotify(
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case CLIENT_CONNECT_CREATE:
		
		break;
	case CLIENT_CONNECT_CLOSE:
		
		break;
	}
}

unsigned _stdcall ThreadFunction( void *pParam )
{
	IServer *pServer = reinterpret_cast< IServer * >( pParam );

	while ( pServer )
	{
		Sleep( 1 );
		
		size_t datalength = 0;

		const void *pData = pServer->GetPackFromClient( 199, datalength );

		if ( !pData || 0 == datalength )
		{
			continue;
		}

		//cprintf( "get a package from client\n" );
	}

	if ( pServer )
	{
		pServer->Release();
		pServer = NULL;
	}

	return 0L;
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	HMODULE hModule = LoadLibrary( "..\\Heaven\\Debug\\Heaven.dll" );
#else
	HMODULE hModule = LoadLibrary( "..\\Heaven\\Release\\Heaven.dll" );
#endif

	pfnQueryServerInterface pfn = ( pfnQueryServerInterface )GetProcAddress( hModule, "QueryServerInterface" );
	
	IServer *pServer = NULL;

	if ( FAILED( pfn( IID_IIOCPServer, reinterpret_cast< void ** >( &pServer ) ) ) )
	{
		printf( "Query server interface failed!" );
		exit( 0 );
	}

	pServer->Startup();

	pServer->RegisterMsgFilter( ServerEventNotify );

	pServer->OpenService( INADDR_ANY, 5001 );
	
	IServer *pClonServer = NULL;
	pServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pClonServer ) );

	unsigned int threadid = 0;
	HANDLE handle = (HANDLE)::_beginthreadex( 0, 0, ThreadFunction, reinterpret_cast< void * >( pClonServer ), 0, &threadid );

	CloseHandle( handle );

	while ( pServer )
	{
		pServer->PreparePackSink();

		pServer->PackDataToClient( 199, "to client", strlen( "to client" ) );

		pServer->SendPackToClient();
		
		Sleep( 1 );
	}

	pServer->CloseService();

	pServer->Cleanup();

	if ( pServer )
	{
		pServer->Release();
		pServer = NULL;
	}

	return 0;
}

