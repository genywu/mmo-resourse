//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   ProcessIndex.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-6-25 10:07:15
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

// ProcessIndex.h: interface for the CProcessIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSINDEX_H__11959E05_CF66_4A40_81F4_D13ED5311C27__INCLUDED_)
#define AFX_PROCESSINDEX_H__11959E05_CF66_4A40_81F4_D13ED5311C27__INCLUDED_

#include "DataDefine.h"
#include <string>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define defEXIT_TYPE_DIRECTORY        0x00000000       // Exit directory by parameter
#define defEXIT_TYPE_IPC              0x00000001       // Exit by parameter and IPC
#define defEXIT_TYPE_SERVICE          0x00000002       // Service
#define defEXIT_TYPE_RPC			  0x00000003       // Exit by parameter and RPC

typedef struct tagKModuleExitInfo
{
    char szFileName[MAX_PATH];   // Module Name : file name or service name
	char szReleaseFileName[MAX_PATH]; // Release Module Name : File Name or service name
    int nExitType;
    unsigned uID;
    char szStopParam[MAX_PATH];
    char szStartParam[MAX_PATH];  

} KModuleExitInfo, *PKModuleExitInfo;

typedef struct tagKModuleExitInfoList
{
    tagKModuleExitInfoList *pNext;
    KModuleExitInfo MouleExitInfo;
} KModuleExitInfoList, *PKModuleExitInfoList;


#define defVersion_ValueSize                512
#define defVersionNotenough                 100
#define defVersionMore						101

class CProcessIndex  
{
private:
    char m_szFileName[MAX_PATH];
    char m_szTxtFileName[MAX_PATH];
    bool m_bInitFlag;
    int m_nVersion_ValueCount;
	int m_nMajorVersionValueCount;
	string m_sWebDownAddress;
   // KVERSION_VALUE m_Version_Value[defVersion_ValueSize];

    bool m_bVersionError;
    int m_nMaxDATFileNum;
    int m_nMaxUPDFileNum;

    CString m_sVirusNames;
    CString m_sReadme;
    CString m_sAnnounce;   

    PKModuleExitInfoList m_pModuleExitInfoList;
    
    int FreeModuleExitInfoList();
    int FreeUpdateItemList();

    int InitModuleExitInfo();
    int InitFileTypeInfo();
    int InitVersionInfo();

    int CheckIsNeedUpdate(
        PKUPDATE_ITEM pUpdateItem,
        int nMainVersion
    );
    int GetItemInfo(
        PKUPDATE_ITEM pUpdateItem, 
        int nUpdateMethod, char szLocalPath[]
    );

    int AddItemInfoToList(
        const char cszSectionName[], int nUpdateMethod,
        char szLocalPath[],
        int nMainVersion
    );    

public:

	char url[MAX_PATH];
	int Uninit();
	int Init(const char cszFileName[], const char cszTxtFileName[]);	
	int AnalysisUpdateFiles(
        int nMethod, 
        char szLocalPath[],
        int nMainVersion
    );    
    
    int GetExitType(PKModuleExitInfo pModuleExitInfo);
    int GetDownloadFileTypeInfo(CString &sMsg, int nDownloadFileType);
    int GetHost(char szHostName[], int *pnNameSize, char szHostURL[], int *pnURLSize);
    int GetPath(char szPath[], int nSize);


    int GetDubaVersion();

    bool IsVersionError();
    bool IsNeedUpdateSelf();
    bool IsNotUpdateItem();    
    CString GetVirusNames();
    CString GetReadme();
    CString GetAnnounce();

	CProcessIndex();
	virtual ~CProcessIndex();

    PKUPDATE_ITEM m_pUpdateItemList;
	char m_szCallBy[2048];
    bool m_bNeedUpdateSelfFirst;
    bool m_bFileError;
    
    
};

#endif // !defined(AFX_PROCESSINDEX_H__11959E05_CF66_4A40_81F4_D13ED5311C27__INCLUDED_)
