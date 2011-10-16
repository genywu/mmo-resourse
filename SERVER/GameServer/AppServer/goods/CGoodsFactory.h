// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED
#define _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED
#include "CGoodsBaseProperties.h"

class CGoods;
class CPlayer;

class CGoodsFactory 
{
public:	
	static VOID Release();
	
	CGoodsFactory();
	
	~CGoodsFactory();

	//根据ID获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBaseProperties(DWORD dwID);

	static VOID RunScriptOnGet(DWORD dwGoodsId,CPlayer* pPlayer);	

	//根据原始名获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByOriginalName(const CHAR* pName);

	//根据名称获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByName(const CHAR* pName);

    //根据ID获取物品栏位
    static CGoodsBaseProperties::EQUIP_PLACE QueryGoodsEuipPlace(DWORD dwID);

    //根据ID获取物品类型
    static CGoodsBaseProperties::GOODS_TYPE QueryGoodsType(DWORD dwID);

    //根据ID获取物品价格
    static DWORD QueryGoodsPrice(DWORD dwID);

    //根据ID获取物品描述
    static const CHAR* QueryGoodsDescribe(DWORD dwID);

	//根据ID获取物品原始名
	static const CHAR* QueryGoodsOriginalName(DWORD dwID);

	//根据ID获取物品名称
	static const CHAR* QueryGoodsName(DWORD dwID);

	static const CHAR* QueryGoodsName(const CHAR* pOriginName);

	//根据物品名获取物品原始名
	static DWORD QueryGoodsIDByOriginalName(const CHAR* pName);

	//根据物品名获取物品ID编号
	static DWORD QueryGoodsIDByName(const CHAR* pName);	

	//序列化到ByteArray
	static BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);

	//通过ByteArray还原
	static BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	//序列化到DataBlock
	static BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);

	//通过DataBlock还原
	static BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);

	//通过制定编号创建物品
	static CGoods* CreateGoods(DWORD dwID,long lType);

	//创建多个物品	
	static VOID CreateGoods(DWORD dwID, DWORD dwAmount, vector<CGoods*>& vOut,long lType);

	//创建物品的附加属性
	static VOID CreateAddonProperties(CGoods* pGoods);
	//重新产生物品的附加属性,保留物品的等级。
	static VOID ReCreateAddonProperties(CGoods* pGoods);

	//创建物品的套装属性
	static VOID CreateSuitProperties(CGoods* pGoods);


	//通过ByteArray还原物品
	static CGoods* UnserializeGoods(BYTE* pStream, LONG& lOffset,long lType);

	//通过ByteArray还原物品
	static CGoods* UnserializeGoods(DBReadSet& setReadDB,long lType);

	//回收物品对象内存
	static BOOL GarbageCollect(CGoods** ppGoods,long lFlag, BOOL bCollectMedal = FALSE);

	//获得‘金钱’的物品编号
	static DWORD GetGoldCoinIndex();

	static const char* GetGoldOrginName();

	static const char* GetGoldName();

	static DWORD GetSilverIndex();
	static const char* GetSilverOrginName();
	static const char* GetSilverName();

	//获得‘宝石’的物品编号
	static DWORD GetGemIndex();

	//获得‘元宝’的物品编号
	static DWORD GetYuanBaoIndex();
	

	//计算物品的修理价格
	static DWORD CalculateRepairPrice(CGoods* pGoods);

	//计算物品的升级价格
	static DWORD CalculateUpgradePrice(CGoods* pGoods);

	//计算物品的出售价格
	static DWORD CalculateVendPrice(CGoods *pGoods);

	//计算物品的银币出售价格
	static DWORD CalculateVendSilverPrice(CGoods *pGoods);
	


	//装备消耗
	static BOOL EquipmentWaste(CGoods* pGoods, BOOL bIsSpecialWaste = FALSE);

	//修理装备
	static BOOL RepairEquipment(CGoods* pGoods);

	//升级装备
	static BOOL UpgradeEquipment(CGoods* pGoods, LONG lTargetLevel);

	//! -----------------------------------2008-1-8 Add-----------------------------------


	//! 改变装备等级（在原有等级基础上改变）
	//! 最小改变到0，最大达到设置的等级上限
	static BOOL ChangeEquipmentLevel(CGoods* pEquipment, long lChangeDegree);

	//! 设置装备等级
	//! 最小改变到0，最大达到设置的等级上限
	static BOOL SetEquipmentLevel(CGoods* pEquipment, LONG lLevel);



	//! 得到装备是否是攻击类
	static BOOL IsAttEquipment(CGoods* pEquipment);



	//! 更新装备属性
	static void UpdateEquipmentAttr(CGoods* pEquipment);

	static  eEquipLimitType QueryPlayerEquipLimitType(CPlayer* pPlayer);

	static eBusinessGoodsType GetBusinessGoodsType(DWORD dwGoodsId);


	//! -----------------------------------2008-1-8 End-----------------------------------

	//! 创建同一个类型的所有物品，并放入一个按物品索引为key的map
	static void CreateGoodsListByType(eGoodsBaseType eType, map<LONG, CGoods*> &mapGoods);

	//物品附加值的修正计算
	static BOOL ModifyGoodsAddonProperty(CGoods* pGoods, DWORD dwID, LONG lValue1, LONG lValue2 = 0, BOOL bIsImplicitAttribute = FALSE);
	//获取物品最大叠加数
	static DWORD QueryGoodsMaxStackNumber( DWORD dwGoodsBasePropertiesID );
	static VOID InitAddonNameMap();
	static DWORD GetAddonByName(string str);

public:
	static BYTE s_GoodsAttrDBSetup[512][2];


private:	
	static map<string, DWORD> s_mOriginalNameIndex;
	
	static map<string, DWORD> s_mNameIndex;
	
	static map<DWORD,CGoodsBaseProperties*> s_mGoodsBaseProperties;

	static map<string,DWORD> s_mAddonNameMap;

	static map<string,string> s_mapSucceedOrder;
	static map<string,CGUID> s_mapPendingOrder;
	static list<string> s_listSuccedOrder;

	static set<CGoods*> s_mGcGoodsSet;

	static vector<CGoods*> s_vNoDelGoods;

	static map<long,set<CGoods*> >	s_UsingGoods;	

	static DWORD s_dwAddonPropertyNum;

	static BOOL Upgrade(CGoods* pGoods, GOODS_ADDON_PROPERTIES gapSource, GOODS_ADDON_PROPERTIES gapDest, BOOL bUpgrade = TRUE );

public:

	static VOID AddGoodsToGCSet(CGoods* pGoods);

	static VOID ReleaseGCGoodsSet();
	static VOID SetNoDelFlag(BOOL bVal){msNoDelFlag=bVal;}
	static BOOL GetNoDelFlag(){return msNoDelFlag;}

	typedef vector<CGoods*>& NoDelGoodsVec;
	static vector<CGoods*>& GetNoDelGoodsVec(){return s_vNoDelGoods;}
	static VOID ReleaseNoDelGoods();

	static bool IsOrderExist(const char* szOrder);
	static void PushOrder(const char* szOrder,const char* szContent);
	static void PushPendingOrder(const char* szOrder,const CGUID& guid);
	static CGUID& GetGUIDByOrderId(const char* szOrder);
	static void ErasePendingOrder(const char* szOrder);

	//! 聊天中的物品信息管理
	////////////////////////////////////////////////////////////////////////////////
public:
	//!							添加一个物品信息
	static		void			AddGoodsInfo(const CGUID &Key, const vector<BYTE> &vGoodsInfo);
	//!							取得一个物品信息
	static		BOOL			GetGoodsInfo(const CGUID &Key, vector<BYTE> &vGoodsInfo);

	static		long			OutputUseGoodsInfo(const char *pszFileName);

private:
	//!							清理
	static		void			ClearInfo(LONG lClearNum);


	//!									保存的物品信息
	static	map<CGUID, vector<BYTE>>	m_mapChatGoodsInfo;
	//!									保存的时间先后顺序
	static	list<pair<CGUID, LONG>>		m_listSaveTimeOrder;

	static BOOL msNoDelFlag;

};

#endif /* _INC_CGOODSFACTORY_41A59A7103B9_INCLUDED */
