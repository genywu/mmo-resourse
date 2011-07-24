//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   LoadHostList.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-7 16:26:05
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoadHostList.h"
#include "PublicFun.h"
#include "Global.h"
#include "DataDefine.h"
#include "Resource.h"
#include "UpdateData.h"

extern KPATH_TABLE g_PathTable;
extern KUPDATE_DATA g_UpdateData;

int KHostList::SaveHostListFileInfo(const char cszDestFileName[], const char cszSrcFileName[])
{
    char szFileName[MAX_PATH]   = {0};
    char szCRC32[20]            = {0};
    UINT uCRC32                 = 0;
    
    strcpy(szFileName, g_PathTable.szModulePath);
    strcat(szFileName, cszSrcFileName);
    uCRC32 = GetFileCRC(szFileName);
    sprintf(szCRC32, "%d", uCRC32);
    WritePrivateProfileString(cszSrcFileName, "CRC32", szCRC32, cszDestFileName);
 
    return true;
}

int KHostList::IsHostListFileChanged(const char cszDestFileName[], const char cszSrcFileName[])
{
    char szFileName[MAX_PATH]   = {0};
    int nRetCode                = 0;
    UINT uCRC32                 = 0;
    
    strcpy(szFileName, g_PathTable.szModulePath);
    strcat(szFileName, cszSrcFileName);
    uCRC32 = GetPrivateProfileInt(cszSrcFileName, "CRC32", 0, cszDestFileName);

    if (!FileExists(szFileName) && uCRC32)
        return true;
   
    nRetCode = CheckFileCRC(uCRC32, szFileName);
    if (nRetCode)
        return false;
    
    return true;
}

void KHostList::AddHostList(KHOST_LIST *pSrcHostList, KHOST_LIST **ppDestHostList)
{
    KHOST_LIST *pHostList = NULL;

    if (!ppDestHostList || !pSrcHostList)
        return;

    if ((*ppDestHostList) == NULL)
    {
        *ppDestHostList = pSrcHostList;
    }
    else
    {
        pHostList = *ppDestHostList;
        while (pHostList->pNext)
            pHostList = pHostList->pNext;

        pHostList->pNext = pSrcHostList;        
    }
}

int KHostList::LoadOrdHostList(const char cszFileName[], KHOST_LIST **ppListHost)
{
    int nResult = 0;
    int i;
    char szHostURLKey[10]       = {0};
    char szHostNameKey[10]      = {0};
    char szIndexKey[10]         = {0};
    char szHostURL[MAX_PATH]    = {0};
    char szHostName[MAX_PATH]   = {0};
    int nIndex                  = 0;
    DWORD dwSize                = 0;
    DWORD dwRetCode             = 0;
    KHOST_LIST *pHostNode       = NULL;


    KAV_PROCESS_ERROR(ppListHost);        

    if (FileExists(cszFileName))
    {
        for (i = 0; ; i++)
        {
            sprintf(szHostURLKey, "URL%d", i);
            sprintf(szHostNameKey, "Name%d", i);
            sprintf(szIndexKey, "Index%d", i);

            dwSize = MAX_PATH;
            dwRetCode = GetPrivateProfileString("HostList", szHostURLKey, "", szHostURL, dwSize, cszFileName);                    
            if (*szHostURL == '\0')
                break;

            dwSize = MAX_PATH;
            dwRetCode = GetPrivateProfileString("HostList", szHostNameKey, "", szHostName, dwSize, cszFileName);
            if (*szHostName == '\0')
                break;

            nIndex = GetPrivateProfileInt("HostList", szIndexKey, 0, cszFileName);

            // add to the list
            pHostNode = new KHOST_LIST;
            KAV_PROCESS_ERROR(pHostNode);
            
            pHostNode->nIndex = nIndex;
            strcpy(pHostNode->szName, szHostName);
            strcpy(pHostNode->szURL, szHostURL);
            pHostNode->pNext = NULL;

            AddHostList(pHostNode, ppListHost);

            pHostNode = NULL;
        }   
    }

    nResult = true;
Exit0:
    return nResult;
}

int KHostList::LoadSrcHostList(const char cszFileName[], KHOST_LIST **ppHostList)
{
    int nResult             = false;
    CString szDubaUpdateName;
    CString sPathName;
    CStdioFile file;
    KHOST_LIST *pHostNode   = NULL;

    KAV_PROCESS_ERROR(ppHostList);

	// Read host information from Update.svr or Update.ext
    if (g_PathTable.szModulePath[0] == '\0')
        goto Exit0;

	sPathName = (CString)g_PathTable.szModulePath + cszFileName;
	if (!FileExists(sPathName))
    {
        if (stricmp(cszFileName, defHOST_LIST_FILE_EXT) != 0)
        {
            // Default Host
            pHostNode = new KHOST_LIST;
            KAV_PROCESS_ERROR(pHostNode);

            pHostNode->pNext = NULL;
            pHostNode->nIndex = -1;
            strcpy(pHostNode->szName, g_UpdateData.szDefHostName);
            strcpy(pHostNode->szURL, g_UpdateData.szDefHostURL);
            
            AddHostList(pHostNode, ppHostList);            
        }
        goto Exit1;
    }         	

	if (file.Open(sPathName, CFile::modeRead | CFile::typeText))
	{
		CString sName, sAddress;
        int nIndex = 0;

		while (file.ReadString(sName))
		{
			sName.TrimLeft();
			sName.TrimRight();

			if (sName == "")
				continue;

            while (file.ReadString(sAddress))
			{
				sAddress.TrimLeft();
				sAddress.TrimRight();

				if (sAddress == "")
					continue;

				pHostNode = new KHOST_LIST;
                KAV_PROCESS_ERROR(pHostNode);

                pHostNode->pNext = NULL;
                pHostNode->nIndex = -1;
                strcpy(pHostNode->szName, sName);
                strcpy(pHostNode->szURL, sAddress);

                AddHostList(pHostNode, ppHostList);            
                
                break;
			}
            nIndex++;
		}

		file.Close();
	} 

Exit1:
    nResult = true;

Exit0:
    return nResult;
}

void KHostList::ReleaseHostList(KHOST_LIST **ppHostList)
{
    KHOST_LIST *pHostList = NULL;

    if (ppHostList == NULL)
        return;

    pHostList = *ppHostList;
    while (pHostList)
    {
        *ppHostList = (*ppHostList)->pNext;
        delete pHostList;
        
        pHostList = *ppHostList;
    }   

    *ppHostList = NULL;
}   

KHostList::KHostList()
{
    m_nSVRChangedFlag   = 0;
    m_nEXTChangedFlag   = 0;
    *m_szFileName       = '\0';
    m_pHostList         = NULL;
}

KHostList::KHostList(const char cszFileName[])
{
    LoadList(cszFileName);
}

KHostList::~KHostList()
{
    ReleaseList();
}

void KHostList::ReleaseList()
{
    ReleaseHostList(&m_pHostList);   
}                

int KHostList::LoadList(const char cszFileName[])
{
    strcpy(m_szFileName, cszFileName);
    m_nSVRChangedFlag = IsHostListFileChanged(m_szFileName, defHOST_LIST_FILE_SVR);
    m_nEXTChangedFlag = IsHostListFileChanged(m_szFileName, defHOST_LIST_FILE_EXT);

    return LoadList();
}

int KHostList::LoadList()
{
    int nResult                 = false;
    KHOST_LIST *pHostNode       = NULL;
    KHOST_LIST *pHostList       = NULL;
    KHOST_LIST *pOrdHostList    = NULL;
    KHOST_LIST *pOrdHostNode    = NULL;
    KHOST_LIST *pSrcHostList    = NULL;
    int nRetCode                = 0;

    
    ReleaseList();

    if (m_nSVRChangedFlag || m_nEXTChangedFlag)
    {
        nRetCode = LoadSrcHostList(defHOST_LIST_FILE_SVR, &m_pHostList);
        KAV_PROCESS_ERROR(nRetCode);
        nRetCode = LoadSrcHostList(defHOST_LIST_FILE_EXT, &m_pHostList);
        KAV_PROCESS_ERROR(nRetCode);

        nRetCode = LoadOrdHostList(m_szFileName, &pOrdHostList);
        KAV_PROCESS_ERROR(nRetCode);

        if (pOrdHostList)
        {
            pHostList = m_pHostList;
            while (pHostList)
            {
                pOrdHostNode = pOrdHostList;
                while (pOrdHostNode)
                {
                    if (stricmp(pHostList->szURL, pOrdHostNode->szURL) == 0)
                    {
                        pHostList->nIndex = pOrdHostNode->nIndex;
                        break;
                    }
                    pOrdHostNode = pOrdHostNode->pNext;
                }

                pHostList = pHostList->pNext;
            }

            ReleaseHostList(&pOrdHostList);
        }
    }
    else
    {
        nRetCode = LoadOrdHostList(m_szFileName, &m_pHostList);
        KAV_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}


void KHostList::AddList(KHOST_LIST *pHostNode)
{
    AddHostList(pHostNode, &m_pHostList);
}

int KHostList::SaveList(char *pszFileName /* = NULL */)
{
    char *pszSaveFileName   = NULL;
    KHOST_LIST *pHostList   = NULL;
    char szHostURLKey[10]   = {0};
    char szHostNameKey[10]  = {0};
    char szIndexKey[10]     = {0};
    char szIndex[10]        = {0};    
    int i;


    pszSaveFileName = pszFileName;
    if (pszSaveFileName == NULL)
        pszSaveFileName = m_szFileName;

    DeleteFile(pszSaveFileName);

    SaveHostListFileInfo(pszSaveFileName, defHOST_LIST_FILE_SVR);
    SaveHostListFileInfo(pszSaveFileName, defHOST_LIST_FILE_EXT);

    i = 0;
    pHostList = m_pHostList;    
    while (pHostList)
    {
        sprintf(szHostURLKey, "URL%d", i);
        sprintf(szHostNameKey, "Name%d", i);
        sprintf(szIndexKey, "Index%d", i);
        
        WritePrivateProfileString("HostList", szHostNameKey, pHostList->szName, pszSaveFileName);
        WritePrivateProfileString("HostList", szHostURLKey, pHostList->szURL, pszSaveFileName);
        sprintf(szIndex, "%d", pHostList->nIndex);
        WritePrivateProfileString("HostList", szIndexKey, szIndex, pszSaveFileName);

        i++;
        pHostList = pHostList->pNext;        
    }

    return true;
}

int KHostList::IsHostListChanged()
{
    return (m_nSVRChangedFlag || m_nEXTChangedFlag);
}

