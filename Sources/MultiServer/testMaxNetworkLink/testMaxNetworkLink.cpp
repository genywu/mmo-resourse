// testMaxNetworkLink.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
//#include <winsock2.h>
#include <process.h>
#include <iostream.h>
#include <stdio.h>

#include "IClient.h"
#include "RainbowInterface.h"

typedef HRESULT ( __stdcall * pfnCreateClientInterface )(
			REFIID	riid,
			void	**ppv
		);

void __stdcall ClientEventNotify(
			LPVOID lpParam,
			const unsigned long &ulnEventType )
{
	switch( ulnEventType )
	{
	case enumServerConnectCreate:
		
		break;
	case enumServerConnectClose:
		
		break;
	}
}

int main(int argc, char* argv[])
{
	bool ok = false;

	IClient *pClient = NULL;
	IClientFactory *pClientFactory = NULL;

	HMODULE hModule = ::LoadLibrary( "rainbow.dll" );
	
	if ( hModule )
	{
		pfnCreateClientInterface pFactroyFun = ( pfnCreateClientInterface )GetProcAddress( hModule, "CreateInterface" );
		
		if ( SUCCEEDED( pFactroyFun( IID_IClientFactory, reinterpret_cast< void ** >( &pClientFactory ) ) ) )
		{
			pClientFactory->SetEnvironment( 50, 8192 );

			ok = true;
		}
	}

	DWORD dwCount = 0;

	while ( ok )
	{
		pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >( &pClient ) );
				
		if ( !pClient )
		{
			return 0;
		}
		
		pClient->Startup();
		
		pClient->RegisterMsgFilter( reinterpret_cast< void * >( pClient ), ClientEventNotify );
		
		pClient->ConnectTo( "61.55.138.19", 5622 );

		static const char *pData = "How are you?";
		pClient->SendPackToServer( pData, strlen( pData ) );

		if ( dwCount ++ & 0x80000000 )
		{
			dwCount = 0;
		}

		cout << "Connections : " << dwCount << endl;
		
		pClient->Cleanup();
		
		pClient->Release();
	}

	pClientFactory->Release();		
	
	::FreeLibrary( hModule );

	return 0;
}


