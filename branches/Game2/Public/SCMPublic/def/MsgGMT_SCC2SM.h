



#pragma once

/*
消息申明约定：第四个单词，统一使用：
	Notify（通知）
	Request（请求）
	Re（回复）
*/


//! SCC到SM消息
enum 
{
	//! 
	MSG_SCC2SM_LOGIN	= MSG_SCC2SM + MAX_SUB * 0,
	//!-----------------------------------------------------
	MSG_SCC2SM_LOGIN_Re_Login,			//! 同意登陆


	//! 信息相关
	MSG_SCC2SM_INFO		= MSG_SCC2SM + MAX_SUB * 1,
	//!-----------------------------------------------------
	MSG_SCC2SM_INFO_SUC_Request_GameInfo,	//! 转发SUC请求最新的游戏信息
	

	//! 控制相关
	MSG_SCC2SM_CTRL		= MSG_SCC2SM + MAX_SUB * 2,
	//!-----------------------------------------------------
	MSG_SCC2SM_CTRL_Request_SHUTDOWN,
	MSG_SCC2SM_CTRL_Request_START,
	MSG_SCC2SM_CTRL_Request_SMAction,		//  让SM执行的动作
	//硬件
	MSG_SCC2SM_CTRL_Request_RESTARTCOM,		//! 重启计算机

	MSG_SCC2SM_CTRL_Request_UpdateGapp,		//! 请求更新一个GAPP
	MSG_SCC2SM_CTRL_Request_UpdateTypeGapp,	//! 请求更新一个类型的所有GAPP

	MSG_SCC2SM_CTRL_Re_File,				//! 回复文件
	MSG_SCC2SM_CTRL_Re_ServerPack,			//! 回复一个GAPP的最新文件包

	MSG_SCC2SM_CTRL_Request_GappAction,		//! 控制Gapp行为（向GAPP发送一个行为枚举值）

	//! 管理相关
	MSG_SCC2SM_MANAGE	= MSG_SCC2SM + MAX_SUB * 3,
	//!-----------------------------------------------------
	MSG_SCC2SM_MANAGE_WATCH,				//! 请求监视某个玩家
	MSG_SCC2SM_MANAGE_Report_Result,		//! 举报处理结果
	MSG_SCC2SM_MANAGE_Notify_Affiche,		//! 发送服务器公告
	MSG_SCC2SM_MANAGE_Request_Gm_Command,	//! 转发操作监视玩家的GM命令请求（命令枚举（DWORD）+ 信息字符串（512 STR））

	/////////////////////////////////////////
	MSG_SCC2SM_END = MSG_SCC2SM + MAX_TYPE,
};

//! SM到SCC消息
enum 
{
	//! 登陆相关
	MSG_SM2SCC_LOGIN	= MSG_SM2SCC + MAX_SUB * 0,
	//!-----------------------------------------------------
	MSG_SM2SCC_LOGIN_Request_Login,				//! 登陆请求


	//! 信息相关
	MSG_SM2SCC_INFO		= MSG_SM2SCC + MAX_SUB * 1,
	//!-----------------------------------------------------
	MSG_SM2SCC_INFO_Notify_HardInfo,			//! 一个硬件信息
	MSG_SM2SCC_INFO_Notify_HardInfoList,		//! 一组硬件信息
	MSG_SM2SCC_INFO_Notify_Gapp_BaseInfo,		//! Gapp基本信息
	MSG_SM2SCC_INFO_Notify_Gapp_ChangeRunState,	//! Gapp运行状态改变
	MSG_SM2SCC_INFO_Notify_ServerInfo,			//服务器信息
	MSG_SM2SCC_INFO_Notify_CTRL_GappAction,		//! Gapp行为通知
	MSG_SM2SCC_INFO_Notify_Error,				//错误消息
	MSG_SM2SCC_INFO_Notify_DELAYTIME,	


	//! 控制相关
	MSG_SM2SCC_CTRL		= MSG_SM2SCC + MAX_SUB * 2,
	//!-----------------------------------------------------
	MSG_SM2SCC_CTRL_Notify_SHUTDOWN,
	MSG_SM2SCC_CTRL_Notify_START,
	MSG_SCC2SM_CTRL_Notify_RESTARTCOM,

	MSG_SM2SCC_CTRL_Notify_UpdateGapp,			//! 回复更新Gapp结果

	MSG_SM2SCC_CTRL_Request_File,				//! 请求文件
	MSG_SM2SCC_CTRL_Request_ServerPack,			//! 请求一个GAPP的最新文件包
	MSG_SM2SCC_CTRL_Notify_ServerPack,			//  获取原始包结果
	MSG_SM2SCC_CTRL_Notify_ReloadFile,			//  重读资源结果

	MSG_SM2SCC_CTRL_Notify_SMAction,			//  SM执行动作返回结果
	//! 管理相关
	MSG_SM2SCC_MANAGE	= MSG_SM2SCC + MAX_SUB * 3,
	//!-----------------------------------------------------
	MSG_SM2SCC_MANAGE_Notify_Report,				//举报信息
	MSG_SM2SCC_MANAGE_WATCH_Reply,				//监视请求应答
	MSG_SM2SCC_MANAGE_Report_Result_Replay,		//返回封号等处理是否成功
	MSG_SM2SCC_MANAGE_Notify_Gm_Command_Info,	//发送服务器到GM类SUC的命令信息（命令枚举（DWORD）+ 信息字符串（512 STR））
	/////////////////////////////////////////
	MSG_SM2SCC_END = MSG_SM2SCC + MAX_TYPE, 
};
