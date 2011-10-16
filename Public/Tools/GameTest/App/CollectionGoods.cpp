#include "stdafx.h"
#include "../App/CollectionGoods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCollectionGoods::CCollectionGoods()
{
	m_lCreatorType = 0;			// 创建关联者的类型
	m_CreatorGuid = CGUID::GUID_INVALID;
}

void CCollectionGoods::SetAction(WORD lState)
{
	if( m_wAction != lState )
	{
		m_wAction = lState;
	}
}

bool CCollectionGoods::DecordFromDataBlock(DBReadSet& readDB,bool bExData /* = true */)
{
	CMoveShape::DecordFromDataBlock(readDB,bExData);
	m_lCreatorType = readDB.GetLongFromByteArray();
	readDB.GetBufferFromByteArray(m_CreatorGuid);
	return true;
}