#pragma once
#include "../MoveShape.h"
#include "Goodslist.h"

class CEffect;
class CBitmapX;
class CClientRegion;

class CGoods:public CMoveShape
{
	friend class CGoodsList;
public:
	enum eAction
	{
		GOOD_DROP,
		GOOD_NORMAL,
		GOOD_EXCEPT
	};

	struct tagBaseProperty
	{
		string	strName;		//	中文名称 < 20
		ulong	dwValue;		//	金币价格
		ulong	dwSilverPrice;	//	银币价格
		ulong	dwBuyPrice;	//  物品买入价格（商业系统使用）
		ulong	dwPriceType;	//  价格类型
		ulong	dwType;			//	物品类型

		tagBaseProperty()
		{
			strName = "";
			dwValue = 0;
			dwSilverPrice = 0;
			dwBuyPrice = 0;
			dwPriceType = 0;
			dwType = 0;
		}
	};

	struct tagCardProperty
	{
		ulong  dwCardIndex;			//  卡片的物编号
		CGUID  CardGUID;			//	卡片的GUID
		string strCardName;			//	卡片的名字
		ulong  dwCardType;			//	卡片的类型
		ulong  dwInlayEquipType;	//	卡片适应的装备类型
		ulong   bInlayEquipLevel;	//	卡片适应的装备级别
		ulong   bMaxNum;				//	此卡片在同一装备最大可使用数量
		bool   bIsSeriesCard;		//此卡是否为套卡
		vector<CGoodsList::tagEffect>  CardEffect;	//卡片的附加属性

		tagCardProperty()
		{
			dwCardIndex = -1;
			CardGUID = CGUID::GUID_INVALID;
			strCardName = "";
			dwCardType = 0;
			dwInlayEquipType = 0;
			bInlayEquipLevel = 0;
			bMaxNum = 0;
			bIsSeriesCard = false;
		}
		tagCardProperty(const tagCardProperty& src)
		{
			dwCardIndex = src.dwCardIndex;
			CardGUID = src.CardGUID;
			strCardName = src.strCardName;
			dwCardType = src.dwCardType;
			dwInlayEquipType = src.dwInlayEquipType;
			bInlayEquipLevel = src.bInlayEquipLevel;
			bMaxNum = src.bMaxNum;
			bIsSeriesCard = src.bIsSeriesCard;
			CGoodsList::tagGoods2* pstGoods=CGoodsList::GetProperty(dwCardIndex);
			if (pstGoods)
			{
				vector<CGoodsList::tagEffect>::iterator EffectIter=pstGoods->vectorEffect.begin();
				for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
				{
					CardEffect.push_back(*EffectIter);
				}
			}
		}
	};

	CGoods(void);
	CGoods(ulong dwIndex);
	CGoods(const CGoods &srcGoods);
	~CGoods(void);
	void AI(void);
	ulong GetIndex(){return m_dwIndex;}
	void  SetIndex(ulong dwVal){m_dwIndex=dwVal;}
	void SetNum(long lVal){m_lNum=lVal;}
	long GetNum(){return m_lNum;}
	const char* GetOrginName(){return GetProperty()->strOrginName.c_str();}
	void SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	ulong GetGoodsBaseType();
	vector<CGoodsList::tagEffect>*	GetEffect()				{return &m_vectorEffect;}
	//vector<CGoodsList::tagFixValues>* GetFixs()				{return &m_vecFixValues;}
	tagBaseProperty* GetBaseProperty()						{return &m_stBaseProperty;}
	void SetBaseProperty(tagBaseProperty *pstBaseProperty)	{m_stBaseProperty=*pstBaseProperty;}

	CGoodsList::tagBaseProperty* GetProperty()	{return &(CGoodsList::GetProperty(m_dwIndex)->BaseProperty);}

	bool AddToByteArray(vector<uchar>*,bool bExtData=true);
	// 解码
	bool DecordFromByteArray(uchar* pByte, long& pos, bool bExData=true);
	bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData=true);


	////////////XY 2007.11.13///////////////
	void UpdateProperty(CMessage* pMsg);
	////////////////////////////////////////

	long GetEffectVal(ushort wType,int nWhich);
	void SetEffectVal(ushort wType,int nWhich,long lVal);
	void SetNewEffectVal(ushort wType,int nWhich,long lVal);		//给物品添加新的附加属性
	//long GetFixVal(ushort wType,int nWhich);
	bool CanRepair();
	bool CanUpgrade();

	bool HasEffect( ushort wType );
	//bool IsEffectChange();
	//bool IsInTradePage(){return m_bInTradePage;}
	//void SetInTradePage(bool bVal){m_bInTradePage=bVal;}
	//bool IsInPlayerShop(){return m_bInPlayerShop;}
	//void SetInPlayerShop(bool bFlag) {m_bInPlayerShop = bFlag;}
	//判断物品是否可以叠加
	static bool isGoodsCanAccumulate(CGoods *pOnGroundGoods);
	ulong RepairPrice();
	ulong GetUpdatePrice();
	ulong GetPrice();
	ulong GetPriceType();
	ulong GetSalePrice();

	bool Display(void);

	void DispalayFlashEffect();			// 显示物品闪光
	void IniFlashEffect();				// 初始化闪光特效

	//void SetNameColor(ulong dwColor);
	//ulong GetNameColor();
	float GetHeight();
	void  SetDropEffect();
	bool  GetDropEffect() {return m_bDropEffect;}

	CEffect& GetFlashEffect() {return (*m_pFlashEffect);}

	//物品的镶嵌孔接口
	bool Stiletto();						//给物品打孔的接口
	bool PlugUpHole(ushort index,ushort wHoleNum);								//封堵镶嵌孔
	bool EnchaseCard(ushort index,tagCardProperty card);			//镶嵌卡片
	bool RemoveCard(ushort index);								//拆除卡片
	void SetHoleNum(ushort wNum);									//设置物品的当前已开的孔数
	long GetHoleNum()	{return m_wHoleNum;}					//获取物品的当前已开的孔数
	long GetCardNum()	{return (long)m_InlayHole.size();}		//返回物品上已镶嵌的卡片数
	map<long,tagCardProperty>& GetInlayHole() {return m_InlayHole;}	//返回物品上的镶嵌孔信息
	bool GetHoleState(ushort HoleIndex);							//获取物品指定位置的镶嵌孔的状态（开、关）
	void SetHoleState(ushort HoleIndex,bool bState);				//设置物品指定位置的镶嵌孔的状态（开、关）

	// 锁定操作
	void Lock();
	void UnLock();
	bool IsLocked();
	////////XY 2008.1.3////////
	bool IsHaveShadow()	{return m_bHaveShadow;}
	void SetHaveShadowState(bool bState)	{m_bHaveShadow = bState;}
	///////////////////////////
	// 根据传入的物品获得对应的品质图片路径以及颜色
	void GetGoodsQuality(string & str, long & color);

	/// 获取物品的时限
	const string GetGoodsLimitTime();

	/// 获取制造者的名字  AddBy X.Y 2009.9.18
	inline const string& GetManufacturerName() { return m_strManufacturerName; }
	/// 设置制造者的名字  AddBy X.Y 2009.9.18
	void SetManufacturerName(const char* pStrName);

	/// 获取物品装备类型
	eEquipType GetEquipType(void);
	/// 获取物品法宝类型
	eTrumpType GetTrumpType(void);

	/// 获取脚本添加的活动信息内容
	//inline const string& GetActiveInfor() { return m_strActiveInfor; }

	/// 设置脚本添加的活动信息内容
	//void SetActiveInfor(const char* pStr);
private:
	ulong	m_dwIndex;						// 编号
	long	m_lNum;
	eGoodsBaseType m_GoodsBaseType;
	//bool	m_bInTradePage;
	//bool	m_bInPlayerShop;
	bool	m_bHaveShadow;					//此物品是否有虚影
	
	///////商业系统中所需属性（特殊的NPC物品才有此属性）///////
	ulong	m_dwPurchaseLimit;				//限购额
	///////////////////////////////////////////////////////////

	tagBaseProperty m_stBaseProperty;		//物品基本属性
	vector<CGoodsList::tagEffect>	m_vectorEffect;		// 扩展属性列表

	///////////镶嵌孔属性//////////
	map<long,tagCardProperty> m_InlayHole;	//镶嵌孔（只有武器或防具等物品可能具有）
	long	m_wHoleNum;						//此物品当前已开的镶嵌孔数
	map<long,bool> m_mapHoleState;			//物品当前各个孔的开孔状态
	///////////////////////////////

	CEffect*		m_pFlashEffect;			//闪光特效
	//ulong			m_dwNameColor;			//名字颜色
	
	//物品掉落的效果
	bool  m_bDropEffect;
	ulong m_dwDropTimeStart;
	float m_fDropHeight;

	// 锁定标志
	long	m_lLockCounter;

	// 制造者名字  AddBy X.Y 2009.9.18
	string  m_strManufacturerName;

	/// 活动信息
	//string  m_strActiveInfor;
};
