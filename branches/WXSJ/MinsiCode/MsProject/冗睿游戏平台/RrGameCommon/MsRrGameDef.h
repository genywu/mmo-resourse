#pragma once                //┃//┃
#ifndef __MSRRGAMEDEF_H__   //┃//┃
#define __MSRRGAMEDEF_H__   //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsRrGameDef.h       //┃
//功能描述：冗睿游戏平台定义    //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━┓
#   define I_KNOW_THE_BASE_TYPE //┃
#   define USE_MS_ASSERTX_FULL  //┃
#   define USE_MS_INI           //┃
#   define USE_MS_LOG           //┃
#   define USE_MS_NET           //┃
#ifdef MS_RRGAME_CLIENT         //┃
#   define USE_MS_GDI_PLUS_IMAGE//┃
#   define USE_MS_WINDOW        //┃
#   define USE_MS_CLIENT_NET    //┃
#include "MsBaseDef.h"          //┃
#else                           //┃
#   define USE_MS_CONSOLE       //┃
#   define USE_MS_SERVER_NET    //┃
#include "MsBaseDef.h"          //┃
#endif                          //┃
//━━━━━━━━━━━━━━━━┛

enum E_RR_GAME_MSM
{
    MSM_RRGAME_SHUT_DWON_SERVER = MSM_MAX_COUNT + 0x0001,   // WPARAM 0         LPARAM 0        通知服务器退出  W(0)    L(0)
};

// 最大的游戏类型个数
#define MAX_GAME_TYPE_COUNT 10

// 游戏类型名称最大长度
#define GAME_TYPE_NAME_LEN  20

// 游戏类型结构体
struct S_GAME_TYPE_INFO 
{
    TCHAR   m_szGameTypeName[GAME_TYPE_NAME_LEN];   // 游戏类型名称
    WORD    m_wIconId;                              // 此项需要的光标Id
    BYTE    m_IsGreenGroup;                         // 是否是绿色游戏组
};

// 鼠标键值
#define HGEK_LBUTTON    0x01
#define HGEK_RBUTTON    0x02
#define HGEK_MBUTTON    0x04

// 节点类型
enum E_NODE_TYPE
{
    NODE_ROOT,
    NODE_GAMETYPE,
    NODE_GAME,
    NODE_DISTRIC,
    NODE_ROOM,
    NODE_DESK,
};

// 各个DllGame封包使用范围
enum E_PACKET_BOUND
{
    E_PACKET_BOUND_BEGIN = 0x00000000,                  // 最开始
    E_PACKET_BOUND_GAME_STATION_BEGIN = 0x00001000,     // 平台开始
    E_PACKET_BOUND_RRTETRIS_BEGIN = 0x00002000,         // 俄罗斯方块开始
};

// 游戏平台配置信息
struct S_GameStationConfig
{
    DWORD dwGameStationWindowWidth;
    DWORD dwGameStationWindowHeight;
    DWORD m_dwBetween;

    DWORD dwGameTypeWidth;
    DWORD dwGameTypeHeight;
    DWORD dwGameTypeIndent;

    DWORD dwGameWidth;
    DWORD dwGameHeight;
    DWORD dwGameIndent;

    DWORD dwDistrictWidth;
    DWORD dwDistrictHeight;
    DWORD dwDistrictIndent;

    DWORD dwRoomWidth;
    DWORD dwRoomHeight;
    DWORD dwRoomIndent;

    DWORD dwDeskWidth;
    DWORD dwDeskHeight;
    DWORD dwDeskIndent;
};

// 服务器配置信息
struct ServerConfig
{
    TCHAR IP[IP_SIZE];
    WORD  wProt;  
};

// 宏定义资源文件名最大长度
#define MAX_RES_FILE_NAME_LEN   20

// 资源个数
#define MAX_RES_COUNT           20
struct S_GameStationResConfig
{
    TCHAR   m_szImageDir[MAX_PATH];
    TCHAR   m_szImageName[MAX_RES_FILE_NAME_LEN][MAX_RES_COUNT];

    TCHAR   m_szMusicDir[MAX_PATH];
    TCHAR   m_szMusicName[MAX_RES_FILE_NAME_LEN][MAX_RES_COUNT];

    TCHAR   m_szSoundDir[MAX_PATH];
    TCHAR   m_szSoundName[MAX_RES_FILE_NAME_LEN][MAX_RES_COUNT];
};

// 游戏平台标题尺寸
#define RR_GAME_CAPTION_TEXT_SIZE   24
//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
