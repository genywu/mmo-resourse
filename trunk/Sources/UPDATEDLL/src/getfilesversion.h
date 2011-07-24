//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   GetFilesVersion.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-2-1 15:49:33
//  Comment     :   The function for get duba files version
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __GET_FILES_VERSION_H__
#define __GET_FILES_VERSION_H__

#include "DataDefine.h"


//------------------------------------------------------------------------------------
int GetKAVFileVersion(
    const char cszFileName[], 
    KCHECKVERSIONMETHOD CheckVersionMethod, 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
);
int GetEngineFileVersion(
    const char cszFileName[], 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
);
int GetSignFileVersion(
    const char cszFileName[], 
    DWORD *pdwMajorVersion, 
    DWORD *pdwMinorVersion
);

int GetLocalSignVersion(DWORD *pdwMajorVersion, DWORD *pdwMinorVersion);
int GetLocalSignVersion(char szVersion[]);

// Check is exist the same version sign file
int CheckExistSameVersionFile(const char cszFileName[]);


int GetLocalSignFileNumber();
int IsSignFile(const char cszPathName[]);
int GetSignFileNumber(const char cszSignName[]);
int IsUpdFileName(const char cszFileName[]);
int IsSignFileName(const char cszFileName[]);
int GetUpdFileNumber(const char cszUpdFileName[]);
int ExistUpdFileByNum(int nNum);

int GetLocalDatFileNumber();

int GetNormalVersion(const char cszPathName[], char szVersion[]);
int GetEngineVersion(const char cszPathName[], char szVersion[]);
int GetVirusCountFromSignFile(const char cszPathName[]);



#endif      //__GET_FILES_VERSION_H__
