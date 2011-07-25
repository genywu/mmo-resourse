#include "Precompiled.h"
//
//// 玩家构造函数
//CPlayer::CPlayer(VOID)
//{
//    ;
//}
//
//// 玩家析构函数
//CPlayer::~CPlayer(VOID)
//{
//    ;
//}
//
//// 消息处理
//// 数据更新状态
//BOOL CPlayer::OnUpDataed(VOID)
//{
//    return CMsSocket::OnUpDataed();
//}
//
//// 退出状态
//BOOL CPlayer::OnExit(VOID)
//{
//    return CMsSocket::OnExit();
//}
//
//// 错误状态
//BOOL CPlayer::OnError(VOID)
//{
//    return CMsSocket::OnError();
//}
//
//// 将发送封包拷贝到待发送缓冲中
//BOOL CPlayer::WritePacket(CBasePacket* lpPacket)
//{
//    return CMsSocket::WritePacket(lpPacket);
//}
//
//// 将收到的数据拷贝到接收缓冲中
//USIZE CPlayer::ReadPacket(CBasePacket* lpPacket)
//{
//    return CMsSocket::ReadPacket(lpPacket);
//}
//
//// 执行收到的封包
//BOOL CPlayer::ExecutePacket(LPVOID pCServerNetManager)
//{
//    CServerNetManager* lpCServerNetManager = (CServerNetManager*)pCServerNetManager;
//    return m_pBasePacket->ExecutePacket(lpCServerNetManager, this);
//}
