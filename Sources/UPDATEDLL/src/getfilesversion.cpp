//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   GetFilesVersion.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-2-1 15:50:02
//  Comment     :   The function for get duba files version
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GetFilesVersion.h"
#include "Global.h"
#include "GetVersion.h"
#include "KAVStrTranslate.h"    
#include "UpdateData.h"
#include "PublicFun.h"

extern KPATH_TABLE      g_PathTable;
extern KUPDATE_DATA     g_UpdateData;

// Get engine file version, such as KAEngine.dat DrWeb32.dat
// Get kingsoft antivirus file version by check version method
int GetKAVFileVersion(
    LPCTSTR lpszFileName, 
    KCHECKVERSIONMETHOD CheckVersionMethod, 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
)
{
    int nResult         = false;
    
    *pdwMajorVersion    = 0;
    *pdwMinorVersion    = 0;
    
    //if this file does not exist in user's disk
    //then won't download it...
    nResult = FileExists(lpszFileName);
    KAV_PROCESS_ERROR(nResult);
    
    switch (CheckVersionMethod)
    {
    case enumCVM_NORMAL:     // Normal file, such as KAV9X.exe, KAEPlat.dll
        nResult = GetFileVersion(lpszFileName, pdwMajorVersion, pdwMinorVersion);
        break;
    case enumCVM_ENGINE:     // Engine file
        nResult = GetEngineFileVersion(lpszFileName, pdwMajorVersion, pdwMinorVersion);
        break;
    case enumCVM_SIGN:       // Sign file
        nResult = GetSignFileVersion(lpszFileName, pdwMajorVersion, pdwMinorVersion);
        break;
    default:      // CheckVersionMethod == enumCVM_DATA
        break;
    }
    
Exit0:
    return nResult;
}


// Get engine file version, such as KAEngine.dat DrWeb32.dat
int GetEngineFileVersion(
    const char cszFileName[], 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
)
{
	int nResult                  = false;
    int nRetCode                 = 0;
    int nReadSize                = 0;
    int nKAVEngineFileHeaderSize = 0;
    HANDLE hFile                 = INVALID_HANDLE_VALUE;
    KAVEngineFileHeader Header;

    ASSERT(pdwMajorVersion);
    ASSERT(pdwMinorVersion);
    
	*pdwMajorVersion = 0;
	*pdwMinorVersion = 0;

    hFile = ::CreateFile(
        cszFileName,  
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hFile);

    nKAVEngineFileHeaderSize = sizeof(KAVEngineFileHeader);
    
    nRetCode = ::ReadFile(hFile, (void *)&Header, nKAVEngineFileHeaderSize, (DWORD *)&nReadSize, NULL);
    KAV_PROCESS_ERROR(!(!nRetCode || (nReadSize != nKAVEngineFileHeaderSize)));
        
    *pdwMajorVersion = Header.dwOrignMajorVersion;
    *pdwMinorVersion = Header.dwOrignMinorVersion;
        
    nResult = true;

Exit0:
    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

	return nResult;
}

// Get sign file version, such as KAV00000.dat KAV00000.upd
int GetSignFileVersion(
    const char cszFileName[], 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
)
{
    int nResult                  = false;
    int nRetCode                 = 0;
    int nReadSize                = 0;
    int  nKAVSignFileHeaderSize  = 0;
    HANDLE hFile                 = INVALID_HANDLE_VALUE;
    KAVSIGNFILEHEADER Header;
    WORD wTemp                   = 0;

    ASSERT(pdwMajorVersion);
    ASSERT(pdwMinorVersion);

    *pdwMajorVersion  = 0;
    *pdwMinorVersion  = 0;

    hFile = ::CreateFile(
        cszFileName,  
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hFile);
        
    nKAVSignFileHeaderSize = sizeof(KAVSIGNFILEHEADER);
    
    nRetCode = ::ReadFile(hFile, (void *)&Header, nKAVSignFileHeaderSize, (DWORD *)&nReadSize, NULL);
    KAV_PROCESS_ERROR(!(!nRetCode || (nReadSize != nKAVSignFileHeaderSize)));

    //dwMajorVersion
    wTemp = ((WORD)(Header.dwDateStamp >> 8)) & 0x00FF;
    *pdwMajorVersion = (Header.dwDateStamp & 0xFFFF0000) + wTemp;
        
    //dwMinorVersion
    wTemp = ((WORD)Header.dwDateStamp) & 0x00FF;
    *pdwMinorVersion = wTemp;
    *pdwMinorVersion = *pdwMinorVersion << 16;

    nResult = true;
   
Exit0:
    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    return nResult;
}

// Get local max sign DAT file number
int GetLocalDatFileNumber()
{
    char szLocalPathName[MAX_PATH + 1];
    char szSignFileName[13]     = defBASE_SIGN_FILENAME;    
    int  nSignFileNameNum       = 0;
    WIN32_FIND_DATA *pFindData  = NULL;
    int  nRetCode               = 0;
    HANDLE hFind                = INVALID_HANDLE_VALUE;
    
    strcpy(szLocalPathName, g_PathTable.szUpdateDestPath);
    strcat(szLocalPathName, "KAV?????.DAT");
    
    pFindData = new WIN32_FIND_DATA;
    KAV_PROCESS_ERROR(pFindData);

    hFind = ::FindFirstFile(szLocalPathName, pFindData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do    // Get max number file name
        {
            if (IsSignFileName(pFindData->cFileName))
                if (stricmp(pFindData->cFileName, szSignFileName) > 0)
                    strcpy(szSignFileName, pFindData->cFileName);
       
        } while (::FindNextFile(hFind, pFindData));
    
        ::FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    if (szSignFileName[8] == '.')   // Get file number
    {
        szSignFileName[8] = '\0';
        nSignFileNameNum  = _StrToInt(szSignFileName + 3);
        szSignFileName[8] = '.';
    }

Exit0:
    KAV_DELETE(pFindData);

    return nSignFileNameNum;
}


int GetLocalSignVersion(DWORD *pdwMajorVersion, DWORD *pdwMinorVersion)
{
    char szSignFileName[MAX_PATH];
    char szFileName[MAX_PATH];
    int  nSignFileNameSize      = 0;
    int  nBaseSignNum           = 0;
    int  nUpdateSignNum         = 0;
    int  nLen                   = 0;
    int  nRetCode               = false;
    int  nResult                = false;
    unsigned uMajorVersion      = 0;
    unsigned uMinorVersion      = 0;


    // Get KAVXXXXX.DAT Version
    nBaseSignNum = GetLocalDatFileNumber();
    sprintf(szFileName, "KAV%05d.DAT", nBaseSignNum);

    strcpy(szSignFileName, g_PathTable.szUpdateDestPath);
    strcat(szSignFileName, szFileName);

    nRetCode = GetSignFileVersion(szSignFileName, (unsigned long *)&uMajorVersion, (unsigned long *)&uMinorVersion);
    if ((uMajorVersion == 0) && (uMinorVersion == 0))
        return false;

    *pdwMajorVersion = uMajorVersion;
    *pdwMinorVersion = uMinorVersion;

    nSignFileNameSize = strlen(szSignFileName) + 1;

    // Find the update sign data file.
    nUpdateSignNum = nBaseSignNum;
    strcpy(szFileName, g_PathTable.szUpdateDestPath);
    strcat(szFileName, defBASE_SIGN_UPDNAME);

    while (true)
    {
        nUpdateSignNum++;

        _IntToNStr(nUpdateSignNum, szFileName + nSignFileNameSize - 10, 5);

        nRetCode = FileExists(szFileName);
        if (!nRetCode)
        {
            break;
        }
        else
        {
            nRetCode = GetSignFileVersion(szFileName, (unsigned long *)&uMajorVersion, (unsigned long *)&uMinorVersion);
            if ((uMajorVersion == 0) && (uMinorVersion == 0))
                break;
            
            *pdwMajorVersion = uMajorVersion;
            *pdwMinorVersion = uMinorVersion;
        }
    }
    
    return true;
}                                   

int GetLocalSignVersion(char szVersion[])
{
    unsigned uMajorVersion      = 0;
    unsigned uMinorVersion      = 0;

    GetLocalSignVersion((unsigned long *)&uMajorVersion, (unsigned long *)&uMinorVersion);

    GetVersionStringFromInt(
            uMajorVersion, uMinorVersion, 
            szVersion, defVERSION_SIZE
    );

    return true;
}


// Get Max number of sign file name
int GetLocalSignFileNumber()
{
    int  nSignFileNameNum       = 0;
    int  nRetCode               = 0;
    
    nSignFileNameNum = GetLocalDatFileNumber();
    nRetCode = ExistUpdFileByNum(nSignFileNameNum + 1);
    while (nRetCode)
    {
        nSignFileNameNum++;
        nRetCode = ExistUpdFileByNum(nSignFileNameNum + 1);
    }

    return nSignFileNameNum;
}

// Check Upd file exists by number
int ExistUpdFileByNum(int nNum)
{
    int nRetCode            = false;
    char szSignVersion[6];
    char szLocalPathName[MAX_PATH + 1];
    int nLocalPathNameLen   = 0;
    
    strcpy(szLocalPathName, g_PathTable.szUpdateDestPath);
    strcat(szLocalPathName, defBASE_SIGN_UPDNAME);
    nLocalPathNameLen = strlen(szLocalPathName);

    nRetCode = _IntToStr(nNum, 6, szSignVersion);
    memcpy(szLocalPathName + nLocalPathNameLen - 4 - nRetCode, szSignVersion, nRetCode);
    
    if (FileExists(szLocalPathName))
        return true;

    return false;
}



// Check sign file by file header
int IsSignFile(const char szFileName[])
{
    int nResult                 = false;
    KAVSIGNFILEHEADER KAVSignFileHeader;
    int nKAVSignFileHeaderSize  = 0;
    int nRetCode                = 0;
    int nReadSize               = 0;
    HANDLE hFile                = INVALID_HANDLE_VALUE;

    hFile = ::CreateFile(
        szFileName,  
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hFile);

    nKAVSignFileHeaderSize = sizeof(KAVSIGNFILEHEADER);
    
    nRetCode = ::ReadFile(hFile, (void *)&KAVSignFileHeader, nKAVSignFileHeaderSize, (DWORD *)&nReadSize, NULL);
    KAV_PROCESS_ERROR(!(!nRetCode || (nReadSize != nKAVSignFileHeaderSize)));
    
    nRetCode = strnicmp(KAVSignFileHeader.cszDescription, "Kingsoft AntiVirus Database", 27);
    if (!nRetCode)
        nResult = true;

Exit0:
    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
 
    return nResult;
}


int GetSignFileNumber(const char cszSignName[])
{
    int nResult         = -1;
    int nSignNameLen    = strlen(cszSignName);
    char szFileNameNum[13];

    KAV_PROCESS_ERROR(nSignNameLen == 12);
    
    KAV_PROCESS_ERROR(IsNumeric(cszSignName + 3, 5));

    strncpy(szFileNameNum, cszSignName + 3, 5);
    szFileNameNum[5] = '\0';

    nResult = _StrToInt(szFileNameNum);    

Exit0:
    return nResult;
}


int IsUpdFileName(const char cszFileName[])
{
    int nResult         = false;
    int nFileNameLen    = strlen(cszFileName);

    KAV_PROCESS_ERROR(nFileNameLen == 12);
        
    KAV_PROCESS_ERROR(strnicmp(cszFileName, "KAV", 3) == 0);
    
    KAV_PROCESS_ERROR(IsNumeric(cszFileName + 3, 5));
        
    if (strnicmp(cszFileName + nFileNameLen - 4, ".UPD", 4) == 0)
        nResult = true;

Exit0:
    return nResult;    
}

int IsSignFileName(const char cszFileName[])
{
    int nResult         = false;
    int nFileNameLen    = strlen(cszFileName);

    KAV_PROCESS_ERROR(nFileNameLen == 12);
        
    KAV_PROCESS_ERROR(strnicmp(cszFileName, "KAV", 3) == 0);
    
    KAV_PROCESS_ERROR(IsNumeric(cszFileName + 3, 5));

    if (strnicmp(cszFileName + nFileNameLen - 4, ".DAT", 4) == 0)
        nResult = true;

Exit0:
    return nResult;    
}
    
int GetUpdFileNumber(const char cszUpdFileName[])
{
    return GetSignFileNumber(cszUpdFileName);
}

// Check is exist the same version sign file
// 0: no
// 1: Exist the same Version file
// -1: Exist the same file
int CheckExistSameVersionFile(const char cszFileName[])
{
    int nRetCode = 0;
    char szFileName[MAX_PATH] = {0};
    char *pszExtName = NULL;
    char szFullFileName[MAX_PATH] = {0};


    strcpy(szFileName, cszFileName);
    pszExtName = szFileName + strlen(szFileName) - 3;
    if (IsUpdFileName(cszFileName))   
		strcpy(pszExtName, "DAT");
    else if (IsSignFileName(cszFileName))
        strcpy(pszExtName, "UPD");

    strcpy(szFullFileName, g_PathTable.szUpdateDestPath);
    strcat(szFullFileName, szFileName);
    nRetCode = FileExists(szFullFileName);
	if (nRetCode)
		return 1;

	strcpy(szFullFileName, g_PathTable.szUpdateDestPath);
	strcat(szFullFileName, cszFileName);

	nRetCode = FileExists(szFullFileName);
	if (nRetCode)
		return -1;        

	return 0;
}


int GetNormalVersion(const char cszPathName[], char szVersion[])
{
    int nRetCode = 0;
    unsigned uMajorVersion = 0;
    unsigned uMinorVersion = 0;

    nRetCode = GetFileVersion(cszPathName, (DWORD *)&uMajorVersion, (DWORD *)&uMinorVersion);
    if (nRetCode)
        nRetCode = GetVersionStringFromInt(
            uMajorVersion, uMinorVersion, 
            szVersion, defVERSION_SIZE
        );
    else 
        nRetCode = GetVersionStringFromInt(1, 0, szVersion, defVERSION_SIZE);

    return nRetCode;
}

int GetEngineVersion(const char cszPathName[], char szVersion[])
{
    int nRetCode = 0;
    unsigned uMajorVersion = 0;
    unsigned uMinorVersion = 0;

    nRetCode = GetEngineFileVersion(cszPathName, (DWORD *)&uMajorVersion, (DWORD *)&uMinorVersion);
    if (!nRetCode)
        return nRetCode;

    nRetCode = GetVersionStringFromInt(
            uMajorVersion, uMinorVersion, 
            szVersion, defVERSION_SIZE
    );    
    return nRetCode;
}


int GetSignVersion(const char cszPathName[], char szVersion[])
{
    int nRetCode = 0;
    unsigned uMajorVersion = 0;
    unsigned uMinorVersion = 0;

    nRetCode = GetSignFileVersion(cszPathName, (DWORD *)&uMajorVersion, (DWORD *)&uMinorVersion);
    
    nRetCode = GetVersionStringFromInt(
            uMajorVersion, uMinorVersion, 
            szVersion, defVERSION_SIZE
    );  

    return nRetCode;
}

int GetVirusCountFromSignFile(const char cszPathName[])
{
    HANDLE hFile    = INVALID_HANDLE_VALUE;
    int nVirusCount = 0;
    KAE_KVSIGN_HEADER SignHeader;
    int nRetCode    = 0;
    DWORD dwRead    = 0;

    nRetCode = IsSignFile(cszPathName);
    KAV_PROCESS_ERROR(nRetCode);
        
    hFile = ::CreateFile(cszPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE, NULL);
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hFile);

    nRetCode = ::ReadFile(hFile, &SignHeader, sizeof(SignHeader), &dwRead, NULL);
    KAV_PROCESS_ERROR(!(!nRetCode || (dwRead != sizeof(SignHeader))));
    
    nVirusCount = SignHeader.uProcessVirusNum;

Exit0:
    if (INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    return nVirusCount;
}


