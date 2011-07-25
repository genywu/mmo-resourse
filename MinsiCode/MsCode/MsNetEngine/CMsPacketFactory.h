#pragma once                    //┃//┃
#ifndef __CMSPACKETFACTORY_H__  //┃//┃
#define __CMSPACKETFACTORY_H__  //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：CMsPacketFactory.h      //┃
//功能描述：网络封包工厂            //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

class CMsPacketFactory
{
public:
    // 构造
    CMsPacketFactory(PACKET_TYPE PacketMaxCount);

    // 析构
    ~CMsPacketFactory(VOID);

    // 注册封包
    BOOL Register(lpCMsBasePacket* lppPacket, DWORD dwPacketCount);

    // 获取封包默认尺寸
    DWORD GetPacketDefaultSize(PACKET_TYPE PacketId);

    // 获取封包数据默认尺寸
    DWORD GetPacketDataDefaultSize(PACKET_TYPE PacketId);

    // 获取封包
    CMsBasePacket*  GetPacket(PACKET_TYPE PacketId);

    // 解析包头得到封包
    CMsBasePacket*  ParsePacketHead(PacketHead* lpPacketHead);

private:
    lpCMsBasePacket*    m_ppPacketPool;     // 封包指针池
    PACKET_TYPE         m_PacketMaxCount;   // 封包最大个数
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
