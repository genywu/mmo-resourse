#pragma once                //┃//┃
#ifndef __GAMESTATION_H__   //┃//┃
#define __GAMESTATION_H__   //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：GameStation.h       //┃
//功能描述：冗睿游戏平台        //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
#include "BaseNode.h"
#include "GameTypeNode.h"
#include "GameTree.h"

class CGameStation
{
public:
    CGameStation(HWND hWnd, DWORD dwCurrentThreadId);
    ~CGameStation(VOID);

    BOOL Init();
    VOID Realese();

    // 判断网络是否有效
    BOOL IsNetValid(VOID);

    //主平台渲染
    VOID Render(VOID);

    //得到配置文件数据  
    S_GameStationConfig* GetConfig(VOID);
    S_GameStationResConfig* GetResConfig(VOID);

    //加载配置文件
    VOID LoadConfig(VOID);  

    // 获取列表树
    CGameTree* GetTree(VOID);
    //请求游戏类型
    //VOID AskNode(CBaseNode* pNode);
    
    //树节点Id维护
    //DWORD GetNodeId(){return m_dwNodeId++;} 
    
    //平台消息处理
    BOOL  GameStationWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
    
    //鼠标点击测试
    //CBaseNode* MouseTest(POINT Pos, CBaseNode* m_Node);                           
    
    // 获取服务器套接字
    lpCMsServerSocket GetServerSocket(VOID);

    HWND GetHwnd(VOID);

    // 获取缓冲图
    CMsGdiBuffImage* GetBuffImage(VOID);

    // DllGame调用接口
    static BOOL DllGameWritePacket(
        CMsPacketFactory* lpFactory,
        PACKET_TYPE PacketId,
        LPVOID lpData,
        DWORD dwSize = INVALID_WID
        );

private:
    BOOL                    m_bInit;                // 初始化标记
    HWND                    m_hWnd;                 // 平台窗口句柄
    DWORD                   m_dwGameWindowWidth;    // 平台窗口宽度
    DWORD                   m_dwGameWindowHight;    // 平台窗口高度

    CGameTree*              m_pGameTree;            // 平台树结构
    CMsGdiBuffImage*        m_pBuffImage;           // 缓冲图对象

    HWND                    m_hDllGameWnd;          // DllGame窗口句柄
    HMODULE                 m_hDllGameModule;       // DllGame模块句柄
    VOID(*m_funSetGameStationLogicThreadId)(DWORD); // DllGame设置逻辑线程Id接口
    HANDLE(*m_funGetDllGameLogicThread)(VOID);      // 得到DllGame逻辑线程句柄接口
    HANDLE(*m_funStopDllGame)(VOID);                // 停止GameDll接口

    S_GameStationConfig     m_Config;               // 配置文件结构
    S_GameStationResConfig  m_ResConfig;            // 资源配置信息
    ServerConfig            m_ServerConfig;         // 服务器配置文件
    CMsPacketFactory*       m_pCMsPacketFactory;    // 封包工厂
    CMsClientNetManager*    m_pCMsClientNetManager; // 网络管理器
    DWORD                   m_dwCurrentThreadId;    // 当前线程Id
};
//全局平台指针
extern CGameStation* g_pCGameStation;

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
