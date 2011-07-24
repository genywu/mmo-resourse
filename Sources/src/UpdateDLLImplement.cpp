//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateDLLImplement.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-9-25 15:43:50
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "UpdateDll.h"
#include "UpdateData.h"
#include "UpdateExport.h"

#include "Global.h"
#include "ProxyUtility.h"
#include "publicfun.h"
#include "UserCheck.h"
#include "Sourcedef.h"

extern KUPDATE_DATA g_UpdateData;
extern CProcessIndex g_ProcessIndex;


static const char *g_pszCheckHostArray[]= {
    "www.duba.net",
    "gz3.duba.net",
	"gz.duba.net",
    "sh.duba.net",
    "bj.duba.net",
    "gz2.duba.net",
	"gz4.duba.net"
	
};
static const int   g_nCheckPortArray[]  = {
    defCHECK_SERVER_PORT,
    defCHECK_SERVER_PORT,
    defCHECK_SERVER_PORT,
    defCHECK_SERVER_PORT,
    defCHECK_SERVER_PORT,
	defCHECK_SERVER_PORT,
	defCHECK_SERVER_PORT

};

static const int g_nCheckServerCount = sizeof(g_pszCheckHostArray)/sizeof(const char *);


int CUpdateDLLApp::Init(CONST KUPDATE_SETTING& UpdateSetting)
{
	int nRetCode = 0;  
    RECT rect = { 0, 0, 100, 20};
	WORD wVersionRequested = 0;
	WSADATA wsaData         = {0};    
    
    m_pfnCallBackProc = UpdateSetting.pfnCallBackProc;
    if (m_pfnCallBackProc)
    {
        m_pfnCallBackProc(defUPDATE_STATUS_INITIALIZING, 0);
    }

	nRetCode = InitPaths();
	if (!nRetCode)
	{
		m_nResultCode = defUPDATE_RESULT_INIT_FAILED;
		goto Exit0;
	}
     // get sock version
    wVersionRequested = MAKEWORD(2, 2);
    nRetCode = WSAStartup(wVersionRequested, &wsaData);
    if (0 == nRetCode)
    {
        m_nInitWSA = true;
    }
    else
    {
        m_nResultCode = defUPDATE_RESULT_LOAD_SOCKET_ERROR;
        goto Exit0;
    }
    InitUpdateData(UpdateSetting.bLog);

    g_UpdateData.nMainVersion = UpdateSetting.nVersion;
    g_UpdateData.nMethod = UpdateSetting.nUpdateMode;
    strcpy(g_UpdateData.szHostURL, UpdateSetting.szUpdateSite);
    strcpy(g_UpdateData.szDefHostURL, UpdateSetting.szUpdateSite);
    g_UpdateData.ulTryTimes = UpdateSetting.ulTryTimes;
    g_UpdateData.bAutoTryNextHost = UpdateSetting.bAutoTryNextHost;
    g_UpdateData.bUseFastestHost = UpdateSetting.bUseFastestHost;
    g_UpdateData.bUseVerify = UpdateSetting.bUseVerify;
    strcpy(g_UpdateData.szVerifyInfo, UpdateSetting.szVerifyInfo);
    // Setting the proxy setting
    g_UpdateData.ProxySetting.nProxyMethod  = UpdateSetting.ProxySetting.nProxyMethod;
    g_UpdateData.ProxySetting.nHostPort     = UpdateSetting.ProxySetting.nHostPort;
    strcpy(g_UpdateData.ProxySetting.szHostAddr, UpdateSetting.ProxySetting.szHostAddr);
    strcpy(g_UpdateData.ProxySetting.szPassword, UpdateSetting.ProxySetting.szPassword);
    strcpy(g_UpdateData.ProxySetting.szUserName, UpdateSetting.ProxySetting.szUserName);
    // setting the download destination directory
    strcpy(g_PathTable.szDownloadDestPath, UpdateSetting.szDownloadPath);        
    strcpy(g_UpdateData.szLocalPath, UpdateSetting.szDownloadPath);
    // INitialize the Update destionation directory
    strcpy(g_PathTable.szUpdateDestPath, UpdateSetting.szUpdatePath);

	strcpy(g_UpdateData.szExecuteProgram, UpdateSetting.szMainExecute);
        
    CDownNotify::SetNotifyMessage(WM_DOWNNOTIFY_DEFAULT);
    
    m_hExitNotify = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    nRetCode = m_wndMessage.Create(
        ::AfxRegisterWndClass(
            CS_DBLCLKS | CS_VREDRAW,
            0,
            (HBRUSH)::GetStockObject(BLACK_BRUSH)),
        "KingSoft Update Notify Window",
        WS_OVERLAPPEDWINDOW & ~WS_VISIBLE,
        rect,
        CWnd::GetDesktopWindow(),
        NULL
    );
    if (!nRetCode) 
        goto Exit0; 
    m_wndMessage.ShowWindow(SW_HIDE);
    m_wndMessage.UpdateWindow();
    
    m_nInitFlag = true;
    m_nResultCode = defUPDATE_RESULT_INIT_SUCCESS;
	if (m_pfnCallBackProc)
    {
        m_pfnCallBackProc(defUPDATE_STATUS_INITIALIZING, 100);
    }
    
Exit0:
	return m_nResultCode;
}

int CUpdateDLLApp::UnInit()
{
    if (m_wndMessage.GetSafeHwnd() != NULL)
        DestroyWindow(m_wndMessage.GetSafeHwnd());
    
    CDownNotify::SetNotifyMessage(0);
    
    if (m_hExitNotify != NULL)
    {
        CloseHandle(m_hExitNotify);
        m_hExitNotify = NULL;
    }

	if (m_nInitWSA)
    {
        WSACleanup();
        m_nInitWSA = false;
    }

    UnInitCheckThread();

	return true;
}

int CUpdateDLLApp::UserVerify()
{
   int nRetCode = false;
    int i = 0;

    if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;

    if (!g_UpdateData.bUseVerify)
	{
		m_nResultCode = defUPDATE_RESULT_USER_VERIFY_SUCCESS;
		goto Exit1;
	}

    if (m_bCheckUserOK)
    {
        m_nResultCode = defUPDATE_RESULT_USER_VERIFY_SUCCESS;
        goto Exit1;
    } 
    
    /*for (i = 0; i < g_nCheckServerCount; i++)
    {
        nRetCode = m_CheckThread.AddCheckServer(
            g_pszCheckHostArray[i],
            g_nCheckPortArray[i]
        );
        ASSERT(nRetCode);
    }*/
    
    nRetCode = m_CheckThread.SetCheckCallback((IKCheckCallback *)this, 0);
    ASSERT(nRetCode);

    m_DownloadFile.SetNotifyWnd(m_wndMessage, WM_DOWNNOTIFY_DEFAULT);
    nRetCode = SetDownloadProxy(m_DownloadFile, g_UpdateData.szHostURL);
    if (nRetCode)
        m_nProcessStep = DOWNINDEX_STEPCHKSNHTTP;
    else
    {
        m_nProcessStep = DOWNINDEX_STEPCHKSNUDP;
    
        nRetCode = InitCheckThread();
        if (!nRetCode)
        {
            m_nResultCode = defUPDATE_RESULT_USER_VERIFY_FAILED;
            goto Exit1;
        }
    } 

    ProcessNextStep(m_nProcessStep);

Exit1:
    return m_nResultCode;
}

int CUpdateDLLApp::CheckNeedUpdate()
{
	char szFileName[MAX_PATH]	= {0};
    
    int nRetCode = 0;
    ULONG ulResult = 0;
    
    MSG stMsg = { 0 };
    HWND hWnd = m_wndMessage.m_hWnd;
    
	if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
    {
		m_strIndexTempFile = (CString)g_PathTable.szDownloadDestPath + defINDEX_FILE_NAME;
        nRetCode = ::ProcessIndexFile(m_strIndexTempFile);
		if (!nRetCode)
		{
			m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_FAILED;
			goto Exit0;
		}
		if(nRetCode == defVersionNotenough)
		{
			m_nResultCode = defUPDATE_RESULT_VERSION_NOT_ENOUGH;
			goto Exit0;
			
		}

        m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_SUCCESS;
        
		if (g_ProcessIndex.IsNotUpdateItem())
			m_nResultCode = defUPDATE_RESULT_NOT_UPDATE_FILE;
        
        goto Exit0;
    }

    m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;
    
	ResetEvent(m_hExitNotify);
	// Download Index.dat
    m_DownloadFile.SetNotifyWnd(m_wndMessage, WM_DOWNNOTIFY_DEFAULT);
    SetDownloadProxy(m_DownloadFile, g_UpdateData.szHostURL);
	nRetCode = ProcessNextStep(DOWNINDEX_STEPINDEX);
	if (!nRetCode)
    {
        m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;
        goto Exit0;
    }
    
    do
    {
        nRetCode = ::WaitForSingleObject(m_hExitNotify, 0);
        if (WAIT_OBJECT_0 == nRetCode)
            break;
        
        nRetCode = ::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE);
        if (0 == nRetCode)
            continue;
        
        if (WM_QUIT == stMsg.message)
            break;
        
        if (-1 == nRetCode)
        {
            // handle the error and possibly exit
        }
        else
        {
            ::TranslateMessage(&stMsg); 
            ::DispatchMessage(&stMsg); 
        }
    } while (true);
    
Exit0:
	m_DownloadFile.StopDownload();
	return m_nResultCode;
}


int CUpdateDLLApp::Download()
{
	int nRetCode = 0;
    MSG stMsg = { 0 };

    if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
        m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;
	
	KUPDATE_ITEM* pDownItem = g_ProcessIndex.m_pUpdateItemList;
	while (pDownItem)
	{
		if (pDownItem->DownloadStatus != enumDOWNLOADSTATUS_DOWNLOADED)
			break;
		pDownItem = pDownItem->pNext;
	}

	if (pDownItem == NULL)
		m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;

	ResetEvent(m_hExitNotify);
	
    m_DownloadFile.SetNotifyWnd(m_wndMessage.GetSafeHwnd(), WM_DOWNNOTIFY_DEFAULT);
    m_DownloadFile.SetSavePath((CString)g_PathTable.szDownloadDestPath);

    SetDownloadProxy(m_DownloadFile, g_UpdateData.szHostURL);
    m_nCurEnableResume = g_UpdateData.bAutoResume;
    nRetCode = DownNextItem(false);
    KAV_PROCESS_ERROR(nRetCode);
    
    do
    {
        nRetCode = ::WaitForSingleObject(m_hExitNotify, 0);
        if (WAIT_OBJECT_0 == nRetCode)
            break;
        
        nRetCode = ::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE);
        if (0 == nRetCode)
            continue;
        
        if (WM_QUIT == stMsg.message)
            break;
        
        if (-1 == nRetCode)
        {
            // handle the error and possibly exit
        }
        else
        {
            ::TranslateMessage(&stMsg); 
            ::DispatchMessage(&stMsg); 
        }
    } while (true);
    
Exit0:
	m_DownloadFile.StopDownload();
    return m_nResultCode;
}					 

int CUpdateDLLApp::CancelDownload()
{
    if (!m_nInitFlag)
		return 0;

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
		return 0;
	
	m_DownloadFile.StopDownload();
	SetEvent(m_hExitNotify);
    return 0;
}					 


int CUpdateDLLApp::Update()
{
	int m_nResultCode	= defUPDATE_RESULT_UPDATE_SUCCESS;
	int nRetCode		= 0;	
	CString sMsg;
                                                     

	if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;
    
    nRetCode = UpdateFiles();
    if (!nRetCode)
    {
		sMsg = defIDS_UPDATE_FINISH_FAILED;
		m_nResultCode = defUPDATE_RESULT_UPDATE_FAILED;
		g_UpdateData.SaveLog.WriteLogString(sMsg, true);
		return m_nResultCode;
	}

    sMsg = defIDS_UPDATE_FINISH_SUCCESS;
    g_UpdateData.SaveLog.WriteLogString(sMsg, true);

    if (g_UpdateData.bUpdateFailed)
    {
        sMsg = defIDS_NOTIFY_REUPDATE;
        g_UpdateData.SaveLog.WriteLogString(sMsg, true);
		m_nResultCode = defUPDATE_RESULT_UPDATE_FAILED;
    }   
	else
	{
		if (g_UpdateData.bNeedRebootFalg)
		{
			sMsg = defIDS_NEED_REBOOT;
			g_UpdateData.SaveLog.WriteLogString(sMsg, true);

			m_nResultCode = defUPDATE_RESULT_UPDATE_SUCCESS_NEED_REBOOT;
		}
		else if (g_UpdateData.bNeedUpdateSelfFirst)
			m_nResultCode = defUPDATE_RESULT_UPDATESELF_SUCCESS;
		else
			m_nResultCode = defUPDATE_RESULT_UPDATE_SUCCESS;
	}

	return m_nResultCode;
}


ULONG CUpdateDLLApp::OnDownResult(ULONG ulOverResult)
{
    ULONG ulResult		= CDownNotify::OnDownResult(ulOverResult);
    int nRetCode		= false;   	
	int nCurDownOK      = false;
	int nRedownCurrent  = false;
	CString sLog, sFormat;

    if (DOWNINDEX_STEPCHKSNHTTP == m_nProcessStep)
    {
        ASSERT(!m_strCgiTempFile.IsEmpty());
        
        if (DOWN_RESULT_FAIL == ulOverResult)
        {
            ASSERT(!m_bCheckUserOK);
            
            //nRetCode = ProcessCheckFailed();
            if (nRetCode)
            {
                //nRetCode = ProcessNextStep(DOWNINDEX_STEPCHKSNHTTP);
                nRetCode = ProcessNextStep(DOWNINDEX_STEPCHKSNUDP);
                ASSERT(nRetCode);
            }
        }
        goto Exit1;
    }                 

    if (DOWNINDEX_STEPINDEX == m_nProcessStep)
    {
		if (DOWN_RESULT_SUCCESS == ulOverResult)
		{
			m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_SUCCESS;
			nRetCode = ProcessNextStep(DOWNINDEX_STEPOVER);
			if(nRetCode == defVersionNotenough) {
				m_nResultCode = nRetCode;
			}
			PostQuitMessage(0);
			SetEvent(m_hExitNotify);
			goto Exit1;
		}
		else
		{
			m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;

            if (!g_UpdateData.bAutoTryNextHost)
            {
				PostQuitMessage(0);
				SetEvent(m_hExitNotify);
				goto Exit1;
			}

			// here to get Next host to download index
			//g_UpdateData.szHostURL
			SetDownloadProxy(m_DownloadFile, g_UpdateData.szHostURL);
			nRetCode = ProcessNextStep(DOWNINDEX_STEPINDEX);
		}
	}
	else
	{
		ASSERT(m_pCurDownItem != NULL);
    
		m_nCurEnableResume = g_UpdateData.bAutoResume;
    
		switch (ulOverResult)
		{
		case DOWN_RESULT_SUCCESS:
		case DOWN_RESULT_SAMEAS:
			m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADED;
        
			// Check file CRC
			nCurDownOK = CheckFileCRC(atoi(m_pCurDownItem->szCRC), m_pCurDownItem->szDownloadTmpFileName);
			if (!nCurDownOK)
			{
				if (!g_UpdateData.bAutoTryNextHost) //if (!ExistsOtherHost())
				{
					m_nResultCode = defUPDATE_RESULT_DOWNLOAD_FAILED;
					m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_ERROR;                
					sFormat = defIDS_DOWNLOAD_FILE_FAILED;
					sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
					g_UpdateData.SaveLog.WriteLogString(sLog, true);
					if (!g_UpdateData.bDownloadFailed)
						g_UpdateData.bDownloadFailed = m_pCurDownItem->bNeedDownload;
				}
				else
				{
					//do not skip download this update file
					nRedownCurrent = true;
					m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_QUEUE;
					m_nCurEnableResume = false;
				}
			}
			else
			{
                m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;
				sFormat = defIDS_DOWNLOAD_FILE_SUCCESSFUL;
				sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
				g_UpdateData.SaveLog.WriteLogString(sLog, true);
			}
			break;
        
		case DOWN_RESULT_FAIL:
			if (m_pCurDownItem->nDownTryTimes < (int)g_UpdateData.ulTryTimes)
			{
				nRedownCurrent = true;
			}
			if (!g_UpdateData.bAutoTryNextHost) //if (!ExistsOtherHost())
			{
				m_nResultCode = defUPDATE_RESULT_DOWNLOAD_FAILED;
				m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_ERROR;
				sFormat = defIDS_DOWNLOAD_FILE_FAILED;
				sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
				g_UpdateData.SaveLog.WriteLogString(sLog, true);
				if (!g_UpdateData.bDownloadFailed)
					g_UpdateData.bDownloadFailed = m_pCurDownItem->bNeedDownload;
			}
			else
			{
				//do not skip download this update file
				nRedownCurrent = true;
				m_pCurDownItem->nDownTryTimes = 0;
				m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_QUEUE;
			}

			break;
        
		default:
			ASSERT(FALSE);
			break;
		}
    
		nRetCode = DownNextItem(nRedownCurrent);
		if (!nRetCode)
		{
			if (nRedownCurrent)
				ASSERT(FALSE);
			else
            {                   
                PostQuitMessage(0);
				SetEvent(m_hExitNotify);
                goto Exit1;
            }
        }

        if (m_nResultCode == defUPDATE_RESULT_CANCEL)
        {
	        PostQuitMessage(0);
			SetEvent(m_hExitNotify);
			goto Exit1;
        }
	}
        
Exit1:
    return ulResult;
}


int CUpdateDLLApp::ProcessNextStep(int nDownStep)
{
    int nResult = false;
    int nRetCode = false;
    CString sMsg;
    CString sTitle;
    MSG stMsg = { 0 };
    HANDLE Handle = NULL;

    
    CString strDownUrl;
    
    switch (nDownStep)
    {
    case DOWNINDEX_STEPCHKSNUDP:
        m_strIndexTempFile.Empty();
        
        nRetCode = m_CheckThread.IsThreadOK();
        if (nRetCode)
        {
            nRetCode = m_CheckThread.StopThread();
            ASSERT(nRetCode);
        }
        nResult = m_CheckThread.StartThread();
        
        Handle = m_CheckThread.GetSafeHandle();
        nRetCode = ::WaitForMultipleObjects(
            1,
            &Handle,
            true,
            INFINITE
        );
        if (WAIT_OBJECT_0 == nRetCode)
            UnInitCheckThread();

        break;
        
    case DOWNINDEX_STEPCHKSNHTTP:
        m_strIndexTempFile.Empty();
        
        strDownUrl = defKAV_CHKSN_CGI_REQUEST;
        strDownUrl += g_UpdateData.szVerifyInfo;
        m_strCgiTempFile = g_PathTable.szTempPath;
        m_strCgiTempFile += defKAV_CHKSN_CGI_TEMPFILE;
        
        m_DownloadFile.SetTimeout(10000, 24000, 24000);
        nResult = m_DownloadFile.StartDownload(strDownUrl, false, m_strCgiTempFile);
        
        do
        {
            nRetCode = ::MsgWaitForMultipleObjects(
                1,
                &m_hExitNotify,
                false,
                INFINITE,
                QS_ALLINPUT
            );
            if (WAIT_OBJECT_0 == nRetCode)
                break;
        
            ASSERT((WAIT_OBJECT_0 + 1) == nRetCode);
        
            nRetCode = ::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE);
            if (0 == nRetCode)
                continue;
        
            if (WM_QUIT == stMsg.message)
                break;
        
            if (-1 == nRetCode)
            {
                // handle the error and possibly exit
            }
            else
            {
                ::TranslateMessage(&stMsg); 
                ::DispatchMessage(&stMsg); 
            }
        } while (true);      

        break;

    case DOWNINDEX_STEPINDEX:
        strDownUrl = g_UpdateData.szHostURL;
        strDownUrl += defINDEX_FILE_NAME;
        
        MkDirEx(g_PathTable.szDownloadDestPath);
        m_strIndexTempFile = (CString)g_PathTable.szDownloadDestPath + defINDEX_FILE_NAME;
        
        m_DownloadFile.SetTimeout(10000, 24000, 24000);
        nResult = m_DownloadFile.StartDownload(strDownUrl, false, m_strIndexTempFile);
        break;
        
    case DOWNINDEX_STEPOVER:
        if (m_pfnCallBackProc)
        {
            if (m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0))
            {
                m_nResultCode = defUPDATE_RESULT_CANCEL;
		        goto Exit0;
            }
        }

        nRetCode = ::ProcessIndexFile(m_strIndexTempFile);
		if (!nRetCode)
		{
			m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_FAILED;
			goto Exit0;
		}
		if(nRetCode == defVersionNotenough) {
//			m_CheckThread.StopThread();
			return defVersionNotenough;
		}
		m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_SUCCESS;

		if (g_ProcessIndex.IsNotUpdateItem())
			m_nResultCode = defUPDATE_RESULT_NOT_UPDATE_FILE;
        
        break;
        
    default:
        ASSERT(FALSE);
        goto Exit0;
    }
    
    m_nProcessStep = nDownStep;
    nResult = true;
Exit0:
    return nResult;
}


int CUpdateDLLApp::DownNextItem(int nRedownCurrent /*=false*/)
{
    int nResult = false;
    int nRetCode = false;
    
    if (nRedownCurrent)
    {
        if (NULL == m_pCurDownItem)
        {
            ASSERT(FALSE);
            goto Exit0;
        }
        
        ASSERT(m_pCurDownItem->DownloadStatus != enumDOWNLOADSTATUS_DOWNLOADED);
    }
    else
    {
        if (m_pCurDownItem != NULL)  
            m_pCurDownItem = m_pCurDownItem->pNext;
        
        if (NULL == m_pCurDownItem)
            m_pCurDownItem = g_ProcessIndex.m_pUpdateItemList;
    }
    
    while (m_pCurDownItem != NULL)
    {
        switch (m_pCurDownItem->DownloadStatus)
        {
        case enumDOWNLOADSTATUS_ERROR:
        case enumDOWNLOADSTATUS_DOWNLOADED:
            if (!nRedownCurrent)
            {
                m_pCurDownItem = m_pCurDownItem->pNext;
                continue;
            }
            break;
            
        case enumDOWNLOADSTATUS_QUEUE:
            m_pCurDownItem->nDownTryTimes = 0;
            break;
            
        default:
            break;
        }
        
        m_pCurDownItem->nDownTryTimes++;
        break;
    }
    
    if (m_pCurDownItem != NULL)
    {
        CString strDownUrlPath;
        
        strDownUrlPath = g_UpdateData.szHostURL;
        strDownUrlPath += m_pCurDownItem->szRemotePath;
        strDownUrlPath += m_pCurDownItem->szFileName;
       
        m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADING;

		if (m_pfnCallBackProc)
		{
			KUPDATE_ITEM * pItem = g_ProcessIndex.m_pUpdateItemList;
			int nCount = 1;
			int nIndex = 1;
			while (pItem)
			{
				nCount++;
				if (m_pCurDownItem == pItem)
				{
					nIndex = nCount;
				}
				pItem = pItem->pNext;
			}

			if (m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, nIndex / nCount * 100))
			{
				m_nResultCode = defUPDATE_RESULT_CANCEL;
				goto Exit0;
			}
		}
        
        nRetCode = m_DownloadFile.StartDownload(
            strDownUrlPath,
            m_nCurEnableResume,
            m_pCurDownItem->szDownloadTmpFileName
        );
        ASSERT(nRetCode);
        
        nResult = true;
    }
    
Exit0:
    return nResult;
}

int CUpdateDLLApp::GetCheckInfo(
    ULONG   ulContextWith,
    int     *pnSendSize,
    unsigned char **ppbySendInfo
)
{
    int nResult = false;
    
    ASSERT_POINTER(pnSendSize, int);
    ASSERT_POINTER(ppbySendInfo, LPVOID);
    
    if (
        (NULL == pnSendSize) || 
        (NULL == ppbySendInfo)
    )
        goto Exit0;
    
    GetUserInfo(m_KavUserInfo, TRUE);
    
    *pnSendSize = sizeof(m_KavUserInfo);
    *ppbySendInfo = (unsigned char *)&m_KavUserInfo;
    
    nResult = true;
    
Exit0:
    return nResult;
}

int CUpdateDLLApp::CheckRecvInfo(
    ULONG   ulContextWith,
    int     nRecvSize,
    unsigned char  *pbyRecvInfo
)
{
    //int nResult = -1;
	int nResult = defConnectServerFailed - 1;
    int nRetCode = 0;
    SERVER_INFO *pServerInfo = NULL;
    
    if (nRecvSize < sizeof(SERVER_INFO))
        goto Exit0;
    
    pServerInfo = (SERVER_INFO *)pbyRecvInfo;
    
    nRetCode = strncmp(
        pServerInfo->szFlag,
        defCHECK_CURRENT_SERVERFLAG,
        sizeof(pServerInfo->szFlag)
    );
    if (nRetCode != 0)
        goto Exit0;
    
    nResult = pServerInfo->nNumber;
    
Exit0:
    return nResult;
}

void CUpdateDLLApp::CheckResult(
    ULONG   ulContextWith,
    int     nSvrResult
)
{
    int nRetCode = false;
    CString sMsg;
    
    if (m_nProcessStep != DOWNINDEX_STEPCHKSNUDP)
    {
        sMsg = defIDS_HTTP_CHECK_FAILED;
        g_UpdateData.SaveLog.WriteLogString(sMsg, true);

        ASSERT(false);
        return;
    }
    
    if (nSvrResult < defInhibitiveSerialNumber)
    {
        sMsg = defIDS_USE_HTTP_CHECK;
        g_UpdateData.SaveLog.WriteLogString(sMsg, true);
        
        nRetCode = ProcessNextStep(DOWNINDEX_STEPCHKSNHTTP);
        ASSERT(nRetCode);
    }
    else
    {   
        ProcessUserCheck(nSvrResult);
    }
}


int CUpdateDLLApp::InitCheckThread()
{
    int nResult = false;
    int nRetCode = false;
    
    if (m_nInitCheckThread)
        goto Exit1;
    
    nRetCode = m_CheckThread.Create();
    if (!nRetCode)
    {
        ASSERT(FALSE);
        goto Exit0;
    }
    
Exit1:
    m_nInitCheckThread = true;
    nResult = true;
    
Exit0:
    return nResult;
}

int CUpdateDLLApp::UnInitCheckThread()
{
    int nResult = false;
    int nRetCode = false;
    
    if (!m_nInitCheckThread)
        goto Exit1;
    
    nRetCode = m_CheckThread.IsThreadOK();
    if (nRetCode)
    {
        nRetCode = m_CheckThread.StopThread();
        ASSERT(nRetCode);
    }
    
    m_CheckThread.Destroy();
    
Exit1:
    m_nInitCheckThread = false;
    nResult = true;
    
//Exit0:
    return nResult;
}


ULONG CUpdateDLLApp::OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus)
{
    ULONG ulResult = 0;
    
    if (DOWNINDEX_STEPCHKSNHTTP == m_nProcessStep)
    {
        if (pDownStatus->dwFileDownloadedSize >= 20)
        {
            m_DownloadFile.StopDownload();
            
            int nRetCode = false;
    
            CFile file;
            int nCheckOK = false;
            HANDLE hCheckFile = INVALID_HANDLE_VALUE;
    
            char szCheckResult[256] = { 0 };
    
            nRetCode = m_strCgiTempFile.IsEmpty();
            if (nRetCode)
            {
                ASSERT(false);
                goto Exit0;
            }
    
            TRY
            {
                // Open the cgi url download temp file
                nRetCode = file.Open(m_strCgiTempFile + ".tmp", CFile::modeRead | CFile::shareDenyNone);
                if (!nRetCode)
                {
                    nRetCode = file.Open(m_strCgiTempFile, CFile::modeRead | CFile::shareDenyNone);
                    if (!nRetCode)
                    {
                        ASSERT(false);
                        goto Exit0;
                    }
                }
        
                nRetCode = file.Read(szCheckResult, sizeof(szCheckResult) - 2);
                if (nRetCode <= 4)
                {
                    ASSERT(false);
                    goto Exit0;
                }
            }
            CATCH_ALL(pException)
            {
                pException->Delete();
                goto Exit0;
            }
            END_CATCH_ALL;
    
            file.Close();
    
            nRetCode = (0 == ::strncmp(szCheckResult, "+OK", 3));
    
            ulResult = ProcessUserCheck(nRetCode);

            PostQuitMessage(0);
        }        
    }
	else if (DOWNINDEX_STEPOVER == m_nProcessStep)
	{
		if (m_pfnCallBackProc && pDownStatus)
		{
			DOWNLOADFILESTATUS info;
			strncpy(info.strFileName, pDownStatus->strFileName, MAX_PATH);
			info.dwFileSize = pDownStatus->dwFileSize;
			info.dwFileDownloadedSize = pDownStatus->dwFileDownloadedSize;
			m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING_FILE, (long)&info);
		}
	}

Exit0:        
    return ulResult;
}           

int CUpdateDLLApp::ProcessUserCheck(int nCheckOK)
{
    int nResult = false;
	int nRetCode = 0;
    CString sLog;
	//int nMsgID = 0;
    
    if (nCheckOK >=0)
        m_bCheckUserOK = true;
    else 
        m_bCheckUserOK = false;
    
    if (nCheckOK >=0)
    {
        if (m_nProcessStep == DOWNINDEX_STEPCHKSNUDP)
        {
            sLog = defIDS_UDP_CHECK_SUCCESSFUL;
            g_UpdateData.SaveLog.WriteLogString(sLog, true);
        }
        else if (m_nProcessStep == DOWNINDEX_STEPCHKSNHTTP)
        {
            sLog = defIDS_HTTP_CHECK_SUCCESSFUL;
            g_UpdateData.SaveLog.WriteLogString(sLog, true);
        }

        m_nResultCode = defUPDATE_RESULT_USER_VERIFY_SUCCESS;
        nResult = true;
    }
    else
    {
		switch (nCheckOK)
		{
		case defInvalidSerialNumber:
			sLog = defIDS_INVALID_USERCHECK;
            m_nResultCode = defUPDATE_RESULT_INVALIDSN;
			break;
		case defPiraticSerialNumber:
            sLog = defIDS_PIRATIC_SERIAL_NUMBER;	
            m_nResultCode = defUPDATE_RESULT_PIRATICSN;
            break;
		case defInhibitiveSerialNumber:		
    		sLog = defIDS_INHIBITIVE_SERIAL_NUMBER;	
            m_nResultCode = defUPDATE_RESULT_INHIBITIVESN;
			break;
		case defConnectServerFailed:
			sLog = defIDS_CONNECT_SERVER_FAILED;
            m_nResultCode = defUPDATE_RESULT_CONNECT_SERVER_FAILED;
			break;
		default:
			sLog = defIDS_CHECK_SERIAL_NUMBER_ERROR;
            m_nResultCode = defUPDATE_RESULT_ERRORSN;
		}
		g_UpdateData.SaveLog.WriteLogString(sLog, true);  
    }
    
    return nResult;
}


