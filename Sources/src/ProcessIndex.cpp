//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   ProcessIndex.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-6-25 10:06:47
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

// ProcessIndex.cpp: implementation of the CProcessIndex class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ProcessIndex.h"
#include "DataDefine.h"
#include "PublicFun.h"
#include "GetVersion.h"
#include "GetFilesVersion.h"
#include "Global.h"
#include "KAVStrTranslate.h"
#include "AtlBase.h"
#include "Resource.h"
#include "UpdateData.h"
#include "SourceDef.h"

extern KPATH_TABLE g_PathTable;
extern KUPDATE_DATA g_UpdateData;

//-------------------------------------------------------------------
// comment function
//-------------------------------------------------------------------


int CheckSectionIsUpdateItem(const char cszFileName[], const char cszItemName[])
{
    DWORD dwRetCode     = 0;
    int nRetCode        = 0;
    char szUpdateFileFlag[10];
	
	
	
    dwRetCode = ::GetPrivateProfileString(cszItemName, "UpdateFileFlag", "", szUpdateFileFlag, 10, cszFileName);
    if (!dwRetCode)
        return 0;
	
    nRetCode = stricmp(szUpdateFileFlag, "FALSE");
    if (!nRetCode)
        return 0;
    nRetCode = stricmp(szUpdateFileFlag, "0");
    if (!nRetCode)
        return 0;
	
    nRetCode = stricmp(szUpdateFileFlag, "TRUE");
    if (!nRetCode)
        return 1;
    nRetCode = stricmp(szUpdateFileFlag, "1");
    if (!nRetCode)
        return 1;
	
    return 0;
}

int GetActuallyPath(char szPath[], const char cszBasePath[])
{
    int nRetCode = 0;
    char *pszPos    = NULL;
    char szTemp[MAX_PATH] = {0};
	
    if (!(*szPath))
    {
        strcpy(szPath, cszBasePath);
        goto Exit0;
    }
    
    nRetCode = stricmp(szPath,"%PROGRAMDIR%");
    if (!nRetCode)
    {
        strcpy(szPath, g_PathTable.szUpdateDestPath);
        goto Exit0;
    }
    
    nRetCode = stricmp(szPath, "%TEMPDIR%");
    if (!nRetCode)
    {
        strcpy(szPath, g_PathTable.szTempPath);
        goto Exit0;
    }
    
    nRetCode = stricmp(szPath, "%SYSTEMDIR%"); 
    if (!nRetCode) 
    {
        strcpy(szPath, g_PathTable.szSystemPath);
        goto Exit0;
    }
    
    nRetCode = stricmp(szPath, "%WINDIR%");
    if (!nRetCode)
    {
        strcpy(szPath, g_PathTable.szWindowsPath);
        goto Exit0;
    }
    
    pszPos = strstr(szPath, "%PROGRAMDIR\\");
    if (pszPos)
    {
        strcpy(szTemp, szPath);
        strcpy(szPath, g_PathTable.szUpdateDestPath);
        pszPos = strchr(szTemp, '\\');
        pszPos++;
        strcat(szPath, pszPos);
        nRetCode = strlen(szPath);
        szPath[nRetCode - 1] = '\0';
        goto Exit0;
    }  
    
    if (!(
        ((*szPath = '\\') || (*szPath = '/')) ||           //Maybe '/' in Unix
        (
		(strlen(szPath) > 2) && (*(szPath + 1) == ':') && 
		((*(szPath + 2) == '\\')  || (*(szPath + 2) == '/'))
        )
		))              // if is sub-dir
    {
        char szTemp[MAX_PATH] = {0};
        strcpy(szTemp, szPath);
        strcpy(szPath, cszBasePath);
        strcat(szPath, szTemp);
    } 
	
Exit0:
    AddPathChar(szPath);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CProcessIndex::GetItemInfo(
							   PKUPDATE_ITEM pUpdateItem, 
							   int nUpdateMethod, char szLocalPath[]
							   )
{
    int     nResult             = false;
    DWORD   dwRetCode           = 0;
    int     nRetCode            = 0;
    char    szValue[50]         = {0};
    char    szBuffer[1024]      = {0};
    int     nBufferLen          = 1024;
    char    szCommentName[20]   = {0};
    int     nComment            = 0;
	int     nLen			    = 0;
    char    szDownloadDir[MAX_PATH] = {0};   
	
    
    KAV_PROCESS_ERROR(pUpdateItem);
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "FileName", "", pUpdateItem->szFileName, MAX_PATH, m_szTxtFileName); 
    KAV_PROCESS_ERROR(dwRetCode);
    
    pUpdateItem->nFileSize = (int)::GetPrivateProfileInt(pUpdateItem->szItemName, "FileSize", 0, m_szTxtFileName);
    KAV_PROCESS_ERROR(pUpdateItem->nFileSize);
	
    pUpdateItem->KCheckVersionMethod = (KCHECKVERSIONMETHOD)::GetPrivateProfileInt(pUpdateItem->szItemName, "CheckVersionMethod", 0, m_szTxtFileName);
    
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "SNMask", "", pUpdateItem->szSNMask, defSNMaskSize, m_szTxtFileName);
	
    // --------------- Process Version As File Date------------------
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "Version", "", szValue, 50, m_szTxtFileName);
    KAV_PROCESS_ERROR(dwRetCode);
	
	strcpy(pUpdateItem->szFileDate, szValue);
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "CRC", "", pUpdateItem->szCRC, 20, m_szTxtFileName);
    KAV_PROCESS_ERROR(dwRetCode);
	
    if ((pUpdateItem->KCheckVersionMethod == enumCVM_TIME) ||
		(pUpdateItem->KCheckVersionMethod == enumCVM_DIRECT) ||
        (pUpdateItem->KCheckVersionMethod == enumCVM_CRC)
		)
    {
        pUpdateItem->uMajorVersion = 0;
        pUpdateItem->uMinorVersion = 0;        
    }
    else
    {
        nRetCode = GetVersionFromString(szValue, &(pUpdateItem->uMajorVersion), &(pUpdateItem->uMinorVersion));
		
        KAV_PROCESS_ERROR(nRetCode);
    }
	
    // --------------- End of Process Version -----------------------
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "Reboot", "", szValue, 50, m_szTxtFileName);
    KAV_PROCESS_ERROR(dwRetCode);
    if (!stricmp(szValue,  "FALSE") || !stricmp(szValue, "0"))
        pUpdateItem->nReboot = 0;
    else 
        pUpdateItem->nReboot = 1;
	
    // Update Relative flag
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "UpdateRelative", "", szValue, 50, m_szTxtFileName);
    if (!stricmp(szValue,  "TRUE") || !stricmp(szValue, "1"))
        pUpdateItem->nUpdateRelative = 1;
    else 
        pUpdateItem->nUpdateRelative = 0;
	
    pUpdateItem->nBelong = ::GetPrivateProfileInt(pUpdateItem->szItemName, "Belong", -1, m_szTxtFileName);
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "SharedNeedReboot", "0", szValue, 50, m_szTxtFileName);
    if (!stricmp(szValue,  "TRUE") || !stricmp(szValue, "1"))
        pUpdateItem->nSharedNeedReboot = 1;
    else 
        pUpdateItem->nSharedNeedReboot = 0;
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "RemotePath", "", pUpdateItem->szRemotePath, MAX_PATH, m_szTxtFileName); 
    KAV_PROCESS_ERROR(dwRetCode);
 //   if (strcmp(pUpdateItem->szRemotePath, ".") == 0)
    if (pUpdateItem->szRemotePath[0] == '.' && (pUpdateItem->szRemotePath[1] == 0 || pUpdateItem->szRemotePath[1] == '/'))
      *(pUpdateItem->szRemotePath) = '\0';
    
    // --------------- Process LocalPath -----------------------
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "LocalPath", "%PROGRAMDIR%", pUpdateItem->szLocalPath, MAX_PATH, m_szTxtFileName);
    KAV_PROCESS_ERROR(dwRetCode);
    nRetCode = GetActuallyPath(pUpdateItem->szLocalPath, g_PathTable.szUpdateDestPath);
    // --------------- End of Process LocalPath -----------------------
	
    if (nUpdateMethod == defUPDATE_METHOD_INTERNET)
    {
        strcpy(pUpdateItem->szDownloadTmpFileName, g_PathTable.szDownloadDestPath);
        strcat(pUpdateItem->szDownloadTmpFileName, pUpdateItem->szRemotePath);
		nLen = strlen(pUpdateItem->szDownloadTmpFileName) - 1;
		for (int i = 0; i <= nLen; i++)
        {
            if (pUpdateItem->szDownloadTmpFileName[i] == '/')
                pUpdateItem->szDownloadTmpFileName[i] = '\\';
        }     
		
        AddPathChar(pUpdateItem->szDownloadTmpFileName);        
    }
    else
    {
        AddPathChar(szLocalPath);
        strcpy(pUpdateItem->szDownloadTmpFileName, szLocalPath); 		
		strcat(pUpdateItem->szDownloadTmpFileName, pUpdateItem->szRemotePath);
		nLen = strlen(pUpdateItem->szDownloadTmpFileName) - 1;
        for (int i = 0; i <= nLen; i++)
        {
            if (pUpdateItem->szDownloadTmpFileName[i] == '/')
                pUpdateItem->szDownloadTmpFileName[i] = '\\';
        }
        AddPathChar(pUpdateItem->szDownloadTmpFileName);
    }
    //MkDirEx(pUpdateItem->szDownloadTmpFileName);
    strcat(pUpdateItem->szDownloadTmpFileName, pUpdateItem->szFileName);
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "Description", "", pUpdateItem->szDescription, 200, m_szTxtFileName);
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "ApplyVersion", "", pUpdateItem->szApplyVersion, defApplyVersionSize, m_szTxtFileName);
    
    if (nUpdateMethod == defUPDATE_METHOD_INTERNET)
    {
        nRetCode = CheckFileCRC(atoi(pUpdateItem->szCRC), pUpdateItem->szDownloadTmpFileName);
        if (nRetCode)
            pUpdateItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADED;
        else
            pUpdateItem->DownloadStatus = enumDOWNLOADSTATUS_QUEUE;
    }
	
    pUpdateItem->strReadme = "";
    for (nComment = 1; ; nComment++)
    {
        sprintf(szCommentName, "Comment%d", nComment);
        dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, szCommentName, "", szBuffer, nBufferLen + 1, m_szTxtFileName); 
        if (*szBuffer == '\0')
            break;
		
        pUpdateItem->strReadme += (CString)szBuffer + "\r\n";
    }
	
    if (!pUpdateItem->strReadme.IsEmpty())
    {
        if (
            (IsSignFileName(pUpdateItem->szFileName)) || 
            (IsUpdFileName(pUpdateItem->szFileName))  || 
            (stricmp(pUpdateItem->szFileName, "KAExtend.DAT") == 0)
			)
            m_sVirusNames += pUpdateItem->strReadme + "\r\n";
        else
            m_sReadme += pUpdateItem->strReadme;
    }
	
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "CallBy", "", pUpdateItem->szCallBy, MAX_PATH, m_szTxtFileName);
	
    *szValue = '\0';
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "FileType", "Program", szValue, 50, m_szTxtFileName);
    pUpdateItem->nItemType = -1;
    dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "NeedDownload", "1", szValue, 50, m_szTxtFileName);
    if (!stricmp(szValue,  "TRUE") || !stricmp(szValue, "1"))
        pUpdateItem->bNeedDownload = 1;
    else 
        pUpdateItem->bNeedDownload = 0; 
	
	dwRetCode = ::GetPrivateProfileString(pUpdateItem->szItemName, "UpdateFileMethod", "Copy", pUpdateItem->szUpdateFileMethod, MAX_PATH, m_szTxtFileName);
	
    pUpdateItem->bNeedUpdate = false;
    nResult = true;
Exit0:
    return nResult;
}


// Return -1 if Version < szDate, 
// Return 0  if Version == szDate,
// Return 1  if Version > szDate
int _CheckVersionByRes(const char cszFileName[], char szDate[])
{
    IMAGE_DOS_HEADER DOSHeader;
    IMAGE_VXD_HEADER VXDHeader;
    CFile File;
    int nResult = 0;
    
    if (File.Open(cszFileName, CFile::modeRead | CFile::shareDenyNone, NULL))
    {
        UINT uReadLen = 0;
        uReadLen = File.Read(&DOSHeader, sizeof(DOSHeader));
        if ((uReadLen == sizeof(DOSHeader)) && (IMAGE_DOS_SIGNATURE == DOSHeader.e_magic))
        {
            File.Seek(DOSHeader.e_lfanew, CFile::begin);
            uReadLen = File.Read(&VXDHeader, sizeof(VXDHeader));
            if ((uReadLen == sizeof(VXDHeader)) && (IMAGE_OS2_SIGNATURE_LE == VXDHeader.e32_magic))
            {
                char szBuf[1024];
                File.Seek(VXDHeader.e32_winresoff, CFile::begin);
                uReadLen = File.Read(szBuf, VXDHeader.e32_winreslen);
                if (uReadLen == VXDHeader.e32_winreslen)
                {
                    unsigned i = 0;
                    for (; i < uReadLen; i++)
                    {
                        if (stricmp(szBuf + i, "FILEVERSION"))
                            continue;
						
                        char *pszVersion = szBuf + i + 11;   // 2001, 2, 23, 27
                        while (*pszVersion == 0)
                            pszVersion++;
                        
                        i = 0;
                        int j = 0;
                        
                        while (*pszVersion)        // cszDate = "2001.2.23,27"
                        {
                            if (szDate[j] == '\0')
                                break;
                            
                            i = _StrToInt(pszVersion);
                            
                            while (*pszVersion != ',')
                            {
                                if (*pszVersion == 0)
                                    break;
                                pszVersion++;
                            }
                            
                            while ((*pszVersion == 0x20) || (*pszVersion == ','))
                                pszVersion++;
                            
                            if (i < (unsigned)_StrToInt(szDate + j))
                            {
                                nResult = -1;
                                break;
                            }
                            else if (i > (unsigned)_StrToInt(szDate + j))
                            {
                                nResult = 1;
                                break;
                            }
                            
                            
                            while ((szDate[j] != '.') && (szDate[j] != '\0'))
                                j++;
                            if (szDate[j] == '.')
                                j++;
                            else
                                break;
                        }
                        
                        if ((*pszVersion == '\0') && (szDate[j] == '\0'))
                            nResult = 0;
                        
                        break;
                    }
                }
            }
        }
        
        File.Close();
    }
    
    return nResult;
}   


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessIndex::CProcessIndex()
{
    m_szFileName[0]           = '\0';
    m_szTxtFileName[0]        = '\0';
    m_bInitFlag               = false; 
    
	m_bVersionError           = true;
    m_nVersion_ValueCount     = 0;  
	m_nMajorVersionValueCount = 0;
	m_sWebDownAddress         = "";
    
	m_nMaxDATFileNum          = 0;
    m_nMaxUPDFileNum          = 0;
    m_sVirusNames             = "";
    m_sReadme                 = "";
    m_sAnnounce               = "";
    m_bNeedUpdateSelfFirst    = false;
	
    m_pUpdateItemList         = NULL;
	
    m_pModuleExitInfoList     = NULL;    
    
    m_bFileError              = false;
	
	m_szCallBy[0]			  = '\0';
}

CProcessIndex::~CProcessIndex()
{
    Uninit();
    
    m_szFileName[0]         = '\0';
    m_szTxtFileName[0]      = '\0';
    m_bInitFlag             = false; 
    m_bVersionError         = true;
    m_nVersion_ValueCount   = 0;     
    m_nMaxDATFileNum        = 0;
    m_nMaxUPDFileNum        = 0;
    m_sVirusNames           = "";
    m_sReadme               = "";
    m_sAnnounce             = "";
    m_bNeedUpdateSelfFirst  = false;
    
    m_pUpdateItemList       = NULL;    
}

int CProcessIndex::Init(const char cszFileName[], const char cszTxtFileName[])
{
    int nRetCode    = 0;
    int nResult     = false;
    
    strcpy(m_szFileName, cszFileName);
    strcpy(m_szTxtFileName, cszTxtFileName);
    
    FreeUpdateItemList();
    FreeModuleExitInfoList();
    
    nRetCode = FileExists(m_szFileName);
    KAV_PROCESS_ERROR(nRetCode);
    
    // UnCompress index.dat to index.txt
    nRetCode = UnCompressFile(m_szFileName, m_szTxtFileName);
    KAV_PROCESS_ERROR(nRetCode);
    
    m_bInitFlag = true;
	
    nResult = true;
Exit0:
    return nResult;    
}

int CProcessIndex::Uninit()
{
    ::DeleteFile(m_szTxtFileName);
    FreeUpdateItemList();
    FreeModuleExitInfoList();
	
    return true;
}

int CProcessIndex::FreeUpdateItemList()
{
    PKUPDATE_ITEM pUpdateItem = NULL;
	
    while (m_pUpdateItemList)
    {
        pUpdateItem = m_pUpdateItemList->pNext;      
        KAV_DELETE(m_pUpdateItemList);         
        m_pUpdateItemList = pUpdateItem;
    }
    return true;
}

int CProcessIndex::FreeModuleExitInfoList()
{
    PKModuleExitInfoList pModuleExitInfoList = NULL;
	
    while (m_pModuleExitInfoList)
    {
        pModuleExitInfoList = m_pModuleExitInfoList->pNext;      
        KAV_DELETE(m_pModuleExitInfoList);         
        m_pModuleExitInfoList = pModuleExitInfoList;
    }
    return true;
}

int CProcessIndex::InitModuleExitInfo()
{
    int nResult = false;
    int i;
    char szKeyName[MAX_PATH] = {0};
    char szFileName[MAX_PATH] = {0};
	char szReleaseFileName[MAX_PATH] = {0};
    PKModuleExitInfoList pModuleExitInfoList = NULL;
	
    KAV_PROCESS_ERROR(m_bInitFlag);
	
    for (i = 0; ; i++)
    {
        sprintf(szKeyName, "FileName%d", i);
        ::GetPrivateProfileString("ExitType", szKeyName, "", szFileName, MAX_PATH, m_szTxtFileName);
        if (szFileName[0] == '\0')
            break;
		
        pModuleExitInfoList = new KModuleExitInfoList;
        if (!pModuleExitInfoList)
            goto Exit0;
		
        pModuleExitInfoList->pNext = NULL;
        strcpy(pModuleExitInfoList->MouleExitInfo.szFileName, szFileName);
		
		sprintf(szKeyName, "ReleaseFileName%d", i);
        ::GetPrivateProfileString("ExitType", szKeyName, "", szReleaseFileName, MAX_PATH, m_szTxtFileName);
        if (szReleaseFileName[0] == '\0')
            strcpy(szReleaseFileName, szFileName);
		
		strcpy(pModuleExitInfoList->MouleExitInfo.szReleaseFileName, szReleaseFileName);
		
        sprintf(szKeyName, "ExitType%d", i);
        pModuleExitInfoList->MouleExitInfo.nExitType = ::GetPrivateProfileInt("ExitType", szKeyName, 0, m_szTxtFileName);
		
		pModuleExitInfoList->MouleExitInfo.uID = 0;
		pModuleExitInfoList->MouleExitInfo.szStartParam[0] = '\0';
		pModuleExitInfoList->MouleExitInfo.szStopParam[0] = '\0';
        
        switch (pModuleExitInfoList->MouleExitInfo.nExitType)
        {
        case defEXIT_TYPE_DIRECTORY:
        case defEXIT_TYPE_IPC:
		case defEXIT_TYPE_RPC:
            sprintf(szKeyName, "StopParam%d", i);
            ::GetPrivateProfileString("ExitType", szKeyName, "", pModuleExitInfoList->MouleExitInfo.szStopParam, MAX_PATH, m_szTxtFileName);    
            sprintf(szKeyName, "StartParam%d", i);
            ::GetPrivateProfileString("ExitType", szKeyName, "", pModuleExitInfoList->MouleExitInfo.szStartParam, MAX_PATH, m_szTxtFileName);    
			
            if ((pModuleExitInfoList->MouleExitInfo.nExitType == defEXIT_TYPE_IPC) ||
				(pModuleExitInfoList->MouleExitInfo.nExitType == defEXIT_TYPE_RPC)
				)
            {
                sprintf(szKeyName, "UID%d", i);
                pModuleExitInfoList->MouleExitInfo.uID = ::GetPrivateProfileInt("ExitType", szKeyName, 0, m_szTxtFileName);    
            }
			
            break;								
			
        case defEXIT_TYPE_SERVICE:
            break;
        default:
            break;
        }
		
        if (!m_pModuleExitInfoList)
        {
            m_pModuleExitInfoList = pModuleExitInfoList;
        }
        else
        {
            pModuleExitInfoList->pNext = m_pModuleExitInfoList;
            m_pModuleExitInfoList = pModuleExitInfoList;
        }
		
        pModuleExitInfoList = NULL;
    }
    // For old index 
    if (0 == i)
    {
        pModuleExitInfoList = new KModuleExitInfoList;
        if (!pModuleExitInfoList)
            goto Exit0;
		
        strcpy(pModuleExitInfoList->MouleExitInfo.szFileName, "KAV9X.EXE");
        pModuleExitInfoList->MouleExitInfo.nExitType = 1;
        pModuleExitInfoList->MouleExitInfo.uID = 1;
        strcpy(pModuleExitInfoList->MouleExitInfo.szStopParam, "/quit");
		pModuleExitInfoList->MouleExitInfo.szStartParam[0] = '\0';
        pModuleExitInfoList->pNext = NULL;
		
        m_pModuleExitInfoList = pModuleExitInfoList;
		
        pModuleExitInfoList = new KModuleExitInfoList;
        if (!pModuleExitInfoList)
            goto Exit0;
		
        strcpy(pModuleExitInfoList->MouleExitInfo.szFileName, "Watcher.EXE");
        pModuleExitInfoList->MouleExitInfo.nExitType = 1;
        pModuleExitInfoList->MouleExitInfo.uID = 2;
        strcpy(pModuleExitInfoList->MouleExitInfo.szStopParam, "/quit");
		pModuleExitInfoList->MouleExitInfo.szStartParam[0] = '\0';
		pModuleExitInfoList->pNext = NULL;  
        
        m_pModuleExitInfoList->pNext = pModuleExitInfoList;
    }
	
    nResult = true;
Exit0:
    return nResult;
}


int CProcessIndex::AddItemInfoToList(
									 const char cszSectionName[], int nUpdateMethod,
									 char szLocalPath[],
									 int nMainVersion
									 )
{
    int nResult                 = false;
    int nRetCode                = 0;
    PKUPDATE_ITEM pUpdateItem   = NULL;
    char szTemp[MAX_PATH]       = {0};
    char *pszFileNamePos        = NULL;
    CString sMsg;
    CString sFormat;
    
	
    nRetCode = CheckSectionIsUpdateItem(m_szTxtFileName, cszSectionName);
    if (nRetCode)
    {
		
		
        pUpdateItem = new KUPDATE_ITEM;
        KAV_PROCESS_ERROR(pUpdateItem);
        
        pUpdateItem->pNext = NULL;
        pUpdateItem->nDownTryTimes = 0;
        
        strcpy(pUpdateItem->szItemName, cszSectionName);
        
        nRetCode = GetItemInfo(pUpdateItem, nUpdateMethod, szLocalPath);
        KAV_PROCESS_ERROR(nRetCode);
		
        nRetCode = CheckIsNeedUpdate(pUpdateItem, nMainVersion);
        if (nRetCode)
        {
            if (nUpdateMethod == defUPDATE_METHOD_INTERNET)
            {
                strcpy(szTemp, pUpdateItem->szDownloadTmpFileName);
                pszFileNamePos = strrchr(szTemp, '\\');
                if (pszFileNamePos)
                    *pszFileNamePos = '\0';
				
                MkDirEx(szTemp);
            }
            else
            {
                nRetCode = CheckFileCRC(atoi(pUpdateItem->szCRC), pUpdateItem->szDownloadTmpFileName);
                if (nRetCode)
                    pUpdateItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADED;
                else 
                {
                    pUpdateItem->DownloadStatus = enumDOWNLOADSTATUS_ERROR;
                    if (!m_bFileError)
                        m_bFileError = pUpdateItem->bNeedDownload;
					
                    sFormat = defIDS_FILE_ERROR_FORMAT;
                    sMsg.Format(sFormat, pUpdateItem->szDownloadTmpFileName);
                    g_UpdateData.SaveLog.WriteLogString(sMsg, true);  
                }
            }
			
            if (!m_pUpdateItemList)
            {
                m_pUpdateItemList = pUpdateItem;
            }
            else
            {
                pUpdateItem->pNext = m_pUpdateItemList;
                m_pUpdateItemList = pUpdateItem;                
            }
			
			StrAddStr(m_szCallBy, pUpdateItem->szCallBy);
			
            // Set bNeedUpdateSelfFirst flag
            if ((pUpdateItem->nUpdateRelative) && pUpdateItem->bNeedUpdate)
                m_bNeedUpdateSelfFirst = true;
			
            // Get max KAVXXXXX.Dat file Number
            nRetCode = GetSignFileNumber(pUpdateItem->szFileName);
            if (IsSignFileName(pUpdateItem->szFileName))
            {
                if (nRetCode > m_nMaxDATFileNum)
                    m_nMaxDATFileNum = nRetCode;
            }
            else 
            {
                if (nRetCode > m_nMaxUPDFileNum)
                    m_nMaxUPDFileNum = nRetCode;
            }
        }
        else
        {
            KAV_DELETE(pUpdateItem);
        }        
		
	}
    nResult = true;
Exit0:
    if (!nResult)
	{
        KAV_DELETE(pUpdateItem);
	}
    return nResult;
	
									 
}
									 
int CProcessIndex::AnalysisUpdateFiles
(int nMethod, 
  char szLocalPath[],
  int nMainVersion
 )
 
 
{
	int nResult                     = false;
	int nRetCode                    = 0;
	char szSectionNames[10 * 1024]  = {0};
	int  nSectionNamesSize          = 1024 * 10;
	char *pszSectionName            = NULL;
	int nSectionNameLen             = 0;
	DWORD dwRetCode                 = 0;
	char szTemp[20]                 = {0};
	PKUPDATE_ITEM pUpdateItem       = NULL;
	PKUPDATE_ITEM pPreUpdateItem    = NULL;
	int nBufferLen		            = 1024;
	char szBuffer[1024]             = {0};
	int nFlag                       = 0;
	int nItemNumber                 = 0;
	int i                           = 0;
	
	KAV_PROCESS_ERROR(m_bInitFlag);
	
	// Free item list at first
	FreeUpdateItemList();
	FreeModuleExitInfoList();
	
	m_nMaxDATFileNum = 0;
	m_nMaxUPDFileNum = 0;
	
	// Get Comment information from index.txt
	m_sAnnounce = "";
	
	// Get message for user
	::GetPrivateProfileString(
		"COMMENT", 
		"Readme", 
		"", 
		szBuffer, 
		nBufferLen, 
		m_szTxtFileName
		);
	
	if (*szBuffer != '\0')
	{
		m_sAnnounce = szBuffer;
		m_sAnnounce.Replace("#", "\r\n");
	}
	
	// Initialize Module exit info
	nRetCode = InitModuleExitInfo();
	KAV_PROCESS_ERROR(nRetCode);
	
	// Initialize Version string and version value
	nRetCode = InitVersionInfo();
	KAV_PROCESS_ERROR(nRetCode);

	if(nRetCode == defVersionNotenough)
	{
		nResult = defVersionNotenough;
		::GetPrivateProfileString(
		"Version", 
		"WebDown", 
		"http://www.jxonline.net", 
		url, 
		MAX_PATH, 
		m_szTxtFileName
		);
		goto Exit0;
	}
	
	// Get Update file information from index.txt
	dwRetCode = ::GetPrivateProfileSectionNames(szSectionNames, nSectionNamesSize, m_szTxtFileName);
	KAV_PROCESS_ERROR(dwRetCode);
	
	pszSectionName = szSectionNames;
	while (*pszSectionName)
	{
		// Process one update file item, add to update file list
		nRetCode = AddItemInfoToList(pszSectionName, nMethod, szLocalPath, nMainVersion);
		
		// Get next Item name
		nSectionNameLen = strlen(pszSectionName);
		pszSectionName += nSectionNameLen + 1;
	}
	
	// Update Update.EXE first
	// Delete the not relative file with update.exe
	if (m_bNeedUpdateSelfFirst)
	{                             
		pUpdateItem     = m_pUpdateItemList;                    
		pPreUpdateItem  = pUpdateItem;
		while (pUpdateItem)
		{
			if (!pUpdateItem->nUpdateRelative)
			{
				if (pUpdateItem == pPreUpdateItem)
				{
					m_pUpdateItemList = m_pUpdateItemList->pNext;
					pPreUpdateItem = m_pUpdateItemList;
					pUpdateItem->pNext = NULL;
					delete pUpdateItem;
					pUpdateItem = m_pUpdateItemList;
				}
				else 
				{
					pPreUpdateItem->pNext = pUpdateItem->pNext;                    
					pUpdateItem->pNext = NULL;
					delete pUpdateItem;
					pUpdateItem = pPreUpdateItem->pNext;
				}
				
				continue;
			}        
			
			if (pUpdateItem != m_pUpdateItemList)
				pPreUpdateItem = pPreUpdateItem->pNext;
			pUpdateItem = pUpdateItem->pNext;            
		}
		
		nResult = true;
		goto Exit0;
	}
	
	nResult = true;
Exit0:
	if (!nResult)
	{
		FreeUpdateItemList();    
		FreeModuleExitInfoList();
	}
	
	
	return nResult;
	
}

int CProcessIndex::GetExitType(PKModuleExitInfo pModuleExitInfo)
{
    int nResult = false;
    PKModuleExitInfoList pModuleExitInfoList = NULL;
	
	
    KAV_PROCESS_ERROR(m_bInitFlag);
    KAV_PROCESS_ERROR(pModuleExitInfo);
	
	pModuleExitInfo->nExitType = -1;
	pModuleExitInfo->szReleaseFileName[0] = '\0';
	pModuleExitInfo->szStartParam[0] = '\0';
	pModuleExitInfo->szStopParam[0] = '\0';
	pModuleExitInfo->uID = 0;
	
    pModuleExitInfoList = m_pModuleExitInfoList;
    while(pModuleExitInfoList)
    {
        if (stricmp(pModuleExitInfo->szFileName, pModuleExitInfoList->MouleExitInfo.szFileName) == 0)
        {
            sprintf(pModuleExitInfo->szReleaseFileName, "%s%s", g_PathTable.szUpdateDestPath, pModuleExitInfoList->MouleExitInfo.szReleaseFileName);
            pModuleExitInfo->nExitType = pModuleExitInfoList->MouleExitInfo.nExitType;
            pModuleExitInfo->uID = pModuleExitInfoList->MouleExitInfo.uID;
            strcpy(pModuleExitInfo->szStartParam, pModuleExitInfoList->MouleExitInfo.szStartParam);
            strcpy(pModuleExitInfo->szStopParam, pModuleExitInfoList->MouleExitInfo.szStopParam);
            break;
        }
        pModuleExitInfoList = pModuleExitInfoList->pNext;
    }
	
    if (!pModuleExitInfoList)
        goto Exit0;
	
    nResult = true;
Exit0:
    return nResult;
}

bool CProcessIndex::IsVersionError()
{
    return m_bVersionError;
}

bool CProcessIndex::IsNeedUpdateSelf()
{
    return m_bNeedUpdateSelfFirst;
}

bool CProcessIndex::IsNotUpdateItem()
{
    return (m_pUpdateItemList == NULL);
}

int CProcessIndex::InitVersionInfo()
{
    
    
    if (!m_bInitFlag)
	{
        return false;
	}
	m_nVersion_ValueCount = ::GetPrivateProfileInt
		(
		"Version", 
		"Version", 
		0,  
		m_szTxtFileName
       	);
	
    m_nMajorVersionValueCount = ::GetPrivateProfileInt
		(
		"Version", 
		"MajorVersion", 
		0,  
		m_szTxtFileName
       	);
	
	
	CString strVersion = g_PathTable.szModulePath;
    int nVersionNumber = ::GetPrivateProfileInt
		(
		"Version",
		"MajorVersion",
		0,
		strVersion + "Version.cfg"
		);
    


	char szWebAddressBuffer[MAX_PATH];
	::GetPrivateProfileString("Version", "WebDown", "http://www.jxonline.net", szWebAddressBuffer, MAX_PATH, m_szTxtFileName);
    m_sWebDownAddress = szWebAddressBuffer;


	if(nVersionNumber < m_nMajorVersionValueCount)
	{
		return defVersionNotenough;
	}
	if(nVersionNumber > m_nMajorVersionValueCount) {
		return false;
	}
		
    return true;
}

int CProcessIndex::CheckIsNeedUpdate(
									 PKUPDATE_ITEM pUpdateItem,
									 int nMainVersion
									 )
{
    int nSignMaxNum     = 0;  
    int nRetCode            = 0;
    int nCurrentVersion     = 0;
    char *pszPos            = NULL;
    int nCurrentNumber      = 0;
    char szLocalFullFileName[MAX_PATH] = {0};
    DWORD dwMajorVersion    = 0;
    DWORD dwMinorVersion    = 0;
    int nSignFileFlag       = 0;
    
    KAV_PROCESS_ERROR(pUpdateItem);
	
    pUpdateItem->bNeedUpdate = false;
	
	// Belone Version error
    if (pUpdateItem->nBelong >= 0 && pUpdateItem->nBelong != nMainVersion)
		goto Exit0;
	
    m_bVersionError = FALSE;
	
    // Data file, such as readme.txt or KAVDX.txt
    if (pUpdateItem->KCheckVersionMethod == enumCVM_DIRECT)
    {
        pUpdateItem->bNeedUpdate = true;
        goto Exit0;
    }
	
    strcpy(szLocalFullFileName, pUpdateItem->szLocalPath);
    strcat(szLocalFullFileName, pUpdateItem->szFileName);
	nRetCode = FileExists(szLocalFullFileName);
    if (!nRetCode)
    {
        pUpdateItem->bNeedUpdate = true;
        goto Exit0;
    }
	
    if (pUpdateItem->KCheckVersionMethod == enumCVM_CRC)
    {
        pUpdateItem->bNeedUpdate = !CheckFileCRC(atoi(pUpdateItem->szCRC), szLocalFullFileName);   // CRC        
        goto Exit0;
    }
	
    if (pUpdateItem->KCheckVersionMethod == enumCVM_TIME)
    {
        nRetCode = CheckFileDate(szLocalFullFileName, pUpdateItem->szFileDate);
        if (nRetCode)
        {
            pUpdateItem->bNeedUpdate = true;
            goto Exit0;
        }
        
        goto Exit1;
    }
    
    if ((pUpdateItem->KCheckVersionMethod == enumCVM_SIGN) &&
        (IsSignFileName(pUpdateItem->szFileName) || IsUpdFileName(pUpdateItem->szFileName)) 
		)
    {
        nRetCode = GetLocalSignVersion(&dwMajorVersion, &dwMinorVersion); 
        nSignFileFlag = 1;
    }
    else
    {
        // Such as KAEngine.dat, KAEPlats KAEngine.dat, KAEPlat.dll, KAV9X.EXE......
        
        nRetCode = GetKAVFileVersion(szLocalFullFileName, pUpdateItem->KCheckVersionMethod, &dwMajorVersion, &dwMinorVersion);
        if (!nRetCode)
		{
			nRetCode = _CheckVersionByRes(szLocalFullFileName, pUpdateItem->szFileDate);
            if (nRetCode < 0)
                pUpdateItem->bNeedUpdate = true;
            else if (nRetCode == 0)
                goto Exit1;
			
            goto Exit0;
		}
    }
	
    pUpdateItem->bNeedUpdate = (
        (pUpdateItem->uMajorVersion > dwMajorVersion) ||
        (
		(pUpdateItem->uMajorVersion == dwMajorVersion) && 
		(pUpdateItem->uMinorVersion > dwMinorVersion)
        ) 
		);
	
    if (pUpdateItem->bNeedUpdate || 
        !((pUpdateItem->uMajorVersion == dwMajorVersion) && 
		(pUpdateItem->uMinorVersion == dwMinorVersion))
		)
        goto Exit0;
	
Exit1:
    // if is the same version but the file was destroy then update this file
    if (nSignFileFlag) 
    {
		nRetCode = CheckExistSameVersionFile(pUpdateItem->szFileName);
		if (nRetCode == 0)
			pUpdateItem->bNeedUpdate = true;
		else if (nRetCode == 1)
			pUpdateItem->bNeedUpdate = false;
		else
			pUpdateItem->bNeedUpdate = !CheckFileCRC(atoi(pUpdateItem->szCRC), szLocalFullFileName);
    }
    else
        pUpdateItem->bNeedUpdate = !CheckFileCRC(atoi(pUpdateItem->szCRC), szLocalFullFileName);
	
Exit0:
    return pUpdateItem->bNeedUpdate;
}

CString CProcessIndex::GetVirusNames()
{
    return m_sVirusNames;
}

CString CProcessIndex::GetReadme()
{
    return m_sReadme;
}

CString CProcessIndex::GetAnnounce()
{
    return m_sAnnounce;
}

int CProcessIndex::GetHost(char szHostName[], int *pnNameSize, char szHostURL[], int *pnURLSize)
{
    int nResult = false;    
    
    if (!pnNameSize || !pnURLSize)
        goto Exit0;
	
	
	
    nResult = true;
Exit0:
    return nResult;
}

int CProcessIndex::GetDubaVersion()
{
    int nResult = -1;
	
    if (!m_bInitFlag)  
		return nResult;
	
	nResult = ::GetPrivateProfileInt("COMMENT", "Version", 0, m_szTxtFileName);
	
	return nResult;
}

int CProcessIndex::GetPath(char szPath[], int nSize)
{
	if (!m_bInitFlag)  
		return false;
	
	::GetPrivateProfileString("COMMENT", "PathName", "", szPath, nSize, m_szTxtFileName);
	
	return true;    
}