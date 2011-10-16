//======================================================================================
/**
*	@file		KitCardRuleSetup.h
*	@data		2010-01-14 10:06
*	@author		wing<totti19851101@gmail.com>
*/
//======================================================================================

#pragma once


/**
*	@class KitCardRuleSetup
*
*	@brief 套卡配置类
*/

class KitCardRuleSetup
{
public:
	/// 套卡价格结构
	struct tagCardPrice
	{
		DWORD dwEquipLev;				// 装备等级
		DWORD dwOpenHolePrice;			// 开孔价格
		DWORD dwEnchasePrice;			// 镶嵌价格
	};

	/// 套卡附加属性值
	struct tagRuleValue
	{
		string strType;					//属性类别
		long  lVal;						//附加属性值
	};

	/// 套卡结构
	struct tagKitCard
	{	
		DWORD RuleId;					//套卡规则ID
		uchar RequireHoleNum;			//要求开孔的数量
		uchar RequireCardNum;			//套卡决定数量
		vector<long> CardIdArray;		//套卡各ID
		vector<tagRuleValue> AttrList;	//属性修正集合
	};

	/// 套卡属性
	struct KitCardRule
	{
		/// 套卡属性结构
		tagKitCard tagKitCardRule;

		/// 套卡属性默认构造
		KitCardRule();

		/// 
		KitCardRule(DWORD dwId,uchar btHoleNum,uchar btCardNum);

		/// 套卡属性析构
		~KitCardRule();
		
		/// 套卡属性编码
		BOOL Serialize(vector<uchar>* pStream, BOOL b = TRUE);

		/// 套卡属性解码
		BOOL Unserialize(uchar* pStream, long& lOffset, BOOL b = TRUE);	

		/// 取得套卡ID
		DWORD GetRuleId(){return tagKitCardRule.RuleId;}

		/// 取得套卡开孔数量
		uchar GetRequireHoleNum(){return tagKitCardRule.RequireHoleNum;}

		/// 取得套卡决定数量
		uchar GetRequireCardNum(){return tagKitCardRule.RequireCardNum;}
	};

public:
	typedef map<int,KitCardRule*> MAP_KITCARDRULE;
	typedef MAP_KITCARDRULE::iterator ITR_KITCARDRULE;

public:	
	/// 构造
	KitCardRuleSetup();

	/// 析构
	~KitCardRuleSetup();

	/// 编码
	static BOOL Serialize(vector<uchar>* pStream, BOOL b = TRUE);

	/// 解码
	static BOOL Unserialize(uchar* pStream, long& lOffset, BOOL b = TRUE);

	/// 加载套卡配置
	static BOOL Load(const char* StrPath);

	/// 清理
	static void Release();

	/// 根据等级取得开孔价格
	static DWORD GetOpenHolePrice(DWORD dwLev);

	/// 根据等级取得镶嵌价格
	static DWORD GetEnchasePrice(DWORD dwLev);

	/// 取得套卡集合列表
	static MAP_KITCARDRULE* GetKitCardList();

private:
	/// 套卡集合列表
	static MAP_KITCARDRULE m_mRules;

	/// 开孔和镶嵌价格列表
	static vector<tagCardPrice> m_vecCardPrice;
};