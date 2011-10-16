
/*
*	file:		GoodsTailorSetup.h
*	Brief:		非副职业制作配方设置
*	detail:		
*	Authtor:	张科智
*	Datae:		2009-1-8
*/


#include "GoodsExDef.h"
#include "DataBlockSetWriteRead.h"

#pragma once

//!-------------------------------------------------------------------------------------------------
//! 制作类型申明
enum eMakeType
{
	eMT_FORM_GOODS,
	eMT_FROM_NPC,
};

//! 一个物品配置信息/消耗物
struct	tagExpend
{
	LONG lGoodsIndex;
	LONG lGoodsNum;

	BOOL operator==(tagExpend& RightObj){ return (this->lGoodsIndex == RightObj.lGoodsIndex); }
};


//! 产物
struct tagProduction : public tagExpend
{
	LONG lOutOdds;

	BOOL operator==(tagExpend& RightObj){ return (this->lGoodsIndex == RightObj.lGoodsIndex); }
};


//!-------------------------------------------------------------------------------------------------
//! 一个配方清单
class OneMakeTable
{
public:
	OneMakeTable(VOID);
	~OneMakeTable(VOID){}

public:
	//!							编码
	VOID						Code					(DBWriteSet& setWriteSet) const;
	//!							解码
	VOID						Decode					(DBReadSet& setReadDB);

	//!							得到ID
	LONG						GetID					(VOID){ return m_lID; }
	//!							得到名称
	const char*					GetName					(VOID){ return m_szName; }
	//!							得到说明
	const char*					GetNarrate				(VOID){ return m_szNarrate; }

	//!							设置基本信息
	BOOL						SetBaseInfo				(LONG lID, const char *pName, const char *pNarrate);
	//!							添加一个耗费物
	BOOL						AddExpend				(tagExpend &Expend);
	//!							添加一个生成物
	BOOL						AddProduction			(tagProduction &Production);

	//!							耗费物
	LONG						GetExpendNum( VOID )			{ return (LONG)m_vExpend.size(); }
	tagExpend*					GetExpend( DWORD dwIndex )		{ return &m_vExpend[ dwIndex ]; }

	//!							生成物
	LONG						GetProductionNum( VOID )		{ return (LONG)m_vProduction.size(); }
	tagProduction*				GetProduction( DWORD dwIndex )	{ return &m_vProduction[ dwIndex ]; }

public:
	//!							比较运算
	BOOL						operator==				(OneMakeTable& RightObj){ return (this->m_lID == RightObj.m_lID); }

protected:
	LONG						m_lID;
	char						m_szName[MAKE_TABLE_NAME_SIZE];
	char						m_szNarrate[MAKE_TABLE_NARRATE_SIZE];
	//!							耗费物列表
	vector<tagExpend>			m_vExpend;
	//!							生成物列表
	vector<tagProduction>		m_vProduction;




private:
	OneMakeTable(OneMakeTable&);
	OneMakeTable& operator=(OneMakeTable&);
};


//!-------------------------------------------------------------------------------------------------
//! 配方配置
class GoodsTailorSetup
{
public:
	GoodsTailorSetup(VOID);
	~GoodsTailorSetup(VOID);

public:
	static	GoodsTailorSetup&	GetInstance				(VOID);

public:
	//!							编码
	VOID						Code					(DBWriteSet& setWriteSet);
	//!							解码
	VOID						Decode					(DBReadSet& setReadDB);

public:
	//!							清理所有配方
	VOID						Clear					(VOID);
	//!							添加一个配方
	BOOL						AddOneMakeTable			(OneMakeTable *pOneMakeTable);
	//!							找出一个配方
	const	OneMakeTable*		FindOneMakeTable		(LONG lID);
	
protected:
	//!												配方列表
	map<LONG, OneMakeTable*>						m_mapOneMakeTable;

	static	GoodsTailorSetup						c_GoodsTailorSetup;
};