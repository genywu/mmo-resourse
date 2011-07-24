////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DownNotify.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  11:43:34
//  Comment     :   CDownNotify class source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "DownNotify.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDownNotify::CDownNotify()
{
}

CDownNotify::CDownNotify(ULONG ulMessage) : CWndNotify(ulMessage)
{
}

CDownNotify::~CDownNotify()
{
}

ULONG CDownNotify::OnStatusFileName(PDOWNLOADSTATUS pDownStatus)
{
    return 0;
}

ULONG CDownNotify::OnStatusFileSize(PDOWNLOADSTATUS pDownStatus)
{
    return 0;
}

ULONG CDownNotify::OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus)
{
    return 0;
}

ULONG CDownNotify::OnDownStatus(PDOWNLOADSTATUS pDownStatus)
{
    ULONG ulResult = 0;
    
    if (NULL == pDownStatus)
        goto Exit0;
    
    switch (pDownStatus->nStatusType)
    {
    case DOWN_STATUS_FILENAME:
        ulResult = OnStatusFileName(pDownStatus);
        break;
    case DOWN_STATUS_FILESIZE:
        ulResult = OnStatusFileSize(pDownStatus);
        break;
    case DOWN_STATUS_FILEDOWNED:
        ulResult = OnStatusFileDowned(pDownStatus);
        break;
        
    default:
        ASSERT(false);
        goto Exit0;
    }
    
Exit0:
    return ulResult;
}

ULONG CDownNotify::OnDownResult(ULONG ulDownResult)
{
    ULONG ulResult = 0;
    
    return ulResult;
}

int CDownNotify::IsNotifyMessage(const MSG *pMsg, ULONG *pulResult)
{
    int nResult = CWndNotify::IsNotifyMessage(pMsg, pulResult);
    ULONG ulResult = 0;
    
    ULONG ulDownResult = 0;
    PDOWNLOADSTATUS pDownStatus = NULL;
    
    ASSERT(pulResult != NULL);
    
    if (!nResult)
        goto Exit0;
    
    switch (pMsg->wParam)
    {
    case MSG_FTPDOWNLOAD_STATUS:
    case MSG_HTTPDOWNLOAD_STATUS:
        pDownStatus = (PDOWNLOADSTATUS)pMsg->lParam;
        ulResult = OnDownStatus(pDownStatus);
        
        nResult = true;
        break;
        
    case MSG_DOWNLOAD_RESULT:
        ulDownResult = (ULONG)pMsg->lParam;
        ulResult = OnDownResult(ulDownResult);
        
        nResult = true;
        break;
        
    default:
        ASSERT(false);
        goto Exit0;
    }
    
    if (pulResult != NULL)
        *pulResult = ulResult;
    
Exit0:
    return nResult;
}
