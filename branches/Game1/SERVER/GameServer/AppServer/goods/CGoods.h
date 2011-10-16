// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODS_41A2E64F0232_INCLUDED
#define _INC_CGOODS_41A2E64F0232_INCLUDED

//#include "BaseObject.h"
#include "CGoodsBaseProperties.h"
#include "..\Shape.h"
#include "..\script\Script.h"
#include "../../../../public/GoodsExDef.h"
#include "../../../setup/GlobeSetup.h"

class CGoodsFactory;
class CEnchaseHole;

//物品的实现类

class CGoods 
: public CShape
{
private:	
public:
	//附加属性
	struct tagAddonProperty
	{
		//附加属性的类型		
		GOODS_ADDON_PROPERTIES     				gapType;		
		//该属性带的值		
		long	lValues[2];
		
		tagAddonProperty();
		
		~tagAddonProperty();
		
		VOID Clear();
		
		BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
		
		BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

		BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);	
		BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	};


	struct tagEnchaseHoleProperty
	{
		BOOL bIsActived;//是否被开发
		CGoods* pCard;//所镶卡片
	};
	
	

    // [070608 AHC]
    bool  SetAttribute(DWORD ID, DWORD value, int valuePos=0/*第pos个值*/);
    DWORD GetAttribute(DWORD ID, int valuePos=0/*第pos个值*/);
	bool  AddAttribute(DWORD ID, DWORD value, int valuePos=0/*第pos个值*/);
    void  ChangeAttribute(DWORD ID, DWORD value, int valuePos=0/*第pos个值*/, bool bOverload = false);
    long  UpdateAttribute(/*const CGUID&ownerID, */const CGUID& sendPlayerID);

    // 脏属性数据 在向客户端发送时重新获取最新数据
    typedef struct tagDIRTYATTRDATA
    {
        DWORD ID;
        int valuePos;

        tagDIRTYATTRDATA()
            :   ID(0),
                valuePos(0){}
        tagDIRTYATTRDATA(DWORD id, int pos)
            :   ID(id),
            valuePos(pos){}

    }DIRTYATTRDATA;
    list<DIRTYATTRDATA>  m_listDirtyAttrData;

private:
	//物品在物品基本属性数据库内的索引编号	
	DWORD m_dwBasePropertiesIndex;
	
	DWORD m_dwAmount;
	
	DWORD m_dwPrice;
	DWORD m_dwSilverPrice;

	DWORD m_dwBuyPrice;

	eGoodsBaseType m_GoodsBaseType;

	//制造者名字
	string m_strMakerName;
	
	vector<CGoods::tagAddonProperty> m_vAddonProperties;
	vector<CGoodsBaseProperties::tagSuitProperty> m_vSuitProperties;

	//tagSubpack m_Subpack[SUBPACK_NUM];	
	DWORD m_dwActivedKitCardId;
	DWORD m_dwMaxHoleNum;
	CEnchaseHole** m_pEnchaseHoleArray;


protected:	
	VOID SetBasePropertiesIndex(DWORD dwID);	
	
public:
	BOOL SetAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID, LONG lValue);	

	BOOL AddAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID, LONG lVal1,LONG lVal2=0,BOOL bEnable=TRUE,BOOL bIsImplicit=FALSE);
	

	BOOL DelAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType);
	BOOL DelAddonPropertyNotInBase(VOID);


	
	
	CGoods();
	
	
	
	~CGoods();
	
	DWORD GetBasePropertiesIndex();

	vector<tagAddonProperty>* GetAddonProperties(){return &m_vAddonProperties;}
	
	VOID SetAmount(DWORD dwAmount);
	
	DWORD GetAmount();
	VOID SetBuyPrice(DWORD dwPrice){m_dwBuyPrice=dwPrice;}
	DWORD GetBuyPrice(){return m_dwBuyPrice;}

	DWORD GetSilverPrice(){return m_dwSilverPrice;}
	VOID SetSilverPrice(DWORD dwPrice){m_dwSilverPrice=dwPrice;}

	//获取最大堆叠数。该属性只能获取，不能设置。
	DWORD GetMaxStackNumber();

	//! 物品的绑定冻结属性
	enum eBindFreeze
	{
		eBF_NoAll,
		eBF_OnlyBind,
		eBF_Freeze,
	};
	//! 得到物品的绑定冻结属性
	eBindFreeze GetBindFreezeProperties(void);

	//! 判断物品能否升级、并返回物品等级，若不能，返回-1
	long CanUpgrade(void);

	//! 判断物品能否魔化、并返回物品魔化等级，若不能，返回-1
	long CanPlugEvil(void);

	BOOL CanUpgraded();
	
	BOOL CanReparied();

	VOID SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	eGoodsBaseType GetGoodsBaseType(){return m_GoodsBaseType;}

	CGoodsBaseProperties::EQUIP_TYPE QueryEquipType();
	eWeaponType GetWeaponType();
	eWeaponHandType GetWeaponHandType();
	eBusinessGoodsType GetBusinessGoodsType();
	

	//是否可以镶嵌
	BOOL CanEnchase();
	//可打孔上限	
	LONG* GetEnchaseHoleData();

	DWORD GetMaxEnchaseHoleNum();
	DWORD GetActivedHoleNum();
	DWORD GetEnchasedCardNum();
	VOID GetEnchasedCard(vector<DWORD>& vecCards);
	CGoods* GetEnchasedCard(int nIndex);
	VOID UpdateEnchaseAttr();

	BOOL Enchase(CPlayer* pPlayer,CGoods* pGoods,int nIndex);
	BOOL RemoveCard(CPlayer* pPlayer,int nIndex,bool bReserve);
	DWORD GetEnchaseNumByGoodsId(DWORD dwId);
	DWORD GetEnchaseNumByCardGroupID(DWORD GroupID);
	VOID EnchaseAttrUpdate(CPlayer* pPlayer,CGoods* pCard);
	VOID EnchaseAttrRemove(CPlayer* pPlayer,CGoods* pCard);
	VOID SetActivedKitCardId(DWORD dwId){m_dwActivedKitCardId=dwId;}
	DWORD GetActivedKitCardId(){return m_dwActivedKitCardId;}


	//是否可以重叠
	BOOL CanStacked();

	BOOL IsSuitGoods();

	VOID RunScriptOnGet(CPlayer* pPlayer);

	VOID RunScriptOnLose(CPlayer* pPlayer);

	VOID RunScriptOnEquip(CPlayer* pPlayer);

	VOID RunScriptOnRemoveEquip(CPlayer* pPlayer);

	VOID OnSuitNumChanged(CPlayer* pPlayer,int new_num);

	VOID InvalidSuitProperties(CPlayer* pPlayer);

	VOID InitEnchaseHole();
	VOID EnchaseRelease();

	BOOL ActiveEnchaseHole(CPlayer* pPlayer,int nIndex);
	DWORD ActiveEnchaseHole(CPlayer* pPlayer);
	BOOL CloseEnchaseHole(CPlayer* pPlayer,int nIndex);
	DWORD CloseEnchaseHole();
	VOID EnchaseArrange();
	VOID ActiveHoles(int nNum);

	BOOL ActiveEnchaseHole(WORD wIndex);

	VOID AddRefine(long lVal);


	//返回所有显式的、被启用的附加属性，所有没有被禁止及所有隐
	//式属性都不会被返回	
	VOID GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut);	
	
	//获取所有生效的附加属性，包括隐式属性	
	VOID GetEnabledAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut);
	
	VOID GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, vector<long>& vOut);
	
	LONG GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID);

	

	bool HasAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType);
	BOOL HasAddonProperty(GOODS_ADDON_PROPERTIES gapType);

	//返回物品的总重量。而不是单个物品的重量。	
	//DWORD GetWeight();
	
	VOID SetPrice(DWORD dwPrice);

	//返回物品的单价	
	DWORD GetPrice();

	VOID EDCalculate();

	/// 计算出该物品增加的PK CP值
	void CalculatePKCP();

	VOID AtkEDUpdate();

	VOID DefEDUpdate();	

	BOOL AddonPropertyUpdate(GOODS_ADDON_PROPERTIES gapProperty,GOODS_ADDON_PROPERTIES gapRange);
	BOOL ImmunityUpdate();//免伤更新
	BOOL PierceUpdate();//穿透更新
	BOOL LuckyUpdate();//幸运更新
	BOOL ConstellationUpdate();//星座更新
	BOOL AddonHpUpdate();//hp上限追加范围

	VOID MountBind(const CGUID& playerID);
	

	

	//物品制造者名称 接口
	const char* GetMakerName(){return m_strMakerName.c_str();}
	bool SetMakerName(const char* pszMakerName){if(256 > strlen(pszMakerName)){ m_strMakerName=pszMakerName;return true;}return false;}

	/// “脚本增加的额外物品描述”相关接口
	/// 9.7.2009 : 使用制作者名字字符串保存此描述，物品该描述与制作者描述不能冲突。
	const char *GetScriptAddonDesc() const { return m_strMakerName.c_str(); }
	bool SetScriptAddonDesc( const std::string &desc ) {if(256 > desc.size()){ m_strMakerName = desc;return true;}return false; }

	/// 更新“脚本额外描述”到客户端
	void UpdateScriptAddonDesc( const CGUID &playerID );

	/// 降低‘星座守护石’类物品的‘星座能量’值
	/// 返回结果值，如果没有该属性，返回-1.
	long DecGuardianEnergy( unsigned long lTimes = 1 );

	/// 获取‘星座能量’值，如果没有这个值，返回-1.
	long GetGuardianEnergy() ;

	/// 检查是否是“星座守护石“物品
	bool IsGuardian() ;

	//获取物品的总价	
	DWORD GetTotalPrice();
	
	VOID SetGoodsName(const CHAR* strGoodsName);
	
	const CHAR* GetGoodsName();		
	
	virtual VOID AI();
	
	virtual BOOL Clone(CBaseObject* pObject);
	
	virtual BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	
	virtual BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	BOOL SerializeForOldClient(vector<BYTE>* pStream, BOOL b = TRUE);

	virtual BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);	
	virtual BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	BOOL SerializeForOldClient(DBWriteSet& setWriteDB, BOOL b = TRUE);

	BOOL SerializeForRgnDisplay(DBWriteSet& setWriteDB,BOOL b=TRUE);
	
	virtual const CHAR* ToString();
	
	virtual VOID Release();

	
	friend class CGoodsFactory;
};

#endif /* _INC_CGOODS_41A2E64F0232_INCLUDED */
