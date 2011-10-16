/// 
/// @file ScriptShapeFunc.h
/// @brief Script内的Shape相关函数
/// @details 凡是涉及到对象类型的，TYPE_PLAYER表示玩家类型，TYPE_MONSTER表示怪物类型。
///

// class Script {
///
/// 设置Shape(怪物/玩家)属性
///
/// @param type Shape类型: TYPE_PLAYER, TYPE_MONSTER
/// @param id Shape的GUID
/// @param attrName 要设置的属性名
/// @param attrVal 新的属性值（数字）
///
void SetAttr( long type, const CGUID &id, const char *attrName, double attrVal );

///
/// 获取Shape(怪物/玩家)的属性
/// @param type Shape类型：TYPE_PLAYER, TYPE_MONSTER
/// @param id Shape的GUID
/// @param attrName 要获取的属性名
/// @return 返回得到的属性值（数字）
///
double GetAttr( long type, const CGUID &id, const char *attrName );

///
/// 改变Shape属性，执行属性加法操作
/// @see SetAttr
///
void ChangeAttr( long type, const CGUID &id, const char *attrName, double attrVal );

///
/// 播放指定的表情
/// @param motion 表情ID
/// @param around 为1时广播给周围玩家
///
void PlayMotion( long motion, long around );

///
/// 在指定对象上运行脚本，该指定对象成为新的脚本实例的源对象
/// @param type 对象类型
/// @param id 对象GUID
/// @param script 脚本文件名
///
void ShapeRunScript( long type, const CGUID &id, const char *script );

///
/// 获取周围玩家数量
/// @param range 范围
/// @param diedFlag [可选]值为0则不获取死亡玩家
/// @param hideFlag [可选]值为0则不获取隐藏玩家
/// @return 玩家数量
///
long GetAroundPlayerNum( long range, long diedFlag = 0, long hideFlag = 0 );

///
/// 获取周围怪物数量
/// @param range 范围
/// @return 返回怪物数量
///
long GetAroundMonsterNum( long range );

///
/// 获取周围与源对象相同类型的怪物，源对象本身必须为怪物
/// @param range 范围
/// @return 返回怪物数量
///
long GetAroundSameMonsterNum( long range );

/// 
/// 为指定对象设置删除定时器
/// @param type 对象类型
/// @param id 对象GUID
/// @param delTime 多少时间后删除
///
void SetDelTime( long type, const CGUID &id, long delTime );

///
/// 改变移动速度
/// @param type 对象类型
/// @param id 对象GUID
/// @param speed 速度值
///
void ChangeMoveSpeed( long type, const CGUID &id, short speed );

///
/// 设置移动速度
/// @param type 对象类型
/// @param id 对象GUID
/// @param speed 速度值
///
void SetMoveSpeed( long type, const CGUID &id, short speed );

///
/// 获取当前速度
/// @param type 对象类型
/// @param id 对象GUID
/// @return 速度值
///
float GetCurrentSpeed( long type, const CGUID &id );

/// 
/// 判断指定对象是否有效
/// @param type 对象类型
/// @param id 对象GUID
/// @return 返回true表示对象有效，否则无效
///
bool IsShapeValid( long type, const CGUID &id );

// };