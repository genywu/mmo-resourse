

#include "stdafx.h"
#include "GoodsTailorSetup.h"


//! OneMakeTable
//!-------------------------------------------------------------------------------------------------
OneMakeTable::OneMakeTable(VOID)
:m_lID(0)
{
	memset(m_szName, 0, MAKE_TABLE_NAME_SIZE);
	memset(m_szNarrate, 0, MAKE_TABLE_NARRATE_SIZE);
}

//! 编码
VOID OneMakeTable::Code(DBWriteSet& setWriteSet) const
{
	setWriteSet.AddToByteArray(m_lID);
	setWriteSet.AddToByteArray((void*)m_szName, MAKE_TABLE_NAME_SIZE);
	setWriteSet.AddToByteArray((void*)m_szNarrate, MAKE_TABLE_NARRATE_SIZE);

	setWriteSet.AddToByteArray((LONG)m_vExpend.size());
	for (LONG i = 0; i < m_vExpend.size(); ++i)
	{
		setWriteSet.AddToByteArray((void*)&m_vExpend[i], sizeof(tagExpend));
	}

	setWriteSet.AddToByteArray((LONG)m_vProduction.size());
	for (LONG i = 0; i < m_vProduction.size(); ++i)
	{
		setWriteSet.AddToByteArray((void*)&m_vProduction[i], sizeof(tagProduction));
	}
}

//! 解码
VOID OneMakeTable::Decode(DBReadSet& setReadDB)
{
	m_lID = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_szName, MAKE_TABLE_NAME_SIZE);
	setReadDB.GetBufferFromByteArray(m_szNarrate, MAKE_TABLE_NARRATE_SIZE);

	m_vExpend.clear();
	LONG lNum = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lNum; ++i)
	{
		tagExpend Expend;
		setReadDB.GetBufferFromByteArray(&Expend, sizeof(tagExpend));
		m_vExpend.push_back(Expend);
	}

	m_vProduction.clear();
	lNum = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lNum; ++i)
	{
		tagProduction Production;
		setReadDB.GetBufferFromByteArray(&Production, sizeof(tagProduction));
		m_vProduction.push_back(Production);
	}
}

//! 设置基本信息
BOOL OneMakeTable::SetBaseInfo(LONG lID, const char *pName, const char *pNarrate)
{
	if(NULL == pName || NULL == pNarrate)
		return FALSE;
	m_lID = lID;

	LONG lNameSize = strlen(pName);
	lNameSize = (lNameSize < MAKE_TABLE_NAME_SIZE) ? lNameSize : MAKE_TABLE_NAME_SIZE - 1;
	memcpy(m_szName, pName, lNameSize);

	LONG lNarrateSize = strlen(pNarrate);
	lNarrateSize = (lNarrateSize < MAKE_TABLE_NARRATE_SIZE) ? lNarrateSize : MAKE_TABLE_NARRATE_SIZE - 1;
	memcpy(m_szNarrate, pNarrate, lNarrateSize);

	return TRUE;
}

//! 添加一个耗费物
BOOL OneMakeTable::AddExpend(tagExpend &Expend)
{
	for (LONG i = 0; i < m_vExpend.size(); ++i)
	{
		if(Expend == m_vExpend[i])
			return FALSE;
	}
	m_vExpend.push_back(Expend);
	return TRUE;
}

//! 添加一个生成物
BOOL OneMakeTable::AddProduction(tagProduction &Production)
{
	for (LONG i = 0; i < m_vProduction.size(); ++i)
	{
		if(Production == m_vProduction[i])
			return FALSE;
	}
	m_vProduction.push_back(Production);
	return TRUE;
}




//! GoodsTailorSetup
//!-------------------------------------------------------------------------------------------------
GoodsTailorSetup GoodsTailorSetup::c_GoodsTailorSetup;

GoodsTailorSetup& GoodsTailorSetup::GetInstance(VOID)
{
	return c_GoodsTailorSetup;
}

GoodsTailorSetup::GoodsTailorSetup(VOID)
{

}
GoodsTailorSetup::~GoodsTailorSetup(VOID)
{
	Clear();
}

//! 编码
VOID GoodsTailorSetup::Code(DBWriteSet& setWriteSet)
{
	//! 数量
	setWriteSet.AddToByteArray((LONG)m_mapOneMakeTable.size());

	//! 数据
	map<LONG, OneMakeTable*>::iterator ite = m_mapOneMakeTable.begin();
	for (; ite != m_mapOneMakeTable.end(); ++ite)
	{
		ite->second->Code(setWriteSet);
	}
}

//! 解码
VOID GoodsTailorSetup::Decode(DBReadSet& setReadDB)
{
	Clear();

	LONG lNum = setReadDB.GetLongFromByteArray();

	for (LONG i = 0; i < lNum; ++i)
	{
		OneMakeTable* pOneMakeTable = new OneMakeTable();
		if(NULL != pOneMakeTable)
		{
			pOneMakeTable->Decode(setReadDB);
			if(!AddOneMakeTable(pOneMakeTable))
			{
				SAFE_DELETE(pOneMakeTable);
			}
		}
	}
}

//! 清理所有配方
VOID GoodsTailorSetup::Clear(VOID)
{
	map<LONG, OneMakeTable*>::iterator ite = m_mapOneMakeTable.begin();
	for (; ite != m_mapOneMakeTable.end(); ++ite)
	{
		SAFE_DELETE(ite->second);
	}
	m_mapOneMakeTable.clear();
}

//! 添加一个配方
BOOL GoodsTailorSetup::AddOneMakeTable(OneMakeTable *pOneMakeTable)
{
	if(NULL == pOneMakeTable)
		return FALSE;

	if(m_mapOneMakeTable.end() != m_mapOneMakeTable.find(pOneMakeTable->GetID()))
		return FALSE;

	m_mapOneMakeTable[pOneMakeTable->GetID()] = pOneMakeTable;
	return TRUE;
}

//! 找出一个配方
const OneMakeTable* GoodsTailorSetup::FindOneMakeTable(LONG lID)
{
	map<LONG, OneMakeTable*>::iterator ite = m_mapOneMakeTable.find(lID);
	if(m_mapOneMakeTable.end() == ite)
		return NULL;

	return ite->second;
}