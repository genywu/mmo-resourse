//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   Global.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2001-12-28 15:56:04
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATE3_GLOBAL_H_
#define _UPDATE3_GLOBAL_H_ 1


#include "DataDefine.h"


extern KPATH_TABLE  g_PathTable;

int InitPaths();
int CheckFileDate(const char cszFileName[], const char cszFileDate[]);


unsigned GetFileCRC(const char cszFileName[]);
int CheckFileCRC(unsigned uCRC32, const char szFullFileName[]);

int UnCompressFile(const char cszSourceFileName[], const char cszDestFileName[]);

int KRSMessageBox(HWND hParent, const CString strMessage, const CString strTitle, int nMsgFlag);

int ExistRegKey(const char cszKeyName[]);
BOOL IsInternetConnection();



static int inline _IntToNStr(int nNum, char *pszStr, int nDigitalNum)
{
	while (nDigitalNum)
	{
        pszStr[nDigitalNum - 1] = '0' + (nNum % 10);

        nNum /= 10;

        nDigitalNum--;
	}

	return true;
}

int inline IsNumeric(const char cszStr[], int nLen)
{
    for (int i = 0; i < nLen; i++)
    {
        if (!
            ((cszStr[i] >= '0') && (cszStr[i] <= '9'))
        )
            return false;
    }

    return true;
}

#endif   //_UPDATE3_GLOBAL_H_

