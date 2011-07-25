#pragma once                        //┃//┃
#ifndef __C2S_ASK_GAMETYPELIST_H__  //┃//┃
#define __C2S_ASK_GAMETYPELIST_H__  //┃//┃
#pragma pack(push, 1)               //┃//┃
//━━━━━━━━━━━━━━━━━━┛//┃
//文件名称：C2S_Ask_GameTypeList.h      //┃
//功能描述：C2S请求游戏类型列表         //┃
//平台相关：Windows                     //┃
//━━━━━━━━━━━━━━━━━━━━┛
// C2S请求游戏类型列表
class C2S_Ask_GameTypeList;
BUILD_PACKET_BEGIN(C2S_Ask_GameTypeList, C2S_PACKET_ASK_GAME_TYPE)

// 封包有效数据
struct{
    DWORD   m_dwGameTypeIndex;  // 客户端游戏类型索引
    DWORD   m_dwData[2000];

}BUILD_PACKET_END(C2S_Ask_GameTypeList)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
