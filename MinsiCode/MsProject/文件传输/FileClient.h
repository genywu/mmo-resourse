#pragma once

class CFileClient
    : public CMsMsgThread
{
public:
    CFileClient(TCHAR* szServerIP, WORD wProt, TCHAR* szPassword);
    ~CFileClient(VOID);

    // 停止之前
    virtual BOOL ForeStop(VOID);

    // 线程事件通知
    virtual VOID OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent);

    // 空闲处理
    virtual VOID OnThreadFreeWork(VOID);

    // 消息处理
    virtual VOID OnThreadMsgExecute(LPMSG lpMsg);

    // 创建文件
    VOID CreateFile(TCHAR* szFileName);

    // 追加文件
    VOID AppendFile(LPVOID lpData, DWORD dwSize);

private:
    CMsClientNetManager*    m_lpCMsClientNetManager;
    CMsPacketFactory*       m_lpCMsPacketFactory;
    TCHAR                   m_szPassword[MAX_PATH];
    HANDLE                  m_hFile;
};

extern CFileClient* g_pCFileClient;