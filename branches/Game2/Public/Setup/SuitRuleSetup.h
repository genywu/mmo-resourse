//======================================================================================
/**
*	@file		SuitRuleSetup.h
*	@data		2010-01-12 15:47
*	@author		wing<totti19851101@gmail.com>
*	@brief		套装配置
*/
//======================================================================================

#pragma once


/**
*	@class SuitRuleSetup
*
*	@brief 套装配置类
*/

class SuitRuleSetup
{
public:
	/// 套装附加属性值
	struct tagRuleValue
	{
		/// 套装附加属性类型
		string strType;

		/// 套装附加属性值
		long  lVal;
	};

	/// 套装属性
	struct SuitRule
	{
		/// 套装部件
		map<int,string> mapEquip;

		/// 套装附加属性值
		map<int,vector<tagRuleValue*>*> mapAttr;

		/// 套装属性构造
		SuitRule();

		/// 套装属性析构
		~SuitRule();

		/// 套装属性编码
		BOOL Serialize(vector<uchar>* pStream, BOOL b = TRUE);

		/// 套装属性解码
		BOOL Unserialize(uchar* pStream, long& lOffset, BOOL b = TRUE);

		/// 套装属性清理
		void Release();
	};

public:
	/// 套装集合结构类型
	typedef map<int,SuitRule*> MAP_SUITRULE;

	/// 套装集合结构类型迭代器
	typedef MAP_SUITRULE::iterator ITR_SUITRULE;

public:
	/// 构造
	SuitRuleSetup();

	/// 析构
	~SuitRuleSetup();

	/// 编码
	static BOOL Serialize(vector<uchar>* pStream, BOOL b = TRUE);

	/// 解码
	static BOOL Unserialize(uchar* pStream, long& lOffset, BOOL b = TRUE);

	/// 加载套装配置
	static BOOL Load(const char* StrPath);

	/// 取得套装配置集合
	static MAP_SUITRULE* GetSuitRules() { return &m_mRules; }

	/// 清理
	static void Release();

private:
	/// 套装属性集合
	static MAP_SUITRULE m_mRules;
};