


#pragma once
//! 信息用到的公共定义
////////////////////////////////////////////////////////////////////////////////////////////////////

//! 通过SCC发送到SM的相关配置
struct  tagScc2SmSetup
{
	DWORD dwGetHardWareInfoTime;
	DWORD dwSaveHardWareInfoSize;

	DWORD dwStartTimeOut;
	DWORD dwShutdownTimeOut;
	DWORD dwPingTimeOut;
	DWORD dwDelayTestTime;

	tagScc2SmSetup(VOID){memset(this, 0, sizeof(tagScc2SmSetup));}
};

//! SM基本信息
//!----------------------------------------------------------------------------------
struct tagSmBaseInfo
{
	//!				信息产生的时间
	DWORD			dwTime;
	//!				ID
	DWORD			dwID;
	//!				名称
	char			szName[MAX_SM_NAME_SZIE];
	//!				详细说明
	char			szExplain[BASE_TEXT_SIZE];
	//!				当前状态
	DWORD			dwState;

	tagSmBaseInfo(VOID){memset(this, 0, sizeof(tagSmBaseInfo));}
};

//! 加上区组信息的SM
struct tagSmInfo : public tagSmBaseInfo
{
	//!				大区ID
	DWORD			dwRegionID;
	//!				大区名字
	char			szRegionName[MAX_SM_NAME_SZIE];
	//!				服务器组ID
	DWORD			dwGroupID;
	//!				服务器组名字
	char			szGroupName[MAX_SM_NAME_SZIE];

	tagSmInfo(VOID){memset(this, 0, sizeof(tagSmInfo));}
};


//! GAPP基本信息
//!----------------------------------------------------------------------------------

//! GAPP状态
enum eServerState
{
	eSS_Stop		= 0,
	eSS_Running,
	eSS_Busy,
};

//! 结构体
struct tagGappBaseInfo
{
	//!				信息产生的时间
	DWORD			_dwGappTime;
	//!				ID
	DWORD			_dwGappID;
	//!				类型
	DWORD			_dwGappType;
	//!				当前版本
	char			_szGappVersion[MAX_GAPP_NAME_SZIE];
	//!				显示名
	char			_szGappName[MAX_GAPP_NAME_SZIE];
	//!				当前状态
	DWORD			_dwGappState;
	//!				当前延迟
	DWORD			_dwGappDelayTime;
	tagGappBaseInfo(VOID){memset(this, 0, sizeof(tagGappBaseInfo));}
};


//! 硬件信息
//!----------------------------------------------------------------------------------

//!				一个消息最多包含的硬件信息数
const DWORD		MAX_MESSAGE_INCLUDE_HARD_INFO			= HARD_INFO_GROUP_NUM * HARD_INFO_CACHE_GROUP_NUM;


struct tagCpuUsage
{
	UCHAR cUsage;//总的CPU使用率
	UCHAR cSubUsage[MAX_CPU_NUM];//单个CPU使用率
	tagCpuUsage(){memset(this,0,sizeof(tagCpuUsage));}
};

struct tagHardwareInfo:public tagCpuUsage
{
	//!				信息产生的时间
	DWORD			dwTime;
	//!				当前内存(物理和虚拟)
	MEMORYSTATUS	stMS;
	tagHardwareInfo(VOID){memset(this, 0, sizeof(tagHardwareInfo));}
}; 


//! 游戏数据库信息
struct tagGameDbInfo
{
	//!				显示名称
	char			_szShowName[BASE_NAME_SIZE];
	//!				管理者ID
	DWORD			_dwSMID;
	//!				自身ID
	DWORD			_dwID;
};

//SM要执行的动作
enum SMActionType
{
	SETPORT=0,		//修改端口
};


//! GM功能
//!----------------------------------------------------------------------------------

enum	{	eUsedSys = 1,eUsedGM=2,};



