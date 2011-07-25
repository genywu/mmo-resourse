#pragma once                //┃//┃
#ifndef __C2S_EXITSERVER_H__//┃//┃
#define __C2S_EXITSERVER_H__//┃//┃
#pragma pack(push, 1)       //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：C2S_ExitServer.h    //┃
//功能描述：C2S命令服务器退出   //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛
// C2S命令服务器退出

// 关闭服务器密码最大长度
#define SHUT_DOWN_PASSWORD_MAX_LEN  26

class C2S_ExitServer;
BUILD_PACKET_BEGIN(C2S_ExitServer, C2S_PACKET_SERVER_EXIT)

// 封包有效数据
struct{
    TCHAR   m_szShutDownPassword[SHUT_DOWN_PASSWORD_MAX_LEN];

}BUILD_PACKET_END(C2S_ExitServer)

//━━━━━━━━━━┓
#pragma pack(pop)   //┃
#endif//#pragma once//┃
//━━━━━━━━━━┛
