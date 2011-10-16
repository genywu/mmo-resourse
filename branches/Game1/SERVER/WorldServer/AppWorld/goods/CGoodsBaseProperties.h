// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CGOODSBASEPROPERTIES_41A547FD0280_INCLUDED
#define _INC_CGOODSBASEPROPERTIES_41A547FD0280_INCLUDED
#include <windows.h>
#include <string>
#include "../../../../public/GoodsDef.h"
#include "../../../../public/GoodsExDef.h"
using namespace std;

class CGoodsFactory;

//##ModelId=41A547FD0280
class CGoodsBaseProperties 
{
public:
	//装备位置标识
	enum EQUIP_PLACE
	{
		EP_UNKNOW,
		EP_HEAD,				//	1：头盔
		EP_NECKLACE,			//	2：项链
		EP_WING,				//	3：翅膀
		EP_BODY,				//	4：盔甲
		EP_BACK,				//	5：腰带
		EP_GLOVE,			//	6：手套	
		EP_BOOT,			//	7：鞋子
		EP_HEADGEAR,			//	8：头饰
		EP_FROCK,			//	9：外套		
		EP_LRING,			//	10：左戒指
		EP_RRING,			//	11：右戒指		
		EP_MEDAL1,			//	12：勋章1
		EP_MEDAL2,			//	13	勋章2
		EP_MEDAL3,			//	14	勋章3
		EP_FAIRY,			//	15：小精灵
		EP_WEAPON,			//	16：武器
		EP_WEAPON2,			//	17：副手武器
		EP_CARD,			// 卡片
		EP_HORSE,
		EP_CLOAK,			//披风
		EP_DECORATION1,		//饰品1
		EP_DECORATION2,		//饰品2
		EP_ADORN,			//配饰
		EP_GUARDIAN,		//星座守护石

		EP_SUM,				//	装备总数量

		EP_PACK1=10001,
		EP_PACK2=10002,
		EP_PACK3=10003,
		EP_PACK4=10004,
		EP_PACK5=10005
	};

	//物品类别
	enum GOODS_TYPE
	{
		//不能用
		GT_USELESS,
		//消耗品
		GT_CONSUMABLE,
		//装备
		GT_EQUIPMENT,
		//背包
		GT_PACK,
		//卡片
		GT_CARD
	};

	//ICON类别
	enum ICON_TYPE
	{
		//容器内		
		IT_CONTAINER,
		//地面上		
		IT_GROUND,
		//装备后		
		IT_EQUIPED
	};

	//声音类别
	enum SOUND_TYPE
	{
		//拾取		
		ST_PICKUP,
		//丢弃		
		ST_DROP,
		//使用		
		ST_USE,
		//装备		
		ST_EQUIP,
		//挥动,未命中		
		ST_FLOURISH,
		//命中,特殊攻击		
		ST_SLASH
	};


	//附加属性的数据（带修正）	
	struct tagAddonPropertyValue
	{
		//编号		
		DWORD		dwId;
		//基本值		
		LONG					lBaseValue;
								
		//是否允许修正		
		//BOOL									bIsEnableModifier;
		//该属性的修正值		
		//vector<tagAddonPropertyValueModifier>	vModifiers;
		
		tagAddonPropertyValue();
		
		~tagAddonPropertyValue();
		
		VOID Clear();
		
		BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
		
		BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

		BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);
		
		BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	};	
	
	struct tagAddonProperty
	{
		//附加属性的类型		
		GOODS_ADDON_PROPERTIES   				gapType;

		//该属性是否被激活，为以后的组合属性预留		
		BOOL									bIsEnabled;

		//是否是隐藏属性，不显示。		
		BOOL									bIsImplicitAttribute;

		//该附加属性出现的几率，最高10000，最低1，缺省10000		
		//DWORD									dwOccurProbability;

		//该属性带的值		
		vector<tagAddonPropertyValue>			vValues;
		
		tagAddonProperty();
		
		~tagAddonProperty();
		
		VOID Clear();
		
		BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
		
		BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

		BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);
		
		BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	};

	
	

	struct tagSuitProperty
	{
		GOODS_ADDON_PROPERTIES  gapType;
		vector<tagAddonPropertyValue>       vValues;
		BOOL bActived;
		tagSuitProperty();		
		~tagSuitProperty();
		
		VOID Clear();
		
		BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
		
		BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

		BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);
		
		BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	};

	//物品激发属性结构定义
	struct tagTriggerProperty
	{
		GOODS_ADDON_PROPERTIES  gapType;
		LONG lVal1;
		LONG lVal2;
		WORD wTriggerLevel;
	};


public:
	static const BYTE PACK_WIDTH=5;

protected:	
	string m_strOriginalName;
	
	string m_strName;

	eGoodsBaseType m_GoodsBaseType;

	friend class CGoodsFactory;

private:	
	DWORD m_dwPrice;
	
	DWORD m_dwSilverPrice;//银币价格
	
	string m_strDescribe;
	
	GOODS_TYPE m_gtGoodsType;
	
	EQUIP_PLACE m_epEquipPlace;
	
	vector<tagAddonProperty> m_vAddonProperties;
	vector<tagSuitProperty> m_vSuitProperties;
	vector<tagTriggerProperty> m_vTriggerProperties;


//#ifdef _CLIENT
//
public:	
	struct tagIcon
	{	
		ICON_TYPE 	itIconType;
	
		DWORD		dwIconID;
	};

private:

	vector<tagIcon> m_vIcons;

	DWORD GetIconID( ICON_TYPE itType );

public:	
	BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
	
	BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

	BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);
	
	BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	
	CGoodsBaseProperties();
	
	~CGoodsBaseProperties();
	
	const CHAR* GetOriginalName();
	
	const CHAR* GetName();
	
	const CHAR* GetDescribe();

	VOID SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	eGoodsBaseType GetGoodsBaseType(){return m_GoodsBaseType;}
	
	DWORD GetPrice();

	DWORD GetSilverPrice(){return m_dwSilverPrice;}
	VOID SetSilverPrice(DWORD dwPrice){m_dwSilverPrice=dwPrice;}
	
	/*DWORD GetHeight();*/
	
	//DWORD GetWidth();
	
	/*DWORD GetWeight();*/
	
	GOODS_TYPE GetGoodsType();
	
	BOOL CanEquip();
	
	EQUIP_PLACE GetEquipPlace();
	
	VOID GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, vector<tagAddonPropertyValue>& vOut);

	//获取所有没有被禁止的有效属性	
	VOID GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut);

	DWORD GetAddonPropertyValue(GOODS_ADDON_PROPERTIES gapType,DWORD dwId);
	
	/*DWORD GetOccurProbability(GOODS_ADDON_PROPERTIES gapType);*/
	
	BOOL IsImplicit(GOODS_ADDON_PROPERTIES gapType);

	//是否有这个附加属性
	BOOL IsHasAddonPropertie(GOODS_ADDON_PROPERTIES gapType);


protected:

	VOID Clear();
};

#endif /* _INC_CGOODSBASEPROPERTIES_41A547FD0280_INCLUDED */
