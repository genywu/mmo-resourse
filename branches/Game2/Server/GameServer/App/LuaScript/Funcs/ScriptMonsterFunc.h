/// 
/// @file ScriptMonsterFunc.h
/// @brief Script内的操作操作函数
///

// class Script {
///
/// 添加定时器让怪物AI继续
///
/// @param millSecs AI触发的时间（毫秒）
/// @return 1表示成功，0表示失败
///
int ContinueAI( ulong millSecs );

///
/// 添加周期行为：移动到指定格子
///
/// @param x, y 目标坐标
///
void MoveToCell( long x, long y );

///
/// 添加周期行为：随机移动一段时间
///
/// @param time 移动的时间（毫秒）
///
void RandomMove();

///
/// 添加周期行为：在指定方向移动指定步数
///
/// @param direct 方向
/// @param step 步数
///
void MoveByStep( long direct, long step );

///
/// 添加周期行为：跟随指定目标一段时间
///
/// @param targetID 目标GUID
/// @param targetType 目标类型
/// @param time 指定时间
///
void FollowTarget( const CGUID &targetID, long targetType, long time );

///
/// 取得当前攻击目标
/// targetID, targetType = GetTarget()
///
/// @return targetID 目标GUID
/// @return targetType 目标类型
///
CGUID GetTarget( long *targetType = 0 );

//}