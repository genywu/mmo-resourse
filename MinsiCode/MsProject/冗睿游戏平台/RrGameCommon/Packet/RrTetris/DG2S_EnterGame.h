#pragma once                //┃//┃
#ifndef __C2S_CONNECT_H__   //┃//┃
#define __C2S_CONNECT_H__   //┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：DG2S_EnterGame.h    //┃
//功能描述：平台C2S连接请求包   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// DG2S游戏进入
#define GAMEENTER_KEY 1234567890  // 连接KEY
class DG2S_EnterGame; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(DG2S_EnterGame, DG2S_PACKET_ENTER_GAME) // 此处禁止分号

// 封包有效数据
struct{
    DWORD   m_dwData;

}BUILD_PACKET_END(DG2S_EnterGame)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
