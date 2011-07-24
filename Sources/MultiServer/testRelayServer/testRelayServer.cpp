// testAccServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>

#pragma warning(disable : 4786)

#include "KProtocolDef.h"

#include "IClient.h"
#include "RainbowInterface.h"

#include "../../Sword3PaySys/S3AccServer/AccountLoginDef.h"

#include "Library.h"
#include "Macro.h"
#include "Buffer.h"
#include "Event.h"
#include "Console.h"

using OnlineGameLib::Win32::CLibrary;
using OnlineGameLib::Win32::CPackager;
using OnlineGameLib::Win32::CBuffer;
using OnlineGameLib::Win32::CEvent;
using OnlineGameLib::Win32::Console::setcolor;

#include <string>
#include <process.h>
#include <iostream>
#include <stdio.h>

#include "KRelayProtocol.h"

using namespace std;



/*
 * Global variable
 */
CLibrary	g_theRainbowDll( "rainbow.dll" );

CBuffer::Allocator	m_theGlobalAllocator( 1024 * 1024, 10 );

CEvent	m_theQuitThreadEvent( NULL, true, false, "" );
CEvent	m_theAfirmThreadQuitEvent( NULL, true, false, "" );


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
		cout << "Server connection was created !" << endl;
		
		break;

	case enumServerConnectClose:
		cout << "Server connection was destroy !"  << endl;
		m_theQuitThreadEvent.Set();
		
		break;
	}
}

unsigned __stdcall ThreadFunction( void *pParam )
{
	IClient *pClient = ( IClient * )pParam;

	ASSERT( pClient );

	while ( !m_theQuitThreadEvent.Wait( 0 ) )
	{
		size_t dataLength = 0;

		const void *pData = pClient->GetPackFromServer( dataLength );

		if ( !pData || 0 == dataLength )
		{
			Sleep( 1 );
			continue;
		}

		EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
		if (pHeader->ProtocolFamily == pf_relay)
		{
			if (pHeader->ProtocolID == relay_c2c_data)
			{
				RELAY_DATA* pRelayCmd = (RELAY_DATA*)pData;

				in_addr ia;
				ia.s_addr = pRelayCmd->nFromIP;
				cout << "relaydata: " << inet_ntoa(ia) << '(' << pRelayCmd->nFromRelayID << ')' << " -- recv size: " << dataLength << endl;
			}
			if (pHeader->ProtocolID == relay_s2c_loseway)
			{
				RELAY_DATA* pRelayCmd = (RELAY_DATA*)pData;

				in_addr ia;
				ia.s_addr = pRelayCmd->nFromIP;
				cout << "loseway: " << inet_ntoa(ia) << '(' << pRelayCmd->nFromRelayID << ')' << " -- recv size: " << dataLength << endl;
			}
		}
	}

	cout << "The read thread was killed safely!" << endl;
	m_theAfirmThreadQuitEvent.Set();

	return 0;
}

void RelayTo(IClient *pClient, DWORD ipTo, unsigned long idTo)
{
	RELAY_DATA serping;

	serping.ProtocolFamily = pf_relay;
	serping.ProtocolID = relay_c2c_data;
	serping.nFromIP = 0;
	serping.nFromRelayID = 0;
	serping.nToIP = ipTo;
	serping.nToRelayID = idTo;
	serping.routeDateLength = 0;


	pClient->SendPackToServer((const void *)&serping, sizeof(serping));

}

void ServerLogin(IClient *pClient)
{
	BYTE mem[sizeof(KServerAccountUserLoginInfo) + 2];
	BYTE* pData = mem;

	const size_t datalength = sizeof(KServerAccountUserLoginInfo) + 2;
	KServerAccountUserLoginInfo serlogin;
	serlogin.Size = sizeof(KServerAccountUserLoginInfo);
	serlogin.Type = ServerAccountUserLoginInfo;
	serlogin.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(serlogin.Account, "wanli");
	strcpy(serlogin.Password, "48DFFCD3317D5A7B94B26CDCE8710CC7");
	*pData = pf_normal;
	*(pData + 1) = c2s_gatewayverify;
	memcpy(pData + 2, &serlogin, sizeof(KServerAccountUserLoginInfo));

	pClient->SendPackToServer((const void *)pData, datalength);
}


/*
 * main
 */
int main(int argc, char* argv[])
{
	setcolor( enumCyanonBlack );

	cout << "Welcome to the example that it can be to test relay server." << endl << endl;

	setcolor( enumDefault );

	IClient *pClient = NULL;

	pfnCreateClientInterface pFactroyFun = 
		(pfnCreateClientInterface)g_theRainbowDll.GetProcAddress("CreateInterface");

	IClientFactory *pClientFactory = NULL;
 
	ASSERT(pFactroyFun);

	if (SUCCEEDED(pFactroyFun(IID_IClientFactory, reinterpret_cast< void ** >(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(8192);
		
		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast< void ** >(&pClient ));
		
		pClientFactory->Release();
	}

	ASSERT(pClient);

	pClient->Startup();

	pClient->RegisterMsgFilter( reinterpret_cast< void * >( pClient ), ClientEventNotify );

	//cout << "Relay Server IP: ";
	string relayip = "192.168.20.15";
	//cin >> relayip;

	if ( FAILED( pClient->ConnectTo( relayip.c_str(), 7777 ) ) )
	{
		cout << "To connect the account server is failed!" << endl;

		exit( -1 );
	}

	IClient *pClonClient = NULL;
	pClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pClonClient ) );

	unsigned int threadID = 0;
	
	HANDLE hThread = (HANDLE)::_beginthreadex(0,
		0, 
		ThreadFunction,
		( void * )pClonClient,
		0, 
		&threadID );

	ASSERT( hThread );
	
	SAFE_CLOSEHANDLE( hThread );		

	//m_theTaskEvent.Wait();

	/*
	 * System command
	 */
_TRY_AGAIN:
	if (argc == 1)
	{
		string sInfo;
		cout << "Command:" << endl;

		cin >> sInfo;
		if (0 == sInfo.compare( "relayto" ))
		{
			cout << "IP:";
			string sip;
			cin >> sip;
			cout << "id:";
			unsigned long id;
			cin >> id;

			for (int i = 0; i < 1000; i++)
				RelayTo(pClient, inet_addr(sip.c_str()), id);
		}
		else if (0 == sInfo.compare( "serverlogin" ))
		{
			ServerLogin(pClient);
		}
		
		if ( 0 != sInfo.compare( "exit" ))
			goto _TRY_AGAIN;
	}
	else
	{
		ServerLogin(pClient);
	}

	m_theQuitThreadEvent.Set();

	m_theAfirmThreadQuitEvent.Wait();

	pClient->Shutdown();

	pClient->Cleanup();

	SAFE_RELEASE( pClient );

	return 0;
}
