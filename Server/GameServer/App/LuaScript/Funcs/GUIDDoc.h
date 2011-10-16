///
/// @file GUIDDoc.h
/// @brief GUID相关函数
/// @details 本文件仅用于doxygen自动产生脚本方GUID的使用说明，不被C++使用
///

///
/// GUID类型
///
/// 创建方式分为2种：
/// @code local g=CGUID() --创建空GUID，由lua自动回收
/// CGUID:CreateGUID( g ) --设置其值
/// @endcode
/// 或者
/// @code local g=CGUID:new() --需要自己手动删除
/// CGUID:CreateGUID( g )
/// g:delete() --手动删除
/// @endcode
///
class CGUID
{
public:
	///
	/// 由字符串构造GUID值
	///
	CGUID( const char *str );

	///
	/// 判断两个GUID是否相等
	/// 
	/// @code local g1=CGUID(), g2=CGUID()
	/// if g1==g2 then something end
	/// @endcode
	/// @return true表示相等
	///
	bool operator == ( const CGUID &other ) const;

	///
	/// 获取该GUID的字符串表示形式
	///
	/// @return 字符串
	///
	const char *tostring() const;

	///
	/// 判断该GUID是否为空
	/// @return true表示为空
	///
	bool IsInvalid();

	///
	/// 将空的GUID设置一个值
	/// @code local g=CGUID()
	/// CGUID:CreateGUID( g )
	/// @endcode
	/// @param guid 想设置的GUID变量
	///
	static bool CreateGUID( CGUID &guid );

	///
	/// 一个空的GUID变量
	/// @code local g=CGUID()
	/// if g==CGUID.GUID_INVALID then something end
	/// @endcode
	///
	static CGUID GUID_INVALID;
};
