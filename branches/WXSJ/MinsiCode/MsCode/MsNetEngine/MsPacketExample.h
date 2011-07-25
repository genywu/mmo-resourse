#pragma once                    //┃//┃
#ifndef __MSPACKETEXAMPLE_H__   //┃//┃
#define __MSPACKETEXAMPLE_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：MsPacketExample.h       //┃
//功能描述：网络封包定义例子        //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 指定内存对齐为1个字节对齐
#pragma pack(push, 1)

// 样例包
class CMsPacketExample
    :public CMsBasePacket
{
public:
    CMsPacketExample(VOID);

    // 获取有效数据地址
    virtual LPVOID GetValidPacketData(DWORD* lpSize);

    // 获取有效数据尺寸
    virtual DWORD GetValidPacketDataSize(VOID);

    // 执行封包
    virtual BOOL  ExecutePacket(CMsSocket* lpCMsSocket);

public:
    // 有效封包数据
    typedef struct
    {
        DWORD   m_dwData;
    }MS_PACKET_DATA, *LPMS_PACKET_DATA;
};

extern CMsPacketExample g_CMsPacketExample;

// 恢复原来的内存对齐
#pragma pack(pop)

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
