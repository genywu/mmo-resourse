


#pragma once



//! 游戏管理工具权限类型枚举
enum eGMTPurviewType
{
	ePT_Penniless			= 0				,//! 无权限

	ePT_GmNormal			= 0x01			,//! 游戏管理员（处理举报）
	ePT_GmAffiche			= 0x02			,//! 发送游戏公告
	ePT_ViewGmOpt			= 0x04			,//! 查看GM的操作记录

	ePT_Scout1				= 0x0100		,//! 查看信息1
	ePT_Scout2				= 0x0200		,//! 查看信息2

	ePT_ServerControl1		= 0x010000		,//! 控制游戏服务器1
	ePT_ServerControl2		= 0x020000		,//! 控制游戏服务器2

	ePT_Update				= 0x01000000	,//! 文件更新
	ePT_UserManage			= 0x02000000	,//! 用户管理

	ePT_AllPurview			= 0xFFFFFFFF	,//! 所有权限
};


//! 一个用户类型的权限
struct tagGMTPurviewSetup
{
	//!						配置的ID
	DWORD					dwUserType;
	//!						配置的权限
	DWORD					dwPurview;
	//!						用户组名称
	char					szName[MAX_USER_GROUP_NAME_SZIE];
	/////
	tagGMTPurviewSetup(VOID){memset(this, 0, sizeof(tagGMTPurviewSetup));}
};