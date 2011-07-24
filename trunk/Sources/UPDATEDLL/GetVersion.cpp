//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   GetVersion.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2001-11-20 4:36:29
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "GetVersion.h"
#include "KAVStrTranslate.h"

int GetVersionStringFromInt(
    int nMajorVersion, int nMinorVersion, 
    char szVersion[], int nVersionLen
)
{
    int Result  = false;
    int nLen    = 0;

    KAV_ASSERT_EXIT(szVersion);
    
    _itoa(nMajorVersion >> 16, szVersion, 10);

    if (nMajorVersion & 0x0FFFF)
    {
        strcat(szVersion, ".");
        nLen = strlen(szVersion);
        _itoa(nMajorVersion & 0x0FFFF, szVersion + nLen, 10);
    }

    if (nMinorVersion >> 16)
    {
        strcat(szVersion, ".");
        nLen = strlen(szVersion);
        _itoa(nMinorVersion >> 16, szVersion + nLen, 10);
    }
    if (nMinorVersion & 0x0FFFF)
    {
        strcat(szVersion, ".");
        nLen = strlen(szVersion);
        _itoa(nMinorVersion & 0x0FFFF, szVersion + nLen, 10);
    }

    Result = true;
Exit0:    
    return Result;
}

int GetVersionFromString(const char cszVersion[], unsigned *puMajorVersion, unsigned *puMinorVersion)
{
    char *pszFileVersionDetail[4];
    char *pszFileVersion    = NULL;
    char *pszVersionPos     = NULL;
    char *pszPointerPos     = NULL;
    int  i                  = 0;
    int  nLen               = 0;   
    int  nValue             = 0;
    int  nResult            = false;
    _int64  nFileVersion    = 0;

    ASSERT(puMajorVersion);
    ASSERT(puMinorVersion);

    *puMajorVersion = 0;
    *puMinorVersion = 0;

    nLen = strlen(cszVersion);
    pszFileVersion = new char[nLen + 1];
    KAV_PROCESS_ERROR(pszFileVersion);

    strcpy(pszFileVersion, cszVersion);
    pszVersionPos = pszFileVersion;
    for (i = 0; i < 4; i++)
    {
        if (!pszVersionPos)
        {
            pszFileVersionDetail[i] = new char[2];
            KAV_PROCESS_ERROR(pszFileVersionDetail[i]);
            strcpy(pszFileVersionDetail[i], "0");
            continue;
        }

        pszPointerPos = strchr(pszVersionPos, '.');
        if ((!pszPointerPos) && pszVersionPos)
        {
            nLen = strlen(pszVersionPos);
            pszFileVersionDetail[i] = new char[nLen + 1];
            KAV_PROCESS_ERROR(pszFileVersionDetail[i]);
            strcpy(pszFileVersionDetail[i], pszVersionPos);
            
            pszVersionPos = NULL;

            continue;
        }
        
        nLen = pszPointerPos - pszVersionPos;
        pszFileVersionDetail[i] = new char[nLen + 1];
        KAV_PROCESS_ERROR(pszFileVersionDetail[i]);
        
        strncpy(pszFileVersionDetail[i], pszVersionPos, nLen);
        *(pszFileVersionDetail[i] + nLen) = '\0';

        pszVersionPos = pszPointerPos + 1;
    }
        
    for (i = 0; i < 4; i++)
    {
        nValue = _StrToInt(pszFileVersionDetail[i]);
        nValue = abs(nValue);
        nFileVersion = (nFileVersion << 16) + (unsigned)nValue;
    }

    *puMajorVersion = (unsigned)(nFileVersion >> 32);
    *puMinorVersion = (unsigned)nFileVersion;

    nResult = true;

Exit0:
    KAV_DELETE_ARRAY(pszFileVersion);

    for (i = 0; i < 4; i++)
        KAV_DELETE_ARRAY(pszFileVersionDetail[i]);

    return nResult;
}


int GetFileVersion(
    const char cszFileName[], 
    DWORD *pdwProductVersionMS, 
    DWORD *pdwProductVersionLS
)
{
    int nResult             = false;
    int nRetCode            = false;
    DWORD dwHandle          = 0;
    DWORD dwFileInfoSize    = 0;

    VS_FIXEDFILEINFO *pFixFileInfo = NULL;
    char *pszFileInfo       = NULL;
    UINT uLen               = 0;
    
    KAV_ASSERT_EXIT(cszFileName);
    KAV_ASSERT_EXIT(pdwProductVersionMS);
    KAV_ASSERT_EXIT(pdwProductVersionLS);

    *pdwProductVersionMS = 0;
    *pdwProductVersionLS = 0;
    
    dwFileInfoSize = ::GetFileVersionInfoSize((char *)cszFileName, &dwHandle);
    KAV_PROCESS_ERROR(dwFileInfoSize);
    
    pszFileInfo = new char[dwFileInfoSize];
    KAV_PROCESS_ERROR(pszFileInfo);
    
    nRetCode = GetFileVersionInfo((char *)cszFileName, dwHandle, dwFileInfoSize, pszFileInfo);
    KAV_PROCESS_ERROR(nRetCode);
    
    //pFixFileInfo = new VS_FIXEDFILEINFO;
    //if (pFixFileInfo == NULL)
    //    goto Exit0;
    
    nRetCode = VerQueryValue(pszFileInfo, "\\", (LPVOID *)&pFixFileInfo, &uLen);
    KAV_PROCESS_ERROR(nRetCode);
    
    if (uLen > 0)
    {
        *pdwProductVersionMS = pFixFileInfo->dwProductVersionMS;
        *pdwProductVersionLS = pFixFileInfo->dwProductVersionLS;
    }    

    nResult = true;    
Exit0:
    KAV_DELETE_ARRAY(pszFileInfo);
   
    return nResult;
}

int GetVersion(const char cszFileName[], char *pszVersion, int nVersionSize)
{
    int nRetCode            = false;
    int nResult             = false;
    DWORD dwMajorVersion    = 0;
    DWORD dwMinorVersion    = 0;
    
    KAV_ASSERT_EXIT(cszFileName);
    KAV_ASSERT_EXIT(pszVersion);
    
    *pszVersion = '\0';

    nRetCode = GetFileVersion(cszFileName, &dwMajorVersion, &dwMinorVersion);
    KAV_ASSERT_EXIT(nRetCode);

    nRetCode = GetVersionStringFromInt(dwMajorVersion, dwMinorVersion, pszVersion, nVersionSize);
    KAV_ASSERT_EXIT(nRetCode);

    nResult = true;
Exit0:    
    return nResult;
}

int GetProgramVersion(CString &sVersion)
{
    HINSTANCE hModule = NULL;
    int nResult                 = false;
    int nRetCode                = false;
    char szFileName[MAX_PATH]   = {0};
    char szVersion[20]          = {0};
    
    KAV_ASSERT_EXIT(sVersion);
    sVersion.Empty();

    hModule = AfxGetInstanceHandle();
    KAV_ASSERT_EXIT(hModule);

    nRetCode = GetModuleFileName((HINSTANCE)hModule, szFileName, MAX_PATH);
    KAV_ASSERT_EXIT(hModule);

    nRetCode = GetVersion(szFileName, szVersion, 20);
    KAV_ASSERT_EXIT(nRetCode);

    sVersion = (CString)szVersion;    

    nResult = true;
Exit0:
    return nResult;
}