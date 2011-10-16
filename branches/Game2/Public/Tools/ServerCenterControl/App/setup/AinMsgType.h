

#pragma once

enum
{
	//! 普通消息
	USER_PWD_CMP_RESULT,	//! 比较用户密码结果
	USER_PWD_CHANGE_RESULT,	//! 用户修改密码结果
	WAITE_HARDWARE_FINISH,	//! 写入硬件信息完成
	WAITE_DBLOG_FINISH,		//! 写入日志操作完成
	WAITE_GAME_INFO_FINISH,	//! 写入游戏信息操作完成
	LOAD_GAME_INFO_FINISH,	//! 写入游戏信息操作完成

	RETURN_REPORT_DATA,		//! 返回举报信息的数据指针

	SQL_FINISH,				//! SQL语句执行完成

	//! 延迟消息
	DEFER_,
};