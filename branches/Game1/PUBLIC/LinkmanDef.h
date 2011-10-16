

/*
*	file:		LinkmanDef.h
*	Brief:		关于好友的一些C/S端共用的定义
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-18
*/

#pragma once

#include "GUID.h"

//! 常量、数值宏
//!-------------------------------------------------------------------------------

//!			联系人组称呼最大长度
const ULONG MAX_APPELLATIVE_SIZE		= 16 + 1;
//!			最大文件路径长度
const ULONG MAX_FILE_PATH_SIZE			= 256;

//!			玩家名字长度
const ULONG MAX_PLAYER_NAME_SIZE		= 20 + 1;
//!			地图名字长度
const ULONG MAX_PLAYER_TITLE_SIZE		= 256 + 1;



//!			玩家联系人操作的最大会话时间（秒）
const ULONG	MAX_LINKMAN_OPT_TIME_SEC	= 60;
//!			无效的联系组ID
const ULONG INVALID_GROUP_ID			= 0xFFFFFFFF;


//! 枚举
//!-------------------------------------------------------------------------------
//!		联系组类型
enum eLinkmanGroupType
{
	//! 好友组
	eLGT_Friend,
	//! 配偶组
	eLGT_Spouse,
	//! 陌生人组
	eLGT_Stranger,
	//! 恶人组
	eLGT_Hellion,
	//! 黑名单
	eLGT_Blacklist,

	//! 最小无效值
	eLGT_Invalid,
};


//! 留言优先级
enum eLeaveWordPRI
{
	//! 不能留言
	eLWPRI_Cant				= 0,

	//! 最稳定的
	eLWPRI_Steadyest		= 1,
	//! 最不稳定的
	eLWPRI_UnSteadyest		= 999,

	
};



//! 聊天频道相关
enum eSecretTalkChannel
{
	//! 频道基本值
	eSTC_BaseValue			= 0xFF00,
	//! 频道最大值
	eSTC_MaxValue			= 0xFFFF,
	//! 最小无效值(无频道)
	eSTC_Invalid			= 0,
};


//! 存储方式
enum eKeepWay
{
	//! 服务器保存
	eKW_ServerKeep,
	//! 客户端保存
	eKW_ClientKeep,
	//! 不保存
	eKW_NotKeep,
};

//! 附加显示信息(按位表示)
enum eTimelyInfoType
{
	//! 显示玩家所在地图
	eTIT_Show_MapName		= 1 << 0,
	//! 显示等级
	eTIT_Show_Level			= 1 << 1,
	//! 显示职业
	eTIT_Show_Occupation	= 1 << 2,

	//! 不显示所有信息
	eTIT_Hide_All			= 0,
	//! 显示所有信息
	eTIT_Show_All			= 0xFFFFFFFF,
};


//! 屏蔽功能（按位表示）
enum eFunctionFiltrateType
{
	//! 屏蔽聊天内容
	eFFT_Filtrate_Chat		= 1 << 0,
	//! 屏蔽组队邀请
	eFFT_Filtrate_Team		= 1 << 1,

	//! 不屏蔽所有
	eFFT_UuFiltrate_All		= 0,
	//! 屏蔽所有
	eFFT_Filtrate_All		= 0xFFFFFFFF,
};

//! 自动添加办法
enum eAutoAddType
{
	//! 不自动添加
	eAAT_NotAdd,
	//! 添加密聊过的人
	eAAT_SecretTalk,
	//! 添加攻击者
	eAAT_Attacker,
	//! 配偶
	eAAT_Spouse,
};

//! 联系组添加删除的操作者
enum eGroupOperatorType
{
	eGOT_Client,
	eGOT_Server,
};


//! 成员移动优先级
enum eOnlyPRT
{
	//! 最稳定的
	eOPRI_Steadyest			= 1,
	//! 最不稳定的
	eOPRI_UnSteadyest		= 999,
};

//! 联系人状态
enum eLinmanStateType
{
	//! 未知状态
	eLST_Unkown,
	//! 删除状态
	eLST_Deleted,
	//! 离线状态
	eLST_Offline,
	//! 在线状态
	eLST_Online,
};



//! 联系人信息枚举
enum eLinmanInfoType
{
	//! 地图
	eLIT_Map,
	//! 等级
	eLIT_Level,
	//! 职业
	eLIT_Occupation,
};



//! 联系人操作类型枚举
enum eLinmanHandleType
{
	//! 添加
	eLHT_Add,
	//! 删除
	eLHT_Del,
};



//! 好友的删除状态
enum eLinmanDelState
{
	//! 未删除
	eLDS_NotDel,
	//! 被标记为删除
	eLDS_FlagDel,
	//! DB数据已真实删除
	eLDS_DataDel,
};



//! 客户端操作错误原因
enum eOptErrorType
{
	//! 操作目标不在本服务器
	eOET_AimNotBeing,
	//! 多次向同一个玩家发送好友相关的请求
	eOET_OverfullToOne,
	//! 对方拒绝添加
	eOET_AimReject,
	//! 对方列表已满
	eOET_ListFull,
	//! 对方在你的其他组中
	eOET_AimInOther,
	//! 对方隐匿
	eOET_AimHideName,
};




//! 结构
//!-------------------------------------------------------------------------------

//! A
struct tagLinkmanBase_data
{
	CGUID		guid;								// GUID
	CHAR		szName[ MAX_PLAYER_NAME_SIZE ];		// 名字
	ULONG		uState;								// 状态（eLinmanStateType）
};

//! A\B
//! 用于保存的联系人数据结构
struct tagLinkman_data : public tagLinkmanBase_data
{
	CHAR		szMapName[ MAX_PLAYER_TITLE_SIZE ];	// 所在地图名字
	LONG		lLevel;								// 等级
	LONG		lOccupation;						// 职业
	
	tagLinkman_data(void){memset(this, 0, sizeof(tagLinkman_data));}
};

//! A\C
//! 用于消息的联系人数据结构
struct tagLinkmanBase : public tagLinkmanBase_data
{
	BOOL		bMutual;							//! 互加关系表示
};

//! A\B\C
//! 用于消息的联系人数据结构
struct tagLinkman : public tagLinkman_data
{
	tagLinkman(void){memset(this, 0, sizeof(tagLinkman));}
	BOOL		bMutual;							//! 互加关系表示
};





//! DB联系数据结构
struct  tagLinkman_DB
{
	tagLinkman_DB(void){memset(this, 0, sizeof(tagLinkman_DB));}
	CGUID		OwnerGUID;								//! 玩家GUID
	CGUID		AimGUID;								//! 被添加人GUID
	ULONG		uGroupID;								//! 联系组配置ID
	LONG		uDelState;								//! 删除状态（eLinmanDelState）
	char		szAimName[MAX_PLAYER_NAME_SIZE];		//! 被添加人名称

	inline bool operator ==(const tagLinkman_DB& right) const
	{
		return (OwnerGUID == right.OwnerGUID && AimGUID == right.AimGUID && uGroupID == right.uGroupID);
	}
	inline bool operator <(const tagLinkman_DB& right) const
	{
		return OwnerGUID < right.OwnerGUID;
	}
};



struct tagMeAddPlayer
{
	BOOL			_IsMutual;	//! 相互关系
	tagLinkman_data	*_date;		//! 数据
};
