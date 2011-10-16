//======================================================================================
/**
*	@file		GoodsExSetup.h
*	@data		2007-12-10
*	@author		张科智
*/
//======================================================================================
#pragma once

#include "../Common/BaseDef.h"
#include "../Common/GoodsDef.h"

/**
*	@class GoodsExSetup
*
*	@brief 物品扩展功能（分解/合成、武器充能、锁定/冻结）配置
*/
class GoodsExSetup
{
public:

	/// 分解配方
	struct tagDisassemblePrescription
	{
		long	lOutGoodsQuality;												//! 分解物品质
		char	arr_szOutGoodsName[MAX_DISASSEMBLE_NUM][MAX_GOOODS_NAME_SIZE];	//! 分解后物品原始名
		long	arr_lOutGoodsId[MAX_DISASSEMBLE_NUM];							//! 对应分解产物ID（配置中没有此项，需计算得出）

		tagDisassemblePrescription(void){memset(this, 0, sizeof(tagDisassemblePrescription));}
	};


	/// 合成配方
	struct  tagSynthesizePrescription
	{
		long lIndex;												//! 配方索引
		long lOutGoodsNum;											//! 可合成物品数量
		char szOutGoodsName[MAX_GOOODS_NAME_SIZE];					//! 被合成物原始名称
		long arr_lStuffNum[MAX_STUFF_NUM];							//! 材料数量
		char sz_arr_StuffName[MAX_STUFF_NUM][MAX_GOOODS_NAME_SIZE];	//! 材料原始名
		long lOutGoodsId;											//! 被合成物ID
		long arr_lStuffId[MAX_STUFF_NUM];							//! 材料ID
		bool bHasPrimary;											//! 是否有主材料
		
		tagSynthesizePrescription(void){memset(this, 0, sizeof(tagSynthesizePrescription));}
	};

	
	//! ------------------------------充能物品附加功能------------------------------

	/// 修正系数
	struct  tagAmendCoefficient
	{
		float	fAmendIT;						//! 修正魔法吟唱时间
		float	fAmendMA;						//! 修正魔法伤害
		float	fAmendPA;						//! 修正非魔法伤害
		float	fAmendMC;						//! 修正魔法治疗
		float	fAmendPC;						//! 修正非魔法治疗
	};

	/// 修正系数类型
	enum eAmendCoefficientType
	{
		eACT_AmendIT,						//! 修正魔法吟唱时间
		eACT_AmendMA,						//! 修正魔法伤害
		eACT_AmendPA,						//! 修正非魔法伤害
		eACT_AmendMC,						//! 修正魔法治疗
		eACT_AmendPC,						//! 修正非魔法治疗
	};

	/// 充能配置
	struct  tagPowerAccessorial
	{
		long				lIntensity;			//! 充能强度
		long				lMExpend;			//! 魔法消耗充能值
		long				lPExpend;			//! 非魔法消耗充能值
		tagAmendCoefficient AmendCoefficient;

		tagPowerAccessorial(void){memset(this, 0, sizeof(tagPowerAccessorial));}
	};

	/// 一个等级范围的充能配置
	struct  tagLevelBoundPA
	{
		long lMinLevel;											//! 等级段下限
		long lMaxLevel;											//! 等级段上限
		map<long, tagPowerAccessorial> mapPowerAccessorial;		//! 以充能强度为key的配置主体
	};
	

	//! ------------------------------装备升级设置------------------------------


	/// 武器升级设置结构
	struct tagUpgradeSet
	{
		long		lOdds;								//! 成功几率
		long		lFailedOperate;						//! 失败时的处理办法（枚举值）
		long		lOperateNum;						//! 失败时的操作数
		long		ArmLevel;							//! 适用的武器等级
		char		szStuffName[MAX_GOOODS_NAME_SIZE];	//! 升级材料原始名
		long		lStuffIndex;						//! 生机材料的物品索引

		tagUpgradeSet(void){memset(this, 0, sizeof(tagUpgradeSet));}
	};

	/// 特殊升级材料
	struct tagExtraStuff
	{
		char		szStuffName[MAX_GOOODS_NAME_SIZE];	//! 升级材料原始名
		long		lMaxUseCount;						//! 每次升级装备最大使用数量
		long		lMinGoodsLevel;						//! 使用该宝石的冲等物品的最小冲等等级
		long		lAddOdds;							//! 使用后增加的成功几率百分数
		long		lFailedMinishLevel;					//! 使用后，冲等失败时，物品降低的冲等级数
		long		lStuffIndex;						//! 材料的物品索引

		tagExtraStuff(void){memset(this, 0, sizeof(tagExtraStuff));}
	};

	//! ------------------------------装备魔化设置------------------------------

	/// 装备等级范围设置 
	struct tagEquipmentLevelBound
	{
		long 					lMinLevel;
		long 					lMaxLevel;
		long 					lGetEffect;
		long 					lConsumeEffect;
	};
	//! 
	struct tagNeedNum
	{
		long 					lElementMinLevel;
		long 					lElementMaxLevel;
		long 					lStuffNumA;
		long 					lStuffNumD;
		//!						万分数的几率
		long 					lSucceedOdds;
	};

	struct tagElementStuff
	{
		long 					lMinLevel;
		long 					lMaxLevel;
		//!
		char					szStuffNameA[MT_Max][MAX_GOOODS_NAME_SIZE];
		//!
		char					szStuffNameD[MT_Max][MAX_GOOODS_NAME_SIZE];
	};
	//! 魔化材料
	struct tagElementBound
	{
		vector<tagElementStuff> vElementStuff;
		//!
		vector<tagNeedNum>		vNeedNum;
	};
	//! 成功率辅助材料
	struct tagHelpfulStuff
	{
		char					szStuffName[MAX_GOOODS_NAME_SIZE];
		//!						万分数的几率
		long					lOdds;
	};
	//! 成功率辅助配置
	struct tagHelpfulSetup
	{
		long 					lMinLevel;
		long 					lMaxLevel;
		vector<tagHelpfulStuff>	vHelpfulStuff;
	};
	//! 重置材料
	struct tagResetStuff
	{
		char					szStuffName[MAX_GOOODS_NAME_SIZE];
		//!						百分数的几率
		long					lOdds[MT_Max];
	};
	//!	元素重置配置
	struct tagResetElement
	{
		long 					lMinLevel;
		long 					lMaxLevel;
		vector<tagResetStuff>	vResetStuff;
	};
	//!
	struct tagXiangXingOdds
	{
		//!						百分数的几率
		long					lOdds[eXXV_NUM];
	};
	//! 相性重置配置
	struct tagResetXiangXing
	{
		long 					lMinLevel;
		long 					lMaxLevel;
		tagXiangXingOdds		XiangXingOdds;
		vector<tagResetStuff>	vResetStuff;
	};
	//!
	struct tagElementValue
	{
		//!						攻击值
		long					lAtkValue;
		//!						防御值
		long					lDefValue;
		//!						防御附加值
		long					lAffixValue;

		tagElementValue(void){memset(this, 0, sizeof(tagElementValue));}
	};
	//! 
	struct tagElementSetup
	{
		//!									装备等级范围设置
		vector<tagEquipmentLevelBound>		vEquipmentLevelBound;
		//!									魔化升级配置
		vector<tagElementBound>				vElementBound;
		//!									几率辅助材料配置
		vector<tagHelpfulSetup>				vHelpfulSetup;
		//!									魔化等级对应魔化属性值
		vector<tagElementValue>				vElementValue;
		
		//!									元素重置配置
		vector<tagResetElement>				vResetElement;
		//!									相性重置配置
		vector<tagResetXiangXing>			vResetXiangXing;
	};

	///////////////////////////////////old
	//! 材料合成几率结构
	struct tagStuffSynthesize
	{
		char		szStuffName[MAX_GOOODS_NAME_SIZE];	//! 材料的原始名
		char		szOutName[MAX_GOOODS_NAME_SIZE];	//! 产物的原始名
		long		lFour2One;							//! 4合1的几率
		long		lThree2One;							//! 3合1的几率
		long		lTwo2One;							//! 2合1的几率
		long		lOne2One;							//! 1合1的几率
		long		lStuffIndex;						//! 材料的物品索引
		long		lOutIndex;							//! 产物的物品索引

		tagStuffSynthesize(void){memset(this, 0, sizeof(tagStuffSynthesize));}

		// 获取操作方法的几率
		long	GetOdds( eEvilStuffOperation eso )
		{
			switch( eso )
			{
				case eESO_1in1:
					return	lOne2One;
				case eESO_2in1:
					return	lTwo2One;
				case eESO_3in1:
					return	lThree2One;
				case eESO_4in1:
					return	lFour2One;
			}
			return 0;
		}
	};


	//! 魔化材料设置
	struct tagPlusEvilStuff
	{
		char		szStuffName[MAX_GOOODS_NAME_SIZE];	//! 需要的魔化材料的原始名
		long		lCurrEvilLevel;						//! 装备当前的魔化等级
		long		lNeedMoney;							//! 需要的金钱
		long		lStuffIndex;						//! 需要的魔化材料的物品索引

		tagPlusEvilStuff(void){memset(this, 0, sizeof(tagPlusEvilStuff));}
	};


	//! 装备类型相关的设置
	struct tagTypeWith
	{
		long lArmType;								//! 装备类型
		long lStuffNum;								//! 每次魔化需要的材料数量
		long lBackoutGainStuffNum;					//! 拆除后获得的同等级材料数
		bool bAmendPA;								//! 该装备是否被加成物理攻击属性
		bool bAmendMA;								//! 该装备是否被加成魔法攻击属性
		bool bAmendPR;								//! 该装备是否被加成物理防御属性
		bool bAmendMR;								//! 该装备是否被加成魔法防御属性

		tagTypeWith(void){memset(this, 0, sizeof(tagTypeWith));}
	};

public:
	/// 构造
	GoodsExSetup(void);

	/// 析构
	~GoodsExSetup(void);

public:

	/*
	* 从XML配置文件中读取配置（部分元素名称错误会导致抛出异常）
	*
	* @return	0	= 成功；
	*			1	= GoodsExSetup配置读取失败，错误的文件名
	*			2	= 跟节点名称不是“GoodsExSetup”，节点获取失败
	*			3	= 获取跟节点指针失败
	*			11	= 读取分解配方失败
	*			21	= 读取合成配方失败
	*			31	= 读取武器充能配置失败		
	*/
	long ReadSetup(const char* fileName);

	/// 初始化服务器配置
	bool InitServerSetup(void);

	/// 编码
	bool Serialize(vector<uchar>* pStream);

	/// 解码
	bool Unserialize(uchar* pStream, long& lOffset);

public:
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 分解、合成
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/// 通过物品品质（lQuality）获取分解配方
	const tagDisassemblePrescription* GetDisassemblePrescription(long lQuality);

	/**
	*  计算分解物数量
	*
	*  @param lValue1：物品分解价值1
	*  @param lValue2：物品分解价值1
	*  @param lQuality：物品品质
	*  @param lOutIdx：第几种产物
	*/
	long GetDisassembleNum(long lValue1, long lValue2, long lQuality, long lOutIdx);

	/// 通过配方索引（lPrescriptionIndex）获取合成配方
	const tagSynthesizePrescription* GetSynthesizePrescription(long lPrescriptionIndex);


	/// 冒泡排序（从大到小）
	static	void	BubbleSort(long lSrcSize, ULONG srcKey[], ULONG srcValue[]);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 武器充能
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/// 得到武器充能最大百分比
	long GetReinforceMaxPercent(void);

	/// 通过武器等级（lLevel）和充能强度（lIntensity）获得一个充能配置
	const tagPowerAccessorial* GetPowerAccessorial(long lLevel, long lIntensity);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 绑定冻结
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/// 测试绑定消耗物品索引是否正确
	bool TestBindGoodsIndex(long lIndex);

	/// 测试解绑消耗物品索引是否正确
	bool TestUntieGoodsIndex(long lIndex);

	/// 测试冻结消耗物品索引是否正确
	bool									TestFrostGoodsIndex(long lIndex);
	//!										测试解冻消耗物品索引是否正确
	bool									TestThawGoodsIndex(long lIndex);

	//!										得到消耗物品索引数组
	void									GetBindGoodsIndex(vector<long> &vecReIdx);
	//!										得到消耗物品索引数组
	void									GetUntieGoodsIndex(vector<long> &vecReIdx);
	//!										得到消耗物品索引数组
	void									GetFrostGoodsIndex(vector<long> &vecReIdx);
	//!										得到消耗物品索引数组
	void									GetThawGoodsIndex(vector<long> &vecReIdx);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 冲等
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//!										通过装备限制等级和装备等级，得到冲等配方
	const	tagUpgradeSet*					GetUpgradeSet(long lLimitLevel, long lArmLevel);

	//!										通过装备等级，装备类型、装备子类型获得需要的金钱
	long									GetUpgradeMoney(long lLimitLevel, long lEquipmentType, long lChildType);

	//!										得到特殊物品的功能配置
	const	tagExtraStuff*					GetExtraStuffIni(const char* pGoodsName);

	//!										得到最大等级限制
	long									GetGoodsMaxLevel(void);

	//!										得到升级3D特效，bSucceed 是否成功，lResultCode 成功失败后的 标识
	long									GetUpgrade3DEffectID( bool bSucceed, long lResultCode = 0);

	const	tagExtraStuff*					GetExtraStuffIni(long lGoodsIndex);
	


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//! 魔化
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	//! 得到魔化效果值
	bool GetElementValue(long lGoodsLevel, long lElementLevel, tagElementValue &RetagElementValue);

	//! 得到魔化升级需求
	typedef const char*	LPReStuffName;
	bool GetElement(bool bIsAtk, long lGoodsLevel, long lElementLevel, long lElementType, LPReStuffName &pReStuffName, long &lNeedNum, long &lSucceedOdds);

	//! 得到魔化的初始升级需求
	ulong GetElement(bool bIsAtk, long lGoodsLevel, LPReStuffName pStuffName, long &lNeedNum, long &lSucceedOdds);

	//! 得到辅助宝石增加的成功几率（万分之），失败返回负数
	long GetHelpfulStuffOdds(long lElementLevel, LPReStuffName pStuffName);

	//! 得到魔化元素重置的材料几率配置
	const tagResetStuff* GetResetElement(long lGoodsLevel, LPReStuffName pStuffName);
	//! 得到魔化相性重置的材料几率配置
	typedef const tagXiangXingOdds* LPCXiangXingOdds;
	const tagResetStuff* GetResetXiangXing(long lGoodsLevel, LPReStuffName pStuffName, LPCXiangXingOdds &pXiangXingOdds);
	

private:
	//!										分解配方列表
	map<long, tagDisassemblePrescription>	m_mapDisassemblePrescription;
	//!										合成配映射，key=配方序号， value=配方
	map<long, tagSynthesizePrescription>	m_mapSynthesizePrescription;

	//!										物品充能最大百分比
	long									m_lReinforceMaxPercent;
	//!										充能配置
	vector<tagLevelBoundPA>					m_vecPowerIni;


	//!										绑定物品索引列表
	set<long>								m_setBindGoodsIndex;
	//!										解绑物品索引列表
	set<long>								m_setUntieGoodsIndex;
	//!										冻结物品索引列表
	set<long>								m_setFrostGoodsIndex;
	//!										解冻物品索引列表
	set<long>								m_setThawGoodsIndex;


	
	//! 装备升级配置对象
	struct tagArmUpgradeSet
	{
		//!							最小限制等级
		long						lMinLimitLevel;
		//!							最大限制等级
		long						lMaxLimitLevel;
		//!							设置key=装备当前等级
		map<long, tagUpgradeSet>	mapUpgradeSet;
	};

	//! 装备升级金钱配置对象
	struct tagArmUpgradeMoneySet
	{
		//!							最小限制等级
		long						lMinLimitLevel;
		//!							最大限制等级
		long						lMaxLimitLevel;
		//!							金钱设置key=装备id（H16装备类型 + L16子类型）
		map<long, long>				mapUpgradeMoneySet;
	};

	//!										装备升级配置
	vector<tagArmUpgradeSet>				m_vecUpgradeSetList;
	//!										装备升级配置
	vector<tagArmUpgradeMoneySet>			m_vecUpgradeMoneySetList;
	//!										特殊物品配置
	vector<tagExtraStuff>					m_vecExtraStuff;
	//!										装备升级的最大等级限制
	long									m_lUpgradeLimit;
	
	//!										新的魔化配置
	tagElementSetup							m_ElementSetup;
};