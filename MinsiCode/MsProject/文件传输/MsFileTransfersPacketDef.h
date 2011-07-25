#pragma once                            //┃    //┃
#ifndef __MSFILETRANSFERSPACKETDEF_H__  //┃    //┃
#define __MSFILETRANSFERSPACKETDEF_H__  //┃    //┃
//━━━━━━━━━━━━━━━━━━━━┛    //┃
//文件名称：MsFileTransfersPacketDef.h          //┃
//功能描述：文件传输封包定义                    //┃
//平台相关：Windows                             //┃
//━━━━━━━━━━━━━━━━━━━━━━━━┛

enum E_PACKET_TYPE
{
    RR_GAME_PACKET_INVALID = -1,

    C2S_PACKET_BEGIN = 0x00000000,      // C2S 封包起始
    C2S_PACKET_CONNECT,                 // C2S 请求连接

    S2C_PACKET_BEGIN = 0x00001000,      // S2C 封包起始
    S2C_PACKET_RET_CONNECT,             // S2C 回复连接
    S2C_PACKET_FLY_FILE,                // S2C 飞文件
    S2C_PACKET_FILE_INFO,               // S2C 文件信息

    RR_PACKET_MAX_COUNT,                // 封包最大值
};

#include "C2S_Connect.h"
#include "S2C_Connect.h"
#include "S2C_FlyFile.h"
#include "S2C_FileInfo.h"
static lpCMsBasePacket g_lpCMsBasePacket[] = 
{
    REGISTER_PACKET(C2S_Connect),
    REGISTER_PACKET(S2C_Connect),
    REGISTER_PACKET(S2C_FlyFile),
    REGISTER_PACKET(S2C_FileInfo),
};

// 宏定义封包个数
#define PACKET_MAX_COUNT sizeof(g_lpCMsBasePacket) / sizeof(lpCMsBasePacket)

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
