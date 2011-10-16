/// 
/// @file ScriptClientFunc.h
/// @brief 与客户端交互的函数
/// @details 凡是与颜色相关的参数，都可以用ARGB构造得到
///

// class Script {
///
/// 发送文字信息给源对象
/// @param str 文字信息
/// @param fgColor 文字颜色
/// @param bkColor 背景颜色
/// @param playerID [可选]指定发送给哪个玩家，默认为源对象
/// 
void AddInfo( const char *str, ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0, const CGUID &playerID = NULL_GUID );

///
/// 发送文字信息给一个场景上的所有玩家
/// @param str 文字信息
/// @see AddInfo
///
void PostRegionInfo( const char *str, ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0 );

///
/// 发送文字信息给全服玩家
/// @see AddInfo
///
void PostWorldInfo( const char *str, ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0 );

///
/// 发送文字信息给指定国家的玩家
/// @see AddInfo
///
void PostCountryInfo( const char *str, long country, ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0 );

///
/// 播放特效动画
/// @param index 特性ID
///
void PlayEffectByIndex( long index );

///
/// 让源玩家说话
/// @param str 说话内容字符串
///
void PlayerTalk( const char *str );

///
/// @see PlayerTalk
///
void PlayerTalkByIndex( long textType, long textID );

///
/// 让背包闪烁
///
void PackGlint();

///
/// 让指定玩家发送世界消息
/// @param playerID 玩家GUID
/// @see AddInfo
///
void PostPlayerInfo( const CGUID &playerID, const char *str, ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0 );

///
/// @see PostPlayerInfo
///
void PostPlayerInfoByIndex( const CGUID &playerID, long textType, long textID,
						   ulong fgColor = 0xFFFFFFFF, ulong bkColor = 0 );

///
/// 显示字符串到客户端屏幕
/// @param posX 屏幕X坐标
/// @param posY 屏幕Y坐标
/// @param str 文字字符串
/// @param isShow 非0显示
///
void ShowString( long posX, long posY, const char *str, long isShow );

///
/// 改变天气
/// @param ID 天气ID
///
void ChangeWeather( long ID );

///
/// 播放特效
/// @param effID 特性ID
/// @param bindPos 
/// @param inRgn 0/1
/// @param x
/// @param y
///
void PlayEffect( long effID, long bindPos, uchar inRgn = 0, long x = 0, long y = 0 );

///
/// 播放动作
/// @param action 动作ID
///
void PlayAction( ushort action );

///
/// 播放声音
/// @param file 声音文件
/// @param toAround 0只播放给自己，1播放给周围
///
void PlaySound( const char *file, uchar toAround );

///
/// @see PlaySound
///
void Play3DSound( const char *file, long loop, long x, long y, long z );

///
/// 播放背景音乐
/// @param file 音乐文件
/// @param times 循环次数
///
void PlayBkMusic( const char *file, long times );

///
/// 停止背景音乐
///
void StopBkMusic();

///
/// 添加物品到快捷栏
/// @param goodsName 物品原始名
/// @param barIndex 快捷栏栏位编号
///
int AddGoodsToShortcutBar( const char *goodsName, long barIndex );

///
/// 添加技能到快捷栏
/// @param skillID 技能ID
/// @param barIndex 快捷栏栏位编号
///
int AddSkillToShortcutBar( long skillID, long barIndex );

///
/// 转动摄像机
///
void TransformCamera( long mode, long param1, long param2, long param3 );

///
/// 操作客户端界面
/// @param mode 1打开，2关闭，3关闭所有
/// @param interfaceName 界面名字
///
void OperInterface( long mode, const char *interfaceName );

///
/// 帮助提示
/// @param script 脚本文件名
/// 
void HelpHint( const char *script, long delTime );

///
/// 界面帮助提示
///
void InterfaceHint( const char *script, long delTime, const char *interfaceName,
				   long x, long y, long w, long h );

///
/// 发送仪态动作
/// @param actionID 动作ID
/// @param srcID 对方的GUID
///
int SendExpAction( long actionID, long srcType, const CGUID &srcID, long destType = 0, const CGUID &destID = NULL_GUID );
// };