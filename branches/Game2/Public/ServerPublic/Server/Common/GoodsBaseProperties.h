/** 
*  @file   GoodsBaseProperties.h
*  @author
*
*  @brief  物品基本属性类
*  @date   
*/

#pragma once

#include "../../../Module/NoCopyable.h"
#include "../../../Common/GoodsDef.h"

class GoodsSetup;
class CGoodsFactory;

class CGoodsBaseProperties 
	: private NonCopyable
{
public:
	/// 附加属性的值
	struct tagAddonPropertyValue
	{
		/// 编号		
		ulong dwId;
		/// 基本值		
		long lBaseValue;		
		
		tagAddonPropertyValue();
		~tagAddonPropertyValue();
		/// 清理
		void Clear(void);
		/// 编码
		bool Serialize(vector<uchar>* pStream, bool b = true);
		/// 解码
		bool Unserialize(uchar* pStream, long& lOffset, bool b = true);
		/// 编码
		bool Serialize(DBWriteSet& setWriteDB, bool b = true);
		/// 解码
		bool Unserialize(DBReadSet& setReadDB, bool b = true);
	};

	typedef vector<tagAddonPropertyValue> AddonPropertyValueList;

	/// 附加属性结构
	struct tagAddonProperty
	{
		/// 附加属性的类型		
		eGoodsAddonProperties gapType;
		/// 该属性是否被激活，为以后的组合属性预留		
		bool bIsEnabled;
		/// 是否是隐藏属性，不显示。		
		bool bIsImplicitAttribute;		
		/// 属性值列表		
		AddonPropertyValueList vValues;
		
		tagAddonProperty();
		~tagAddonProperty();	
		/// 清理
		void Clear();
		/// 编码
		bool Serialize(vector<uchar>* pStream, bool b = true);
		/// 解码
		bool Unserialize(uchar* pStream, long& lOffset, bool b = true);
		/// 编码
		bool Serialize(DBWriteSet& setWriteDB, bool b = true);
		/// 解码
		bool Unserialize(DBReadSet& setReadDB, bool b = true);
	};

	/// 套装属性
	struct tagSuitProperty
	{
		/// 属性类型
		eGoodsAddonProperties gapType;
		/// 属性值列表
		AddonPropertyValueList vValues;
		/// 是否激活
		bool bActived;

		tagSuitProperty();		
		~tagSuitProperty();
		/// 清理
		void Clear();
		/// 编码
		bool Serialize(vector<uchar>* pStream, bool b = true);
		/// 解码
		bool Unserialize(uchar* pStream, long& lOffset, bool b = true);
		/// 编码
		bool Serialize(DBWriteSet& setWriteDB, bool b = true);
		/// 解码
		bool Unserialize(DBReadSet& setReadDB, bool b = true);
	};

	/// 物品激发属性
	struct tagTriggerProperty
	{
		/// 属性类型
		eGoodsAddonProperties  gapType;
		/// 值1
		long lVal1;
		/// 值2
		long lVal2;
		/// 激发等级
		ushort wTriggerLevel;
	};

	/// 物品信息
	struct tagGoodsInfo
	{
		/// 物编ID
		ulong dwGoodsId;
		/// 数量
		ulong dwAmount;
	};

	/// 物品删除信息
	struct tagGoodsDelInfo
	{
		/// 位置
		ulong dwPos;
		/// 数量
		ulong dwNum;
	};

	/// 图标信息
	struct tagIcon
	{	
		/// 图标类型
		eIconType itIconType;
		/// 图标ID
		ulong dwIconID;
	};

	typedef vector<tagAddonProperty> AddonPropertyList;
	typedef vector<tagSuitProperty> SuitPropertyList;
	typedef vector<tagTriggerProperty> TriggerPropertyList;
	typedef vector<tagIcon> IconList;

private:
	/// 物编ID
	ulong m_index;
	/// 原始名
	string m_strOriginalName;
	/// 显示名
	string m_strName;
	/// 物品价格
	ulong m_dwPrice;
	/// 物品银币价格
	ulong m_dwSilverPrice;
	/// 物品描述
	string m_strDescribe;
	/// 物品基本类型
	eGoodsBaseType m_GoodsBaseType;
	/// 附加属性集
	AddonPropertyList m_vAddonProperties;
	/// 套装属性集
	SuitPropertyList m_vSuitProperties;
	/// 激发属性集
	TriggerPropertyList m_vTriggerProperties;
	/// 图标集
	IconList m_vIcons;

	friend class GoodsSetup;
	friend class CGoodsFactory;
	
public:
	CGoodsBaseProperties();
	~CGoodsBaseProperties();
	/// 编码
	bool Serialize(vector<uchar>* pStream, bool b = true);
	/// 解码
	bool Unserialize(uchar* pStream, long& lOffset, bool b = true);
	/// 编码
	bool Serialize(DBWriteSet& setWriteDB, bool b = true);
	/// 解码
	bool Unserialize(DBReadSet& setReadDB, bool b = true);

	/// 获取物编ID
	ulong GetIndex(void) {return m_index;}
	/// 获取原始名
	const char* GetOriginalName();
	/// 获取显示名
	const char* GetName();
	/// 获取描述
	const char* GetDescribe();
	/// 获取价格
	ulong GetPrice();
	/// 根据图标类型获取图标ID
	ulong GetIconID( eIconType itType );
	/// 获取银币价格
	ulong GetSilverPrice(){return m_dwSilverPrice;}
	/// 获取物品基本类型
	eGoodsBaseType GetGoodsBaseType(){return m_GoodsBaseType;}
	/// 获取附加属性集
	AddonPropertyList& GetAddonProperty(){return m_vAddonProperties;}
	/// 获取套装属性集
	SuitPropertyList* GetSuitProperties(){return &m_vSuitProperties;}
	/// 获取激发属性集
	TriggerPropertyList& GetTriggerProperties(){return m_vTriggerProperties;}
	/// 根据附加属性类型获取附加属性
	void GetAddonPropertyValues(eGoodsAddonProperties gapType, AddonPropertyValueList& vOut);
	/// 根据附加属性类型和附加属性编号获取附加属性值
	ulong GetAddonPropertyValue(eGoodsAddonProperties gapType,ulong dwId);
	/// 获取所有没有被禁止的有效附加属性集
	void GetValidAddonProperties(vector<eGoodsAddonProperties>& vOut);
	/// 判断附加属性是否隐藏
	bool IsImplicit(eGoodsAddonProperties gapType);
	/// 是否有这个附加属性
	bool IsHasAddonPropertie(eGoodsAddonProperties gapType);

private:
	/// 清理
	void Clear();
	/// 设置物品基本类型
	void SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
};
