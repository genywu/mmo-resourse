//////////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   SaveLog.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-18 11:54:06
//  Comment     :   
//  
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __UPDATE_SAVE_LOG_H__
#define __UPDATE_SAVE_LOG_H__

#include "stdafx.h"


#define defLOG_FILE_MAX_SIZE                1024 * 1024


class KSaveLog
{
public:
    KSaveLog();
    ~KSaveLog();
    
private:
    BOOL    WriteLogEnd();
    
public:
    int m_nLogSectionPos;
    CString m_sLog;
	BOOL m_bWriteLog;

    BOOL    Init(BOOL bWrite);
    BOOL    WriteLogTitle();
    CString GetCurLogString();
    BOOL    WriteLogString(CString sMes, int nTimeFlag);
};              

#endif  //__UPDATE_SAVE_LOG_H__

