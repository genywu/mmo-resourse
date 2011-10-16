
/*
*	file:		LeaveWordDef.h
*	Brief:		留言系统的定义文件
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-03-06
*/




#pragma once

//!			留言长度
const ULONG MAX_LEAVE_WORD_SIZE			= 1024;
//!			发送者名字长度
const ULONG MAX_SENDER_NAME_SIZE		= 20 + 1;

//! 留言者的类型
enum eSenderType
{
	//! 系统
	eSender_System,
	//! 玩家
	eSender_Player,
	//! 已删除角色的玩家
	eSender_DeletedPlayer,
};

//! 留言结果
enum eLeaveWordResult
{
	//! 成功
	eLWR_OK						= 1,
	//! 留言过于频繁，请稍后继续操作
	eLWR_SendMore				= 0,
	//! 你对该玩家留言太多
	eLWR_U2PlayerTooMore		= -1,
	//! 该玩家信箱已买
	eLWR_ThePlayerBoxFull		= -2,
};

//! 留言 数据结构（S2C）
struct  tagLeaveWord
{
	ULONG		uSenderType;						// 留言者的类型（系统、）
	CHAR		szName[ MAX_SENDER_NAME_SIZE ];		// 留言者的名字
	DWORD		dwTime;								// 留言者的时间戳，从1970年后的秒数，转换函数 struct tm *localtime( const time_t *timer );
	tagLeaveWord(void){memset(this, 0, sizeof(tagLeaveWord));}
};


#ifdef USE_IN_SERVER

//! 留言配置结构
struct tagLeaveWordSetup
{
	//!		操作时段内最大发送留言数
	LONG	lMaxLeaveWordNum;
	//!		操作时段(秒)
	LONG	lOptPeriodOfTime;
	//!		最大发送到单一角色留言数
	LONG	lMaxToPlayerLeaveWordNum;
	//!		最大保存留言数
	LONG	lMaxSaveLeaveWordNum;
	//!		最大保存期限（秒）
	LONG	lMaxSaveTime;
};

struct tagLeaveWord_W2DB
{
	CGUID	ReceiverGuid;
	ULONG	uPRI;
	CGUID 	SenderGuid;
	LONG	dwSenderType;
	union
	{
		CHAR	szSenderName[ MAX_SENDER_NAME_SIZE ];//! 用作数据发送的时候保存发送者的名字
		CHAR	szReceiverName[ MAX_SENDER_NAME_SIZE ];//! 用作信息保存的时候保存接收者的名字
	};	
};


#endif //! USE_IN_SERVER