#pragma once                //┃//┃
#ifndef __S2C_FLYFILE_H__   //┃//┃
#define __S2C_FLYFILE_H__   //┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：S2C_FlyFile.h       //┃
//功能描述：飞文件              //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// S2C飞文件
class S2C_FlyFile; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(S2C_FlyFile, S2C_PACKET_FLY_FILE) // 此处禁止分号

// 封包有效数据
struct{
    BYTE m_Data[KB_SIZE * 5];

}BUILD_PACKET_END(S2C_FlyFile)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
