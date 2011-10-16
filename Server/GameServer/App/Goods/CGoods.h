/** 
*  @file   CGoods.h
*  @author
*
*  @brief  物品的实现类
*  @date   
*/

#pragma once

#include "../Shape.h"
#include "../../../../Public/ServerPublic/Server/Common/GoodsBaseProperties.h"

class CGoodsFactory;
class CEnchaseHole;

class CGoods 
: public CShape
{
public:
	//附加属性
	struct tagAddonProperty
	{
		//附加属性的类型		
		eGoodsAddonProperties gapType;		
		//该属性带的值		
		long	lValues[2];
		
		tagAddonProperty();
		
		~tagAddonProperty();
		
		void Clear();
		
		bool Serialize(vector<uchar>* pStream, bool b = true);
		
		bool Unserialize(uchar* pStream, long& lOffset, bool b = true);

		bool Serialize(DBWriteSet& setWriteDB, bool b = true);	
		bool Unserialize(DBReadSet& setReadDB, bool b = true);
	};


	struct tagEnchaseHoleProperty
	{
		bool bIsActived;//是否被开发
		CGoods* pCard;//所镶卡片
	};
	
	bool  SetAttribute(ulong ID, ulong value, int valuePos=0/*第pos个值*/);
    ulong GetAttribute(ulong ID, int valuePos=0/*第pos个值*/);
	bool  AddAttribute(ulong ID, ulong value, int valuePos=0/*第pos个值*/);
    void  ChangeAttribute(ulong ID, ulong value, int valuePos=0/*第pos个值*/, bool bOverload = false);
    long  UpdateAttribute(/*const CGUID&ownerID, */const CGUID& sendPlayerID);

    // 脏属性数据 在向客户端发送时重新获取最新数据
    typedef struct tagDIRTYATTRDATA
    {
        ulong ID;
        int valuePos;

        tagDIRTYATTRDATA()
            :   ID(0),
                valuePos(0){}
        tagDIRTYATTRDATA(ulong id, int pos)
            :   ID(id),
            valuePos(pos){}

    }DIRTYATTRDATA;
    list<DIRTYATTRDATA>  m_listDirtyAttrData;


protected:	
	void SetBasePropertiesIndex(ulong dwID);	
	
public:
	bool SetAddonPropertyBaseValues(eGoodsAddonProperties gapType, ulong dwID, long lValue);	
	bool AddAddonPropertyBaseValues(eGoodsAddonProperties gapType, ulong dwID, long lVal1,long lVal2=0,bool bEnable=true,bool bIsImplicit=false);
	bool DelAddonPropertyBaseValues(eGoodsAddonProperties gapType);
	bool DelAddonPropertyNotInBase(void);
	
	CGoods();
	~CGoods();
	
	ulong GetBasePropertiesIndex();

	vector<tagAddonProperty>* GetAddonProperties(){return &m_vAddonProperties;}
	
	void SetAmount(ulong dwAmount);
	ulong GetAmount();
	void SetBuyPrice(ulong dwPrice){m_dwBuyPrice=dwPrice;}
	ulong GetBuyPrice(){return m_dwBuyPrice;}

	//返回物品的单价	
	ulong GetPrice();
	ulong GetSilverPrice(){ return m_baseProperties->GetSilverPrice(); }

	/// 获取物品基础属性
	CGoodsBaseProperties* GetBaseProperties(void) { return m_baseProperties; }
	/// 设置物品基础属性
	void SetBaseProperties(CGoodsBaseProperties* baseProperties) { m_baseProperties = baseProperties; }

	//获取最大堆叠数。该属性只能获取，不能设置。
	ulong GetMaxStackNumber();

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

	bool CanUpgraded();
	bool CanReparied();

	eGoodsBaseType GetGoodsBaseType(){ return m_baseProperties->GetGoodsBaseType(); }

	eEquipType QueryEquipType();
	eWeaponType GetWeaponType();
	eWeaponHandType GetWeaponHandType();
	eBusinessGoodsType GetBusinessGoodsType();

	/// 获取物品装备类型
	eEquipType GetEquipType(void);
	/// 获取物品法宝类型
	eTrumpType GetTrumpType(void);
	

	//是否可以镶嵌
	bool CanEnchase();
	//可打孔上限	
	long* GetEnchaseHoleData();

	ulong GetMaxEnchaseHoleNum();
	ulong GetActivedHoleNum();
	ulong GetEnchasedCardNum();
	void GetEnchasedCard(vector<ulong>& vecCards);
	CGoods* GetEnchasedCard(int nIndex);
	void UpdateEnchaseAttr();

	bool Enchase(CPlayer* pPlayer,CGoods* pGoods,int nIndex);
	bool RemoveCard(CPlayer* pPlayer,int nIndex,bool bReserve);
	ulong GetEnchaseNumByGoodsId(ulong dwId);
	void EnchaseAttrUpdate(CPlayer* pPlayer,CGoods* pCard);
	void EnchaseAttrRemove(CPlayer* pPlayer,CGoods* pCard);
	void SetActivedKitCardId(ulong dwId){m_dwActivedKitCardId=dwId;}
	ulong GetActivedKitCardId(){return m_dwActivedKitCardId;}


	//是否可以重叠
	bool CanStacked();
	bool IsSuitGoods();

	void RunScriptOnGet(CPlayer* pPlayer);
	void RunScriptOnLose(CPlayer* pPlayer);
	void RunScriptOnEquip(CPlayer* pPlayer);
	void RunScriptOnRemoveEquip(CPlayer* pPlayer);
	void OnSuitNumChanged(CPlayer* pPlayer,int new_num);
	void InvalidSuitProperties(CPlayer* pPlayer);

	void InitEnchaseHole();
	void EnchaseRelease();

	bool ActiveEnchaseHole(CPlayer* pPlayer,int nIndex);
	ulong ActiveEnchaseHole(CPlayer* pPlayer);
	bool CloseEnchaseHole(CPlayer* pPlayer,int nIndex);
	ulong CloseEnchaseHole();
	void EnchaseArrange();
	void ActiveHoles(int nNum);

	bool ActiveEnchaseHole(WORD wIndex);

	//返回所有显式的、被启用的附加属性，所有没有被禁止及所有隐
	//式属性都不会被返回	
	void GetValidAddonProperties(vector<eGoodsAddonProperties>& vOut);	
	
	//获取所有生效的附加属性，包括隐式属性	
	void GetEnabledAddonProperties(vector<eGoodsAddonProperties>& vOut);
	void GetAddonPropertyValues(eGoodsAddonProperties gapType, vector<long>& vOut);
	long GetAddonPropertyValues(eGoodsAddonProperties gapType, ulong dwID);
	bool HasAddonPropertyValues(eGoodsAddonProperties gapType);
	bool HasAddonProperty(eGoodsAddonProperties gapType);

	bool AddonPropertyUpdate(eGoodsAddonProperties gapProperty,eGoodsAddonProperties gapRange);

	void MountBind(const CGUID& playerID);
	void AddRefine(long lVal);

	//物品制造者名称 接口
	const char* GetMakerName(){return m_strMakerName.c_str();}
	bool SetMakerName(const char* pszMakerName){if(256 > strlen(pszMakerName)){ m_strMakerName=pszMakerName;return true;}return false;}

	/// “脚本增加的额外物品描述”相关接口
	/// 9.7.2009 : 使用制作者名字字符串保存此描述，物品该描述与制作者描述不能冲突。
	const char *GetScriptAddonDesc() const { return m_strMakerName.c_str(); }
	bool SetScriptAddonDesc( const std::string &desc ) {if(256 > desc.size()){ m_strMakerName = desc;return true;}return false; }

	/// 更新“脚本额外描述”到客户端
	void UpdateScriptAddonDesc( const CGUID &playerID );


	//获取物品的总价	
	ulong GetTotalPrice();
	
	void SetGoodsName(const CHAR* strGoodsName);
	
	const CHAR* GetGoodsName();		
	
	virtual void AI();
	
	virtual bool Clone(CBaseObject* pObject);
	
	virtual bool Serialize(vector<uchar>* pStream, bool b = true);
	
	virtual bool Unserialize(uchar* pStream, long& lOffset, bool b = true);

	bool SerializeForOldClient(vector<uchar>* pStream, bool b = true);

	virtual bool Serialize(DBWriteSet& setWriteDB, bool b = true);	
	virtual bool Unserialize(DBReadSet& setReadDB, bool b = true);
	bool SerializeForOldClient(DBWriteSet& setWriteDB, bool b = true);

	bool SerializeForRgnDisplay(DBWriteSet& setWriteDB,bool b=true);
	
	virtual const CHAR* ToString();
	
	virtual void Release();

	friend class CGoodsFactory;

private:
	/// 物编ID	
	ulong m_dwBasePropertiesIndex;
	/// 物品基本属性
	CGoodsBaseProperties* m_baseProperties;
	/// 数量
	ulong m_dwAmount;

	ulong m_dwBuyPrice;

	/// 制造者名字
	string m_strMakerName;
	/// 物品附加属性集
	vector<CGoods::tagAddonProperty> m_vAddonProperties;
	/// 物品套装属性集
	vector<CGoodsBaseProperties::tagSuitProperty> m_vSuitProperties;

	/// 激活套卡ID
	ulong m_dwActivedKitCardId;
	/// 最大镶嵌孔数
	ulong m_dwMaxHoleNum;
	/// 镶嵌卡属性
	CEnchaseHole** m_pEnchaseHoleArray;

private:
	//void AddRefine(long lVal);
	//void EDCalculate();
	/// 计算出该物品增加的PK CP值
	//void CalculatePKCP();
	//void AtkEDUpdate();
	//void DefEDUpdate();	
	//bool ImmunityUpdate();//免伤更新
	//bool PierceUpdate();//穿透更新
	//bool LuckyUpdate();//幸运更新
	//bool ConstellationUpdate();//星座更新
	//bool AddonHpUpdate();//hp上限追加范围
};
