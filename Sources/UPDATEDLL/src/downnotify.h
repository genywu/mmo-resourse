////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   DownNotify.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  11:43:34
//  Comment     :   CDownNotify class header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNNOTIFY_H__EA13D2FF_1112_4AAD_89E5_82E8E885C779__INCLUDED_)
#define AFX_DOWNNOTIFY_H__EA13D2FF_1112_4AAD_89E5_82E8E885C779__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndNotify.h"
#include "DownloadFile.h"

#define WM_DOWNLOAD_NOTIFY                  (WM_USER + 20)
#define WM_DOWNNOTIFY_DEFAULT               (WM_DOWNLOAD_NOTIFY)


class CDownNotify : public CWndNotify
{
public:
    CDownNotify();
    CDownNotify(ULONG ulMessage);
    
    virtual ~CDownNotify();
    
protected:
    virtual ULONG OnStatusFileName(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnStatusFileSize(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus);
    
    virtual ULONG OnDownStatus(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnDownResult(ULONG ulDownResult);
    
protected:
    virtual int IsNotifyMessage(const MSG *pMsg, ULONG *pulResult);
};

#endif // !defined(AFX_DOWNNOTIFY_H__EA13D2FF_1112_4AAD_89E5_82E8E885C779__INCLUDED_)

