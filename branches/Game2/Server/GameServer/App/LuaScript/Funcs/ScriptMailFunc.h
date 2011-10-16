/// 
/// @file ScriptMailFunc.h
/// @brief Script内的邮件操作函数
///

// class Script {
///
/// 收取邮件
/// @param playerID 玩家GUID
/// @return true表示成功请求
///
bool ReceiveMail( const CGUID &playerID );

///
/// 发送系统邮件给源玩家
/// @param mailID 邮件ID
/// @param writer 发送者名字
/// @return true成功
///
bool SendSysMail( ulong mailID, const char *writer );

///
/// 收取系统邮件
///
void ReceiveSysMail();

// }
