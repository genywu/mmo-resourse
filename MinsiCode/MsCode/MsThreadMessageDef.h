#pragma once                        //┃//┃
#ifndef __MSTHREADMESSAGEDEF_H__    //┃//┃
#define __MSTHREADMESSAGEDEF_H__    //┃//┃
//━━━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsThreadMessageDef.h        //┃
//功能描述：线程消息定义                //┃
//平台相关：WINDOWS                     //┃
//━━━━━━━━━━━━━━━━━━━━┛

enum E_MSM
{
    MSM_NOTHING = 0,                                // WPARAM 0                     LPARAM 0        无事通知
    // 客户端通知
    MSM_CLIENT_SOCKET_ON_ENTER  = WM_USER + 0x0001, // WPARAM CMsServerSocket*      LPARAM 0        客户端套接字连接通知    W(服务器套接字)     L(0)
    MSM_CLIENT_SOCKET_ON_EXIT,                      // WPARAM CMsServerSocket*      LPARAM 0        客户端套接字断开通知    W(服务器套接字)     L(0)

    // 服务器通知
    MSM_SERVER_SOCKET_ON_ENTER  = WM_USER + 0x0011, // WPARAM CMsClientSocket*      LPARAM 0        服务器套接字连接通知    W(客户端套接字)     L(0)
    MSM_SERVER_SOCKET_ON_EXIT,                      // WPARAM CMsClientSocket*      LPARAM 0        服务器套接字断开通知    W(客户端套接字)     L(0)

    // 完成端口通知
    MSM_IOCP_UPDATE             = WM_USER + 0x0101, // WPARAM S_IOCP_UPDATE*        LPARAM 0        完成端口更新通知        W(更新通知结构体)   L(0)
    MSM_IOCP_EXIT_OR_ERROR,                         // WPARAM ULONG_PTR             LPARAM 0        完成端口断开或错误通知  W(完成端口通知KEY)  L(0)
    MSM_IOCP_POST_QUEUED_COMPLETION_STATUS,         // WPARAM CMsSocket*            LPARAM 0        外部投递一个完成消息    W(套接字)           L(0)

    // 消息线程通知
    MSM_MSG_THREAD_STOP         = WM_USER + 0x0151, // WPARAM 0                     LPARAM 0        通知线程退出            W(0)                L(0)
    MSM_MSG_THREAD_ON_RUN,                          // WPARAM 0                     LPARAM 0        通知线程已经运行        W(0)                L(0)
    MSM_MSG_THREAD_ON_SUSPEND,                      // WPARAM 0                     LPARAM 0        通知线程已经挂起        W(0)                L(0)

    // 套接字消息通知
    MSM_MSG_SOCKET_EXECUTE,                         // WPARAM CMsSocket*            LPARAM 0        通知套接字需要执行      W(套接字)           L(0)

    // 套接字消息通知
    MSM_DLL_NOTIFY_HWND,                            // WPARAM hWnd                  LPARAM 0        通知游戏平台窗口句柄    W(DllGame游戏窗口)  L(0)
    MSM_DLL_EXIT,                                   // WPARAM HMODULE               LPARAM 0        通知游戏平台DllGame退出 W(DllGame实例句柄)  L(0)

    // 窗口消息
    MSM_INIT_MESSAGE,                               // WPARAM 0                     LPARAM 0        通知窗口初始化          W(0)                L(0)
    MSM_QUIT_MESSAGE,                               // WPARAM 0                     LPARAM 0        通知窗口消息线程退出    W(0)                L(0)
    MSM_SYS_ICON_MESSAGE,                           // WPARAM 0                     LPARAM 0        系统托盘消息通知        W(0)                L(0)

    // 线程消息最大值
    MSM_MAX_COUNT,                                  // WPARAM 0                     LPARAM 0        无事通知
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
