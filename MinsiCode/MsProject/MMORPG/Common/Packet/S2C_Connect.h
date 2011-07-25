#pragma once                //┃//┃
#ifndef __S2C_CONNECT_H__   //┃//┃
#define __S2C_CONNECT_H__   //┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：S2C_Connect.h       //┃
//功能描述：平台S2C连接回复包   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// S2C连接包
#define CONNECT_KEY 1234567890  // 连接KEY
class S2C_Connect; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(S2C_Connect, S2C_PACKET_RET_CONNECT) // 此处禁止分号

// 封包有效数据
struct{
    DWORD   m_dwData;

}BUILD_PACKET_END(S2C_Connect)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
