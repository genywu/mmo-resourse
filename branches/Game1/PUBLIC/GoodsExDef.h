
#include "GUID.h"

#pragma once

//!		物品扩展操作窗口类型
enum	eGoodsExWindowType
{
	eGWT_DisassembleWindow,		//! 分解窗口
	eGWT_PrescriptionWindow,	//! 合成窗口、绑定和冻结窗口（加纳为魔盒）
	//eGWT_BindAndFrostWindow,	//! 
	eGWT_UpgradeWindow = 3,			//! 升级窗口
	eGWT_PlusEvilWindow,		//! 魔化窗口
	eGWT_ResetElementTypeWindow,		//! 重置魔化类型窗口
	eGWT_ResetElementXiangXingWindow,	//! 重置魔化相性窗口
	eGWT_DepotWindow,			//! 仓库窗口
	/////XY 2008.1.21/////
	eGWT_CardFuntionWindow,		//!	镶卡主功能页面	
	eGWT_PlayerTradeWindow,		//! 玩家交易功能页面
	//////////////////////
	eGWT_End,
};

enum eEnchaseWindowType
{
	EBWT_OPEN_HOLE=1,
	EWT_CLOSE_HOLE,
	EWT_ENCHASE_CARD,
	EWT_REMOVE_CARD,
};

enum eIncrementConsumeType
{
	eICT_NORMAL_BUY_ITEM=1, //普通购买
	eICT_MAIL_BUY_ITEM,		//邮件购买
	eICT_USE_FUNCTION		//使用增值功能
};

enum eIncrementBuyRet
{
	eICR_BUY_SUCCEED=1,	//成功
	eICR_GOLD_NOT_ENOUGH, //金币不足
	eICR_TICKET_NOT_ENOUGH,	//点券不足
	eICR_WEIMIAN_NOT_ENOUGH,//位面不足
	eICR_PERSONAL_NUM_LIMIT,//个人限购
	eICR_WORLD_NUM_LIMIT,   //世界限购
	eICR_TIME_LIMIT		    //时间限制
};
enum eIncrementFunction
{
	eIFUNC_RELIVE=90000		//原地复活
};

enum eGoodsBaseType
{
	GT_NORMAL,//普通
	GT_CONSUMABLE,//消耗
	GT_CARD,//卡
	GT_GOODS,//商品
	GT_HEAD,//头盔
	GT_BODY,//衣服
	GT_GLOVE,//手套
	GT_BOOT,//鞋子
	GT_WEAPON,//武器
	GT_WEAPON2,//盾牌
	GT_NECKLACE,//项链
	GT_RING,//戒指
	GT_BACK,//腰带
	GT_WING,//翅膀
	GT_HEADGEAR,//头饰
	GT_FROCK,//外套
	GT_FAIRY,//精灵
	GT_MEDAL,//勋章
	GT_SUBPACK, //子背包
	GT_ARROW,	//箭矢
	GT_MONEY,	//金币
	GT_HORSE,	//坐骑
	GT_SILVERMONEY,	//银币
	GT_PICKGOODS,//拾取品
	GT_MANTLE,	//披风
	GT_JEWELRY,	//饰品
	GT_ACCESSORIES,	//配饰
	GT_GUARDIAN,  //星座守护石
};

enum eWeaponType
{
	WT_UNKNOWN,
	WT_SINGLE_SWORD,//单手剑
	WT_SINGLE_HAMMER,//单手锤
	WT_SINGLE_KNIFE,//匕首
	WT_WAND,//法杖
	WT_HEAVY_SWORD,//双手剑
	WT_HEAVY_AX,//双手斧
	WT_BOW,//弓
	WT_DOUBLE_SWORD,//双剑
	WT_FIST//拳刃
};

enum eWeaponHandType
{
	WHT_UNKNOWN,
	WHT_SINGLE_HAND=1,
	WHT_DOUBLE_HAND
};

//子背包类型
enum eSubPackType
{
	SPT_NORMAL=1,//普通子背包
	SPT_BUSINESS	//跑商专用背包
};

enum eBusinessGoodsType 
{
	BGT_NOT_BUSINESS_GOODS,

	BGT_FOOD,		///< 食物
	BGT_ORE,		///< 矿石
	BGT_PLANT,		///< 植物
	BGT_FIBRE,		///< 纤维
	BGT_ALCOHOL,	///< 酒
	BGT_WOOD,		///< 木材
	BGT_LEATHER,	///< 皮革
	BGT_CLOTH,		///< 布
	BGT_WEAPON,		///< 武具
	BGT_FLAVOR,		///< 香料
	BGT_HANDIWORK,	///< 艺品
	BGT_GEM,		///< 宝石

	BGT_MAX_TYPE,
};

enum eAccountLockRetVal
{
	ACCOUNT_LOCK_OK=1,
	ACCOUNT_LOCK_ERROR_CDKEY,
	ACCOUNT_LOCK_NOT_ENOUGH,
	ACCOUNT_LOCK_UNKNOWN_ERROR
};



//!		物品扩展功能，S2C信息代码
enum	eGoodsExTxtCode
{
	eGTC_Failed			= 0,		//! 失败
	eGTC_Succeed		= 1,		//! 成功

	eGTC_SessionLapse	= -0xFFFF,	//! 对话距离过远或已经失效
	eGTC_UnknownErr,				//! 未知原因的错误

	eGTC_CantDisassemble,			//! 不能分解
	eGTC_CantFindAim,				//! 不能找到操作目标
	eGTC_NoEnoughSpace,				//! 包袱空间不够
	eGTC_SrcIsLock,					//! 原料锁定中

	eGTC_NoPrescription,			//! 合成配方不存在
	eGTC_ErrSP,						//! 错误的合成配方

	eGTC_NoArmInHand,				//! 手中没有武器 “该武器不能充能！”
	eGTC_NoReinforceArm,			//! 不是充能武器 “该武器不能充能！”
	eGTC_LevelError,				//! “等级不符不能充能！”
	eGTC_ArmIsFull,					//! “能量充足时不能被同级或更低的能量替换！”

	eGTC_ErrOperation,				//! 错误的操作值
	eGTC_ErrAssistant,				//! 辅助材料错误
	eGTC_CantOperateAim,			//! 目标不能作此操作
	eGTC_DelAssistantFailed,		//! 删除辅助材料失败

	eGTC_ErrStuff,					//! 主材料错误
	eGTC_LevelTiptop,				//! 等级已到最高
	eGTC_NotEnoughMoney,			//! 钱不够

	eGTC_LimitTooLow,				//! 限制等级太低
	eGTC_QualityTooLow,				//! 品质太低

	eGTC_NumError,					//! 操作数量错误
	eGTC_NotUpgrade,                //! 没有强化的装备不能进行附魔操作
	eGTC_UpgradeLow,                //! 强化等级太低
};

//! 装备升级结果
enum eUpgradeResult
{
	eUR_Succeed,					//! 成功
	eUR_Failed_And_LevelNotChange,	//! 失败，等级未改变
	eUR_Failed_And_Degraded,		//! 失败，并且被降级
};

//! 绑定属性值
enum eBindType
{
	//! 未绑定
	eBT_Untie = 0,
	//! 已绑定
	eBT_Bind,
	//! 不能绑定
	eBT_CantBind,
	//! 不能松绑
	eBT_CantUntie,
	//! 得到即绑定（且不能解绑）
	eBT_GetAndCantUntie,
	//! 得到即绑定（可以解绑）
	eBT_GetAndBind,
	//! 装备后绑定（且不能解绑）
	eBT_DressAndCantUntie,
	//! 装备后绑定（可以解绑）
	eBT_DressAndBind,
};


//! 冻结属性值
enum eFrostType
{
	//! 未冻结
	eFT_UnFrost = 0,
	//! 已冻结
	eFT_Frost,
	//! 解冻中
	eFT_Thawing,
	//! 不可解冻（永久冻结）
	eFT_CantThaw,

	//! 不可冻结
	eFT_CantFrost,
};


//! 装备升级失败后的操作
enum eFailedOperation
{
	//! 无操作
	eFO_Null,
	//! 减小等级
	eFO_Reduce,
	//! 设置等级为
	eFO_SetTo,
};


//! 装备升级的特殊材料功能
enum eExtraStuffOperate
{
	//! 无功能
	eESO_Null,
	//! 增加成功率
	eESO_AddOdds,
	//! 替换失败操作为
	eESO_AmendFailOpt,
};

//! 魔化材料合成办法
enum eEvilStuffOperation
{
	eESO_1in1 = 1,	//! 1合1
	eESO_2in1,	//! 2合1
	eESO_3in1,	//! 3合1
	eESO_4in1,	//! 4合1
};



//!				分解物种类最大数量
#define			MAX_DISASSEMBLE_NUM		3U

//!				最大物品原始名字长
#define			MAX_GOOODS_NAME_SIZE	32U

//!				最大魔化等级
#define			MAX_ELEMENT_LEVEL		2000U
//！			最大魔化等级old
#define			MAX_EVIL_LEVEL			10U

//!				分解物种类数量
#define			DISASSEMBLE_TYPE_NUM	8U
//!				分解物种类的子类型数量
#define			DISASSEMBLE_CHILD_TYPE_NUM	10U

//!				非副职业制作配方名称长度
#define			MAKE_TABLE_NAME_SIZE	21U
//!				非副职业制作配方说明长度
#define			MAKE_TABLE_NARRATE_SIZE	2048U




//!			最大合成材料数量
#define		MAX_STUFF_NUM			15U

//! C2S消息用 合成物数组结构
struct tagSynthesizeGoods
{
	CGUID	arrGoodsGuid[MAX_STUFF_NUM];		//! 物品GUID
	long	arrContainerFlag[MAX_STUFF_NUM];	//! 物品所在容器
	long	arrPos[MAX_STUFF_NUM];				//! 物品所在容器位置
	long	arrNum[MAX_STUFF_NUM];
};




//!			冲等最大主材料数量
#define		MAX_UPGRADE_STUFF_NUM			1U
//!			冲等最大辅助材料数量
#define		MAX_UPGRADE_ASSISTANT_NUM		3U
//!			数量说明[1个升级目标 + MAX_UPGRADE_STUFF_NUM个主料 + MAX_UPGRADE_ASSISTANT_NUM个辅料]
#define		IN_TAGUPGRADESTUFF_GOODS_NUM	(1 + MAX_UPGRADE_STUFF_NUM + MAX_UPGRADE_ASSISTANT_NUM)

//! C2S消息用 冲等材料结构

struct tagUpgradeStuff
{
	CGUID	arrGoodsGuid[IN_TAGUPGRADESTUFF_GOODS_NUM];		//! 物品GUID
	long	arrContainerFlag[IN_TAGUPGRADESTUFF_GOODS_NUM];	//! 物品所在容器
	long	arrPos[IN_TAGUPGRADESTUFF_GOODS_NUM];			//! 物品所在容器位置
};


struct tagEvilStuff
{
	CGUID	GoodsGuid;		//! 物品GUID
	long	lContainerFlag;	//! 物品所在容器
	long	lPos;			//! 物品所在容器位置
	long	lUseNum;		//! 消耗数量
};

//物品公用冷却时间
struct tagGoodsCommonCooldown
{
	DWORD dwCooldownTime;//公用冷却时间
	DWORD dwCooldownStartTime;//开始时间
};

//跑商物品
struct tagBusinessGoods
{
	string strOriginName;//原始名
	DWORD dwId;//物品ID
	BOOL bSell;//是否出售
	BOOL bSpecial;//是否特产
	BOOL bNeed;//是否特需
	DWORD dwBear;//承载量
	DWORD dwCategory;//类别
	DWORD dwBasePrice;//基本价
	double dBuyPriceMin;//买入最低
	double dBuyPriceMax;//买入最高
	double dSellPriceMin;//卖出最低
	double dSellPriceMax;//卖出最高
};

//打开商业买卖界面时传给CLIENT的结构
struct tagBusinessGoodsToClient
{
	DWORD dwId;//物品ID
	DWORD dwPrice;//价格
	DWORD dwBuyLimit;//限购额
	BYTE btSpecial;//是否特产
	BYTE btNeed;//是否特需品
};

//跑商物品类别
struct tagBusinessGoodsCategory
{
	DWORD dwCategoryId;//类别ID
	string strName;//名称
	DWORD dwBuyLimit;//限购额
};

struct tagTradeSpotData
{
	DWORD dwSpotId;
	DWORD dwGameServerId;
	DWORD dwAreaId;
	DWORD dwCountryId;
	map<DWORD,tagBusinessGoodsCategory> mapCategory;
	map<DWORD,tagBusinessGoods> mapGoods;
};


