#pragma once
#include "../../../Public/Common/GoodsDef.h"

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
	~CGoodsList(void);

//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:

	// 物品基本属性，每个物品都有的
	struct tagBaseProperty
	{
		ulong	dwIndex;		//	编号
		string  strOrginName;	//	原始名   < 20
		string	strName;		//	中文名称 < 20

		// 客户端专用属性
		ulong	dwIconId;		//	界面图形ID
		ulong	dwGroundId;		//	落地后图形ID
		ulong	dwEquipID;		//	换装图形ID
		ulong	dwSound;		//	丢弃声音
		ulong	dwSoundID1;		//	挥动/弱伤害声音
		ulong	dwSoundID2;		//	特殊攻击/击中声音
		bool	bSoundSwitch;	//	攻击时是否混音
		ulong	dwWeaponActType;//	武器对应的动作类型
		ulong	dwType;			//	类型
		ulong	dwPrice;		//	金币价格
		ulong	dwSilverPrice;	//	银币价格
		//ulong	dwWeight;		//	重量

		string	strContent;		//	中文描述 < 64
	};

	// 物品附加属性，各个物品数量不一
	struct tagEffect
	{
		ushort	wType;			// 属性类型
		bool	bEnable;		// 是否有效(为将来组合属性预留)
		bool	bHide;			// 是否是隐藏属性
		long	dwValue1;		// 第一个值
		long	dwValue2;		// 第二个值
		//ushort	wOdds;			// 出现几率

		tagEffect()
		{
			wType = 0;
			bEnable = false;
			bHide = false;
			dwValue1 = 0;
			dwValue2 = 0;
		}
	};
	
	//受控的附加属性结构（1.对应受控的某条附加属性的编号；2.此附加属性的值1；3.此附加属性的值2；4.此附加属性受控的条件值。主要用于套装和属性激发。）
	struct  tagGoodsBeControlledAttri
	{
		ushort	wPropertyName;	// 对应某个附加属性编号（即附加属性的枚举值）
		long	dwValue1;		// 附加属性值1
		long	dwValue2;		// 附加属性值2
		ushort	wActiveNum;	// 要激活此属性需达到的最小数值
	};

	//套装属性
	struct  tagSuitAttribute
	{
		vector<string> Suit_OriginName;															// 存放套装的原始名
		uchar	wSuit_Count;																	// 套装总的件数
		map<ulong/*特定的件数*/,map<ulong/*属性名称*/,tagEffect/*此附加属性的整个结构*/>>	 Suit_ActiveProperty;	// 存放套装特定的件数可激活的属性及其值
	};

	//套卡属性
	struct tagSeriesCardProperty 
	{
		uchar HOLE_NUM_REQUIRE;							//要求开孔的数量
		uchar CARD_NUM_REQUIRE;							//套卡决定数量
		vector<ulong>         CardsID;          		//各张卡片ID(即卡片的物编号)
		map<ulong,int>     CardsEffects;				//套卡的附加属性
	};

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
	static map<ulong,string>		m_mapSuitName;				// 用套装ID对应存放所有套装的名称
	static map<ulong, tagGoods2>	m_mapGoodsList;				// 原始物品列表(wIndex, tagGoods)
	static map<string, tagGoods2*>	m_mapGoodsListByName;		// 以名字为索引的原始物品列表(strOrginName, *tagGoods)
	static map<ulong/*套装ID*/, tagSuitAttribute>	m_SuitList;	// 套装的属性表单，存放所有套装在各种件数条件下激发的总的属性
	static vector<tagMoneyGraphics>	m_stMoneyGraphics;			// 金币的配置信息
	static vector<tagMoneyGraphics>	m_stSilverMoneyGraphics;	// 银币的配置信息
	//static map<string,ulong>		m_mapEffects;				// 映射物品名称和附加属性
	static map<string,ulong>		m_mapAllEffects;			// 映射附加属性名称和附加属性枚举值
	static map<ulong,string>		m_mapEffectReverse;			// 映射物品附加属性和名称
	static void InitMapEffects();								// 进行物品附加属性和名称的具体映射

	static map<ulong/*套卡的ID号*/,tagSeriesCardProperty>	m_mapSeriesCardList;	// 套卡属性列表

	 //m_mapGoodsEffectValue;	// 物品的附加属性与值的映射
public:
	//获取通过套装ID查找对应的套装名称
	static string&						 GetSuitName(ulong dwSuitID);
	//获取属性映射map
	//static map<string,ulong>&			 GetMapEffects()							{return m_mapEffects;}
	//获取反转的映射map
	static map<ulong,string>&			 GetMapEffectReverse()						{return m_mapEffectReverse;}
	//获取附加属性名和附加属性值的映射表
	static map<string,ulong>&		     GetMapAllEffect()							{return m_mapAllEffects;}
	//获取所有的套装的信息列表
	static map<ulong, tagSuitAttribute>& GetSuitList()								{return m_SuitList;}
	//获取此ID的套装的总件数
	static WORD							 GetSuitCount(ulong SuitID);
	//获取此ID的套装的原始名列表
	static vector<string>*				 GetSuitOriginNameList(ulong SuitID)		{return &(m_SuitList[SuitID].Suit_OriginName);}
	//获取此ID的套装的属性信息列表
	static map<ulong,map<ulong,tagEffect>>&	 GetSuitAttributeInfor(ulong SuitID)		{return m_SuitList[SuitID].Suit_ActiveProperty;}
	//获取此ID的套装物品的自身属性(nWhich = 1 取套装激发物品的某个属性值1， = 2取属性值2，= 3取属性值3（第三个是激发此属性需达到的套装物品个数）)
	static long GetSuitAttrOfGoods(ulong dwIndex,ulong wType,int nWhich);

	/////////////////套卡属性接口//////////////
	//获取所有的套卡信息
	static map<ulong/*套卡的ID号*/,tagSeriesCardProperty>& GetSeriesCardList()		{return m_mapSeriesCardList;}
	//获取某类套卡
	static tagSeriesCardProperty& GetSeriesCardProperty(ulong index)				{return m_mapSeriesCardList[index];}
	//获取某类套卡的附加属性列表
	static map<ulong,int>& GetSeriesCardEffectList(ulong index)						{return m_mapSeriesCardList[index].CardsEffects;}
	//获取某类套卡的卡片ID列表
	static vector<ulong>&  GetSeriesCardIDList(ulong index)							{return m_mapSeriesCardList[index].CardsID;}
	//获取某类套卡的决定数量
	static ulong GetSeriesCardRequireNum(ulong index)								{return m_mapSeriesCardList[index].CARD_NUM_REQUIRE;}
	//根据某个卡片的ID查找此类套卡的ID
	static ulong GetSeriesCardIndexByCardID(ulong cardIndex);
	///////////////////////////////////////////

	static bool LoadGoodsList(const char* filename);			// 从文件读取原始物品列表
	static void	LoadSuitList(const char* filename);				// 从文件读取原始套装列表

	static void LoadCardList(const char* filename);				// 从文件读取套卡列表信息

	static map<ulong, tagGoods2>& GetMapGoodsList()		{return m_mapGoodsList;}
	static map<string, tagGoods2*>& GetMapGoodsListByName() {return m_mapGoodsListByName;}
	static tagGoods2*	GetProperty(ulong dwIndex)				{return (m_mapGoodsList.find(dwIndex)!=m_mapGoodsList.end())?(&m_mapGoodsList[dwIndex]):NULL;}
	static tagGoods2*	GetProperty(const char* strOrginName)
	{
		if(NULL == strOrginName)	return NULL;
		return (m_mapGoodsListByName.find(strOrginName)!=m_mapGoodsListByName.end())?m_mapGoodsListByName[strOrginName]:NULL;
	}
	static long GetEffectVal(ulong dwIndex,ushort wType,int nWhich);
	static tagEffect* GetEffectTag(ulong dwIndex,ushort wType);
	static bool InitMoneyGraphics(const char* filename);		// 从文件读取金钱的配置信息
    static long GetMoneyGraphics(long lMoneyNum,int iMoneyType = GBT_MONEY);	// 根据货币的种类和数量获取对应的货币模型ID（iMoneyType 0=金币、1=银币）
	static tagGoods2*	GetPropertyByEquipID(ulong dwEquipID);	// 根据装备换装编号获取物品属性
	static bool FillGoodsInfo(ulong dwIndex,CGoods *pGood);		// 把索引的物品信息填充给物品类

	// 判断是否能装备
	static bool IsCanEquip(CGoods *pGoods);


	// 根据传入的物品获得对应的品质图片路径以及颜色
	static void GetGoodsQuality(ulong dwIndex,string & str, long & color);

	/// 获取物品的时限
	static const string GetGoodsLimitTime(ulong dwGoodsIndex);

	/// 是否有此附加属性
	static bool HaveEffect(ulong dwGoodsIndex,ushort wEffectType);
};
