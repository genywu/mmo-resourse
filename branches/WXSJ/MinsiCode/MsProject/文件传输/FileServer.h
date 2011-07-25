#pragma once
#include "S2C_FileInfo.h"
#include "S2C_FlyFile.h"

class CFileServer
    : public CMsMsgThread
{
public:
    CFileServer(WORD wProt, DWORD dwMaxClient, TCHAR* szPassword);
    ~CFileServer(VOID);

    // 停止之前
    virtual BOOL ForeStop(VOID);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent);

    // 空闲处理
    virtual VOID OnThreadFreeWork(VOID);

    // 消息处理
    virtual VOID OnThreadMsgExecute(LPMSG lpMsg);

    // 获取密码
    TCHAR*  GetPassword(VOID);

    // 飞文件
    BOOL    FlyFile(DWORD dwIndex, TCHAR* szFileName);

private:
    CMsServerNetManager*    m_lpCMsServerNetManager;
    CMsPacketFactory*       m_lpCMsPacketFactory;
    CMsFindFileManager*     m_lpCMsFindFileManager;
    TCHAR                   m_szPassword[MAX_PATH];
    S2C_FlyFile::MS_PACKET_DATA m_S2C_FlyFileData;
};

extern CFileServer* g_pCFileServer;
