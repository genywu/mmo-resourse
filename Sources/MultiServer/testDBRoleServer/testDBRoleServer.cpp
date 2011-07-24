// testDBRoleServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>

#pragma warning( disable : 4786 )

#include "KProtocolDef.h"

#include "..\..\RoleDBManager\kroledbheader.h"
#include "S3DBInterface.h"

#include "IClient.h"
#include "RainbowInterface.h"

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

using namespace std;

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
		//::MessageBox( NULL, "Server connection was created!", "Info", MB_OK );
		
		break;

	case enumServerConnectClose:
		//::MessageBox( NULL, "Server connection was destroy!", "Info", MB_OK );
		
		break;
	}
}

void SmallPackProcess( const void *pData, size_t dataLength );
void LargePackProcess( const void *pData, size_t dataLength );

unsigned __stdcall ThreadFunction( void *pParam )
{
	IClient *pClient = ( IClient * )pParam;

	ASSERT( pClient );

	while ( true )
	{
		size_t dataLength = 0;

		const void *pData = pClient->GetPackFromServer( dataLength );

		if ( !pData || 0 == dataLength )
		{
			Sleep( 1 );
			continue;
		}

		BYTE cProtocol = CPackager::Peek( pData );

		if ( cProtocol < s2c_micropackbegin )
		{
			LargePackProcess( pData, dataLength );
		}
		else if ( cProtocol > s2c_micropackbegin )
		{
			SmallPackProcess( pData, dataLength );
		}
		else
		{
			ASSERT( FALSE && "Error!" );
		}
	}

	return 0;
}

/*
 * Global variable
 */
CLibrary	g_theRainbowDll( "rainbow.dll" );

const size_t g_nIdentity = 0x20;	/* Random */
const size_t g_nRoleListCount = 3;

const char *g_pAccountName = "test";
const char *g_pAccountPassword = NULL;
const char *g_pRoleName = "test_role1";
const char *g_pDelRoleName = "test_role";
const char *g_pNewRoleName = "test_role";

CBuffer::Allocator	m_theGlobalAllocator( 1024 * 64, 10 );
CPackager			m_theRecv;

CEvent	m_theTaskEvent( NULL, false, true, "task" );

/*
 * main
 */
int main(int argc, char* argv[])
{
	setcolor( enumCyanonBlack );

	cout << "Welcome to the example that it can be to test role dbserver." << endl << endl;

	setcolor( enumDefault );

	IClient *pClient = NULL;

	pfnCreateClientInterface pFactroyFun = 
		( pfnCreateClientInterface )g_theRainbowDll.GetProcAddress( "CreateInterface" );

	IClientFactory *pClientFactory = NULL;
 
	ASSERT( pFactroyFun );

	if ( SUCCEEDED( pFactroyFun( IID_IClientFactory, reinterpret_cast< void ** >( &pClientFactory ) ) ) )
	{
		pClientFactory->SetEnvironment( 50, 8192 );
		
		pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >( &pClient ) );
		
		pClientFactory->Release();
	}

	ASSERT( pClient );

	pClient->Startup();

	pClient->RegisterMsgFilter( reinterpret_cast< void * >( pClient ), ClientEventNotify );

	if ( FAILED( pClient->ConnectTo( "192.168.22.104", 5001 ) ) )
	{
		cout << "To connectthe role datebase server is failed!" << endl;

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

_TRY_AGAIN:

	/*
	 * Require a role list from the role database server
	 */
	ASSERT( g_pAccountName );

	m_theTaskEvent.Wait();

	cout << "It get rolelist from the role db-server who is " << g_pAccountName << endl;

	const size_t lenAN = strlen( g_pAccountName );
	const size_t lenPL = lenAN + sizeof( TStreamData );
	
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();	

	TStreamData *pPlayerList = reinterpret_cast< TStreamData * >( const_cast< BYTE * >( pBuffer->GetBuffer() ) );

	ASSERT( pPlayerList );

	pPlayerList->nProtoId = c2s_roleserver_getrolelist;
	pPlayerList->nDataLen = lenAN + 1;
	pPlayerList->ulIdentity = g_nIdentity;

	pPlayerList->pDataBuffer[0] = g_nRoleListCount;
	memcpy( &pPlayerList->pDataBuffer[1], g_pAccountName, lenAN );

	pBuffer->Use( lenPL );

	pClient->SendPackToServer( ( const void * )pPlayerList, lenPL );

	SAFE_RELEASE( pBuffer );

	m_theTaskEvent.Wait();

	/*
	 * Require a role infromation from the role database server
	 */
	cout << "It get information from the role db-server who is " << g_pRoleName << endl;

	const size_t lenRN = strlen( g_pRoleName );
	const size_t lenRI = sizeof( TStreamData ) - 1 + lenRN;

	pBuffer = m_theGlobalAllocator.Allocate();
			
	TStreamData *pRoleInfo = reinterpret_cast< TStreamData * >( const_cast< BYTE * >( pBuffer->GetBuffer() ) );
	
	ASSERT( pRoleInfo );
	
	pRoleInfo->nProtoId = c2s_roleserver_getroleinfo;
	pRoleInfo->nDataLen = lenRN;
	pRoleInfo->ulIdentity = g_nIdentity;
	memcpy( &pRoleInfo->pDataBuffer[0], g_pRoleName, lenRN );
	
	pBuffer->Use( lenRI );
	
	pClient->SendPackToServer( ( const void * )pRoleInfo, lenRI );
	
	SAFE_RELEASE( pBuffer );

	m_theTaskEvent.Wait();
	
	/*
	 * Delete a role from the role database server
	 */
	cout << "It delete a role from the role db-server who is " << g_pDelRoleName << endl;

	const size_t lenDRN = strlen( g_pDelRoleName );
	const size_t lenDRI = sizeof( TStreamData ) - 1 + lenDRN;

	pBuffer = m_theGlobalAllocator.Allocate();

	TStreamData *pDelRoleInfo = reinterpret_cast< TStreamData * >( const_cast< BYTE * >( pBuffer->GetBuffer() ) );
	
	ASSERT( pDelRoleInfo );
	
	pDelRoleInfo->nProtoId = c2s_roleserver_deleteplayer;
	pDelRoleInfo->nDataLen = lenDRN;
	pDelRoleInfo->ulIdentity = g_nIdentity;
	memcpy( &pDelRoleInfo->pDataBuffer[0], g_pDelRoleName, lenDRN );
	
	pBuffer->Use( lenDRI );
	
	pClient->SendPackToServer( ( const void * )pDelRoleInfo, lenDRI );
	
	SAFE_RELEASE( pBuffer );

	//m_theTaskEvent.Wait();

	/*
	 * Create a role from inifile local
	 */
	cout << "Create a role from ini" << endl;

	/*
	 * System command
	 */
	string sInfo;
	cout << "Are your try again? ( [Y]es / [N]o )" << endl;

	cin >> sInfo;
	if ( 0 == sInfo.compare( "Y" ) || 0 == sInfo.compare( "y" ) )
	{
		m_theTaskEvent.Set();
		
		goto _TRY_AGAIN;
	}

	pClient->Shutdown();

	pClient->Cleanup();

	SAFE_RELEASE( pClient );

	return 0;
}

void SmallPackProcess( const void *pData, size_t dataLength )
{
	ASSERT( pData && dataLength );

	BYTE cProtocol = CPackager::Peek( pData );

	TProcessData *pPA = ( TProcessData * )pData;

	ASSERT( g_nIdentity == pPA->ulIdentity );

	switch ( cProtocol )
	{
	case s2c_roleserver_getrolelist_result:
		{
			int nRoleCount = *( char * )( pPA->pDataBuffer );
			const S3DBI_RoleBaseInfo *pRoleList = ( const S3DBI_RoleBaseInfo * )( ( const char * )( pPA->pDataBuffer ) + 1/* sizeof( char ) */ );

			const size_t datalength = sizeof( S3DBI_RoleBaseInfo ) * nRoleCount;

			setcolor( enumYellowonBlack );

			if ( nRoleCount <= 0 )
			{
				cout << "Don't exist anyone!" << endl;
			}

			for ( int no = 0; no < nRoleCount; no ++ )
			{
				cout << no << " ) " <<
					"[RoleName] : " << pRoleList[no].szRoleName << 
					" - [Sex] : " << ( pRoleList[no].nSex ? "girl" : "boy" ) << 
					" - [Level] : " << pRoleList[no].nLevel << endl;
			}
			
			setcolor( enumDefault );
		}
		break;

	default:
		break;
	}

	m_theTaskEvent.Set();
}

void LargePackProcess( const void *pData, size_t dataLength )
{
	ASSERT( pData && dataLength );

	CBuffer *pBuffer = m_theRecv.PackUp( pData, dataLength );

	if ( pBuffer )
	{
		BYTE cProtocol = CPackager::Peek( pBuffer->GetBuffer() );
		
		TProcessData *pPA = ( TProcessData * )( pBuffer->GetBuffer() );
		
		ASSERT( g_nIdentity == pPA->ulIdentity );

		const TRoleData *pRoleData = ( const TRoleData * )( ( const char * )( pPA->pDataBuffer ) );
		
		cout << "AccName : " << pRoleData->BaseInfo.caccname;
		cout << " - Name : " << pRoleData->BaseInfo.szName << endl;

		SAFE_RELEASE( pBuffer );

		m_theTaskEvent.Set();
	}
}