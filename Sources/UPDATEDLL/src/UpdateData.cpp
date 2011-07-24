//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateData.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-3-15 17:21:26
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AtlBase.h"
#include "UpdateData.h"
#include "Resource.h"
//#include "KAPMessageBox.h"
#include "GetFilesVersion.h"
#include "GetVersion.h"
#include "PublicFun.h"

KUPDATE_DATA g_UpdateData;
extern KPATH_TABLE g_PathTable;

int InitUpdateData(BOOL bLog)
{
    int Result = 0;
    int nRetCode = false;

    CString sDownloadHostName;
    char szUpdateInfDat[MAX_PATH] = {0};
    char szUpdateInfIni[MAX_PATH] = {0};

    g_UpdateData.nMainVersion               = 0;
    g_UpdateData.szParameter[0]         = '\0';
	g_UpdateData.szExecuteProgram[0]         = '\0';

    g_UpdateData.bVersionError      = false;    
    
    nRetCode = g_UpdateData.SaveLog.Init(bLog);
    g_UpdateData.nMethod                = defUPDATE_METHOD_INTERNET;   //Default from internet    
    g_UpdateData.szLocalPath[0]         = '\0';
    
    g_UpdateData.ProxySetting.nProxyMethod  = PROXY_METHOD_DIRECT;
    g_UpdateData.ProxySetting.nHostPort     = 0;
    g_UpdateData.ProxySetting.szHostAddr[0] = '\0';
    g_UpdateData.ProxySetting.szPassword[0] = '\0';
    g_UpdateData.ProxySetting.szUserName[0] = '\0';
    
    g_UpdateData.bAutoResume            = true;
    
    g_UpdateData.bNeedUpdateSelfFirst   = false;
    g_UpdateData.nNewsItemCount         = 0;
    g_UpdateData.sReadme                = "";
    g_UpdateData.sAnnounce              = "";
    g_UpdateData.nOSPlatVersion         = -1;

    g_UpdateData.bDownloadFailed        = false;
    g_UpdateData.bUpdateFailed          = false;
    g_UpdateData.bNeedRebootFalg        = false;
    g_UpdateData.bRebootFlag            = false;
    g_UpdateData.bAutoRebootFlag        = false;
    g_UpdateData.bRebootFinishUpdateFlag = false;

    return true;
}

int UninitUpdateData()
{
    return true;
}
