/// 
/// @file ScriptBaseFunc.h
/// @brief Script内的基础函数
///

// class Script {
///
/// 获取源对象的GUID
///
/// @return 源对象GUID
///
CGUID GetSrcShapeID();

///
/// 获取目标对象的GUID
///
/// @return 目标对象GUID
///
///
CGUID GetDestShapeID();

///
/// 取得当前场景
///
/// @return 当前场景GUID
///
CGUID GetCurRgn();

// };