
#pragma once

#include "GUID.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 成员的队伍状态
enum eMemberState
{
	eMS_ONLINE,		//在线
	eMS_LEAVE,		//离线
};

//! 物品分配方式
enum eGoodsAllot
{
	eGA_FREE,	//任意拾取
	eGA_QUEUE,	//轮流拾取
	eGA_RANDOM, //随机拾取
};

//! 对象数据改变
enum eTeamChange
{
	eTC_NewMaster,		//新队长
	eTC_NewGoodsAllot,	//新的物品分配规则
	eTC_PlayerLogin,	//成员上线
	eTC_PlayerLeave,	//成员下线
};

//! 队伍操作控制
enum eTeamOptControlType
{
	eTOCT_CantJoin		= 0x1, //! 不能加入
	eTOCT_CantExit		= 0x2, //! 不能离开
	eTOCT_CantDisband	= 0x4, //! 不能解散

	eTOCT_CantAll		= 0xFFFFFFFF,//! 全部禁止
};

//! 队员间的最大获益距离
#define	MAX_LUCRATIVE_BOUND		21U

//! 招募文本的最大长度（包括结尾/0字符）
#define	MAX_RECRUIT_TEXT_SIZE	32U
//! 招募密码文本的最大长度（包括结尾/0字符）
#define	MAX_RECRUIT_PWD_SIZE	32U

//! 招募状态
struct tagRecruitState
{
	bool bIsRecruiting;
	char szRecruitName[MAX_RECRUIT_TEXT_SIZE];
	char szPassword[MAX_RECRUIT_PWD_SIZE];
	tagRecruitState()
	{
		bIsRecruiting = false; 
		memset(szRecruitName, 0, MAX_RECRUIT_TEXT_SIZE);
		memset(szPassword, 0, MAX_RECRUIT_PWD_SIZE);
	}
};

//! 成员信息
struct tagWSMemberInfo
{
	CGUID guid;
	long lState;
	long lOccupation;
	long lSex;				//性别
	DWORD dwJoinTime;
	long lCompleteQuest;	//已完成的队伍共享的任务
	char szName[20];
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////


//! 要更新的非组队信息说明
enum eUpdateIdioinfo
{
	eMIS_Pos			,//更新位置

	eMIS_CurrHp			,//更新当前HP
	eMIS_CurrMp			,//更新当前MP
	eMIS_CurrEnergy		,//更新当前CP值(为不引起客户端编译错误，没有修改枚举值名称)

	eMIS_MaxHp			,//更新最大HP
	eMIS_MaxMp			,//更新最大HP
	eMIS_MaxEnergy		,//更新最CP值(为不引起客户端编译错误，没有修改枚举值名称)

	eMIS_Level			,//更新等级
	eMIS_Occupation		,//包含职业
	eMIS_Sex			,//性别

	eMIS_State			,//附加状态
};

//! 非队伍成员信息（用于消息发送）
struct tagMemberIdioinfo
{
	float fPosX;
	float fPosY;
	long lRegionID;			//所在地图ID
	CGUID RegionGuid;		//所在地图GUID

	long lCurrHp;			//当前HP
	long lCurrMp;			//当前MP
	long lCurrEnergy;		//当前能量

	long lMaxHp;			//最大HP
	long lMaxMp;			//最大HP
	long lMaxEnergy;		//最大能量

	long lLevel;			//当前等级
	long lOccupation;		//职业
	tagMemberIdioinfo()
	{
		memset(this,0,sizeof(tagMemberIdioinfo));
	}
};


//! 战队系统错误值
enum eSentaiErrInfo
{
	eSEI_NoTeam,			//! 你还没有组队
	eSEI_NotLeader,			//! 你不是队长
	eSEI_IsIn,				//! 你们已经参加战场排队了
	eSEI_NotJoin,			//! 你们还没有参加战场排队
	eSEI_AllOnline, 		//! 排队前应保证所有队员在线
	eSEI_Entering,			//! 正在进场倒计时，不能做此操作
	eSEI_Battle,			//! 正在战斗中，不能做此操作
	eSEI_MemberNumErr,		//! 成员数不合要求
	eSEI_MemberLvlRrr,		//! 成员等级不合要求
	eSEI_Waiting,			//! 已经开始进场队伍配对，不能做此操作
	eSEI_NotInSameRegion,	//! 队员不在同一张地图
};