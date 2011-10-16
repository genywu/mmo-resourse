

#include "StdAfx.h"
#include "CGameInfo.h"



//! 编码到数据集
BOOL CGameInfo::Code(DBWriteSet &setWriteDB) const
{
	setWriteDB.AddToByteArray(m_dwType);
	setWriteDB.AddToByteArray((LONG)m_vGameInfoData.size());

	for (LONG i = 0; i < m_vGameInfoData.size(); ++i)
	{
		setWriteDB.AddToByteArray((void*)&m_vGameInfoData[i], sizeof(tagGameInfoData));
	}

	return TRUE;
}
//! 从数据集解码
BOOL CGameInfo::DeCode(DBReadSet &setReadDB)
{
	m_dwType = setReadDB.GetDwordFromByteArray();
	LONG lSize = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lSize; ++i)
	{
		tagGameInfoData GameInfoData;
		setReadDB.GetBufferFromByteArray(&GameInfoData, sizeof(tagGameInfoData));
	}

	return TRUE;
}