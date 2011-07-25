#pragma once                //┃//┃
#ifndef __C2S_CONNECT_H__   //┃//┃
#define __C2S_CONNECT_H__   //┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：C2S_Connect.h       //┃
//功能描述：平台C2S连接请求包   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// C2S连接包
class C2S_Connect; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(C2S_Connect, C2S_PACKET_CONNECT) // 此处禁止分号

// 封包有效数据
struct{
    TCHAR   sz_Password[MAX_PATH];

}BUILD_PACKET_END(C2S_Connect)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
