//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateExport.cpp
//  Version     :   1.0
//  Creater     :   Cheng bitao
//  Date        :   2002-11-28 17:24:46
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "UpdateExport.h"
#include "UpdateDLL.h"
#include "UpdateData.h"
#include "UpdatePublic.h"

extern CUpdateDLLApp g_theApp;

int __stdcall Update_Init(KUPDATE_SETTING UpdateSetting)
{
    return g_theApp.Init(UpdateSetting);
}

int __stdcall Update_UnInit()
{
    return g_theApp.UnInit();
}

extern KUPDATE_DATA g_UpdateData;

int __stdcall Update_Start()
{
	//MessageBox(NULL,"¾ÉµÄUPDATEDLL.DLL","",0);
    int nRetCode = 0;
	CString sLog;
	sLog = g_UpdateData.szHostURL;
	g_UpdateData.SaveLog.WriteLogString(sLog, true);

    if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_VERIFING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }

	TRACE0("UserVerify\n");
    nRetCode = g_theApp.UserVerify();
	TRACE0("UserVerify End\n");
    if (nRetCode != defUPDATE_RESULT_USER_VERIFY_SUCCESS)
        return nRetCode;
    if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_VERIFING, 100);
    }

    if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }
	TRACE0("CheckNeedUpdate\n");
    nRetCode = g_theApp.CheckNeedUpdate();
	TRACE0("CheckNeedUpdate End\n");
    if (nRetCode != defUPDATE_RESULT_PROCESS_INDEX_SUCCESS)
    {
		if (nRetCode == defUPDATE_RESULT_NOT_UPDATE_FILE)
		{
			sLog = "Not Update File OK!";
			g_UpdateData.SaveLog.WriteLogString(sLog, true);
			return nRetCode;
		}
		if(nRetCode == 1)
		{
//			if (g_theApp.m_pfnCallBackProc)
			{
//				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_UPDATE_SUCCESS, 0);
				return defUPDATE_RESULT_UPDATE_SUCCESS;
		    }
		}
		if( nRetCode == defVersionNotenough)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_NOT_ENOUGH, (long)&g_ProcessIndex.url[0]);
		    }
			return nRetCode;
		}
		if( nRetCode == defVersionMore)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_MORE, 0);
		    }
			return nRetCode;
		}
		
	
		sLog = "Process Index.dat Failed!";
		g_UpdateData.SaveLog.WriteLogString(sLog, true);
		if (g_theApp.m_pfnCallBackProc)
		{
			g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0);
		}
		
		return nRetCode;
	}
	sLog = "Process Index.dat OK!";
	g_UpdateData.SaveLog.WriteLogString(sLog, true);
	if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 100);
    }

	if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }

	TRACE0("Download\n");
    nRetCode = g_theApp.Download();
	TRACE0("Download End\n");
    if (nRetCode != defUPDATE_RESULT_DOWNLOAD_SUCCESS)
    {
		sLog = "Download Failed!";
		g_UpdateData.SaveLog.WriteLogString(sLog, true);
		return nRetCode;
	}
	sLog = "Download OK!";
	g_UpdateData.SaveLog.WriteLogString(sLog, true);
	if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, 100);
    }

	if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_UPDATING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }

	TRACE0("Update\n");
    nRetCode = g_theApp.Update();
	TRACE0("Update End\n");
	if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_UPDATING, 100);
    }

Exit0: 
    return nRetCode;
}

int __stdcall Update_Cancel()
{
	return g_theApp.CancelDownload();
}
