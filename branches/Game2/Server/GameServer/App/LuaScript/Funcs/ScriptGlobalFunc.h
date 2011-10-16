/// 
/// @file ScriptGlobalFunc.h
/// @brief Script内的全局使用函数(GM)
///

// class Script {
///
/// 踢指定场景下的所有玩家
/// @param regionID 场景GUID
///
void KickRegionPlayer( const CGUID &regionID );

///
/// 踢全服所有玩家
///
void KickAllPlayer();

///
/// 踢指定玩家
/// @param name 玩家名字
///
void KickPlayer( const char *name );

///
/// 显示在线GM
///
int ListOnlineGM();

///
/// 显示禁言玩家
///
void ListSelencePlayer();

///
/// 封号指定玩家
/// @param name 玩家名字
/// @param time 封号时间
///
void BanPlayer( const char *name, long time );

///
/// 获取字符串资源
/// @param type 资源类型
/// @param id 资源ID
/// @return 字符串
///
const char *GetTextRes( const char* id );

///
/// 发送场景区域新闻
/// @param rgnType 场景类型
/// @param num 
/// @param msg
///
void RollRegionInfo( long rgnType, long num, const char *msg, long fileNum, long rgnID = -1 );

///
/// 发送服务器新闻
///
void RollWorldInfo( long num, const char *str, long fileNum );

///
/// 传送一批玩家到指定场景
///
/// @param rgnType 目标场景类型
/// @param centerPlayer 中心玩家名字
/// @param x 目标X坐标
/// @param y 目标Y坐标
///
void ChangePlayerRegionEx( long rgnType, const char *centerPlayer, const CGUID &rgnID, long x, long y );
// };