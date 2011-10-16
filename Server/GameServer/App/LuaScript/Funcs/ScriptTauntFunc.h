/// 
/// @file ScriptTauntFunc.h
/// @brief 怪物仇恨相关函数
///
// class Script {
///
/// 取得仇恨列表中指定索引的仇恨值
///
/// @param index 索引，从0开始
/// @return 仇恨值，-1表示未找到
///
int GetTauntValue( int index );

///
/// 设置指定对象的仇恨值
///
/// @param id 对象GUID
/// @param value 仇恨值
///
void SetTauntValue( const CGUID &id, int value );

///
/// 取得仇恨列表中指定GUID对象的仇恨值
///
/// @param id 对象GUID
/// @return 仇恨值，-1表示未找到
///
int GetTauntValue( const CGUID &id );

///
/// 在仇恨列表中随机选择指定类型的对象
///
/// @param type 对象类型
/// @return 对象GUID，列表为空返回NULL_GUID
///
CGUID GetRandObject( long type );

///
/// 让仇恨列表中指定类型的对象执行一个脚本
///
/// @param type 对象类型
/// @param file 脚本名
/// 
void ObjectRunScript( long type, const char *file );

///
/// 强制设定为仇恨列表的第一位
///
/// @param id 对象ID
///
bool ForceToFirst( const CGUID &id );

///
/// 返回仇恨列表长度
///
int GetTauntSize();

///
/// 施放者对某对象释放增益技能，在技能伤害脚本里使用。
///
/// @see GetGUIDByParam
/// @param firer 施放者GUID
/// @param owner 获得BUFF的GUID
/// @param tvalue 增加（获得）的仇恨值
///
void TauntEvents( const CGUID &firer, long ftype, const CGUID &owner, long otype, int tvalue );

// }
