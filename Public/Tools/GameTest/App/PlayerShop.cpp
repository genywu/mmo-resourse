



#include "stdafx.h"
#include "PlayerShop.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CPlayerShop::DecodeFromDataBlock(DBReadSet& dbRead)
{
	dbRead.GetBufferFromByteArray(m_SessionGuid);
	m_lPlugID = 0;
	ZeroMemory(m_szName,32);
	ZeroMemory(m_szTradePlayerName,32);

	SetOpened(false);
	if(m_SessionGuid != NULL_GUID)
	{
		m_lPlugID = dbRead.GetLongFromByteArray();
		char strPlayerShopName[17] = {};
		dbRead.GetStringFromByteArray(strPlayerShopName,17);

		SetName(strPlayerShopName);
		SetOpened(true);			
	}
}

void CPlayerShop::Release()
{
}