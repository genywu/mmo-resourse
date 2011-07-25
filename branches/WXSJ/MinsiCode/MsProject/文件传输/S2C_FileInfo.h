#pragma once                //┃//┃
#ifndef __S2C_FILEINFO_H__  //┃//┃
#define __S2C_FILEINFO_H__  //┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：S2C_FileInfo.h      //┃
//功能描述：文件信息              //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// S2C文件信息
class S2C_FileInfo; // 为了VA能让他变蓝
BUILD_PACKET_BEGIN(S2C_FileInfo, S2C_PACKET_FILE_INFO) // 此处禁止分号
struct{ // 封包有效数据
    DWORD dwFileSize;
    TCHAR szFileName[MAX_PATH];
}BUILD_PACKET_END(S2C_FileInfo)
//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
