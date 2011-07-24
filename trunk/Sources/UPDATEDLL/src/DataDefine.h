//////////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DataDefine.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2001-12-21 17:26:18
//  Comment     :   
//  
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_  1

#include "KAESign.h"
#include "SaveLog.h"
//#include "KIPCUtility.h"
#include "DownloadFile.h"

#define defINDEX_FILE_NAME                  "index.dat"
#define defINDEX_TXT_FILE_NAME              "index.txt"

#define defUPDATE_DIRECTORY                 "Update\\"
#define defUPDATE_SELF_FILE                 "UpdateSelf.DAT"
#define defUPDATE_SELF_INFO_FILE            "Update.inf"
#define defDIALOG_FILE_NAME                 "KAVDialog.EXE"

#define defUPDATE_FILE_BAT_FILE_NAME        "KAVMove.BAT"
#define defUPDATE_FILE_EXE_FILE_NAME        "KAVMove.EXE"
#define defUPDATE_FILE_EXE_FILE_NAME_ME     "CopyFile.EXE"
#define defUPDATE_FILES_INI_FILE_NAME       "CopyFile.ini"


#define defBASE_SIGN_UPDNAME                "KAV00000.UPD"
#define defBASE_SIGN_FILENAME               "KAV00000.DAT"

#define defKAV_CHKSN_CGI_TEMPFILE           "kcgichk.tmp"
#define defKAV_CHKSN_CGI_REQUEST            "http://www.duba.net/cgi-bin/chksn3.cgi?sn="


/////////////////////////////////////////////////////////////////
// Define the data stuct of update system
/////////////////////////////////////////////////////////////////

#define defUPDATE_METHOD_INTERNET           0
#define defUPDATE_METHOD_LAN                1




// Download item type
#define defITEMTYPE_NONE            0x00
#define defITEMTYPE_INCSIGN         0x01   // Increment sign files(KAVXXXXX.UPD¡¢KAVXXXXX.DAT)
#define defITEMTYPE_MAXSIGN         0x02   // Whole sign file (KAVXXXXX.DAT)
#define defITEMTYPE_SIGN            0x03   // Sign files
#define defITEMTYPE_PROGRAM         0x04   // Program files
#define defITEMTYPE_ENGINE          0x08   // Engine files
#define defITEMTYPE_ASSISTANT       0x10   // Tools and help files


// Operation system platform type
#define defOSPLATFORM_WIN31         0x00    //Win3.1
#define defOSPLATFORM_WIN9598       0x01    //Win95/98
#define defOSPLATFORM_WINME         0x02    //WinME
#define defOSPLATFORM_WINNT         0x03    //WinNT


//----------------------------------------------------------------------
// Define return value of checke serial number
#define defInvalidSerialNumber    -1
#define defPiraticSerialNumber    -2
#define defInhibitiveSerialNumber -3
#define defConnectServerFailed	  -4




#define defINSTALL_COMPONENT_KEY_NAME       "\\Install\\"

#pragma pack(push, 1)   

// Struct of Engine file header
typedef struct tagKAVEngineFileHeader
{
	char  cszDescription[48];
    DWORD dwSizeOfHeader;		
    DWORD dwMajorVersion;		    //Major Version
    DWORD dwMinorVersion;		    //Minor Version
    DWORD dwOrignMajorVersion;      //Orign PE File Major Version
    DWORD dwOrignMinorVersion;      //Orign PE File Minor Version
    DWORD dwOrignLength;
    DWORD dwCompressFlag;           //defKSCOMPRESS_FLAG
    DWORD dwSizePacked;             //Use for Local Second File Position
    DWORD dwCheckSum;
    char cszReserve[64];
}KAVEngineFileHeader, *PKAVEngineFileHeader;

// Struct of sign file header
typedef struct tagKAVSignFileHeader
{
    char  cszDescription[0x50];     //"Kingsoft AntiVirus Database. Copyright (c) 1999, 2000 Kingsoft AntiVirus Lab.\x1A"
    
    DWORD dwVersion;                //dat file version
    DWORD dwDateStamp;              //Update Date, High 16 bit is year, Low 16 bit is Month(High 8 bit),Day(Low 8 bit)
    DWORD dwProcessVirusNum;        //Number of virus can be process
    WORD  wPackMethod;              //0: data is not pack 1: pack by Aplib
    WORD  wEncryptMethod;           //0: data is not Encrypt
    DWORD dwCheckSum;               //CheckSum value for data, CRC32
    DWORD dwDataAreaEntry;          //Data Area Entry offset in file
    DWORD dwDataAreaLen;            //Total Length of Data Area
    DWORD dwSignDataItemCount;
    
    KAE_KAVSIGNDATA_ITEM SignDataItem[defMAXSIGNDATAITEM];
    
} KAVSIGNFILEHEADER;


#define defVERSION_SIZE                     20


typedef struct tagKVERSION_VALUE
{
    char szVersionStr[MAX_PATH];
    __int64  nVersion;
} KVERSION_VALUE;


typedef struct tagKPATH_TABLE
{
    char szModulePath[MAX_PATH];    // Update.exe path
    char szSystemPath[MAX_PATH];
    char szWindowsPath[MAX_PATH];
    char szTempPath[MAX_PATH];
    char szCurrentPath[MAX_PATH];
    char szDownloadDestPath[MAX_PATH];      // Download destination path
    char szUpdateDestPath[MAX_PATH];        // Update destination path
} KPATH_TABLE;

// Check version method
typedef enum _KCHECKVERSIONMETHOD
{
    enumCVM_NORMAL      = 0,
    enumCVM_ENGINE      = 1, 
    enumCVM_SIGN        = 2, 
    enumCVM_CRC         = 3, // new method, Check Version by CRC32(in old update program, download directly);
    enumCVM_DIRECT      = 4, // new method, download directly;
    enumCVM_TIME        = 5, // new method, compare file time with version;
    enumCVM_SNMASK	    = 6  // new method, by serial number mask;
} KCHECKVERSIONMETHOD;

// Download status
typedef enum _KAVDOWNLOADSTATUS
{
    enumDOWNLOADSTATUS_QUEUE        = 0,    // Wait for download
    enumDOWNLOADSTATUS_DOWNLOADING	= 1,    // Downloading 
    enumDOWNLOADSTATUS_ERROR        = 2,    // Error
    enumDOWNLOADSTATUS_DOWNLOADED	= 3     // Downloaded at local
} KAVDOWNLOADSTATUS;

// Struct of update item

#define defApplyVersionSize         1024
#define defSNMaskSize               30
typedef struct tagKUPDATE_ITEM
{
    tagKUPDATE_ITEM *pNext;
    
    char    szItemName[100];
    char    szFileName[MAX_PATH];
    
    int     nFileSize;
    char    szFileDate[20];
    unsigned    uMajorVersion;
    unsigned    uMinorVersion;
    
    KCHECKVERSIONMETHOD KCheckVersionMethod;
    
    int     nDownTryTimes;              //L: current download retry times
    KAVDOWNLOADSTATUS   DownloadStatus; //L: current download status
    
    int     nReboot;                    //L: request reboot for this download file after update
    int     nSharedNeedReboot;          //L: need reboot when update occur shared conflict
    int     nItemType;                  // File type: Program/IncSign/MaxSign
    char    szRemotePath[MAX_PATH];     //L: remote full url path
    char    szLocalPath[MAX_PATH];      //L: current save local path
    char    szDescription[256];         //L: donwload file description
    char    szApplyVersion[defApplyVersionSize];        // Apply version: Standard/OEM/Share
    char    szSNMask[defSNMaskSize];               // Serial mask
    char    szCRC[20];                  // download file CRC check string
    char    szDownloadTmpFileName[MAX_PATH];
    CString strReadme;
    int     nUpdateRelative;            // Update relative flag, update self first
    int     nBelong;                    // Belong to version
    
    bool    bNeedUpdate;
    bool    bNeedDownload;
    
    char    szCallBy[MAX_PATH];         // Module name of Call by  : KAV9X.EXE,KAVPlus.EXE

	char    szUpdateFileMethod[MAX_PATH];         // Module name of Call by  : KAV9X.EXE,KAVPlus.EXE

} KUPDATE_ITEM, *PKUPDATE_ITEM;

#pragma pack(pop)

#endif //_DATA_DEFINE_H_
