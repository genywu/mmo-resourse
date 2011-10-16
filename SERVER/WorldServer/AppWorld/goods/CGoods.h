// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODS_41A2E64F0232_INCLUDED
#define _INC_CGOODS_41A2E64F0232_INCLUDED

//#include "BaseObject.h"
#include "CGoodsBaseProperties.h"
#include "..\Shape.h"
#include "../../../../public/GoodsExDef.h"
#include <string>
using namespace std;

class CGoodsFactory;
class CRsPlayer;
class CEnchaseHole;

//物品的实现类
class CGoods 
: public CShape
{
public:
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

private:
	//物品在物品基本属性数据库内的索引编号	
	DWORD m_dwBasePropertiesIndex;
	
	DWORD m_dwAmount;
	
	DWORD m_dwPrice;

	DWORD m_dwSilverPrice;

	DWORD m_dwBuyPrice;
	
	//string m_strGoodsName;
	eGoodsBaseType m_GoodsBaseType;

	//制造者名字
	string m_strMakerName;


	vector<CGoods::tagAddonProperty> m_vAddonProperties;
	vector<CGoodsBaseProperties::tagSuitProperty> m_vSuitProperties;

	CEnchaseHole** m_pEnchaseHoleArray;
	int		m_nInitHoleNum;
	DWORD m_dwActivedKitCardId;

public:	
	VOID SetBasePropertiesIndex(DWORD dwID);


public:
	vector<CGoods::tagAddonProperty>& GetAllAddonProperties();

	void SetExID(CGUID& guid){m_guExID = guid;}
	
	CGoods();
	
	CGoods(OBJECT_TYPE otType, LONG lID);

	~CGoods();
	
	DWORD GetBasePropertiesIndex();
	
	VOID SetAmount(DWORD dwAmount);
	
	DWORD GetAmount();


	VOID SetBuyPrice(DWORD dwPrice){m_dwBuyPrice=dwPrice;}
	DWORD GetBuyPrice(){return m_dwBuyPrice;}

	DWORD GetSilverPrice(){return m_dwSilverPrice;}
	VOID SetSilverPrice(DWORD dwPrice){m_dwSilverPrice=dwPrice;}

	VOID SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	eGoodsBaseType GetGoodsBaseType(){return m_GoodsBaseType;}

	//获取最大堆叠数。该属性只能获取，不能设置。	
	DWORD GetMaxStackNumber();

	DWORD GetMaxEnchaseHoleNum();
	
	BOOL CanReparied();	

	VOID InitEnchaseHole();
	VOID EnchaseRelease();
	DWORD GetActivedHoleNum();

	VOID EnchaseArrange();

	//是否可以镶嵌
	BOOL CanEnchase();
	//可打孔上限	
	LONG* GetEnchaseHoleData();
	VOID SetEnchaseHoleState(int nIndex,BOOL bVal);

	//物品制造者名称 接口
	const char* GetMakerName(){return m_strMakerName.c_str();}
	void SetMakerName(const char* pszMakerName){m_strMakerName=pszMakerName;}
	
	/// “脚本增加的额外物品描述”相关接口
	/// 9.7.2009 : 使用制作者名字字符串保存此描述，物品该描述与制作者描述不能冲突。
	const char *GetScriptAddonDesc() const { return m_strMakerName.c_str(); }
	void SetScriptAddonDesc( const std::string &desc ) { m_strMakerName = desc; }

	//
	BOOL Enchase(CGoods* pGoods,int nIndex);

	//返回所有显式的、被启用的附加属性，所有没有被禁止及所有隐
	//式属性都不会被返回。	
	VOID GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut);	
	
	//VOID GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, vector<CGoods::tagAddonPropertyValue>& vOut);

	//获取已经修正过的值	
	VOID GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, vector<LONG>& vOut);
	
	LONG GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID);

	// 该属性是否存在改物品上。
	bool IsAddonProperyExist(GOODS_ADDON_PROPERTIES gapType);

	BOOL SetAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID, LONG lValue);
	BOOL AddAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType,DWORD dwID,LONG lVal1,LONG lVal2=0,BOOL bEnable=TRUE,BOOL bIsImplicit=FALSE);
	BOOL DelAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType);

	VOID AddAddonProperty(BYTE* pStream, long size );

	//返回物品的总重量。而不是单个物品的重量。	
	//DWORD GetWeight();
	
	VOID SetPrice(DWORD dwPrice);
	
	DWORD GetPrice();

	//获取物品的总价	
	DWORD GetTotalPrice();
	
	VOID SetGoodsName(const CHAR* strGoodsName);
	
	const CHAR* GetGoodsName();
	
	VOID SetGoodsDescribe(const CHAR* strGoodsName);

	//##ModelId=41B8096400E0
	const CHAR* GetGoodsDescribe();

	//##ModelId=41A6961A00AB
	virtual VOID AI();

	//##ModelId=41A696A3034B
	virtual BOOL Clone(CBaseObject* pObject);

	//##ModelId=41A696BB02BF
	virtual BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);

	//##ModelId=41A696E50261
	virtual BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	virtual BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);	
	virtual BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);

	//##ModelId=41A6977C03D8
	virtual const CHAR* ToString();

	//##ModelId=41A6979401D4
	virtual VOID Release();

	friend class CGoodsFactory;
	friend class CDBGoods;
};

#endif /* _INC_CGOODS_41A2E64F0232_INCLUDED */
