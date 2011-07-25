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
    DG2S_PACKET_BEGIN = E_PACKET_BOUND_RRTETRIS_BEGIN, // 封包起始
    DG2S_PACKET_ENTER_GAME,     // DG2S 进入游戏

    DG2S_PACKET_MAX_COUNT,      // 俄罗斯方块封包最大值
};

#include "DG2S_EnterGame.h"
static lpCMsBasePacket g_lpCMsBasePacket[] = 
{
    REGISTER_PACKET(DG2S_EnterGame),
};

// 宏定义封包个数
#define PACKET_MAX_COUNT sizeof(g_lpCMsBasePacket) / sizeof(lpCMsBasePacket)

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
