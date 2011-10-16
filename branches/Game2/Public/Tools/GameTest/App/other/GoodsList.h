#pragma once
class CGoods;

#define MONEY_INDEX	1185	// 钱的编号
#define YUANBAO_INDEX	48	// 元宝的编号
#define JIFEN_INDEX	303023044	// 积分的编号

// 物品属性全局基本表
class CGoodsList
{
	friend class CGoods;
public:
	CGoodsList(void);
	virtual ~CGoodsList(void);

	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
public:

	// 物品类型
	enum eGoodsType
	{
		GT_NORMAL,				// 0普通(不能使用，不能装备)
		GT_EXPENDABLE,			// 1消耗类
		GT_CARD,				// 2卡片
		GT_MERCHANDISE,			// 3跑商物品
		GT_HEAD,				// 4头盔
		GT_BODY,				// 5盔甲
		GT_GLOVE,				// 6手套
		GT_BOOT,				// 7靴子		
		GT_WEAPON,				// 8武器
		GT_WEAPON2,				// 9副手
		GT_NECKLACE,			// 10项链
		GT_RING,				// 11戒指
		GT_WAISTBAND,			// 12腰带
		GT_WING,				// 13翅膀
		GT_HEADGEAR,			// 14头饰
		GT_FROCK,				// 15外套
		GT_FAIRY,				// 16精灵
		GT_TATTOO,				// 17纹身
		GT_PACK,				// 18背包
		GT_MEDAL,				// 19勋章
		GT_HORSE,				// 20坐骑

		GT_NUM,					
	};


	// 物品基本属性，每个物品都有的
	struct tagBaseProperty
	{
		DWORD	dwIndex;		//	编号
		string  strOrginName;	//	原始名   < 20
		string	strName;		//	中文名称 < 20

		// 客户端专用属性
		DWORD	dwIconId;		//	界面图形ID
		DWORD	dwGroundId;		//	落地后图形ID
		DWORD	dwEquipID;		//	换装图形ID
		DWORD	dwSound;		//	丢弃声音
		DWORD	dwSoundID1;		//	挥动/弱伤害声音
		DWORD	dwSoundID2;		//	特殊攻击/击中声音
		bool	bSoundSwitch;	//	攻击时是否混音
		DWORD	dwWeaponActType;//	武器对应的动作类型
		DWORD	dwType;			//	类型
		DWORD	dwPrice;		//	金币价格
		DWORD	dwSilverPrice;	//	银币价格
		//DWORD	dwWeight;		//	重量

		string	strContent;		//	中文描述 < 64
	};

	// 物品附加属性，各个物品数量不一
	struct tagEffect
	{
		WORD	wType;			// 属性类型
		bool	bEnable;		// 是否有效(为将来组合属性预留)
		bool	bHide;			// 是否是隐藏属性
		long	dwValue1;		// 第一个值
		long	dwValue2;		// 第二个值
		//WORD	wOdds;			// 出现几率
	};

	//属于套装的单个物品的特有属性
	struct  tagGoodsBeControlledAttri
	{
		WORD	wPropertyName;	// 对应某个附加属性名称（即附加属性的枚举值）
		long	dwValue1;		// 附加属性值1
		long	dwValue2;		// 附加属性值2
		WORD	wActiveNum;		// 要激活此属性需达到的套装件数
	};

	//套装属性
	struct  tagSuitAttribute
	{
		vector<string> Suit_OriginName;															// 存放套装的原始名
		BYTE	wSuit_Count;																	// 套装总的件数
		map<DWORD/*特定的件数*/,map<DWORD/*属性名称*/,int/*属性值*/>>	 Suit_ActiveProperty;	// 存放套装特定的件数可激活的属性及其值
	};

	//套卡属性
	struct tagSeriesCardProperty 
	{
		BYTE HOLE_NUM_REQUIRE;							//要求开孔的数量
		BYTE CARD_NUM_REQUIRE;							//套卡决定数量
		vector<DWORD>         CardsID;          		//各张卡片ID(即卡片的物编号)
		map<DWORD,int>     CardsEffects;				//套卡的附加属性
	};

	/*
	struct tagFixValues
	{		
	WORD wType;		
	long lFixVal1;
	long lFixVal2;
	};*/

	// 一个基本物品结构
	struct tagGoods2
	{
		tagBaseProperty			BaseProperty;			// 基本属性
		vector<tagEffect>		vectorEffect;			// 附加属性
		vector<tagGoodsBeControlledAttri>		m_SuitProperty;		// 物品作为套装时具有的特殊属性
		vector<tagGoodsBeControlledAttri>		m_AkiviertProperty;	// 激发属性
	};

	struct tagMoneyGraphics
	{
		long lMinNum;		//数量下限
		long lMaxNum;		//数量上限
		long lGraphicsID;	//图形编号
	};

private:
	static map<DWORD, tagGoods2>	m_mapGoodsList;				// 原始物品列表(wIndex, tagGoods)
	static map<string, tagGoods2*>	m_mapGoodsListByName;		// 以名字为索引的原始物品列表(strOrginName, *tagGoods)
	static map<DWORD/*套装ID*/, tagSuitAttribute>	m_SuitList;	// 套装的属性表单，存放所有套装在各种件数条件下激发的总的属性
	static vector<tagMoneyGraphics>	m_stMoneyGraphics;			// 金钱的配置信息
	static map<string,DWORD>		m_mapEffects;				// 映射物品名称和附加属性
	static map<string,DWORD>		m_mapAllEffects;			// 映射附加属性名称和附加属性枚举值
	static map<DWORD,string>		m_mapEffectReverse;			// 映射物品附加属性和名称
	static void InitMapEffects();								// 进行物品附加属性和名称的具体映射

	static map<DWORD/*套卡的ID号*/,tagSeriesCardProperty>	m_mapSeriesCardList;	// 套卡属性列表

	//m_mapGoodsEffectValue;	// 物品的附加属性与值的映射
public:
	//获取属性映射map
	static map<string,DWORD>&			 GetMapEffects()							{return m_mapEffects;}
	//获取反转的映射map
	static map<DWORD,string>&			 GetMapEffectReverse()						{return m_mapEffectReverse;}
	//获取附加属性名和附加属性值的映射表
	static map<string,DWORD>&		     GetMapAllEffect()							{return m_mapAllEffects;}
	//获取所有的套装的信息列表
	static map<DWORD, tagSuitAttribute>& GetSuitList()								{return m_SuitList;}
	//获取此ID的套装的总件数
	static BYTE							 GetSuitCount(DWORD SuitID);
	//获取此ID的套装的原始名列表
	static vector<string>*				 GetSuitOriginNameList(DWORD SuitID)		{return &(m_SuitList[SuitID].Suit_OriginName);}
	//获取此ID的套装的属性信息列表
	static map<DWORD,map<DWORD,int>>&	 GetSuitAttributeInfor(DWORD SuitID)		{return m_SuitList[SuitID].Suit_ActiveProperty;}
	//获取此ID的套装物品的自身属性(nWhich = 1 取套装激发物品的某个属性值1， = 2取属性值2，= 3取属性值3（第三个是激发此属性需达到的套装物品个数）)
	static long GetSuitAttrOfGoods(DWORD dwIndex,DWORD wType,int nWhich);

	/////////////////套卡属性接口//////////////
	//获取所有的套卡信息
	static map<DWORD/*套卡的ID号*/,tagSeriesCardProperty>& GetSeriesCardList()		{return m_mapSeriesCardList;}
	//获取某类套卡
	static tagSeriesCardProperty& GetSeriesCardProperty(DWORD index)				{return m_mapSeriesCardList[index];}
	//获取某类套卡的附加属性列表
	static map<DWORD,int>& GetSeriesCardEffectList(DWORD index)						{return m_mapSeriesCardList[index].CardsEffects;}
	//获取某类套卡的卡片ID列表
	static vector<DWORD>&  GetSeriesCardIDList(DWORD index)							{return m_mapSeriesCardList[index].CardsID;}
	//获取某类套卡的决定数量
	static BYTE GetSeriesCardRequireNum(DWORD index)								{return m_mapSeriesCardList[index].CARD_NUM_REQUIRE;}
	//根据某个卡片的ID查找此类套卡的ID
	static DWORD GetSeriesCardIndexByCardID(DWORD cardIndex);
	///////////////////////////////////////////

	static BOOL LoadGoodsList(const char* filename);			// 从文件读取原始物品列表
	static void	LoadSuitList(const char* filename);				// 从文件读取原始套装列表

	static void LoadCardList(const char* filename);				// 从文件读取套卡列表信息

	static tagGoods2*	GetProperty(DWORD dwIndex)				{return (m_mapGoodsList.find(dwIndex)!=m_mapGoodsList.end())?(&m_mapGoodsList[dwIndex]):NULL;}
	static tagGoods2*	GetProperty(const char* strOrginName)
	{
		if(NULL == strOrginName)	return NULL;
		return (m_mapGoodsListByName.find(strOrginName)!=m_mapGoodsListByName.end())?m_mapGoodsListByName[strOrginName]:NULL;
	}
	static long GetEffectVal(DWORD dwIndex,WORD wType,int nWhich);
	static tagEffect* GetEffectTag(DWORD dwIndex,WORD wType);
	static bool InitMoneyGraphics(const char* filename);		// 从文件读取金钱的配置信息
	static long GetMoneyGraphics(long lMoneyNum);
	static tagGoods2*	GetPropertyByEquipID(DWORD dwEquipID);	// 根据装备换装编号获取物品属性
	static bool FillGoodsInfo(DWORD dwIndex,CGoods *pGood);		// 把索引的物品信息填充给物品类
};
