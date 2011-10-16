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
#include "../../../../public/BaseDef.h"



using namespace std;

class CGoodsFactory;



/*
*物品基本属性类
*/
class CGoodsBaseProperties 
{
public:
	//装备位置标识
	static enum EQUIP_PLACE
	{
		EP_UNKNOW,
		EP_HEAD,				//	0：头盔
		EP_NECKLACE,			//	1：项链
		EP_WING,				//	2：翅膀
		EP_BODY,				//	3：盔甲
		EP_BACK,				//	4：腰带
		EP_GLOVE,			//	5：手套	
		EP_BOOT,			//	6：鞋子
		EP_HEADGEAR,			//	7：头饰
		EP_FROCK,			//	8：外套		
		EP_LRING,			//	9：左戒指
		EP_RRING,			//	10：右戒指		
		EP_MEDAL1,			//	11：勋章1
		EP_MEDAL2,			//	12	勋章2
		EP_MEDAL3,			//	13	勋章3
		EP_FAIRY,			//	14：小精灵
		EP_WEAPON,			//	15：武器
		EP_WEAPON2,			//	16：副手武器
		EP_CARD,			//	17：卡片
		EP_HORSE,
		EP_CLOAK,			//披风
		EP_DECORATION1,		//饰品1
		EP_DECORATION2,		//饰品2
		EP_ADORN,			//佩饰
		EP_GUARDIAN,		//星座守护石

		EP_SUM,				//	装备总数量

		EP_PACK1=10001,
		EP_PACK2=10002,
		EP_PACK3=10003,
		EP_PACK4=10004,
		EP_PACK5=10005
	};

	//物品类别
	static enum GOODS_TYPE
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

	//装备是武器还是防具
	static enum EQUIP_TYPE
	{
		NEITHER_ATTACK_DEFENSE,//都不是
		ATTACK,//是武器
		DEFENSE //是防具
	};

	//卡片类型
	static enum CARD_TYPE
	{
		//镶嵌型
		CARD_CAN_ENCHASE=1,
		CARD_ENHANCE_ATTACK,	//+攻击
		CARD_ENHANCE_DEFEND,	//+防御

		//非镶嵌型
		CARD_CANNOT_ENCHASE,
		CARD_COLLECT			//搜集型
	};

	//ICON类别
	static enum ICON_TYPE
	{
		//容器内		
		IT_CONTAINER,
		//地面上		
		IT_GROUND,
		//装备后		
		IT_EQUIPED
	};

	//声音类别
	static enum SOUND_TYPE
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

	
		
	//##玩家的角色划分,用于宠物属性
	static enum PLAYER_ROLE
	{
		//##普通玩家
		PR_NORMAL_PLAYER			= 0x00000001,
		//##工会会长
		PR_GUILD_MASTER				= 0x00000002,
		//##联盟会长
		PR_UNION_MASTER				= 0x00000004,
		//##城主
		PR_CITY_MASTER				= 0x00000008,
	};

	//特殊属性
	static enum PARTICULAR_ATTRIBUTE
	{
		//不可修理		
		PA_CANNOT_REPAIRED			= 0x00000001,
		//永不掉落		
		PA_NEVER_DROP_DOWN			= 0x00000002,
		//离线后会掉出		
		PA_DROP_DOWN_WHEN_LOGOUT	= 0x00000004,
		//死亡必掉落		
		PA_DROP_DOWN_WHEN_DEAD		= 0x00000008,
		//落地后会消失		
		PA_DELETE_WHEN_DROP_DOWN	= 0x00000010,
		//不可交易也不可存仓库不可摆摊		
		PA_CANNOT_TRADE_AND_STORE	= 0x00000020,
		//神器		
		PA_UNIQUE					= 0x00000040,
		//使用回城卷\瞬移卷后掉落		
		PA_DROP_DOWN_WHEN_RECALL	= 0x00000080,
		//珍贵
		PA_RARITY					= 0x00000100
	};
	
	//附加属性的值
	struct tagAddonPropertyValue
	{
		//编号		
		DWORD									dwId;
		//基本值		
		LONG									lBaseValue;		
		
		tagAddonPropertyValue();

		
		~tagAddonPropertyValue();
		
		VOID Clear();
		
		BOOL Serialize(vector<BYTE>* pStream, BOOL b = TRUE);
		
		BOOL Unserialize(BYTE* pStream, LONG& lOffset, BOOL b = TRUE);

		BOOL Serialize(DBWriteSet& setWriteDB, BOOL b = TRUE);
		
		BOOL Unserialize(DBReadSet& setReadDB, BOOL b = TRUE);
	};	

	//附加属性结构
	struct tagAddonProperty
	{
		//附加属性的类型		
		GOODS_ADDON_PROPERTIES   				gapType;
		//该属性是否被激活，为以后的组合属性预留		
		BOOL									bIsEnabled;
		//是否是隐藏属性，不显示。		
		BOOL									bIsImplicitAttribute;		

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

	struct tagGoodsInfo
	{
		DWORD dwGoodsId;
		DWORD dwAmount;
	};

	struct tagGoodsDelInfo
	{
		DWORD dwPos;
		DWORD dwNum;
	};

protected:
	//原始名
	string m_strOriginalName;

	//显示名
	string m_strName;

	friend class CGoodsFactory;

private:
	//物品价格
	DWORD m_dwPrice;	

	DWORD m_dwSilverPrice;

	//物品描述
	string m_strDescribe;
	
	GOODS_TYPE m_gtGoodsType;

	eGoodsBaseType m_GoodsBaseType;

	//装备栏位标识
	EQUIP_PLACE m_epEquipPlace;

	//附加属性存放容器
	vector<tagAddonProperty> m_vAddonProperties;
	vector<tagSuitProperty> m_vSuitProperties;
	vector<tagTriggerProperty> m_vTriggerProperties;


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
	
	DWORD GetPrice();	

	DWORD GetSilverPrice(){return m_dwSilverPrice;}
	VOID SetSilverPrice(DWORD dwPrice){m_dwSilverPrice=dwPrice;}
	
	GOODS_TYPE GetGoodsType();

	VOID SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	eGoodsBaseType GetGoodsBaseType(){return m_GoodsBaseType;}
	
	BOOL CanEquip();
	
	EQUIP_PLACE GetEquipPlace();

	vector<tagAddonProperty>& GetAddonProperty(){return m_vAddonProperties;}
	vector<tagTriggerProperty>& GetTriggerProperties(){return m_vTriggerProperties;}

	
	VOID GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, vector<tagAddonPropertyValue>& vOut);

	DWORD GetAddonPropertyValue(GOODS_ADDON_PROPERTIES gapType,DWORD dwId);

	//获取所有没有被禁止的有效属性	
	VOID GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut);

	vector<tagSuitProperty>* GetSuitProperties(){return &m_vSuitProperties;}
	
	
	
	
	
	
	BOOL IsImplicit(GOODS_ADDON_PROPERTIES gapType);

	//是否有这个附加属性
	BOOL IsHasAddonPropertie(GOODS_ADDON_PROPERTIES gapType);

protected:

	VOID Clear();
};

#endif /* _INC_CGOODSBASEPROPERTIES_41A547FD0280_INCLUDED */
