


#ifndef _GT_Define_H_
#define _GT_Define_H_
#pragma once

#define HOTDOG_SUM	8				
#define MAX_MONEY 2000000000
#define MAX_SILVERMONEY 2000000000
#define COTE_NUM	5				//子背包的个数
#define PACK_WIDE   5				//子背包的列数
#define	PACK_NUM	6				//所有背包的个数
#define HOST_GRIDITEM	20			//原始背包的总格子数
#define DEPOT_MAIN_CELL_NUM	42		//主仓库格子总数
#define PACK_KB108_CELL_NUM	20

#define GRID_H_ITEM 5							//横排5个道具格子
#define GRID_V_ITEM 4							//竖排4个道具格子
#define GRIDITEM	GRID_H_ITEM*GRID_V_ITEM		//主背包的格子的总数
#define GRIDPACK	5							//子背包栏总数
#define GRID_V_SUBPACK	6						//子背包最大行数

#define GRID_H_INTEG 8							//整合背包界面的横排格子数
#define GRID_V_INTEGMAXLINE	15					//整合背包的最大行数
#define GRIDINTEGMAXNUM  GRID_H_INTEG*GRID_V_INTEGMAXLINE	//整合背包的最大格子数

//typedef CGoodsList::eGoodsType eGoodsType;

const DWORD ROLE_PRAME_NUM=10;
const DWORD ROLE_NUM_MAX=30;

#define MAIL_PREPAGE 8			// 邮件列表一页的个数

enum ePacketType
{
	ePT_Main,
	ePT_Sub,
	ePT_Business,
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

//! 要更新的非组队信息说明
enum eUpdateIdioinfo
{
	eMIS_Pos			,//更新位置

	eMIS_CurrHp			,//更新当前HP
	eMIS_CurrMp			,//更新当前MP
	eMIS_CurrEnergy		,//更新当前能量

	eMIS_MaxHp			,//更新最大HP
	eMIS_MaxMp			,//更新最大HP
	eMIS_MaxEnergy		,//更新最大能量

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

	long lCurrHp;			//当前HP
	long lCurrMp;			//当前MP
	long lCurrEnergy;		//当前能量

	long lMaxHp;			//最大HP
	long lMaxMp;			//最大HP
	long lMaxEnergy;		//最大能量

	long lLevel;			//当前等级
	long lOccupation;		//职业
	long lSex;				//性别
};

//任务变量
struct tagRoleParam 
{
	WORD m_wParam;
	WORD m_wExcessRecordIndex;
	LONG m_lAccParam;
	tagRoleParam()
	{
		m_wParam = 0;
		m_wExcessRecordIndex = 0;
		m_lAccParam = 0;
	}
};

//任务记录
struct tagRoleRecord 
{
	BYTE m_cRoleStep;	//任务进行的步骤
	BYTE m_cAimNum;
	long m_lRoleID;		//任务ID;
	tagRoleParam m_Param[ROLE_PRAME_NUM];
};
//扩展任务记录
struct tagExcessRecord 
{
	DWORD m_lRegionID;
	DWORD m_lRegionX;
	DWORD m_lRegionY;

	tagExcessRecord():
	m_lRegionID(0),
		m_lRegionX(0),
		m_lRegionY(0)
	{}
};
//人物技能
struct tagPlayerSkill
{
	DWORD dwID;
	tagPlayerSkill(void)	{ dwID = 0; }
};
// 物品结构
struct tagGoods
{
	class CGoods *pGoods;					//	物品结构体内所指向的物品指针对象
	bool	bChange;				//	是否会发生属性变化
	WORD	lNum;					//	数量
	DWORD	lPos;					//	在背包栏中的坐标
	DWORD	lType;					//	物品所在的容器类型

	int	GetPosX()	{ return (int)lPos%5; }	//取得X坐标
	int	GetPosY()	{ return (int)lPos/5; } //...y...

	tagGoods() :
	pGoods(NULL),
		bChange(false),
		lNum(0),
		lPos(0),
		lType(0)			
	{}

	~tagGoods()
	{
		::ZeroMemory(this, sizeof(tagGoods));
	}
};

//背包快捷栏位结构
struct tagCote
{
	CGoods* pGoods;						//背包结构体内物品指针对象
	list<tagGoods> m_listCoteGoods;
	set<DWORD> m_setCoteHaveGoods;

	tagCote() :
	pGoods(NULL)
	{
		m_listCoteGoods.clear();
		m_setCoteHaveGoods.clear();
	}
};
struct tagownedReg
{
	long lRegionID;						//场景ID
	WORD wType;							//场景类型
};

struct tagPlayerShopGoods
{
	BYTE	btNum;
	BYTE	btOldPos;
	BYTE	btPos;

	long	lValue;

	union
	{
		DWORD dwType;
		CGoods* pGoods;
	};

	tagPlayerShopGoods()
	{
		::ZeroMemory(this, sizeof(tagPlayerShopGoods));
	}
};
// 装备栏索引
enum eEquipIndex
{
	EQUIP_HEAD,				//	0：头盔
	EQUIP_NECKLACE,			//	1：项链
	EQUIP_WING,				//	2：翅膀
	EQUIP_BODY,				//	3：盔甲
	EQUIP_WAISTBAND,		//	4：腰带
	EQUIP_GLOVE,			//	5：手套	
	EQUIP_SHOES,			//	6：鞋子
	EQUIP_HEADGEAR,			//	7：头饰
	EQUIP_FROCK,			//	8：外套		
	EQUIP_LRING,			//	9：左戒指
	EQUIP_RRING,			//	10：右戒指		
	EQUIP_MEDAL1,			//	11：勋章1
	EQUIP_MEDAL2,			//	12	勋章2
	EQUIP_MEDAL3,			//	13	勋章3
	EQUIP_FAIRY,			//	14：小精灵
	EQUIP_WEAPON,			//	15：武器
	EQUIP_WEAPON2,			//	16：副手武器
	EQUIP_PIFENG,			//	17：披风
	EQUIP_HORSE,			//	18：坐骑

	EQUIP_SUM,				//	装备总数量
};

enum PLAYER_EXTEND_ID
{
	PEI_PACKET					= 1,	// 背包
	PEI_EQUIPMENT,						// 装备
	PEI_WALLET,							// 金币钱包
	PEI_SILVERWALLET,					// 银币钱包
	PEI_GEM,							// 宝石
	PEI_YUANBAO,						// 元宝
	PEI_JIFEN,							// 积分
	PEI_MEDAL,                          // 勋章容器
	PEI_SUBPACK=10000,
	PEI_PACK1=10001,
	PEI_PACK2=10002,
	PEI_PACK3=10003,
	PEI_PACK4=10004,
	PEI_PACK5=10005,
	PEI_BUSSINESPACKFIELD = 20000,		// 放跑墒背包的栏位ID
	PEI_BUSSINESPACK = 20001			// 跑墒背包容器ID
};

//装备限制
enum EquipRestrict
{
	UNKNOW_NOTFIT,
	LEVEL_NOTFIT,		//等级不够
	STR_NOTFIT,			//力量不够
	DEX_NOTFIT,			//敏捷不够
	CON_NOTFIT,			//体质不够
	INT_NOTFIT,			//灵力不够
	OCCUPATION_NOTFIT,	//职业不对
	SEX_NOTFIT,			//性别不对
	REGION_FORBID,		//场景禁止
	EQUIPFIT,			//可以装备
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//仓库 数据结构
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum eDEPOT_PACK_ID				//	*********************
{								//	*	*	*	*	*	*
	DEPOT_MAIN_PACK,			//	*********************		
	DEPOT_ITEM_PACK,			//	*	*	*	*	*	*
	DEPOT_EXTEND_PACK1,			//	*	*	*	*	*	*
	DEPOT_EXTEND_PACK2,			//	*	*	*	*	*	*
	DEPOT_EXTEND_PACK3,			//	*	*	*	*	*	*
	DEPOT_EXTEND_PACK4,			//	*	*	*	*	*	*
	DEPOT_PACK_NUM,				//	*	*	*	*	*	*
};								//	*********************
//仓库背包数据结构
struct tagDepotPackData 
{
	//		list<tagGoods> listGoods;
	long	lContainerID;				//ID-----主仓库/默认子背包/子背包/(金币/银币栏位)等等
	long	lGoodsAmount;				//数量
	long	lCubage;					
	hash_map<CGUID, tagGoods, hash_guid_compare> mapGoodsList;
	map<DWORD, BOOL> mapHaveGoods;

	tagDepotPackData() :	
	lContainerID(-1),
		lGoodsAmount(0),
		lCubage(0)
	{}
};
//仓库数据结构
struct tagDepotData 
{
	long	lMoneyAmount;
	long	lSilverAmount;
	CGUID	guidMoney;
	CGUID	guidSilver;
	BOOL	bOpen;
	bool	bHasPassWord;
	char	strOldPwd[DEPOT_PASSWORD_SIZE+1];		//保存旧密码
	char	strNewPwd[DEPOT_PASSWORD_SIZE+1];		//保存新密码
	tagDepotPackData m_ArrPackData[DEPOT_PACK_NUM];
	tagDepotData() :
	lMoneyAmount(0),
		lSilverAmount(0),
		guidMoney(CGUID::GUID_INVALID),
		guidSilver(CGUID::GUID_INVALID),
		bOpen(FALSE),
		bHasPassWord(false)
		//m_ArrPackData[DEPOT_PACK_NUM];
	{
		::ZeroMemory(strOldPwd, DEPOT_PASSWORD_SIZE+1);
		::ZeroMemory(strNewPwd, DEPOT_PASSWORD_SIZE+1);
	}
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//小精灵
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! 数据对象（客户端可按需修改）
//struct tagSetupOnServer
//{
//	// ??? member data在那??查找到使用地方为自动喝药
//};
//补给类型
enum eAutoSupplyType{
	eAST_Hp1,
	eAST_Hp2,
	eAST_Mp,
	eAST_SpriteMp,
	eAST_ArmPower,
	AutoSupplyTypeNum,
};
//! 单个自动补给配置对象
struct tagAutoSupplyData 
{
	BOOL bActive;			//状态
	LONG lSpringScale;		//触发百分比
	LONG lGoodsIndex;		//补给物品ID
	//tagAutoSupplyData(void) {memset(this,0,sizeof(tagAutoSupplyData));}
	tagAutoSupplyData() : bActive(FALSE),
		lSpringScale(0),
		lGoodsIndex(0)
	{}
};

struct tagAutoPixie{
	long  lItemGoodsNum;
	long* pItemGoodsList;
	tagAutoPixie() : lItemGoodsNum(0),
		pItemGoodsList(NULL) 
	{}
};

// 新建队伍数据结构 [12/26/2008 Square]
struct tagOneTeammateInfo
{
	CGUID PlayerGuid;
	//		long lOccu;
	float fPosX;
	float fPosY;
	bool  bOnline;
	char szName[20];

	tagOneTeammateInfo()
	{
		::ZeroMemory(this, sizeof(tagOneTeammateInfo) );
	}
};
typedef  hash_map<CGUID,tagOneTeammateInfo,hash_guid_compare> HSM_OneTM_Info;

//---------------------------------------------------------------------------
//单个成员的数据结构
struct tagTeammate
{
	CGUID	 PlayerGuid;						//成员guid
	float	fPosX;
	float	fPosY;
	bool	bOnLine;					//是否在线
	tagTeammate(CGUID& id):PlayerGuid(id)/*, wHP(0), wMaxHP(0), occupation(0), RegionID(CGUID::GUID_INVALID), strName("")	*/					
	{}
	tagTeammate():PlayerGuid(CGUID::GUID_INVALID)/*,wHP(0), wMaxHP(0), occupation(0), RegionID(CGUID::GUID_INVALID), strName("")	*/
	{}
	bool operator == (const tagTeammate & t) { return t.PlayerGuid == PlayerGuid; }
};

struct tagPlayerInfo
{
};

	enum eUIPage
	{
		UI_PAGE_CHARACTER = 0,
		UI_PAGE_SKILL, 
		UI_PAGE_DEPOT, 
		UI_PAGE_MAIL, 
		UI_PAGE_FRIEND, 
		UI_PAGE_TEAM, 
		UI_PAGE_ITEM, 
		UI_PAGE_SUBBACK, 
		UI_PAGE_MAX_NUM, 
	};


	struct tagGtBounsInfo
	{
		tagBounsInfo*	pInfo;
		bool	bQuestGet;
		DWORD	dwLastTime;
		tagGtBounsInfo(void)
		{
			pInfo = NULL;
			dwLastTime = 0;
			bQuestGet = false;
		}
	};

	//宠物功能
	struct tagPetInfo
	{
		//	宠物类型
		BYTE byType;
		//	guid
		CGUID guid;
		//	名字
		char szName[17];
		tagPetInfo(void) : 
		byType(0),
			guid(CGUID::GUID_INVALID)
		{
			::ZeroMemory(szName, 17*sizeof(char));
		}
	};


	struct tagTradePageGoods
	{
		long lNum;		//数量
		long lPos;		//位置
		DWORD dwIconID;
		CGUID guid;
		tagTradePageGoods() :
		lNum(-1),
			lPos(-1),
			dwIconID(0),
			guid(CGUID::GUID_INVALID)
		{}
	};

	// 联系人分组面板枚举
	enum ENUM_PANEL
	{	
		PANEL_FRIENDS,			// 好友
		PANEL_LOVER,			// 配偶
		PANEL_ENEMY,			// 恶人
		PANEL_STRANGER,			// 陌生人
		PANEL_BLACK,			// 黑名单

		MAXNUM_PANELS
	};

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

	//GroupPlane下的成员信息
	struct tagLinkmanBase_data
	{
		CGUID		guid;								// GUID
		CHAR		szName[21];							// 名字
		ULONG		uState;								// 状态（eLinmanStateType）
		tagLinkmanBase_data()
			:guid(CGUID::GUID_INVALID),
			uState(0)
		{
			::ZeroMemory(szName, 21*sizeof(CHAR));
		}
	};

	//! A\B
	//! 用于保存的联系人数据结构
	struct tagLinkman_data : public tagLinkmanBase_data
	{
		CHAR		szMapName[267];						// 所在地图名字
		LONG		lLevel;								// 等级
		LONG		lOccupation;						// 职业

		tagLinkman_data(void)
		{
			memset(this, 0, sizeof(tagLinkman_data));
		}
	};

	//! A\C
	//! 用于消息的联系人数据结构
	struct tagLinkmanBase : public tagLinkmanBase_data
	{
		BOOL		bMutual;							//! 互加关系表示
		tagLinkmanBase() : bMutual(FALSE)
		{}
	};

	//! A\B\C
	//! 用于消息的联系人数据结构
	struct tagLinkman : public tagLinkman_data
	{
		tagLinkman(void)
		{
			memset(this, 0, sizeof(tagLinkman));
		}
		BOOL		bMutual;							//! 互加关系表示
	};
	//GroupPlane

	struct SPanel
	{	
		int					 groupid;								 //所在分组服务器端id
		int					 number;								 //已经存在的人数
		list<tagLinkman>     m_PanleMemberData;					     //分组属性
		SPanel() : groupid(0), number(0)
		{}
	};
#endif//_GT_Define_H_