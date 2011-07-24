//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateData.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-3-15 17:20:54
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATE_DATA_DEFINE_H_
#define _UPDATE_DATA_DEFINE_H_  1

#include "DataDefine.h"
#include "KCloseProgramMgr.h"  
#include "UpdateExport.h"

typedef struct tagKUPDATE_DATA
{
    int nMainVersion;               // Local Program Version
    char szExecuteProgram[MAX_PATH];     // Local Program
	char szParameter[MAX_PATH];     // Local Program Parameter
   
    BOOL bVersionError;   // 升级数据中没有支持该版本的文件的标志

    KCloseProgramMgr CloseProgramMgr;	//管理被关闭的程序
    
    KSaveLog SaveLog;	// 写日志的接口
    
    int nMethod;                    // Method of update 0: from internet  1: from LAN
    
    char szLocalPath[MAX_PATH];     // Path of update file,just index.dat path from LAN
    char szHostURL[MAX_PATH];       // URL of download host
    char szHostName[MAX_PATH];      // Name of download host
    char szDefHostURL[MAX_PATH];    // Default URL of download host
    char szDefHostName[MAX_PATH];   // Default Name of download host

    ULONG   ulTryTimes;
    BOOL    bAutoTryNextHost;              // Flag of try use next faster host when failed
    BOOL    bUseFastestHost;               // Flag use the fastest host
    
    BOOL    bUseVerify;					
    CHAR    szVerifyInfo[256];

    
    KPROXY_SETTING  ProxySetting;	// 代理设置
    
    BOOL    bAutoResume;            //L: auto-resume the previous download file

    BOOL    bNeedUpdateSelfFirst;   // Need update self first flag
    int     nNewsItemCount;			// 新闻项的总数
    CString sAnnounce;				// 升级后显示给用户的通知或者重要通告

    CString sReadme;				// 升级后显示给用户的更新文件的修正说明或者其他信息
    
    int  nOSPlatVersion;			// 操作系统的版本
    
    BOOL bDownloadFailed;            // 1 : Failed              0 : successful
    BOOL bUpdateFailed;              // 1 : Failed              0 : successful
    BOOL bNeedRebootFalg;            // 1 : Need reboot Now     0 : not need reboot
    
    BOOL bRebootFlag;                // 1 : reboot now          0 : reboot later
    BOOL bAutoRebootFlag;            // 1 : auto ruboot         0 : reboot later 
    BOOL bRebootFinishUpdateFlag;    // 1 : Need reboot to finish update 0: not need reboot

} KUPDATE_DATA;


int InitUpdateData(BOOL bLog);
int UninitUpdateData();


#endif //_UPDATE_DATA_DEFINE_H_