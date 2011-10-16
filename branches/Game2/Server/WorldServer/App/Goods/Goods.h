/** 
*  @file   Goods.h
*  @author
*
*  @brief  物品实现类
*  @date   
*/

#pragma once

#include "../Shape.h"
#include "../../../../Public/ServerPublic/Server/Common/GoodsBaseProperties.h"

class CGoodsFactory;
class CRsPlayer;
class CEnchaseHole;

class CGoods 
: public CShape
{
public:
	/// 附加属性
	struct tagAddonProperty
	{
		/// 附加属性的类型		
		eGoodsAddonProperties gapType;		
		/// 该属性带的值		
		long lValues[2];

		tagAddonProperty();
		~tagAddonProperty();

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

public:
	CGoods();
	CGoods(OBJECT_TYPE otType);
	~CGoods();

	/// 获取物品附加属性列表
	vector<CGoods::tagAddonProperty>& GetAllAddonProperties(void);

	/// 设置GUID
	void SetExID(CGUID& guid){m_guExID = guid;}
	/// 获取物编索引ID
	ulong GetBasePropertiesIndex(void);
	
	/// 设置数量
	void SetAmount(ulong dwAmount);
	/// 获取数量
	ulong GetAmount(void);
	/// 获取银币价格
	ulong GetSilverPrice(void){return m_baseProperties->GetSilverPrice();}
	/// 获取物品类型
	eGoodsBaseType GetGoodsBaseType(void){return m_baseProperties->GetGoodsBaseType();}
	/// 获取价格
	ulong GetPrice(void);
	/// 获取名称
	const char* GetGoodsName(void);
	/// 获取描述
	const char* GetDescribe(void);

	/// 获取物品装备类型
	eEquipType GetEquipType(void);
	/// 获取物品法宝类型
	eTrumpType GetTrumpType(void);

	//获取物品的总价	
	ulong GetTotalPrice(void);
	/// 获取最大镶嵌孔
	ulong GetMaxEnchaseHoleNum(void);
	/// 是否可以修理
	bool CanReparied();	
	/// 初始化镶嵌孔
	void InitEnchaseHole(void);
	/// 取得激活镶嵌孔数量
	ulong GetActivedHoleNum(void);
	/// 镶嵌重排
	void EnchaseArrange();
	/// 是否可以镶嵌
	bool CanEnchase();
	/// 可打孔上限	
	long* GetEnchaseHoleData();
	/// 设置镶嵌孔状态
	void SetEnchaseHoleState(int nIndex,bool bVal);

	/// 取得物品基础属性
	CGoodsBaseProperties* GetBaseProperties(void) { return m_baseProperties; }
	/// 获取物品制造者名称
	const char* GetMakerName(){return m_strMakerName.c_str();}
	/// 设置物品制造者名称
	void SetMakerName(const char* pszMakerName){m_strMakerName=pszMakerName;}
	
	/// “脚本增加的额外物品描述”相关接口
	/// 9.7.2009 : 使用制作者名字字符串保存此描述，物品该描述与制作者描述不能冲突。
	const char *GetScriptAddonDesc() const { return m_strMakerName.c_str(); }
	void SetScriptAddonDesc( const std::string &desc ) { m_strMakerName = desc; }

	/// 镶嵌
	bool Enchase(CGoods* pGoods,int nIndex);

	//返回所有显式的、被启用的附加属性，所有没有被禁止及所有隐
	//式属性都不会被返回。	
	void GetValidAddonProperties(vector<eGoodsAddonProperties>& vOut);	
	//获取已经修正过的值	
	void GetAddonPropertyValues(eGoodsAddonProperties gapType, vector<long>& vOut);
	long GetAddonPropertyValues(eGoodsAddonProperties gapType, ulong dwID);

	// 该属性是否存在改物品上。
	bool IsAddonProperyExist(eGoodsAddonProperties gapType);
	bool SetAddonPropertyBaseValues(eGoodsAddonProperties gapType, ulong dwID, long lValue);
	bool AddAddonPropertyBaseValues(eGoodsAddonProperties gapType,ulong dwID,long lVal1,long lVal2=0,bool bEnable=true,bool bIsImplicit=FALSE);
	bool DelAddonPropertyBaseValues(eGoodsAddonProperties gapType);
	void AddAddonProperty(uchar* pStream, long size );
	
	/// 克隆
	virtual bool Clone(CBaseObject* pObject);
	/// 编码
	virtual bool Serialize(vector<uchar>* pStream, bool b = true);
	/// 解码
	virtual bool Unserialize(uchar* pStream, long& lOffset, bool b = true);
	/// 编码
	virtual bool Serialize(DBWriteSet& setWriteDB, bool b = true);
	/// 解码
	virtual bool Unserialize(DBReadSet& setReadDB, bool b = true);
	/// 清除
	virtual void Release();

	friend class CGoodsFactory;

private:
	/// 设置索引ID
	void SetBasePropertiesIndex(ulong dwID);
	/// 设置物品基础属性
	void SetBaseProperties(CGoodsBaseProperties* baseProperties) { m_baseProperties = baseProperties; }
	/// 清理物品镶嵌属性
	void EnchaseRelease(void);

private:
	/// 物编索引ID
	ulong m_dwBasePropertiesIndex;
	/// 物品基础属性
	CGoodsBaseProperties* m_baseProperties;
	/// 数量
	ulong m_dwAmount;
	/// 制造者名字
	string m_strMakerName;

	/// 附加属性列表
	vector<CGoods::tagAddonProperty> m_vAddonPropertiesList;

	/// 镶嵌卡属性
	CEnchaseHole** m_pEnchaseHoleArray;
	/// 初始镶嵌孔数量
	int	m_nInitHoleNum;
	/// 激活套卡ID
	ulong m_dwActivedKitCardId;
};
