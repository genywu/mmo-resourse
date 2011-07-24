// testAccServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>

#pragma warning(disable : 4786)

#include "KProtocolDef.h"

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

#include "../../Sword3PaySys/S3AccServer/AccountLoginDef.h"

using namespace std;

#pragma pack(push, 1)
typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwTime;
} PING_COMMAND;
#pragma pack(pop)

/*
 * Global variable
 */
CLibrary	g_theRainbowDll( "rainbow.dll" );

CBuffer::Allocator	m_theGlobalAllocator( 1024 * 1024, 10 );

CEvent	m_theQuitThreadEvent( NULL, true, false, NULL );
CEvent	m_theAfirmThreadQuitEvent( NULL, true, false, NULL );

//const char *g_pServerIP = "192.168.22.105";
const char *g_pServerIP = "192.168.20.15";
const short g_nPort = 5002;

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

#define MAXFAST 4000

unsigned __stdcall ThreadFunction( void *pParam )
{
	IClient *pClient = ( IClient * )pParam;

	ASSERT( pClient );

	int nMax = 0;

	while ( !m_theQuitThreadEvent.Wait( 0 ) )
	{
		size_t dataLength = 0;

		const void *pData = pClient->GetPackFromServer( dataLength );

		if ( !pData || 0 == dataLength )
		{
			Sleep( 1 );
			continue;
		}

		nMax++;
		if (nMax < MAXFAST * 4)
			continue;
		BYTE cProtocol = CPackager::Peek( pData );

		switch ( cProtocol )
		{
		case s2c_gatewayverify:
			{
				KAccountUserReturn* pReturn = (KAccountUserReturn*)(((char*)pData) + 1);
				dataLength--;
				if (pReturn && dataLength == sizeof(KAccountUserReturn))
				{
					cout << "server " << pReturn->Account << " ";
					if (pReturn->nReturn == ACTION_SUCCESS)
						cout << "Login Success !";
					else if (pReturn->nReturn == E_ACCOUNT_OR_PASSWORD)
						cout << "Login Failed ! --- Name, Password";
					else if (pReturn->nReturn == E_ACCOUNT_EXIST)
						cout << "Login Failed ! --- Already login";
					else if (pReturn->nReturn == E_ADDRESS_OR_PORT)
						cout << "Login Failed ! --- Address, Port";
					else
						cout << "Login Failed !";
					cout << endl;
				}
			}
			break;
		case s2c_accountlogin:
			{
				KAccountUserReturn* pReturn = (KAccountUserReturn*)(((char*)pData) + 1);
				dataLength--;
				if (pReturn && dataLength == sizeof(KAccountUserReturn))
				{
					cout << "user " << pReturn->Account << " ";
					if (pReturn->nReturn == ACTION_SUCCESS)
						cout << "Login Success !";
					else if (pReturn->nReturn == E_ACCOUNT_OR_PASSWORD)
						cout << "Login Failed ! --- Name, Password";
					else if (pReturn->nReturn == E_ACCOUNT_EXIST)
						cout << "Login Failed ! --- Already login";
					else if (pReturn->nReturn == E_ACCOUNT_NODEPOSIT)
						cout << "Login Failed ! --- No Money";
					else
						cout << "Login Failed !";
					cout << endl;
				}
			}
			break;

		case s2c_gamelogin:
			{
				KAccountUserReturn* pReturn = (KAccountUserReturn*)(((char*)pData) + 1);
				dataLength--;
				if (pReturn && dataLength == sizeof(KAccountUserReturn))
				{
					cout << "user " << pReturn->Account << " ";
					if (pReturn->nReturn == ACTION_SUCCESS)
						cout << "Login Game Success !";
					else if (pReturn->nReturn == E_ACCOUNT_OR_PASSWORD)
						cout << "Login Game Failed ! --- Name, Password";
					else if (pReturn->nReturn == E_ACCOUNT_ACCESSDENIED)
						cout << "Login Game Failed ! --- login first";
					else
						cout << "Login Game Failed !";
					cout << endl;
				}
			}
			break;
		case s2c_accountlogout:
			{
				KAccountUserReturn* pReturn = (KAccountUserReturn*)(((char*)pData) + 1);
				dataLength--;
				if (pReturn && dataLength == sizeof(KAccountUserReturn))
				{
					cout << "user " << pReturn->Account << " ";
					if (pReturn->nReturn == ACTION_SUCCESS)
						cout << "Logout Success !";
					else if (pReturn->nReturn == E_ACCOUNT_OR_PASSWORD)
						cout << "Logout Failed ! --- Name, Password";
					else if (pReturn->nReturn == E_ACCOUNT_ACCESSDENIED)
						cout << "Logout Failed ! --- login first";
					else
						cout << "Logout Failed !";
					cout << endl;
				}
			}
			break;
		case s2c_gatewayinfo:
			{
				KServerInfo* pReturn = (KServerInfo*)(((char*)pData) + 1);
				dataLength--;
				if (pReturn && dataLength == sizeof(KServerInfo))
				{
					struct in_addr addr;
					
					switch (pReturn->nServerType)
					{
					case server_Login:
						cout << "server " << pReturn->Account << " ";
						addr.s_addr = pReturn->nValue;
						cout << "Login Success ! IP is " << inet_ntoa(addr);
						break;
					case server_Logout:
						cout << "server " << pReturn->Account << " ";
						addr.s_addr = pReturn->nValue;
						cout << "Logout Success ! IP is " << inet_ntoa(addr);
						break;
					case server_RegisterCount:
						cout << "server " << "total" << " ";
						cout << "Register User Count is " << pReturn->nValue;
						break;
					case server_OnlinePlayerCount:
						if (pReturn->Account[0] != 0)
							cout << "server " << pReturn->Account << " ";
						else
							cout << "server " << "total" << " ";
						cout << "Online Account Count is " << pReturn->nValue;
						break;
					case server_PlayerWhere:
						cout << "Player " << pReturn->Account << " ";
						addr.s_addr = pReturn->nValue;
						cout << "is login at Server " << inet_ntoa(addr);
						break;
					}
					cout << endl;
				}
			}
			break;
		case s2c_ping:
			{
				PING_COMMAND* pReturn = (PING_COMMAND*)pData;
				if (pReturn && dataLength == sizeof(PING_COMMAND))
				{
					cout << "server ping: " << pReturn->m_dwTime << " ";
					cout << endl;
				}
			}
			break;
		default:
			break;
		}
	}

	cout << "The read thread was killed safely!" << endl;
	m_theAfirmThreadQuitEvent.Set();

	return 0;
}


void ServerLogin(IClient *pClient)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KServerAccountUserLoginInfo) + 1;
	KServerAccountUserLoginInfo serlogin;
	serlogin.Size = sizeof(KServerAccountUserLoginInfo);
	serlogin.Type = ServerAccountUserLoginInfo;
	serlogin.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(serlogin.Account, "wanli");
	strcpy(serlogin.Password, "48DFFCD3317D5A7B94B26CDCE8710CC7");
	*pData = c2s_gatewayverify;
	memcpy(pData + 1, &serlogin, sizeof(KServerAccountUserLoginInfo));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void ServerLoginErr(IClient *pClient)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KServerAccountUserLoginInfo) + 1;
	KServerAccountUserLoginInfo serlogin;
	serlogin.Size = sizeof(KServerAccountUserLoginInfo);
	serlogin.Type = ServerAccountUserLoginInfo;
	serlogin.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(serlogin.Account, "wanli2");
	strcpy(serlogin.Password, "wanli");
	*pData = c2s_gatewayverify;
	memcpy(pData + 1, &serlogin, sizeof(KServerAccountUserLoginInfo));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}


void ServerLoginAgain(IClient *pClient)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KServerAccountUserLoginInfo) + 1;
	KServerAccountUserLoginInfo serlogin;
	serlogin.Size = sizeof(KServerAccountUserLoginInfo);
	serlogin.Type = ServerAccountUserLoginInfo;
	serlogin.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(serlogin.Account, "wanli");
	strcpy(serlogin.Password, "48DFFCD3317D5A7B94B26CDCE8710CC7");
	*pData = c2s_gatewayverifyagain;
	memcpy(pData + 1, &serlogin, sizeof(KServerAccountUserLoginInfo));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void UserLogin(IClient *pClient, const char* user, const char* password)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KAccountUserLoginInfo) + 1;
	KAccountUserLoginInfo userlogin;
	userlogin.Size = sizeof(KAccountUserLoginInfo);
	userlogin.Type = AccountUserLoginInfo;
	userlogin.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(userlogin.Account, user);
	strcpy(userlogin.Password, password);
	*pData = c2s_accountlogin;
	memcpy(pData + 1, &userlogin, sizeof(KAccountUserLoginInfo));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void UserLogin1(IClient *pClient)
{
	UserLogin(pClient, "wanlitest", "A63757F6C20852995151276AC40767EA");
}

void UserLogin2(IClient *pClient)
{
	UserLogin(pClient, "wanli2", "wanli2");
}

void UserLoginAll(IClient *pClient)
{
	char user[2];
	user[0] = 'a';
	user[1] = 0;
	char password[2];
	password[0] = 'a';
	password[1] = 0;
	for (int i = 0; i < 10; i++)
	{
		UserLogin(pClient, user, password);
		user[0]++;
		password[0]++;
	}
}

void UserLogout(IClient *pClient, const char* szAccount)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KAccountUser) + 1;
	KAccountUser user;
	user.Size = sizeof(KAccountUser);
	user.Type = AccountUser;
	user.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(user.Account, szAccount);
	*pData = c2s_accountlogout;
	memcpy(pData + 1, &user, sizeof(KAccountUser));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void UserLogout1(IClient *pClient)
{
	UserLogout(pClient, "wanlitest");
}

void UserGameLogin(IClient *pClient)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KAccountUser) + 1;
	KAccountUser user;
	user.Size = sizeof(KAccountUser);
	user.Type = AccountUser;
	user.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(user.Account, "wanlitest");
	*pData = c2s_gamelogin;
	memcpy(pData + 1, &user, sizeof(KAccountUser));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void GatewayInfo(IClient *pClient, short nType, const char* szAccount)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(KServerInfo) + 1;
	KServerInfo user;
	user.Size = sizeof(KServerInfo);
	user.Type = ServerInfo;
	user.Version = ACCOUNT_CURRENT_VERSION;
	strcpy(user.Account, szAccount);
	user.nServerType = nType;
	*pData = c2s_gatewayinfo;
	memcpy(pData + 1, &user, sizeof(KServerInfo));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

void ServerPing(IClient *pClient)
{
	CBuffer *pBuffer = m_theGlobalAllocator.Allocate();

	BYTE *pData = const_cast< BYTE * >( pBuffer->GetBuffer() );

	const size_t datalength = sizeof(PING_COMMAND);
	PING_COMMAND serping;
	serping.ProtocolType = c2s_ping;
	serping.m_dwTime = GetTickCount();
	memcpy(pData, &serping, sizeof(PING_COMMAND));

	pBuffer->Use(datalength);

	pClient->SendPackToServer((const void *)pData, datalength);

	SAFE_RELEASE(pBuffer);
}

/*
 * main
 */
int main(int argc, char* argv[])
{
	setcolor( enumCyanonBlack );

	cout << "Welcome to the example that it can be to test account server." << endl << endl;

	setcolor( enumDefault );

	IClient *pClient = NULL;

	pfnCreateClientInterface pFactroyFun = 
		(pfnCreateClientInterface)g_theRainbowDll.GetProcAddress("CreateInterface");

	IClientFactory *pClientFactory = NULL;
 
	ASSERT(pFactroyFun);

	if (SUCCEEDED(pFactroyFun(IID_IClientFactory, reinterpret_cast< void ** >(&pClientFactory))))
	{
		pClientFactory->SetEnvironment(90 * 1024);
		
		pClientFactory->CreateClientInterface(IID_IESClient, reinterpret_cast< void ** >(&pClient ));
		
		pClientFactory->Release();
	}

	ASSERT(pClient);

	pClient->Startup();

	pClient->RegisterMsgFilter( reinterpret_cast< void * >( pClient ), ClientEventNotify );

	if ( FAILED( pClient->ConnectTo( g_pServerIP, g_nPort ) ) )
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
	
	if (argc == 1)
		ServerLogin(pClient);
	else if (argc == 2)
		ServerLoginAgain(pClient);
	else
		ServerLoginErr(pClient);
		

	//m_theTaskEvent.Wait();

	/*
	 * System command
	 */
_TRY_AGAIN:
	string sInfo;
	cout << "Command:" << endl;

	cin >> sInfo;
	if ( 0 == sInfo.compare( "userlogin1" ))
	{
		UserLogin1(pClient);
	}
	else if ( 0 == sInfo.compare( "userlogin" ))
	{
		string sUser;
		string sPassword;
		cout << "Account:" << endl;
		cin >> sUser;
		cout << "Password:" << endl;
		cin >> sPassword;
		UserLogin(pClient, sUser.c_str(), sPassword.c_str());
	}
	else if ( 0 == sInfo.compare( "userlogin2" ))
	{
		UserLogin2(pClient);
	}
	else if ( 0 == sInfo.compare( "userloginall" ))
	{
		UserLoginAll(pClient);
	}
	else if ( 0 == sInfo.compare( "fast" ))
	{
		for (int i = 0; i < MAXFAST; i++)
		{
			UserLogin1(pClient);
			UserGameLogin(pClient);
			UserLogout1(pClient);
			ServerPing(pClient);
		}
	}
	else if ( 0 == sInfo.compare( "usergamelogin" ))
	{
		UserGameLogin(pClient);
	}
	else if ( 0 == sInfo.compare( "userlogout" ))
	{
		string sUser;
		cout << "Account:" << endl;
		cin >> sUser;
		UserLogout(pClient, sUser.c_str());
	}
	else if ( 0 == sInfo.compare( "userlogout1" ))
	{
		UserLogout1(pClient);
	}
	else if ( 0 == sInfo.compare( "ping" ))
	{
		ServerPing(pClient);
	}
	else if ( 0 == sInfo.compare( "gatewayinfo" ))
	{
		string sUser;
		cout << "Account:" << endl;
		cin >> sUser;
		GatewayInfo(pClient, server_OnlinePlayerCount, sUser.c_str());
		GatewayInfo(pClient, server_RegisterCount, "");
		GatewayInfo(pClient, server_PlayerWhere, sUser.c_str());
	}
	
	if ( 0 != sInfo.compare( "exit" ))
		goto _TRY_AGAIN;

	m_theQuitThreadEvent.Set();

	m_theAfirmThreadQuitEvent.Wait();

	pClient->Shutdown();

	pClient->Cleanup();

	SAFE_RELEASE( pClient );

	return 0;
}
