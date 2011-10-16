


#pragma once

/*
消息申明约定：第四个单词，统一使用：
	Notify（通知）
	Request（请求）
	Re（回复）
*/

//! SCC到SUC消息
enum 
{
	//! 登陆相关
	MSG_SCC2SUC_LOGIN = MSG_SCC2SUC + MAX_SUB * 0,
	//!-----------------------------------------------------
	MSG_SCC2SUC_LOGIN_Re_Login,						//! 响应登录
	MSG_SCC2SUC_LOGIN_Re_ChangePwd,					//! 修改密码回复


	//! 信息相关
	MSG_SCC2SUC_INFO	= MSG_SCC2SUC + MAX_SUB * 1,
	//!-----------------------------------------------------
	MSG_SCC2SUC_INFO_Notify_Sm_BaseInfo,			//! 一个SM的基本信息
	MSG_SCC2SUC_INFO_Notify_Sm_Close,				//! 一个SM关闭

	MSG_SCC2SUC_INFO_Notify_AddHardInfo,			//! 添加一个硬件信息
	MSG_SCC2SUC_INFO_Notify_HardInfoList,			//! 发送一个SM的一组硬件信息

	MSG_SCC2SUC_INFO_Notify_Gapp_BaseInfo,			//! Gapp基本信息
	MSG_SCC2SUC_INFO_Notify_Gapp_ChangeVersion,		//! Gapp的版本改变
	MSG_SCC2SUC_INFO_Notify_Gapp_ChangeRunState,	//! Gapp的运行状态改变
	MSG_SCC2SUC_INFO_Notify_Gapp_DELAYTIME,			//! 通知SM ---->GAPP Echo消耗时间

	MSG_SCC2SUC_INFO_Notify_PackList,				//! 发送当前服务器的更新包列表
	MSG_SCC2SUC_INFO_Notify_UpLoadPack,				//! 上传更新包通知
	MSG_SCC2SUC_INFO_Notify_Error,					//! 一个错误通知

	MSG_SCC2SUC_INFO_Re_Show,						//! 回复SUC查找的数据

	MSG_SCC2SUC_INFO_Re_UpdateFile,					//! 回复更新文件列表
	MSG_SCC2SUC_INFO_Notify_CTRL_GappAction,		//! Gapp行为通知

	//! 控制相关
	MSG_SCC2SUC_CTRL	= MSG_SCC2SUC + MAX_SUB * 2,
	//!-----------------------------------------------------
	MSG_SCC2SUC_CTRL_Re_InCtrlRoom,					//! 进入控制空间回复 
	MSG_SCC2SUC_CTRL_Re_SHUTDOWN,					//! 关闭GAPP
	MSG_SCC2SUC_CTRL_Re_START,						//! 启动GAPP
	MSG_SCC2SUC_CTRL_Re_RESTART,					//! 重启GAPP

	MSG_SCC2SUC_CTRL_Re_UpLoadPack,					//! 一个更新包上传结果
	MSG_SCC2SUC_CTRL_Re_UpdateGapp,					//! 回复更新一个GAPP
	MSG_SCC2SUC_CTRL_Re_ReLoadGapp,					//! 回复重读Gapp原始文件包
	MSG_SCC2SUC_CTRL_Re_UploadFile,					//! 回复上传文件结果

	MSG_SCC2SUC_CTRL_Re_GappAction,					//! 回复控制Gapp行为（向GAPP发送一个行为枚举值）

	MSG_SCC2SUC_CTRL_Re_SMAction,					//	回复一个SM控制行为
	
	//! 管理相关
	MSG_SCC2SUC_MANAGE	= MSG_SCC2SUC + MAX_SUB * 3,
	//!-----------------------------------------------------
	MSG_SCC2SUC_MANAGE_Notify_GmToWs,				//! Gm进入或退出Ws的通知
	MSG_SCC2SUC_MANAGE_Notify_Watch,				//! GM监视玩家状态通知
	MSG_SCC2SUC_MANAGE_Notify_ReportList,			//! 举报列表通知
	MSG_SCC2SUC_MANAGE_Notify_ResultList,			//! 处理结果列表通知
	MSG_SCC2SUC_MANAGE_Notify_Gm_Command_Info,		//! 发送服务器到GM类SUC的命令信息（命令枚举（DWORD）+ 信息字符串（512 STR））

	//! 杂项
	MSG_SCC2SUC_OTHER	= MSG_SCC2SUC + MAX_SUB * 4,
	//!-----------------------------------------------------
	MSG_SCC2SUC_OTHER_Notify_SendText,				//! 发送一个文本信息
	MSG_SCC2SUC_OTHER_Notify_NotReady,				//! SCC没有准备好，不接受SUC消息


	/////////////////////////////////////////
	MSG_SCC2SUC_END = MSG_SCC2SUC + MAX_TYPE,
};


//! SUC到SCC消息
enum 
{
	//! 登陆相关
	MSG_SUC2SCC_LOGIN	= MSG_SUC2SCC + MAX_SUB * 0,
	//!-----------------------------------------------------
	MSG_SUC2SCC_LOGIN_Request_Login,				//! 请求登录
	MSG_SUC2SCC_LOGIN_Request_ChangePwdQuest,		//! 请求修改密码

	//! 信息相关
	MSG_SUC2SCC_INFO	= MSG_SUC2SCC + MAX_SUB * 1,
	//!-----------------------------------------------------
	MSG_SUC2SCC_INFO_Request_Ctrl,					//! 信息控制	

	MSG_SUC2SCC_INFO_Request_Show,					//! 请求一个Gapp的某类型信息（在一段时间）
	MSG_SUC2SCC_INFO_Request_New,					//! 请求一个Gapp的某类型最新信息

	MSG_SUC2SCC_INFO_Request_UpdateFile,			//! 获取更新文件列表

	//! 控制相关
	MSG_SUC2SCC_CTRL	= MSG_SUC2SCC + MAX_SUB * 2,
	//!-----------------------------------------------------
	MSG_SUC2SCC_CTRL_Request_InCtrlRoom,			//! 进入、退出控制空间
	MSG_SUC2SCC_CTRL_Request_SHUTDOWN,				//! 关闭GAPP
	MSG_SUC2SCC_CTRL_Request_START,					//! 启动GAPP
	MSG_SUC2SCC_CTRL_Request_RESTART,				//! 重启GAPP

	MSG_SUC2SCC_CTRL_Request_UpLoadPack,			//! 上传一个更新包
	MSG_SUC2SCC_CTRL_Request_UpdateGapp,			//! 更新一个GAPP
	MSG_SUC2SCC_CTRL_Request_ReLoadGapp,			//! 重读Gapp原始文件包

	MSG_SUC2SCC_CTRL_Request_UploadFile,			//	上传一个文件

	MSG_SUC2SCC_CTRL_Request_GappAction,			//! 控制Gapp行为（向GAPP发送一个行为枚举值）

	MSG_SUC2SCC_CTRL_Request_SMAction,				//	SM行为
	

	//! 管理相关
	MSG_SUC2SCC_MANAGE	= MSG_SUC2SCC + MAX_SUB * 3,
	//!-----------------------------------------------------
	MSG_SUC2SCC_MANAGE_Request_GmToWs,				//! Gm进入或退出Ws的请求
	MSG_SUC2SCC_MANAGE_Request_VeiwReport,			//! 查看举报请求
	MSG_SUC2SCC_MANAGE_Request_VeiwResult,			//! 查看处理结果请求
	MSG_SUC2SCC_MANAGE_Request_ScoutPlayer,			//! 监视玩家行为请求
	MSG_SUC2SCC_MANAGE_Request_OptPlayer,			//! 处理玩家请求
	MSG_SUC2SCC_MANAGE_Request_SendAffiche,			//! 发送公告请求
	MSG_SUC2SCC_MANAGE_Request_Gm_Command,			//! 发送操作监视玩家的GM命令请求（命令枚举（DWORD）+ 信息字符串（512 STR））


	/////////////////////////////////////////
	MSG_SUC2SCC_END = MSG_SUC2SCC + MAX_TYPE,
};