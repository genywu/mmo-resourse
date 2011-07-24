// testHeaven.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
#include <winsock2.h>
#include <process.h>

#include "IServer.h"
#include "HeavenInterface.h"

static const int g_snMaxPlayerCount = 5000;
static const int g_snPrecision = 1;
static const int g_snMaxBuffer = 15;
static const int g_snBufferSize = 1024;

typedef HRESULT ( __stdcall * pfnCreateServerInterface )(
			REFIID	riid,
			void	**ppv
		);

void __stdcall ServerEventNotify(
			LPVOID lpParam,
			const unsigned long &ulnID,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumClientConnectCreate:
		printf( "%d\n", ulnID );
		break;
	case enumClientConnectClose:
		
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

		const void *pData = pServer->GetPackFromClient( 4, datalength );

		if ( !pData || 0 == datalength )
		{
			continue;
		}

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
	IServer *pServer = NULL;

	HMODULE hModule = ::LoadLibrary( "..\\heaven\\debug\\heaven.dll" );

	if ( hModule )
	{
		pfnCreateServerInterface pFactroyFun = ( pfnCreateServerInterface )GetProcAddress( hModule, "CreateInterface" );

		IServerFactory *pServerFactory = NULL;

		if ( SUCCEEDED( pFactroyFun( IID_IServerFactory, reinterpret_cast< void ** >( &pServerFactory ) ) ) )
		{
			pServerFactory->SetEnvironment( g_snMaxPlayerCount, g_snPrecision, g_snMaxBuffer, g_snBufferSize  );

			pServerFactory->CreateServerInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pServer ) );

			pServerFactory->Release();
		}
	}

	pServer->Startup();

	pServer->RegisterMsgFilter( reinterpret_cast< void * >( pServer ), ServerEventNotify );

	pServer->OpenService( INADDR_ANY, 5001 );
/*
	IServer *pClonServer = NULL;
	pServer->QueryInterface( IID_IIOCPServer, reinterpret_cast< void ** >( &pClonServer ) );

	unsigned int threadid = 0;
	HANDLE handle = (HANDLE)::_beginthreadex( 0, 0, ThreadFunction, reinterpret_cast< void * >( pClonServer ), 0, &threadid );

	CloseHandle( handle );
	
	const size_t len = strlen( "to client" );

	while ( pServer )
	{
		const char *pInfo = pServer->GetClientInfo( 4 );

		pServer->PreparePackSink();

		pServer->PackDataToClient( 4, "to client", len );

		pServer->SendPackToClient();
		
		Sleep( 100 );
	}
*/
	::MessageBox( NULL, "OK", "Info", MB_OK );

	pServer->CloseService();
	
	pServer->Cleanup();
	
	pServer->Release();

	::FreeLibrary( hModule );

	return 0;
}

