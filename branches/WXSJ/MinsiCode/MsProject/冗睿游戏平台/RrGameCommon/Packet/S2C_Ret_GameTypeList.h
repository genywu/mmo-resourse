#pragma once                        //┃//┃
#ifndef __S2C_RET_GAMETYPELIST_H__  //┃//┃
#define __S2C_RET_GAMETYPELIST_H__  //┃//┃
#pragma pack(push, 1)               //┃//┃
//━━━━━━━━━━━━━━━━━━┛//┃
//文件名称：S2C_Ret_GameTypeList.h      //┃
//功能描述：C2S请求游戏类型列表         //┃
//平台相关：Windows                     //┃
//━━━━━━━━━━━━━━━━━━━━┛
// S2C回复游戏类型列表
class S2C_Ret_GameTypeList; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(S2C_Ret_GameTypeList, S2C_PACKET_RET_GAME_TYPE)   // 此处禁止分号

// 封包有效数据
struct{
    S_GAME_TYPE_INFO    m_GameTypeInfo[MAX_GAME_TYPE_COUNT];
    DWORD               m_dwCount;
    DWORD               m_dwGameTypeIndex;

}BUILD_PACKET_END(S2C_Ret_GameTypeList)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
