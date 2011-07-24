//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   GetVersion.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2001-11-20 4:54:50
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _GETVERSION_H_
#define _GETVERSION_H_ 1

int GetVersionStringFromInt(
    int nMajorVersion, int nMinorVersion, 
    char szVersion[], int nVersionLen
);

int GetVersionFromString(
    const char cszVersion[], 
    unsigned *puMajorVersion, 
    unsigned *puMinorVersion
);

int GetFileVersion(
    const char cszFileName[], 
    DWORD *pdwProductVersionMS, 
    DWORD *pdwProductVersionLS
);

int GetVersion(const char cszFileName[], char *pszVersion, int nVersionSize);

int GetProgramVersion(CString &sVersion);

#endif  // _GETVERSION_H_

