//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   SaveLog.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-18 11:52:10
//  Comment     :   interface of save log
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DataDefine.h"
#include "SaveLog.h"
#include "PublicFun.h"

extern KPATH_TABLE g_PathTable;

CString GetLogPathName()
{
    char szUserName[100]    = {0};
    int nSize               = 100;
    int nRetCode            = 0;
	CString sPathName       = (CString)g_PathTable.szUpdateDestPath;

	ASSERT(sPathName.Right(1) == "\\");

    /*nRetCode = GetUserName(szUserName, (unsigned long *)&nSize);
    if (!nRetCode)
        *szUserName = '\0';

    sPathName += "LOGS";
    MkDirEx(sPathName);

	sPathName = (CString)g_PathTable.szModulePath + "LOGS\\" + (CString)szUserName + "Update.LOG";
    */
    sPathName += "Update.LOG";

	return sPathName;
}

CString GetCurTimeString(BOOL bYear)
{
	CTime now = CTime::GetCurrentTime();
	CString sMes;

	if (bYear)
	{
		sMes.Format(
			"%04d-%02d-%02d %02d:%02d:%02d",
			now.GetYear(),
			now.GetMonth(),
			now.GetDay(),
			now.GetHour(),
			now.GetMinute(),
			now.GetSecond()
			);
	}
	else
	{
		sMes.Format(
			"%02d:%02d:%02d",
			now.GetHour(),
			now.GetMinute(),
			now.GetSecond()
			);
	}

	return sMes;
}

KSaveLog::KSaveLog()
{
    m_nLogSectionPos = 0;
    m_sLog.Empty();
	m_bWriteLog = TRUE;
}

KSaveLog::~KSaveLog()
{
    WriteLogEnd();
    m_nLogSectionPos = 0;
    m_sLog.Empty();
}

BOOL KSaveLog::Init(BOOL bWrite)
{
	m_bWriteLog = bWrite;
    return WriteLogTitle();
}

BOOL KSaveLog::WriteLogTitle()
{
	CString sPathName = GetLogPathName();
    CFile file;

	if (file.Open(sPathName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
        int nLen = 0;

        nLen = file.GetLength();
        if (nLen > defLOG_FILE_MAX_SIZE)
        {
            CString sNewPathName;

            sNewPathName = sPathName + ".old";
            file.Close();

            CopyFile(sPathName, sNewPathName, false);
            DeleteFile(sPathName);

            if (!file.Open(sPathName, CFile::modeCreate | CFile::modeWrite))
                return false;
        }
		file.SeekToEnd();
		m_nLogSectionPos = file.GetPosition();

		CString sMes = "\r\n[---------";
		sMes += GetCurTimeString(TRUE);
		sMes += "---------]\r\n";

		file.Write(sMes, sMes.GetLength());

		file.Close();
	}
	else
		return FALSE;

	return TRUE;
}

BOOL KSaveLog::WriteLogString(CString sMes, int nTimeFlag)
{
	if (!m_bWriteLog)
		return TRUE;
	CString sPathName = GetLogPathName();
	CFile file;

    CString sContent = "";
    if (nTimeFlag)
    {
        sContent = GetCurTimeString(FALSE);
		sContent += " " + sMes + "\r\n";
    }
    else 
    {
        sContent = sMes + "\r\n";
    }
    
    m_sLog += sContent;

	if (file.Open(sPathName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		
		file.SeekToEnd();
		file.Write(sContent, sContent.GetLength());

		file.Close();
	}
	else
		return FALSE;

	return TRUE;
}

BOOL KSaveLog::WriteLogEnd()
{
	return TRUE;
}

CString KSaveLog::GetCurLogString()
{
	CString sPathName = GetLogPathName();
	CFile file;
	CString s = "";

	if (file.Open(sPathName, CFile::modeRead | CFile::shareDenyNone))
	{
		int nLen = file.GetLength() - m_nLogSectionPos;
		char *pBuf = new char [nLen + 1];
		ASSERT(pBuf != NULL);

		file.Seek(m_nLogSectionPos, CFile::begin);
		file.Read(pBuf, nLen);
		pBuf[nLen] = '\0';

		s = pBuf;

		KAV_DELETE_ARRAY(pBuf);

		file.Close();
	}
	
	return s;
}
