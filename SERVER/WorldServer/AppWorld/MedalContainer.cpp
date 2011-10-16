
#include "StdAfx.h"
#include "MedalContainer.h"
#include "Goods\CGoodsFactory.h"



MedalContainer::MedalContainer(void)
{
}
MedalContainer::~MedalContainer(void)
{
	m_setHold.clear();
}





//! ±àÂë
void MedalContainer::AddToByteArray(vector<BYTE> &vData)
{
	_AddToByteArray(&vData, (LONG)m_setHold.size());
	for (set<LONG>::iterator ite = m_setHold.begin(); ite != m_setHold.end(); ++ite)
	{
		_AddToByteArray(&vData, *ite);
	}
}

//! ½âÂë
void MedalContainer::Decode(BYTE *pData, LONG &lPos)
{
	m_setHold.clear();
	LONG lNum = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lNum; ++i)
	{
		m_setHold.insert(_GetLongFromByteArray(pData, lPos));
	}
}

//!						±àÂë
void MedalContainer::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray( (LONG)m_setHold.size());
	for (set<LONG>::iterator ite = m_setHold.begin(); ite != m_setHold.end(); ++ite)
	{
		setWriteDB.AddToByteArray(*ite);
	}
}
	//!						±àÂë
void MedalContainer::DecodeFromDataBlock(DBReadSet& setReadDB)
{
	m_setHold.clear();
	LONG lNum = setReadDB.GetLongFromByteArray( );
	for (LONG i = 0; i < lNum; ++i)
	{
		m_setHold.insert(setReadDB.GetLongFromByteArray( ));
	}
}