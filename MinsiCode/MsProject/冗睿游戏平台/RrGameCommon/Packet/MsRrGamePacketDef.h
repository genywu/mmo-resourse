#pragma once                    //┃//┃
#ifndef __MSRRGAMEPACKETDEF_H__ //┃//┃
#define __MSRRGAMEPACKETDEF_H__ //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsRrGamePacketDef.h     //┃
//功能描述：冗睿游戏平台包定义      //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

enum E_PACKET_TYPE
{
    RR_GAME_PACKET_INVALID = -1,

    C2S_PACKET_BEGIN = 0x00000000,      // C2S 封包起始
    C2S_PACKET_CONNECT,                 // C2S 请求连接
    C2S_PACKET_ASK_GAME_TYPE,           // C2S 请求游戏类型列表
    C2S_PACKET_ASK_GAME,                // C2S 请求游戏列表
    C2S_PACKET_ASK_DISTRIC,             // C2S 请求大区列表
    C2S_PACKET_ASK_ROOM,                // C2S 请求房间列表
    C2S_PACKET_ASK_DESK,                // C2S 请求桌子列表

    C2S_PACKET_SERVER_EXIT,             // 调试使用,命令服务器退出

    S2C_PACKET_BEGIN = 0x00001000,      // S2C 封包起始
    S2C_PACKET_RET_CONNECT,             // S2C 回复连接
    S2C_PACKET_RET_GAME_TYPE,           // S2C 回复游戏类型列表
    C2S_PACKET_RET_GAME,                // S2C 回复游戏列表
    C2S_PACKET_RET_DISTRIC,             // S2C 回复大区列表
    C2S_PACKET_RET_ROOM,                // S2C 回复房间列表
    C2S_PACKET_RET_DESK,                // S2C 回复桌子列表

    RR_GAME_PACKET_MAX_COUNT,           // 平台封包最大值
};

#include "C2S_Connect.h"
#include "S2C_Connect.h"
#include "C2S_Ask_GameTypeList.h"
#include "S2C_Ret_GameTypeList.h"
#include "C2S_ExitServer.h"
static lpCMsBasePacket g_lpCMsBasePacket[] = 
{
    REGISTER_PACKET(C2S_Connect),
    REGISTER_PACKET(S2C_Connect),
    REGISTER_PACKET(C2S_Ask_GameTypeList),
    REGISTER_PACKET(S2C_Ret_GameTypeList),
    REGISTER_PACKET(C2S_ExitServer),
};

// 宏定义封包个数
#define PACKET_MAX_COUNT sizeof(g_lpCMsBasePacket) / sizeof(lpCMsBasePacket)

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
