//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   Global.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2001-12-28 15:55:54
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "htmlhelp.h"
#include "CRC32.h"
#include "KAVStrTranslate.h"

#include "APLIB.h"
#include "AtlBase.h"

#include "Global.h"
#include "PublicFun.h"

KPATH_TABLE g_PathTable;




///////////////////////////////////////////////////////////////////////////////
// Init paths  System, Windows, update destination path, temp, current, Module path
int InitPaths()
{
    int Result = false;
    int ErrorFlag = false;
    unsigned uRetCode = 0;

    char *pszOffset = NULL;
    
    uRetCode = ::GetWindowsDirectory(g_PathTable.szWindowsPath, MAX_PATH);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = true;    
    AddPathChar(g_PathTable.szWindowsPath);

    uRetCode = (unsigned)::GetSystemDirectory(g_PathTable.szSystemPath, MAX_PATH);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = true;
    AddPathChar(g_PathTable.szSystemPath);

    uRetCode = ::GetCurrentDirectory(MAX_PATH, g_PathTable.szCurrentPath);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = true;
    AddPathChar(g_PathTable.szCurrentPath);

    uRetCode = ::GetModuleFileName(NULL, g_PathTable.szModulePath, (MAX_PATH + 1));
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = true;
    pszOffset = strrchr(g_PathTable.szModulePath, '\\');
    ASSERT(pszOffset);
    pszOffset[1] = '\0';  


    uRetCode = ::GetTempPath(MAX_PATH, g_PathTable.szTempPath);
    ASSERT(uRetCode);    
    if (!uRetCode)
    {
        MkDirEx("C:\\Temp");    
        strcpy(g_PathTable.szTempPath, "C:\\Temp\\");
    }
    else
        AddPathChar(g_PathTable.szTempPath);   

    // Initialize the download destination directory
    strcpy(g_PathTable.szDownloadDestPath, g_PathTable.szModulePath);    
    strcat(g_PathTable.szDownloadDestPath, defUPDATE_DIRECTORY);
    
    // INitialize the Update destionation directory
    strcpy(g_PathTable.szUpdateDestPath, g_PathTable.szModulePath);
    
//Exit0:
    if (ErrorFlag)
        Result = false;
    else
        Result = true;

    return Result;
}


int inline _GetDateFromString(char szString[], int *pnLen)
{
    int i       = 0;
    int nResult = 0;

    *pnLen = 0;

    while (*(szString + i) != '\0')
    {
        if (*(szString + i) == '.') 
        {
            *(szString + i) = '\0';
            break;
        }
        i++;
    }

    *pnLen = i;
    nResult = _StrToInt(szString);
    return nResult;
}

// return 0 : not need update
// return 1 : need update
int CheckFileDate(const char cszFileName[], const char cszFileDate[])
{
    char        *pszFileDate    = NULL;
    int         nResult         = false;
    int         nYear           = 0;
    int         nMonth          = 0;
    int         nDay            = 0;
    int         nLen            = 0;
    char        *pszStr         = NULL;
	CFileStatus status;
	int         nModifyYear     = 0;
	int         nModifyMonth    = 0;
	int         nModifyDay      = 0;

    pszFileDate = new char[strlen(cszFileDate) + 1];
    KAV_PROCESS_ERROR(pszFileDate);

    strcpy(pszFileDate, cszFileDate);
    pszStr = pszFileDate;
    
    nYear   = _GetDateFromString(pszStr, &nLen);
    pszStr  += nLen + 1;
    nMonth  = _GetDateFromString(pszStr, &nLen);
    pszStr  += nLen + 1;
    nDay    = _GetDateFromString(pszStr, &nLen);

	if (!CFile::GetStatus(cszFileName, status))
	{
		nResult = true;
		goto Exit0;
	}

	nModifyYear     = status.m_mtime.GetYear();
	nModifyMonth    = status.m_mtime.GetMonth();
	nModifyDay      = status.m_mtime.GetDay();

	KAV_PROCESS_ERROR(nYear >= nModifyYear);

    KAV_PROCESS_ERROR_RET_CODE(nYear <= nModifyYear, true);

	KAV_PROCESS_ERROR(nMonth >= nModifyMonth);

	KAV_PROCESS_ERROR_RET_CODE(nMonth <= nModifyMonth, true);

	KAV_PROCESS_ERROR(nDay > nModifyDay);

    nResult = true;

Exit0:
    KAV_DELETE_ARRAY(pszFileDate);

    return nResult;
}


///////////////////////////////////////////////////////////
// CRC32
unsigned GetFileCRC(const char cszFileName[])
{
    CFile File;
    CString szCRC;
    char *pvBuffer = NULL;
    int nRetCode   = 0;
    unsigned uResult    = 0;
    
    if (File.Open(cszFileName, CFile::modeRead | CFile::shareDenyNone, NULL))
    {
        int nLen = File.GetLength();
        
        if (nLen > 0)
        {
            pvBuffer = new char[nLen + 1];
            if (pvBuffer != NULL)
            {
                nRetCode = File.Read(pvBuffer, nLen);
                if (nRetCode == nLen)
                    uResult = CRC32(0, pvBuffer, nLen);
            }
        }
        
        File.Close();
    }
    
    KAV_DELETE_ARRAY(pvBuffer);
    
    return uResult;
}

// Return true if the CRC matching, otherwise return false
int CheckFileCRC(unsigned uCRC32, const char szFullFileName[])
{
    unsigned uRetCode   = 0;
    int nResult     = false;
    
    uRetCode = GetFileCRC(szFullFileName);
    if (uRetCode == uCRC32)
        nResult = true;

    return nResult;
}

/////////////////////////////////////////////////////////////////////////////////
// UnCompress file whitch compress by CompressDLL
/////////////////////////////////////////////////////////////////////////////////
int UnCompressFile(const char cszSourceFileName[], const char cszDestFileName[])
{
    int nResult                     = false;
    int nSourceFileSize             = 0;
    int nDestFileSize               = 0;
    unsigned char *pbySourceData    = NULL;
    unsigned char *pbyDestData      = NULL;
    unsigned char *pbySourceDataPos = NULL;
    HANDLE hSourceFile              = INVALID_HANDLE_VALUE;
    HANDLE hDestFile                = INVALID_HANDLE_VALUE;
    int nReadSize                   = 0;
    int nWrittenSize                = 0;
    int nRetCode                    = 0;

    // Read source data
    hSourceFile = ::CreateFile(cszSourceFileName, GENERIC_READ, 
        FILE_SHARE_READ, NULL, 
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hSourceFile);

    nSourceFileSize = ::GetFileSize(hSourceFile, NULL);
    KAV_PROCESS_ERROR(INVALID_FILE_SIZE != nSourceFileSize);

    pbySourceData = (unsigned char *)new char[nSourceFileSize];
    KAV_PROCESS_ERROR(pbySourceData);

    nRetCode = ::ReadFile(hSourceFile, pbySourceData, nSourceFileSize, (unsigned long*)&nReadSize, NULL);
    if ((nRetCode == 0) || (nReadSize != nSourceFileSize))
        goto Exit0;

    ::CloseHandle(hSourceFile);
    hSourceFile = INVALID_HANDLE_VALUE;

    // Uncompress
    nDestFileSize = PKAVEngineFileHeader(pbySourceData)->dwOrignLength;

    pbyDestData = (unsigned char *)new char[nDestFileSize];
    KAV_PROCESS_ERROR(pbyDestData);
    
    pbySourceDataPos = pbySourceData + PKAVEngineFileHeader(pbySourceData)->dwSizeOfHeader;
    nRetCode = aP_depack_asm_fast(pbySourceDataPos, pbyDestData);
    KAV_DELETE_ARRAY(pbySourceData);

    // Create the destination
    hDestFile = ::CreateFile(cszDestFileName, GENERIC_WRITE, 
        FILE_SHARE_READ, NULL, 
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    KAV_PROCESS_ERROR(INVALID_HANDLE_VALUE != hDestFile);

    nRetCode = ::WriteFile(hDestFile, pbyDestData, nDestFileSize, (unsigned long *)&nWrittenSize, NULL);
    if ((nRetCode == 0) || (nWrittenSize != nDestFileSize))
        goto Exit0;

    ::CloseHandle(hDestFile);
    hDestFile = INVALID_HANDLE_VALUE;    

    nResult = true;

Exit0:   
    
    KAV_DELETE_ARRAY(pbySourceData);
    KAV_DELETE_ARRAY(pbyDestData);

    if (hSourceFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hSourceFile);
        hSourceFile = INVALID_HANDLE_VALUE;
    }

    if (hDestFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hDestFile);
        hDestFile = INVALID_HANDLE_VALUE;
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////////////////////////
//  
//  WinUI routines
//  
//    int  KRSMessageBox
//  
//////////////////////////////////////////////////////////////////////////////////////
int KRSMessageBox(HWND hParent, const CString strMessage, const CString strTitle, int nMsgFlag)
{
    int nResult = false;
    //CString strMessage, strTitle;
    
    //VERIFY(strMessage.LoadString(nMsgResID));
    //VERIFY(strTitle.LoadString(nTitleResID));
    
    nResult = MessageBox(hParent, strMessage, strTitle, (UINT)nMsgFlag);
    
    return nResult;
}


///////////////////////////////////////////////////////////////////////////////////////
//  
//      Check if the cszKeyName exist in HKEY_LOACL_MACHINE or HKEY_CURRNET_USER
//
///////////////////////////////////////////////////////////////////////////////////////
int ExistRegKey(const char cszKeyName[])
{
   	CRegKey regkey;
    LONG lRetCode = 0;
    
    
    if (regkey.Open(HKEY_LOCAL_MACHINE, cszKeyName) == ERROR_SUCCESS)
    {
        regkey.Close();
        return true;
    }
    else if (regkey.Open(HKEY_CURRENT_USER, cszKeyName) == ERROR_SUCCESS) 
    {
        regkey.Close();
        return true;
    }
    
    return false;
}

BOOL IsInternetConnection()
{
    int nRetCode = 0;

    nRetCode = ::GetSystemMetrics(SM_NETWORK);
    if (nRetCode & 0x01)
        return true;

    return false;
}

