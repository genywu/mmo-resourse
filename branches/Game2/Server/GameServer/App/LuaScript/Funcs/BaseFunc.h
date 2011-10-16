///
/// @file BaseFunc.h
/// @brief 全局基础支持函数
/// @see BaseFunc.cpp
///
#ifndef ___BASE_FUNC_H_
#define ___BASE_FUNC_H_

struct lua_State;
namespace ScriptFuncs
{
	void OpenBaseLibs( lua_State *L );

	// registered by tolua++ below
	//tolua_begin
	///
	/// 构造一个颜色值，可用于AddInfo之类的函数参数
	/// 
	/// @param a Alpha分量
	/// @param r Red分量
	/// @param g Green分量
	/// @param b Blue分量
	/// @return 颜色值
	ulong ARGB( uchar a, uchar r, uchar g, uchar b );

	///
	/// 注册一个脚本定时器，定时器触发时执行一个脚本
	///
	/// 定时器触发执行的脚本实例里源/目标对象都为空
	/// @param file 脚本文件名
	/// @param userData 用户标识符，用于注销定时器
	/// @param trigerTime 第一次触发时间
	/// @param interTime 每次触发的间隔时间，为0时则只触发一次
	/// @return 定时器ID
	/// @see UnRegisterTime
	long RegisterTimer( const char *file, long userData, long trigerTime, long interTime );

	///
	/// 注销一个脚本定时器
	///
	/// @param timeID 定时器ID，由RegisterTime返回
	/// @param userData 用户标识符
	/// @see RegisterTimer
	void UnRegisterTimer( long timerID, long userData );
	//tolua_end

}

#endif
