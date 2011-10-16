#include "Date.h"
#include "GUID.h"

#pragma once

#pragma pack(push, 4)

//!				玩家名字长度
const	DWORD 	MAX_MEMBER_NAME_SIZE	= 21U;


//! class COrganizing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//!				最大留言条数
const	DWORD	MAX_LeaveWordNum			= 256;
//!				每条留言的最大字符数量
const	DWORD	MAX_PerWordCharNum			= 256;
//!				宣言的最大字符数量
const	DWORD	MAX_PronounceCharNum		= 2048;

//!				工会福利：增加经验的间隔时间
const	DWORD	ADD_EXP_SPACE_TIME			= 60 * 1000;

enum eOperator
{
	OP_Delete						=0,										//删除
	OP_Add,																	//添加
	OP_Update,																//更新
};

//留言板的每条留言结构236
struct tagOrgLeaveWord
{
	CGUID							Guid;									//GUID
	CGUID							MemberGuid;								//留言的玩家
	char							szName[MAX_MEMBER_NAME_SIZE];			//留言人的名字
	LONG							Time;									//留言的时间
	char							szBody[MAX_PerWordCharNum];				//留言的内容

	tagOrgLeaveWord(void){memset(this, 0, sizeof(tagOrgLeaveWord));}
};

//宣言结构2084
struct tagOrgPronounceWord
{
	CGUID							MemberGuid;								//宣言的玩家
	char							szName[MAX_MEMBER_NAME_SIZE];			//宣言人的名字
	LONG							Time;									//宣言的时间
	char							szBody[MAX_PronounceCharNum];			//宣言的内容

	tagOrgPronounceWord(void){memset(this, 0, sizeof(tagOrgPronounceWord));}
};


//! class CFaction
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!				工会名称长度
const	LONG	MAXSIZE_FactionName			= 17;		
		
//!				最大的申请人员列表数量
const	LONG	MAX_ApplyPersonNum			= 256;
//!				最大会徽数据字节数(32k)
const	LONG	MAX_ICON_SIZE				= 1024 * 32;

//!				最大官阶数量
const	LONG	MAXNUM_JobLvl				= 5;
//!				特殊官阶（表示会长）
const	LONG	Exceptive_JobLvl			= -1;




//! 帮会升级条件类型
enum eUpgradeConditionType
{
	eUCT_Money						= 1,									//! 金钱
	eUCT_LeaderLevel,														//! 领导者等级
	eUCT_FacLevel,															//! 工会自身等级
	eUCT_FacRes,															//! 工会资源需求
	eUCT_FacExp,															//! 工会经验需求
	eUCT_Goods,																//! 物品
	eUCT_BaseNum,															//! 占领的城镇
};

//! 升级类型
enum eUpgradeType
{
	eUT_FactionLevel,														//! 工会等级
	eUT_ExpLevel,															//! 经验福利等级
	eUT_SpecialityLevel,													//! 专业等级
	eUT_BussinessLevel,														//! 商业等级
	eUT_BattleLevel,														//! 战争等级
	eUT_NobleLevel,															//! 贵族等级
};

//! 升级条件
struct tagUpgradeCondition
{
	LONG 							lConditionType;							//! 条件类型eUpgradeConditionType
	LONG 							lValue;									//! 条件值
	tagUpgradeCondition(void){memset(this, 0, sizeof(tagUpgradeCondition));}
};

//! 帮会等级相关配置结构
struct tagFacLevelSetup
{
	LONG							lLevel;									//! 等级
	LONG							lMaxMemNum;								//! 成员最大数
	LONG							lDisbandMemNum;							//! 解散人数
	
	vector<tagUpgradeCondition>		vCondition;								//! 到达条件集
};

//! 帮会经验等级
struct tagFacExpLevelSetup
{
	LONG							lLevel;									//! 等级
	float							fBaseExp;								//! 基本奖励经验

	vector<tagUpgradeCondition>		vCondition;								//! 到达条件集
};

//! 帮会经验等级
struct tagFacDepotLevelSetup
{
	LONG							lLevel;									//! 等级
	LONG							lCellNum;								//! 仓库的容量

	vector<tagUpgradeCondition>		vCondition;								//! 到达条件集
};

//! 通用附加等级设置（除了等级和条件之外没有其他数据）
struct tagFacSubLevelSetup
{
	LONG							lLevel;									//! 等级

	vector<tagUpgradeCondition>		vCondition;								//! 到达条件集
};

//! 与自定义等级相关的设置
struct tagFacSetWithLevel
{
	LONG							lLevel;									//! 等级大小
	LONG							lExpPerMinute;							//! 该等级下成员每分钟获得的经验
	tagFacSetWithLevel(void){memset(this, 0, sizeof(tagFacSetWithLevel));}
};

//! 条件集
typedef	vector<tagUpgradeCondition> VecCondition;


//! 工会全局配置
struct tagGlobeSetup
{
	LONG							lDemiseTime;							//! 会长不上线的最长时间，超过则自动转让会长
	LONG							lDisbandFactionTime;					//! 解散小于固定人数的帮会的延迟时间
	LONG							lNegativeTime;							//!	资源为负的时间
	LONG							lBaseBattleStart;						//! 据点战开始时间,小时
	LONG							lBaseBattleLast;						//!	据点战持续时间，小时
	BYTE							byClearBaseTime;						//!	清空据点的时间(星期)
	tagGlobeSetup(void){memset(this, 0, sizeof(tagGlobeSetup));}
};

//! 帮会权限的顺序
enum eFactionPurviewIndex
{
	//! 前面是与会员权限相同的部分
	eFPI_SetAffiche,														//! 设置公告
	eFPI_Authorize,															//! 批准新会员
	eFPI_Fire,																//! 开除成员
	eFPI_TakeOut,															//! 操作仓库
	eFPI_OpenQuest,															//! 开启工会任务
	eFPI_Invite,															//! 邀请新会员
	eFPI_EditTitle,															//! 设置称号
	eFPI_EditIcon,															//! 上传徽章
	eFPI_BeachheadWar,														//! 据点战报名
	eFPI_ResWar,															//! 发动资源战争

	//! 后面是独有的
	eFPI_Disband,															//! 解散行会
	eFPI_Demise,															//! 会长职位转让
	eFPI_SetMemberPurview,													//! 设置会员权限
	
	MAXNUM_FactionPurview,													//! 帮会权限总数
};

//!				会员权限总数
const	LONG	MAXNUM_MemberPurview		= eFPI_Disband;

//! 成员的所有权限,PV: purview
enum eMemberPurview
{
	eMP_Null						= 0,									//! 没有权限

	eMP_SetAffiche					= 1 << 0,								//! 设置公告
	eMP_Authorize					= 1 << 1,								//! 批准新会员
	eMP_Fire						= 1 << 2,								//! 开除成员
	eMP_TakeOut						= 1 << 3,								//! 仓库取物
	eMP_OpenQuest					= 1 << 4,								//! 开启工会任务
	eMP_Invite						= 1 << 5,								//! 邀请新会员
	eMP_EditTitle					= 1 << 6,								//! 设置称号
	eMP_EditIcon					= 1 << 7,								//! 上传徽章
	eMP_BeachheadWar				= 1 << 8,								//! 据点战报名
	eMP_ResWar						= 1 << 9,								//! 发动资源战争

	//! 成员最高权限
	eMP_MemberTiptop				= 0x0FFFFFFF,
	//! 全部权限（master专用）
	eMP_All							= 0xFFFFFFFF,
};

//! 基本信息数据
struct tagFacPublicData
{
	CGUID							guid;									//! 帮会ID
	char							szName[MAXSIZE_FactionName];			//! 帮会名称
	LONG							lCurrSkillID;							//! 当前使用的技能ID
	LONG							lLevel;									//! 帮会等级

	tagFacPublicData(void){memset(this, 0, sizeof(tagFacPublicData));}
};

//! 申请加入时，能看到的信息
struct tagApplyInfo
{
	CGUID							guid;									//! 帮会ID
	char							szName[MAXSIZE_FactionName];			//! 帮会名称
	LONG							lLevel;									//! 帮会等级
	LONG							lMemberNum;								//! 成员数量
	LONG							lMaxMemberNum;							//! 最大成员数量
	BOOL							bIsRecruiting;							//! 是否正在招募
	LONG							lCurrRes;								//! 当前拥有资源
	

	tagApplyInfo(void){memset(this, 0, sizeof(tagApplyInfo));}
};

//! 基本信息数据
struct tagFacBaseData
{
	LONG							lCountryID;								//! 所属国家ID

	CGUID							MasterGuid;								//! 帮主ID
	char							szMasterName[MAX_MEMBER_NAME_SIZE];		//! 帮主名字

	BOOL							bOpenResWar;							//! 是否参与资源战争的开关
	LONG							lCurrRes;								//! 当前拥有资源
	LONG							lCurrExp;								//! 当前的工会经验

	LONG							lSubExpLevel;							//! 附加经验等级(家族福利)
	LONG							lSpecialityLevel;						//! 工会仓库等级(改为家族专业)
	LONG							lBattleLevel;							//! 锻造等级(改为家族战争）
	LONG							lBussinessLevel;						//! 炼金等级(改为家族商业)
	LONG							lNoblelevel;							//! 缝纫等级(改为家族显贵)

	LONG							CreateTime;								//! 创建时间
	CGUID							SuperiorGuid;							//! 上级的ID
	char							arr_szJobName[MAXNUM_JobLvl][MAX_MEMBER_NAME_SIZE];		//! 所有官阶的权限
	LONG							arrJobPurview[MAXNUM_JobLvl];			//! 所有官阶的权限
	LONG							lMemberNum;								//! 成员数量（服务器不使用该值，在向客户端发送时赋值）

	BOOL							bIsRecruiting;							//! 是否正在招募
	LONG							lFacBattle;								//! 战功总值
	LONG							lFacArmyInvest;							//! 军事投资总值




	tagFacBaseData(void){memset(this, 0, sizeof(tagFacBaseData));bIsRecruiting = TRUE;}
};


//! 在组织中每个成员的信息
struct  tagFacMemInfo
{
	CGUID							MemberGuid;								//! 成员ID
	char							szName[MAX_MEMBER_NAME_SIZE];			//! 成员名字
	char							szTitle[MAX_MEMBER_NAME_SIZE];			//! 称号名字
	LONG							lLvl;									//! 成员等级
	LONG							lOccu;									//! 成员职业
	LONG							lJobLvl;								//! 该成员的职级
	LONG 							LastOnlineTime;							//! 最后一次在线时间
	LONG							lContribute;							//! 贡献值
	BOOL							bIsOnline;								//! 是否在线
	char							szRegion[MAX_MEMBER_NAME_SIZE];			//! 所在地图
	BYTE							cHide;									//! 是否为隐匿者
	LONG							lBattle;								//! 战功值
	LONG							lArmyInvest;							//! 军事投资值
	BOOL							bJoinBattle;							//! 领取据点战门票的权力
	tagFacMemInfo(void){memset(this, 0, sizeof(tagFacMemInfo));}
};


//! 申请人员结构
struct tagFacApplyPerson
{
	CGUID							PlayerGuid;
	char							szName[MAX_MEMBER_NAME_SIZE];
	long							lOccu;
	long							lLvl;
	long							lTime;

	tagFacApplyPerson(void){memset(this, 0, sizeof(tagFacApplyPerson));}
};


//! 工会数据变化标记（联盟也使用）
enum eFacDataChangeFlag
{
	eFDCF_Not						= 0,									//! 没有改变

	eFDCF_BaseInfo					= 1 << 0,								//! 基本数据改变
	eFDCF_Member					= 1 << 1,								//! 成员改变
	eFDCF_Apply						= 1 << 2,								//! 申请人员改变
	eFDCF_LeaveWord					= 1 << 3,								//! 留言改变

	eFDCF_All						= 0xFFFFFFFF,							//! 全部改变
};

//! 操作成功
const LONG PLAYER_OPT_SUCCEED	= 1;
//! 操作失败
const LONG PLAYER_OPT_FAILED	= 0;

//! 行会操作错误信息
enum eFactionOptErrInfo
{
	//! --------服务器使用的值--------
	eFOEI_NotErr,

	//! 国家错误
	eFOEI_Err_Country,
	//! 已有行会
	eFOEI_Err_HadFaction,

	//! 行会名称重复（收到这个错误可以继续发送新的名字，会话并未中断）
	eFOEI_Err_ReName,
	//! 名称中含有敏感字符
	eFOEI_Err_SensitiveWord,
	//! 玩家等级错误
	eFOEI_Err_Level,
	//! 金钱错误
	eFOEI_Err_Money,
	//! 宝石错误
	eFOEI_Err_Gem,
	//! 物品错误
	eFOEI_Err_Goods,
	//! 组织资源不够
	eFOEI_FacRes_NotEnough,
	//! 组织经验不够
	eFOEI_FacExp_NotEnough,
	//! 配置获取还未完成
	eFOEI_Err_SetupLoading,

	//! 核心服务器错误，创建失败
	eFOEI_CoreServerError,

	//! 帮会等级不够
	eFOEI_Err_Level_Noenough,
	//! 你的权限不够
	eFOEI_Err_Purview_Noenough,
	//! 不能操作有同样权限的成员
	eFOEI_Err_SamePurview,
	//! 目标不在线
	eFOEI_Err_AimAbsent,
	//! 目标已不存在
	eFOEI_Err_AimNoBeing,
	//! 目标已有行会
	eFOEI_Err_AimHasFac,
	//! 工会已达到人数上限
	eFOEI_Err_MemberFull,
	//! 目标拒绝
	eFOEI_Err_AimRefuse,
	//! 不能重复申请加入，请先取消之前的申请后在进行操作
	eFOEI_Err_ReJoin,
	//! 操作时限已到，请重新点击NPC
	eFOEI_Err_SessionLost,

	//! 没有加入行会
	eFOEI_Err_NotInFaction,
	//! 原设置与新设置相同
	eFOEI_Err_SameValue,
	//! 目标等级不够
	eFOEI_Err_AimLevelTooLow,

	//! 已有联盟
	eFOEI_Err_HadUnion,
	//! 对方已有联盟
	eFOEI_Err_AimHadUnion,
	//! 频繁发送邀请
	eFOEI_Err_ContinualInvite,


	//! 邀请者已经下线
	eFOEI_Err_InviterOff,

	//! 对方是隐匿者
	eFOET_Err_AimHideName,

	//! 占领城镇条件不符
	eFOET_Err_BaseNum,

	//! 未知错误，放于最后
	eFOEI_Err_Unknown,
};


//! class CUnion
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! 成员的所有权限,PV: purview
enum eUnionMemberPurview
{
	//! 没有权限
	eUMP_Null						= 0,

	//! 设置公告
	eUMP_SetAffiche					= 1 << 0,
	//! 开除成员
	eUMP_Fire						= 1 << 2,
	//! 开启任务
	eUMP_OpenQuest					= 1 << 4,
	//! 开除成员
	eUMP_Invite						= 1 << 5,
	//! 设置称号
	eUMP_EditTitle					= 1 << 6,

	//! 成员最高权限
	eUMP_MemberTiptop				= 0x0FFFFFFF,
	//! 全部权限（master专用）
	eUMP_All						= 0xFFFFFFFF,							
};



//! 联盟基本资料
struct  tagUnionBaseData
{
	CGUID							guid;									//! ID
	char							szName[MAXSIZE_FactionName];			//! 名称
	LONG							lCountryID;								//! 所属国家ID
	CGUID							MasterGuid;								//! 盟主ID（盟主是Player）
	CGUID							MasterFacGuid;							//! 盟主所在工会
	char							szMasterName[MAX_MEMBER_NAME_SIZE];		//! 盟主名字	
	LONG							CreateTime;								//! 创建时间

	tagUnionBaseData(void){memset(this, 0, sizeof(tagUnionBaseData));}
};


//! 联盟中每个成员的信息
struct  tagUnionMemInfo
{
	CGUID							MemberGuid;								//! 成员ID（成员是Faction）
	char							szName[MAX_MEMBER_NAME_SIZE];			//! 成员名字
	LONG							lJoinDate;								//! 加入时间
	LONG							lPurview;								//! 成员权限
	

	tagUnionMemInfo(void){memset(this, 0, sizeof(tagUnionMemInfo));}
};

//! 家族等级对应的家族科技最高级
struct tagTechInfo
{
	LONG	_lSubExpLv;//福利等级
	LONG	_lSpecialityLv;//专业等级
	LONG	_lBussinessLv;//商业等级
	LONG	_lBattleLv;//战争等级
	LONG	_lNoblelv;//贵族等级
	tagTechInfo(){ZeroMemory(this,sizeof(tagTechInfo));}
};

struct tagBaseConsume
{
	LONG	_BaseVal;//基数
	LONG	_Critical;//临界值
};

//一个据点的信息
struct tagBaseInfo
{
	char		_cBaseName[32];//据点名称
	LONG		_lTickets;//门票数
	LONG		_lBattleVal;//战功值
	CGUID		_FacGuid;//占领家族的GUID
	tagBaseInfo(){ZeroMemory(this,sizeof(tagBaseInfo));}
};
//战争类型
enum eBattleType
{
	eFactionWar=0,
	eBaseWar,
	eCityWar,

	eMaxWar,
};

//敌对家族信息
struct tagEnemy
{
	char _cName[MAXSIZE_FactionName];//敌对家族名称
	DWORD _dwStartTime;//战争开始时间
	DWORD _dwEndTime;
};

#pragma pack(pop)