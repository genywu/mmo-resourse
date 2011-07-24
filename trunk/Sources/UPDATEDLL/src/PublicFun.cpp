//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   PublicFun.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-4-8 16:42:22
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PublicFun.h"
#include "Direct.h"
#include "winsvc.h"
#include "KString.h"

void AddPathChar(char szPath[])
{
    int nPathLen = strlen(szPath);

    if (szPath[nPathLen - 1] != '\\')
        memcpy(&szPath[nPathLen], "\\", 2);
}

void AddPathChar(char szPath[], char chPathChar)
{
    int nPathLen = 0;
  
    //ASSERT(*szPath);

	if (szPath[0] == '\0')
		return;

    nPathLen = strlen(szPath);
    if (szPath[nPathLen - 1] != chPathChar)
    {
        szPath[nPathLen] = chPathChar;
        szPath[nPathLen + 1] = '\0';
    }
}


void MkDirEx(const char cszPathDir[])
{
    char szDir[MAX_PATH]    = {0};
    char szTemp[MAX_PATH]   = {0};
    char *pszPos            = NULL;
    char *pszTemp           = NULL;
    int nLen                = 0;

    
    strcpy(szTemp, cszPathDir);
    pszPos = strchr(szTemp, '\\');
    pszTemp = szTemp;

	while(pszPos != NULL)
	{
		if (*szDir == '\0')
            strncpy(szDir, pszTemp, pszPos - pszTemp);
        else
            strncat(szDir, pszTemp, pszPos - pszTemp);

        pszTemp = pszPos + 1;        

        nLen = strlen(szDir);
		if (szDir[nLen - 1] != ':')//such as "c:"
			_mkdir(szDir);

		strcat(szDir, "\\");

		pszPos = strchr(pszTemp, '\\');
	}

    if (*szDir == '\0')
        strcpy(szDir, pszTemp);
    else
        strcat(szDir, pszTemp);

	_mkdir(szDir);
}

 
// Check file exists
int FileExists(LPCTSTR lpszFileName)
{
    int nResult                     = false;
    HANDLE hFind                    = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA *pFindFileData  = NULL;


    pFindFileData = new WIN32_FIND_DATA;
    KAV_PROCESS_ERROR(pFindFileData);

    hFind = ::FindFirstFile(lpszFileName, pFindFileData);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        if (!((pFindFileData->dwFileAttributes) & FILE_ATTRIBUTE_DIRECTORY))
            nResult = true;
     
        ::FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

Exit0:
    KAV_DELETE(pFindFileData);

    return nResult;
}


int CopyDir(const char cszSource[], const char cszDest[], int nFailedIfExist)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE Handle;
    char szSource[MAX_PATH];
    char szSourceFile[MAX_PATH];
    char szDest[MAX_PATH];

    strcpy(szSource, cszSource);
    AddPathChar(szSource, '\\');
    strcat(szSource, "*.*");

    MkDirEx(cszDest);

    Handle = ::FindFirstFile(szSource, &FindFileData);
    if (Handle != INVALID_HANDLE_VALUE)
    {
        do
        {   
            strcpy(szDest, cszDest);
            AddPathChar(szDest);
            strcat(szDest, FindFileData.cFileName);

            strcpy(szSourceFile, cszSource);
            AddPathChar(szSourceFile);
            strcat(szSourceFile, FindFileData.cFileName);

            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((strcmp(FindFileData.cFileName, ".") != 0) && (strcmp(FindFileData.cFileName, "..") != 0))
                    CopyDir(szSourceFile, szDest, nFailedIfExist);
                continue;
            }                   

            ::CopyFile(szSourceFile, szDest, nFailedIfExist);            
        }
        while(::FindNextFile(Handle, &FindFileData));

        ::FindClose(Handle);
    }

    return true;
}

// Return  1 : Exist process and exit successfully
//         0 : Exist process but exit failed
//        -1 : Not exist process
int StopAService(const char cszServiceName[])
{
    int nResult                 = -1;
    SC_HANDLE SCMgrHandle       = NULL;
    SC_HANDLE SCServiceHandle   = NULL;
    SERVICE_STATUS ssStatus;
    DWORD dwErrorCode           = 0;

	DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;

    SCMgrHandle = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    KAV_PROCESS_ERROR(SCMgrHandle);

    SCServiceHandle = OpenService(SCMgrHandle, cszServiceName, SC_MANAGER_ALL_ACCESS);
    if (!SCServiceHandle)
    {
        dwErrorCode = GetLastError();
        if ((dwErrorCode == ERROR_SERVICE_DOES_NOT_EXIST) || (dwErrorCode == ERROR_INVALID_NAME))
            nResult = -1;

        goto Exit0;
    }

    if (!ControlService(SCServiceHandle, SERVICE_CONTROL_STOP, &ssStatus))
    {
		nResult = 0;
		goto Exit0;
	}

	// Check the status until the service is no longer start pending. 
    if (!QueryServiceStatus( 
		SCServiceHandle,   // handle to service 
		&ssStatus) 
	)  // address of status information structure
    {
		nResult = 0;
		goto Exit0;
	}
 
    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) 
    { 
        // Do not wait longer than the wait hint. A good interval is 
        // one tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if(dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again. 
 
        if (!QueryServiceStatus( 
            SCServiceHandle,   // handle to service 
            &ssStatus)
		)  // address of structure
            break; 
 
        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // The service is making progress.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    } 

    if (ssStatus.dwCurrentState == SERVICE_STOPPED) 
		nResult = 1;
	else 
		nResult = 0;

Exit0:
    if (SCServiceHandle)
    {
        CloseServiceHandle(SCServiceHandle);
        SCServiceHandle = NULL;
    }

    if (SCMgrHandle)
    {
        CloseServiceHandle(SCMgrHandle);
        SCMgrHandle = NULL;
    }

    return nResult;
}


int StartAService(const char cszServiceName[])
{
    int nResult                 = false;
	int nRetCode				= 0;
    SC_HANDLE SCMgrHandle       = NULL;
    SC_HANDLE SCServiceHandle   = NULL;

    SERVICE_STATUS ssStatus;
	DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    

    SCMgrHandle = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    KAV_PROCESS_ERROR(SCMgrHandle);

    SCServiceHandle = OpenService(SCMgrHandle, cszServiceName, SC_MANAGER_ALL_ACCESS);
    KAV_PROCESS_ERROR(SCServiceHandle);

    nRetCode = StartService(SCServiceHandle, 0, NULL);    
	KAV_PROCESS_ERROR(nRetCode);

	// Check the status until the service is no longer start pending. 
    if (!QueryServiceStatus( 
		SCServiceHandle,   // handle to service 
        &ssStatus)
	)  // address of status information structure
        goto Exit0;
 
    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
    { 
        // Do not wait longer than the wait hint. A good interval is 
        // one tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if(dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again. 
 
        if (!QueryServiceStatus( 
            SCServiceHandle,   // handle to service 
            &ssStatus) 
		)  // address of structure
            break; 
 
        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // The service is making progress.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    } 

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
		nResult = true;

Exit0:
    if (SCServiceHandle)
    {
        CloseServiceHandle(SCServiceHandle);
        SCServiceHandle = NULL;
    }

    if (SCMgrHandle)
    {
        CloseServiceHandle(SCMgrHandle);
        SCMgrHandle = NULL;
    }

    return nResult;
}

//Commented by LiuHaifeng
//When compile KavRunPlus, we must comment following

int GetExitCommandLineParameter()
{
	int nResult  = false;
    int nRetCode = false;
    CWinApp *pWinApp    = NULL;
    char *pszParam      = NULL;
    char *pszParamSet   = NULL;
    char *pszPos        = NULL;
    
    pWinApp = ::AfxGetApp();
    if (pWinApp == NULL)
        goto Exit0;
    
    pszParamSet = pWinApp->m_lpCmdLine;
    
    while (pszParamSet)
    {
        pszParam = pszParamSet;
        
        pszPos = strchr(pszParamSet, ' ');
        if (pszPos)
        {
            *pszPos = '\0';
            pszParamSet = pszPos + 1; 
        }
        else 
        {
            pszParamSet = NULL;
        }
        
        if ((*pszParam == '-') || (*pszParam == '/'))
        {
            pszParam++;
            
            nRetCode = stricmp(pszParam, "QUIT");
            if (!nRetCode)
            {
                nResult = TRUE;
                
                break;
            }
        }
    }
    
Exit0:
    
    return nResult;
}


void StrAddStr(char szStr1[], const char cszStr2[])
{
	char szTemp1[2048]	= {0};
	char szTemp2[2048]	= {0};
	
	strcpy(szTemp1, szStr1);
	strcpy(szTemp2, cszStr2);

	//Commented by LiuHaifeng
    //When compile KavRunPlus, we must comment following
    StrUpper(szTemp1);
	StrUpper(szTemp2);

	if (strstr(szTemp1, szTemp2) == NULL)
		strcat(szStr1, cszStr2);
}	


int GetRegKeyValueByKey(HKEY hKey, const char cszRegKey[], const char cszKeyName[], char szValue[], int nSize)
{
	HKEY Key;
	int nRetCode = -1;
	DWORD dwCount = nSize;
	LONG lRetCode = 0;

	lRetCode = RegOpenKeyEx(hKey, cszRegKey, 0, KEY_READ, &Key);
	if (ERROR_SUCCESS == lRetCode)
	{
		nRetCode = 0;
		lRetCode = RegQueryValueEx(Key, cszKeyName, NULL, NULL, (LPBYTE)szValue, &dwCount);
		if (ERROR_SUCCESS == lRetCode)
			nRetCode = 1;

		RegCloseKey(Key);
	}

	return nRetCode;
}


int GetRegKeyValue(const char cszRegKey[], const char cszKeyName[], char szValue[], int nSize)
{
	int nRetCode = 0;

	nRetCode = GetRegKeyValueByKey(HKEY_CURRENT_USER, cszRegKey, cszKeyName, szValue, nSize);
	if (0 >= nRetCode)
		nRetCode = GetRegKeyValueByKey(HKEY_LOCAL_MACHINE, cszRegKey, cszKeyName, szValue, nSize);

	return nRetCode;
}
