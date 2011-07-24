//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   PublicFun.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-4-8 16:40:16
//  Comment     :   Define the public function
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __PUBLIC_FUNCTION_H__
#define __PUBLIC_FUNCTION_H__


#pragma pack(push, 1)

typedef struct tagFILE_MAPPING
{
	HWND hWnd;
}FILE_MAPPING, *PFILE_MAPPING;

#pragma pack(pop)

 
void AddPathChar(char szPath[], char chPathChar); 
void AddPathChar(char szPath[]);
void MkDirEx(const char cszPathDir[]);
int FileExists(LPCTSTR lpszFileName);

int CopyDir(const char cszSource[], const char cszDest[], int nFailedIfExist);

int StopAService(const char cszServiceName[]);
int StartAService(const char cszServiceName[]);

// Get Exit CommandLine parameter
// Return 1:  Quit Parameter
// Return 0:  No Quit parameter
int GetExitCommandLineParameter();

void StrAddStr(char szStr1[], const char cszStr2[]);

int GetRegKeyValue(const char cszRegKey[], const char cszKeyName[], char szValue[], int nSize);

#endif  //__PUBLIC_FUNCTION_H__