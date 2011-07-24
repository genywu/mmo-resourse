//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSG_RoleProcess.cpp
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   2003-9-15 7:45:21
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#pragma warning( disable : 4786 ) 

#include "winsock2.h"
#include "CriticalSection.h"
#include "KSG_RoleProcess.h"

#include <vector>
#include <map>
#include <algorithm>

using namespace std;
using namespace OnlineGameLib::Win32;

#include "kprotocoldef.h"
#include "KProtocol.h"
#include "../../../../Headers/KGmProtocol.h"

#include "IDBRoleServer.h"

#include "Macro.h"

#include "RoleNameFilter.h"


#define _NAME_LENGTH 64



typedef struct _THREAD_PARAM
{
    HANDLE          hThread;
    SOCKET          hSocket;
    long            lID;
    HWND            hMainWnd;

    unsigned char   InBuffer [64 * 1024];
    unsigned char   OutBuffer[64 * 1024];

} THREAD_PARAM;

typedef void (*ProcessArray[c2s_end])(THREAD_PARAM *pParam,  const void *pData, size_t dataLength);

ProcessArray g_theProcessArray;

void _QueryRoleList(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _CreateRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _SaveRoleInfo(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _DelRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _GetRoleInfo(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _RelayExtend(THREAD_PARAM *pParam, const void *pData, size_t dataLength);
void _GetGameStat(THREAD_PARAM *pParam, const void *pData, size_t dataLength);//发送游戏统计数据(By Fellow,2003.7.22)
void _LockOrUnlockRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength);

bool IsRoleLock(char* szRole);
bool IsRoleLockBySelf(size_t ID, char* szRole);
bool UnlockRoleSelf(size_t ID, char* szRole);
void UnlockAllRole(size_t ID);



static int gs_nWSAStartupFlag = false;

static SOCKET gs_sckListen = INVALID_SOCKET;
static int    gs_nExitFlag = false;

static long g_lID = 0;

static CCriticalSection     gs_ThreadsLock;

typedef vector<THREAD_PARAM *>  ThreadsType; 
static  ThreadsType             gs_Threads;

HWND    g_hMainWnd = NULL;

#define SOCKET_VERSION_REQUIRED     0x202

int KSG_RoleProcess_Init(int nPort, HWND hMainWnd)
{
    int nResult  = false;
    int nRetCode = false;
    int nWSAStartupFlag = false;


    memset(&g_theProcessArray, 0, sizeof(g_theProcessArray));

	g_theProcessArray[c2s_roleserver_getrolelist] = _QueryRoleList;
	g_theProcessArray[c2s_roleserver_createroleinfo] = _CreateRole;
	g_theProcessArray[c2s_roleserver_saveroleinfo] = _SaveRoleInfo;
	g_theProcessArray[c2s_roleserver_deleteplayer] = _DelRole;
	g_theProcessArray[c2s_roleserver_getroleinfo] = _GetRoleInfo;
    //g_theProcessArray[c2s_extend] = _RelayExtend;
	g_theProcessArray[c2s_gamestatistic] = _GetGameStat;
	g_theProcessArray[c2s_roleserver_lock] = _LockOrUnlockRole;

    g_hMainWnd = hMainWnd;

    WSADATA WSAData = { 0 };
    struct sockaddr_in addr = { 0 };

    nRetCode = WSAStartup(SOCKET_VERSION_REQUIRED, &WSAData);
    if (nRetCode != 0)
        goto Exit0;
    
    gs_nWSAStartupFlag = true;

    gs_Threads.reserve(256);

    gs_sckListen = socket(AF_INET, SOCK_STREAM, 0);
    if (gs_sckListen == INVALID_SOCKET)
        goto Exit0;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(nPort);

    nRetCode = bind(gs_sckListen, (struct sockaddr *)&addr, sizeof(addr));
    if (nRetCode == SOCKET_ERROR)
        goto Exit0;

    nRetCode = listen(gs_sckListen, 5);
    if (nRetCode == SOCKET_ERROR)
        goto Exit0;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (gs_sckListen != INVALID_SOCKET)
        {
            closesocket(gs_sckListen);
            gs_sckListen = INVALID_SOCKET;
        }
        

        if (gs_nWSAStartupFlag)
        {
            WSACleanup();
            gs_nWSAStartupFlag = false; 
        }
    }
    return nResult;
}

int KSG_RoleProcess_UnInit()
{
    int nResult  = false;
    int nRetCode = false;
    
    {
        CCriticalSection::Owner lock(gs_ThreadsLock);
        
        gs_nExitFlag = true;    // 设置退出标志

        ThreadsType::iterator pPos;
        for (pPos = gs_Threads.begin(); pPos != gs_Threads.end(); ++pPos)
        {
            if (((*pPos)->hSocket) != INVALID_SOCKET)
            {
		        LINGER lingerStruct;
		    
		        lingerStruct.l_onoff = 1;
		        lingerStruct.l_linger = 0;
		    
                setsockopt( 
                    (*pPos)->hSocket, 
				    SOL_SOCKET, 
				    SO_LINGER, 
				    (char *)&lingerStruct, 
				    sizeof(lingerStruct)
                );

                closesocket((*pPos)->hSocket);
            }
        }


        if (gs_sckListen != INVALID_SOCKET)
        {
		    LINGER lingerStruct;
		
		    lingerStruct.l_onoff = 1;
		    lingerStruct.l_linger = 0;
		
            // notify accept thread to exit
            setsockopt( 
                gs_sckListen, 
				SOL_SOCKET, 
				SO_LINGER, 
				(char *)&lingerStruct, 
				sizeof(lingerStruct)
            );
        
            closesocket(gs_sckListen);
            gs_sckListen = INVALID_SOCKET;
        }
    }

    while (true)
    {
        {
            CCriticalSection::Owner lock(gs_ThreadsLock);

            if (gs_Threads.empty())
                break;
        }

        Sleep(250);
    }

    nRetCode = WSACleanup();
    if (nRetCode != 0)
        goto Exit0;

    nResult = true;
Exit0:
    return nResult;
}


static inline int _RecvBuffer(SOCKET sckSocket, int nBufferSize, unsigned char *pbyBuffer)
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(sckSocket != INVALID_SOCKET);
    ASSERT(nBufferSize > 0);
    ASSERT(pbyBuffer != NULL);

    while (nBufferSize > 0)
    {
        nRetCode = recv(sckSocket, (char *)pbyBuffer, nBufferSize, 0);
        if (
            (nRetCode == SOCKET_ERROR) ||
            (nRetCode == 0)
        )
            goto Exit0;

        pbyBuffer += nRetCode;
        nBufferSize -= nRetCode;
    }


    nResult = true;
Exit0:
    return nResult;
}

static inline int _SendBuffer(SOCKET sckSocket, int nBufferSize, unsigned char *pbyBuffer)
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(sckSocket != INVALID_SOCKET);
    ASSERT(nBufferSize > 0);
    ASSERT(pbyBuffer != NULL);

    while (nBufferSize > 0)
    {
        nRetCode = send(sckSocket, (char *)pbyBuffer, nBufferSize, 0);
        if (nRetCode == SOCKET_ERROR)
            goto Exit0;

        pbyBuffer += nRetCode;
        nBufferSize -= nRetCode;
    }


    nResult = true;
Exit0:
    return nResult;
}

static DWORD WINAPI KSG_RoleProcessThread(LPVOID lpParam)
{
    int nResult  = false;
    int nRetCode = false;
    struct sockaddr_in addr_from = { 0 };
    int addr_from_len = sizeof(addr_from);

    unsigned uProcessDataSize     = 0;
    unsigned char *pbyProcessData = NULL;
    int  nNotifyAddClientFlag = false;

    SOCKET sckSocketProcess = INVALID_SOCKET;
    THREAD_PARAM *pThreadParam = (THREAD_PARAM *)lpParam;

    if (!pThreadParam)
        goto Exit0;

    if (gs_nExitFlag)
        goto Exit1;

    pThreadParam->lID = InterlockedIncrement(&g_lID);

    sckSocketProcess = accept(gs_sckListen, (struct sockaddr *)&addr_from, &addr_from_len);
    if (sckSocketProcess == INVALID_SOCKET)
        goto Exit0;

    pThreadParam->hSocket = sckSocketProcess;

    if (gs_nExitFlag)
        goto Exit0;

    // notfiy Add Client
    if (pThreadParam->hMainWnd)
    {
        ::SendMessage(pThreadParam->hMainWnd, WM_ADD_CLIENT, (WPARAM)addr_from_len, (LPARAM)&addr_from);
        nNotifyAddClientFlag = true;
    }

    nRetCode = KSG_RoleProcess_Start();
    if (!nRetCode)
        goto Exit0;

    
    while (true)
    {
        if (gs_nExitFlag)
            goto Exit0;

        nRetCode = _RecvBuffer(sckSocketProcess, sizeof(WORD), pThreadParam->InBuffer);
        if (!nRetCode)
            goto Exit0;

        uProcessDataSize = *((WORD *)pThreadParam->InBuffer);
        uProcessDataSize -= sizeof(WORD);

        pbyProcessData = (pThreadParam->InBuffer) + sizeof(WORD);

        nRetCode = _RecvBuffer(sckSocketProcess, uProcessDataSize, pbyProcessData);
        if (!nRetCode)
            goto Exit0;


		BYTE cProtocol;

        cProtocol = *pbyProcessData;

		if ( 
            (cProtocol < c2s_end) && 
            (g_theProcessArray[cProtocol]) 
        )
		{
			(*g_theProcessArray[cProtocol])(pThreadParam, (const void *)pbyProcessData, uProcessDataSize);
		}
    }


Exit1:
    nResult = true;
Exit0:

    if (nNotifyAddClientFlag)
    {
        if (!gs_nExitFlag)
        {
            if (pThreadParam->hMainWnd)
            {
                ::SendMessage(pThreadParam->hMainWnd, WM_DEL_CLIENT, (WPARAM)addr_from_len, (LPARAM)&addr_from);
            }
        }

        nNotifyAddClientFlag = false;
    }


    UnlockAllRole(pThreadParam->lID);

    {
        CCriticalSection::Owner lock(gs_ThreadsLock);

        ThreadsType::iterator pPos;
        
        pPos = find(gs_Threads.begin(), gs_Threads.end(), pThreadParam);
        if (pPos != gs_Threads.end())
        {
            CloseHandle((*pPos)->hThread);
            *pPos = NULL;
    
            gs_Threads.erase(pPos);
        }
   
        if (sckSocketProcess != INVALID_SOCKET)
        {
		    LINGER lingerStruct;
	    
		    lingerStruct.l_onoff = 1;
		    lingerStruct.l_linger = 0;
	    
            // notify accept thread to exit
            setsockopt( 
                gs_sckListen, 
			    SOL_SOCKET, 
			    SO_LINGER, 
			    (char *)&lingerStruct, 
			    sizeof(lingerStruct)
            );

            closesocket(sckSocketProcess);
            sckSocketProcess = INVALID_SOCKET;
        }
    }

    if (pThreadParam)
    {
        delete pThreadParam;
        pThreadParam = NULL;
    }

    return nResult;
}


int KSG_RoleProcess_Start()
{
    int nResult  = false;
    int nRetCode = false;
    HANDLE hThread = NULL;
    DWORD dwThreadID = 0;

    THREAD_PARAM *pThreadParam = NULL;

    pThreadParam = new THREAD_PARAM;
    if (!pThreadParam)
        goto Exit0;

    pThreadParam->hMainWnd = g_hMainWnd;
    pThreadParam->hSocket  = INVALID_SOCKET;

    hThread = CreateThread(
        NULL, 
        0,
        KSG_RoleProcessThread,
        (LPVOID)pThreadParam,
        CREATE_SUSPENDED,
        &dwThreadID
    );
    if (!hThread)
        goto Exit0;

    {
        CCriticalSection::Owner lock(gs_ThreadsLock);

        pThreadParam->hThread = hThread;
        gs_Threads.push_back(pThreadParam);

        ResumeThread(hThread);

        hThread = NULL;
        dwThreadID = 0;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pThreadParam)
        {
            delete pThreadParam;
            pThreadParam = NULL;
        }
    }

    return nResult;
}


/////////////////////////////////////////////////////////////////////////////////

// 处理角色锁定
typedef map< std::string, size_t >	stdRoleLockMap;

stdRoleLockMap      gs_csRoleLock;
CCriticalSection	gs_csCR;

void _LockOrUnlockRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength)	//强行加解锁
{
	CCriticalSection::Owner lock( gs_csCR );
	if (pData && dataLength == sizeof(tagRoleEnterGame))
	{
		tagRoleEnterGame* pRole = (tagRoleEnterGame*)pData;
		char szRole[32];
		strncpy(szRole, pRole->Name, 31);
		szRole[31] = 0;
		bool bLock = pRole->bLock;

		if (szRole && szRole[0] != 0)
		{
			if (bLock)
				gs_csRoleLock[szRole] = pParam->lID;
			else
			{
				stdRoleLockMap::iterator it = gs_csRoleLock.find(szRole);
				if (it != gs_csRoleLock.end())
					gs_csRoleLock.erase(it);
			}
		}
	}
}

bool IsRoleLock(char* szRole)
{
	CCriticalSection::Owner lock( gs_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = gs_csRoleLock.find(szRole);
		if (it != gs_csRoleLock.end() && it->second != -1)
			return true;
	}
	return false;
}

bool IsRoleLockBySelf(size_t ID, char* szRole)
{
	CCriticalSection::Owner lock( gs_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = gs_csRoleLock.find(szRole);
		if (it != gs_csRoleLock.end() && it->second == ID)
			return true;
	}
	return false;
}

bool UnlockRoleSelf(size_t ID, char* szRole)
{
	CCriticalSection::Owner lock( gs_csCR );
	if (szRole && szRole[0] != 0)
	{
		stdRoleLockMap::iterator it = gs_csRoleLock.find(szRole);
		if (it != gs_csRoleLock.end() && it->second == ID)
		{
			gs_csRoleLock.erase(it);
			return true;
		}
	}
	return false;
}

void UnlockAllRole(size_t ID)
{
	CCriticalSection::Owner lock( gs_csCR );
	stdRoleLockMap::iterator it = gs_csRoleLock.begin();
	while (it != gs_csRoleLock.end())
	{
		if (it->second == ID)
			it->second = -1;
		it++;
	}
}

/////////////////////////////////////////////////////////////////////////////////
    
//============= Process Data =======================

void _QueryRoleList(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
	char szAccountName[_NAME_LENGTH];

	ASSERT( pParam && pData && dataLength);

    #ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::Begin" << endl;
    #endif

	TProcessData *pPlayerList = ( TProcessData * )pData;

	int nRoleListCount = pPlayerList->pDataBuffer[0];
	int nLen = pPlayerList->nDataLen;
	unsigned long ulIdentity = pPlayerList->ulIdentity;

	if ( nLen <= 1 || nLen >= _NAME_LENGTH )
	{
        #ifdef CONSOLE_DEBUG
		cout << "_QueryRoleList::Name is invalid" << endl;
        #endif

		return;
	}

	memcpy( szAccountName, ( const char * )( &pPlayerList->pDataBuffer[1] ), nLen - 1 );
	szAccountName[nLen - 1] = '\0';

	/*
	 * Database
	 */
	S3DBI_RoleBaseInfo DBI[4];
	const size_t s_nStructSize = sizeof( S3DBI_RoleBaseInfo );

    #ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::GetRoleListOfAccount " << szAccountName << endl;
    #endif

	int nCount = GetRoleListOfAccount( szAccountName, &DBI[0], nRoleListCount );

	TProcessData *pListData = (TProcessData *)((pParam->OutBuffer) + sizeof(WORD));

	pListData->nProtoId = s2c_roleserver_getrolelist_result;
	pListData->ulIdentity = ulIdentity;

	int nDataLen = nCount * s_nStructSize + 1;
	pListData->nDataLen = nDataLen;
	
	pListData->pDataBuffer[0] = nCount;
	memcpy( &pListData->pDataBuffer[1], &DBI[0], nDataLen );

	int nUsedLen = sizeof( TProcessData ) - 1 + nDataLen;

    *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

    _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);

    #ifdef CONSOLE_DEBUG
	cout << "_QueryRoleList::end" << endl;
    #endif
}

void _CreateRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
	ASSERT( pParam && pData && dataLength);

    #ifdef CONSOLE_DEBUG
	cout << "_CreateRole::Begin" << endl;
    #endif

	TProcessData *pPD = ( TProcessData * )pData;

	int nResult = 0;

	{{
	extern CRoleNameFilter g_fltRoleName;

	TRoleData* pRoleData = (TRoleData*)(pPD->pDataBuffer + 1);

	if (pRoleData->BaseInfo.szName[0])
	{
		for (size_t pos = sizeof(pRoleData->BaseInfo.szName) - 1; pos >= 1; pos--)
		{
			if (!pRoleData->BaseInfo.szName[pos])
				break;
		}
		if (pos >= 1)
		{
			if (g_fltRoleName.IsTextPass(pRoleData->BaseInfo.szName))
				nResult = SaveRoleInfo( &pPD->pDataBuffer[1], NULL, TRUE );
			else
				nResult = -1;
		}
	}
	}}

	TProcessData *pInfo = (TProcessData *)(pParam->OutBuffer + sizeof(WORD));

	pInfo->nProtoId = s2c_roleserver_createrole_result;
	pInfo->ulIdentity = pPD->ulIdentity;
	pInfo->nDataLen = 1;
	pInfo->pDataBuffer[0] = ( nResult == 1 ) ? 1 : -1;

    int nUsedLen = sizeof(TProcessData);

    *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

    _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);

    #ifdef CONSOLE_DEBUG
	cout << "_CreateRole::End" << endl;
    #endif
}

void _SaveRoleInfo(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
	ASSERT( pParam && pData && dataLength);
    
    #ifdef CONSOLE_DEBUG
	cout << "_SaveRoleInfo::Begin" << endl;
    #endif

	TProcessData *pPD = ( TProcessData * )pData;
	
	int nResult = 0;
	TRoleData* pRole = (TRoleData*)(&pPD->pDataBuffer[0]);
	char szName[32];
	int len = strlen( pRole->BaseInfo.szName );
	ASSERT( len > 0 );
	len = len > 31 ? 31 : len;
	memcpy( szName, pRole->BaseInfo.szName, len );
	szName[len] = '\0';

	if (IsRoleLockBySelf(pParam->lID, szName))
	{
		nResult = SaveRoleInfo( &pPD->pDataBuffer[0], NULL, FALSE );
		if (pPD->bLeave)
		{
			UnlockRoleSelf(pParam->lID, szName);
		}
	}
	else
		nResult = 0;

	if ( pPD->ulIdentity >= 0 )
	{
    	TProcessData *pInfo = (TProcessData *)(pParam->OutBuffer + sizeof(WORD));
		
		pInfo->nProtoId = s2c_roleserver_saverole_result;
		pInfo->ulIdentity = pPD->ulIdentity;
		pInfo->nDataLen = 1;
		pInfo->pDataBuffer[0] = ( nResult == 1 ) ? 1 : -1;
		

        int nUsedLen = sizeof(TProcessData);

        *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

        _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);
	}

    #ifdef CONSOLE_DEBUG
	cout << "_SaveRoleInfo::End" << endl;
    #endif
}

void _DelRole(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
	ASSERT( pParam && pData && dataLength);

    #ifdef CONSOLE_DEBUG
	cout << "_DelRole::Begin" << endl;
    #endif

	TProcessData *pRoleInfo = ( TProcessData * )pData;

	char szRoleName[_NAME_LENGTH];

	int nDataLen = pRoleInfo->nDataLen;
	nDataLen = ( nDataLen > _NAME_LENGTH ) ? _NAME_LENGTH : nDataLen;

	int result = -1;

	if ( nDataLen > 0 )
	{
		memcpy( szRoleName, &pRoleInfo->pDataBuffer[0], nDataLen );
		szRoleName[nDataLen] = '\0';

        #ifdef CONSOLE_DEBUG
		cout << "_DelRole::DeleteRole " << szRoleName << endl;
        #endif

		if ( DeleteRole( szRoleName ) )
		{
			result = 1;
		}
		else
		{
			result = -1;
		}
	}

	if ( pRoleInfo->ulIdentity >= 0 )
	{
    	TProcessData *pInfo = (TProcessData *)(pParam->OutBuffer + sizeof(WORD));
		
		pInfo->nProtoId = s2c_roleserver_deleterole_result;
		pInfo->ulIdentity = pRoleInfo->ulIdentity;
		pInfo->nDataLen = 1;
		pInfo->pDataBuffer[0] = result;
		
        int nUsedLen = sizeof(TProcessData);

        *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

        _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);
	}

    #ifdef CONSOLE_DEBUG	
	cout << "_DelRole::End" << endl;
    #endif
}

void _GetRoleInfo(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
	ASSERT( pParam && pData && dataLength);

    #ifdef CONSOLE_DEBUG
	cout << "_GetRoleInfo::Begin" << endl;
    #endif

	TProcessData *pRoleInfo = ( TProcessData * )pData;

	char szRoleName[_NAME_LENGTH];

	int nDataLen = pRoleInfo->nDataLen;
	nDataLen = ( nDataLen > _NAME_LENGTH ) ? _NAME_LENGTH : nDataLen;

    TProcessData *pRoleData = (TProcessData *)(pParam->OutBuffer + sizeof(WORD));

	int nUsedLen = sizeof( TProcessData );

	size_t nIdentity = pRoleInfo->ulIdentity;
	
	pRoleData->nProtoId = s2c_roleserver_getroleinfo_result;
	pRoleData->pDataBuffer[0] = -1;
	pRoleData->nDataLen = 1;	
	pRoleData->ulIdentity = nIdentity;

	if ( nDataLen > 0 )
	{
		memcpy( szRoleName, &pRoleInfo->pDataBuffer[0], nDataLen );
		szRoleName[nDataLen] = '\0';
		
		int result = -1;

        #ifdef CONSOLE_DEBUG
		cout << "_GetRoleInfo::GetRoleInfo " << szRoleName << endl;
        #endif

		int nRoleInfoLen = 0;
		
		if (!IsRoleLock(szRoleName))
        {
			GetRoleInfo( &pRoleData->pDataBuffer[1], szRoleName, nRoleInfoLen );
        }

		if ( nRoleInfoLen > 0 )
		{
			pRoleData->pDataBuffer[0] = 1;
			pRoleData->nDataLen = nRoleInfoLen + 1;

			nUsedLen = sizeof( TProcessData ) + nRoleInfoLen;
		}
	}

    *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

    _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);

    #ifdef CONSOLE_DEBUG
	cout << "_GetRoleInfo::End" << endl;
    #endif
}

void _GetGameStat(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
{
    //发送游戏统计数据(By Fellow,2003.7.22)
	ASSERT( pParam && pData && dataLength);

    #ifdef CONSOLE_DEBUG
	cout << "_GetGameStat::Begin" << endl;
    #endif

	//收到的数据包，只有控制字没有数据
	TProcessData *pProData = ( TProcessData * )pData;
	size_t nIdentity = pProData->ulIdentity;

	//取得统计数据
	TProcessData *pStatData = (TProcessData *)(pParam->OutBuffer + sizeof(WORD));
	pStatData->nProtoId = s2c_gamestatistic;
	pStatData->ulIdentity = nIdentity;

   	TGAME_STAT_DATA *pGameStatData = (TGAME_STAT_DATA *)(&pStatData->pDataBuffer[0]);

    GetGameStat(pGameStatData);

	int nDataLen = sizeof(TGAME_STAT_DATA);

	pStatData->nDataLen = nDataLen;

	int nUsedLen = sizeof( TProcessData ) - 1 + nDataLen;

    *((WORD *)pParam->OutBuffer) = ((WORD)nUsedLen) + sizeof(WORD);

    _SendBuffer(pParam->hSocket, *((WORD *)pParam->OutBuffer), pParam->OutBuffer);

    #ifdef CONSOLE_DEBUG
	cout << "_GetGameStat::End" << endl;
    #endif
}


//for Relay System and GM [wxb 2003-7-22]
//void _RelayExtend(THREAD_PARAM *pParam, const void *pData, size_t dataLength)
//{
//	ASSERT(c2s_extend == *((unsigned char *)pData));
//	char* pNewData = ((char *)pData) + 1;
//	switch (*((WORD*)pNewData))
//	{
//	case MAKEWORD(pf_gamemaster, gm_c2s_getrole):
//		{
//			ASSERT(dataLength == 1 + sizeof(GM_GET_ROLE_DATA_COMMAND));
//			GM_GET_ROLE_DATA_COMMAND* pGMData = (GM_GET_ROLE_DATA_COMMAND*)pNewData;
//			int nResultBuflen = 0;
//			switch(pGMData->wGetID)
//			{
//			case gm_role_entergame_position:
//				{
//					ASSERT(0 == pGMData->wLength);
//					char* pReturnBuffer = (char*)_alloca(1 + sizeof(GM_GET_ROLE_DATA_SYNC) + sizeof(GM_ROLE_DATA_SUB_ENTER_POS));
//
//					*pReturnBuffer = (char)s2c_extend;
//
//					GM_GET_ROLE_DATA_SYNC* pGMReturn = (GM_GET_ROLE_DATA_SYNC*)(pReturnBuffer + 1);
//					strncpy(pGMReturn->AccountName, pGMData->AccountName, sizeof(pGMData->AccountName));
//					pGMReturn->ProtocolFamily = pf_gamemaster;
//					pGMReturn->ProtocolType = gm_s2c_getrole;
//					pGMReturn->wGetID = gm_role_entergame_position;
//					pGMReturn->wLength = sizeof(GM_ROLE_DATA_SUB_ENTER_POS);
//
//					GetRoleInfoForGM(pGMData->wGetID, (char*)(pGMReturn + 1), pGMData->AccountName, nResultBuflen);
//					ASSERT(pGMReturn->wLength == nResultBuflen);
//					m_pServer->SendData( m_nIndentity, pReturnBuffer, 1 + sizeof(GM_GET_ROLE_DATA_SYNC) + sizeof(GM_ROLE_DATA_SUB_ENTER_POS) );	
//				}
//				break;
//			default:
//				ASSERT(0);
//				break;
//			}
//		}
//		break;
//
//	case MAKEWORD(pf_gamemaster, gm_c2s_setrole):
//		{
//			ASSERT(dataLength >= 1 + sizeof(GM_SET_ROLE_DATA_COMMAND));
//			GM_SET_ROLE_DATA_COMMAND* pGMData = (GM_SET_ROLE_DATA_COMMAND*)pNewData;
//			SetRoleInfoForGM(pGMData->wSetID, (char*)(pGMData + 1), pGMData->AccountName, pGMData->wLength);
//		}
//		break;
//	default:
//		ASSERT(0);
//		break;
//	}
//}
