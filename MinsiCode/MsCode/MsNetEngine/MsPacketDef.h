#pragma once                //┃    //┃
#ifndef __MSPACKETDEF_H__   //┃    //┃
#define __MSPACKETDEF_H__   //┃    //┃
//━━━━━━━━━━━━━━┛    //┃
//文件名称：MsPacketDef.h           //┃
//功能描述：基本网络封包定义        //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 封包头标识符
#define PACKET_IDENTIFIER   (DWORD)0xCCDDAABB

// 最大封包尺寸
#define MAX_PACKET_LEN      KB_SIZE * 10

// 定义包类型, 等同于用户 E_PACKET_TYPE 类型
typedef DWORD PACKET_TYPE;

// 封包密钥类型枚举
enum E_KEY_TYPE
{
    KEY_TYPE_NORMAL = 0,
    KEY_TYPE_USER_ID,
    KEY_TYPE_COUNT,
    KEY_TYPEY_MAX = 0xFF,
};

// 宏定义注册全局封包
#define REGISTER_PACKET(pack)   &g_##pack

// 构造封包ID
#define BUILD_PACKET_TYPE(pack) g##pack##_ID
#define BPT BUILD_PACKET_TYPE

// 开始构造封包
#define BUILD_PACKET_BEGIN(pack,type)   static PACKET_TYPE g##pack##_ID = type;                                 \
                                        class pack:public CMsBasePacket{                                        \
                                        public:                                                                 \
                                        pack::pack(VOID)                                                        \
                                        :CMsBasePacket(type, sizeof(MS_PACKET_DATA) + sizeof(PacketHead)){}     \
                                        virtual BOOL ExecutePacket(CMsSocket* lpCMsSocket);                     \
                                        typedef

// 结束构造封包
#define BUILD_PACKET_END(pack)          MS_PACKET_DATA, *LPMS_PACKET_DATA;}; static pack g_##pack;


// 指定内存对齐为1个字节对齐
#pragma pack(push, 1)

// 包头
struct PacketHead
{
    PacketHead(
        DWORD wIdentifier,
        PACKET_TYPE ePackType,
        E_KEY_TYPE eKeyType,
        DWORD dwSize
        );

    PacketHead(VOID)
    {
        ZeroMemory(this, sizeof(PacketHead));
    }

    // 检查封包是否合法
    BOOL CheckRegular(VOID);
    DWORD           m_dwIdentifier; // 封包标识符
    PACKET_TYPE     m_ePackType;    // 封包类型     E_PACKET_TYPE
    E_KEY_TYPE      m_eKeyType;     // 封包密钥类型
    DWORD           m_dwPackSize;   // 封包大小
    INT64           m_i64Time;      // 封包64位时间戳
};
typedef PacketHead* lpPacketHead;

class CMsSocket;
// 基础包
class CMsBasePacket
{
public:
    CMsBasePacket(
        PACKET_TYPE ePackType,
        DWORD dwSize,
        DWORD wIdentifier = PACKET_IDENTIFIER,
        E_KEY_TYPE eKeyType = KEY_TYPE_NORMAL
        );

    // 锁定封包
    VOID Lock(VOID);

    // 获取封包尺寸
    DWORD GetPacketSize(VOID);

    // 获取封包类型
    PACKET_TYPE GetPacketType(VOID);

    // 获取封包时间
    INT64 GetPacketTime(VOID);

    // 获取封包密钥
    E_KEY_TYPE GetPacketKeyType(VOID);

    // 判断封包是否已锁定
    BOOL IsLock(VOID);

    // 检查封包
    BOOL CheckRegular(VOID);

    // 获取包头地址
    lpPacketHead  GetPacketHead(DWORD* lpSize = NULL);

    // 重置封包
    virtual VOID  Reset(VOID);

    // 执行封包
    virtual BOOL  ExecutePacket(CMsSocket* lpCMsSocket);

private:
    // 当前对象是否锁定
    BOOL    m_IsLock;

    // 封包头
    PacketHead  m_PackHead;
public:
    // 有效封包数据
    typedef struct
    {
    }MS_PACKET_DATA, *LPMS_PACKET_DATA;
};
typedef CMsBasePacket* lpCMsBasePacket;

// 恢复原来的内存对齐
#pragma pack(pop)

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
