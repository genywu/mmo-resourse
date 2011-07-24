// testRainbow.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
//#include <winsock2.h>
#include <process.h>
#include <iostream.h>
#include <stdio.h>
#include "..\..\roledbserver\KRoleBlockProcess.h"


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

struct S3DBI_RoleBaseInfo
{
	char szRoleName[32];
	int	 nSex;
	int	 nHelmType;
	int	 nArmorType;
	int	 nWeaponType;
	int	 nLevel;
};

int main(int argc, char* argv[])
{
	IClient *pClient = NULL;

	HMODULE hModule = ::LoadLibrary( "rainbow.dll" );

	if ( hModule )
	{
		pfnCreateClientInterface pFactroyFun = ( pfnCreateClientInterface )GetProcAddress( hModule, "CreateInterface" );

		IClientFactory *pClientFactory = NULL;

		if ( SUCCEEDED( pFactroyFun( IID_IClientFactory, reinterpret_cast< void ** >( &pClientFactory ) ) ) )
		{
			pClientFactory->SetEnvironment( 50, 8192 );

			pClientFactory->CreateClientInterface( IID_IESClient, reinterpret_cast< void ** >( &pClient ) );

			pClientFactory->Release();
		}
	}

	pClient->Startup();

	pClient->RegisterMsgFilter( reinterpret_cast< void * >( pClient ), ClientEventNotify );

	pClient->ConnectTo( "127.0.0.1", 5001 );

	IClient *pClonClient = NULL;
	pClient->QueryInterface( IID_IESClient, reinterpret_cast< void ** >( &pClonClient ) );

	size_t datalength = 0;
	

	{
		//test getrolelist
		//data[0] = GetRoleListCount
		//data[1...n] = AccountName

		char szAccoutName[]="huyi";
		TStreamData * pGetPlayerList =(TStreamData*) new char[sizeof(szAccoutName) + sizeof(TStreamData)];
		pGetPlayerList->nDataLen = sizeof(szAccoutName);
		pGetPlayerList->nProtoId = PROTOL_GETROLELIST;
		pGetPlayerList->ulNetId = 0;
		
		pGetPlayerList->pDataBuffer[0] = 3;//Max Get 3 RoleCount
		memcpy(&pGetPlayerList->pDataBuffer[1], szAccoutName, sizeof(szAccoutName));

		
		
		KRoleBlockProcess RoleBlocks(pGetPlayerList, sizeof(szAccoutName) + sizeof(TStreamData));
		
		//Send Command
		for (int t = 0 ; t < (int)RoleBlocks.GetBlockCount(1024); t++)
		{
			KBlock * pSendBlock = (KBlock *)new char[1024] ;
			size_t nSendSize = RoleBlocks.GetBlockPtr(pSendBlock, t, 1024);
			pClient->SendPackToServer(pSendBlock, nSendSize);
			delete []pSendBlock;
		}


		//Get RoleList Result
		KRoleBlockProcess RoleListMsg;
		int i = 0;
		while(1)
		{
			const void *pData = pClient->GetPackFromServer( datalength );
			
			if ( pData && datalength )
			{
				if (i == 0)
				{
					RoleListMsg.GenStream(((KBlock*)pData)->nOffset + sizeof(TStreamData) - 1);
				}
				RoleListMsg.SetBlock((KBlock * )pData);
				i++;
				if (RoleListMsg.IsComplete()) break;//When All Block Recv, Then Break;
			}
			Sleep(10);
		}

		//(char)data[0] = RoleCount
		//data[1...n] ListInfos
		S3DBI_RoleBaseInfo * pRoleList = (S3DBI_RoleBaseInfo *) ((char *)RoleListMsg.GetProcessData() + 1);
		for (i  = 0 ; i < *(char *)RoleListMsg.GetProcessData(); i ++)
		{
			printf("Name[%s],Sex[%d], Level[%d] \n", pRoleList->szRoleName, pRoleList->nSex, pRoleList->nLevel);
			pRoleList ++;
		}


		//GetRoleAllInfomation
		pRoleList--;
		TStreamData * pGetRoleInfo = (TStreamData *) new char[sizeof(TStreamData) - 1 + strlen(pRoleList->szRoleName)];
		pGetRoleInfo->nDataLen = strlen(pRoleList->szRoleName);
		pGetRoleInfo->nProtoId = PROTOL_LOADROLE;
		memcpy(&pGetRoleInfo->pDataBuffer[0], pRoleList->szRoleName, strlen(pRoleList->szRoleName));

		KRoleBlockProcess RoleInfoMsg(pGetRoleInfo, sizeof(TStreamData) - 1 + strlen(pRoleList->szRoleName));

		for (t = 0 ; t < (int)RoleInfoMsg.GetBlockCount(MAX_BLOCK_SIZE); t++)
		{
			KBlock * pSendBlock = ( KBlock * )new char[MAX_BLOCK_SIZE] ;
			size_t nSendSize = RoleInfoMsg.GetBlockPtr(pSendBlock, t, MAX_BLOCK_SIZE);
			pClient->SendPackToServer(pSendBlock, nSendSize);
			delete []pSendBlock;
		}

		KRoleBlockProcess RoleInfo;
		i = 0;
		while(1)
		{
			const void *pData = pClient->GetPackFromServer( datalength );
			
			
			if ( pData && datalength )
			{
				if (i == 0)
				{
					RoleInfo.GenStream(((KBlock*)pData)->nOffset + sizeof(TStreamData) -1);
				}
				i++;
				RoleInfo.SetBlock((KBlock * )pData);
				if (RoleInfo.IsComplete()) break;
			}
			Sleep(10);
		}

		RoleInfo.m_pProcessData->nProtoId = PROTOL_ONLYSAVEROLE;
		for (i = 0; i < (int)RoleInfo.GetBlockCount(MAX_BLOCK_SIZE); i ++)
		{
			KBlock * pSendBlock = ( KBlock * )new char[MAX_BLOCK_SIZE] ;
			size_t nSendSize = RoleInfo.GetBlockPtr(pSendBlock, i, MAX_BLOCK_SIZE);
			pClient->SendPackToServer(pSendBlock, nSendSize);
			delete []pSendBlock;
			Sleep(10);
		}

	}

	pClient->Cleanup();

	pClient->Release();
	
	::FreeLibrary( hModule );

	return 0;
}
