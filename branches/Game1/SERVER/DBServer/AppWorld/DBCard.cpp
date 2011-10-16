#include "StdAfx.h"
#include "..\..\..\nets\netdb\message.h"
#include "..\..\..\dbaccess\worlddb\RSCards.h"
#include "DBentityManager.h"
#include "DBCard.h"
#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBCard::CDBCard()
: CBaseDBEntity(),
m_lSellerID(NULL_GUID),m_lBuyerID(NULL_GUID),m_dwPrice(0),m_dwChargePrice(0),
m_dwCardState(0),m_dwCardTime(0),m_dwStartTime(0),m_dwSerialNumber(0)
{
	memset(m_CardNumber, 0, sizeof(m_CardNumber));
	memset(m_CardType, 0, sizeof(m_CardType));
	memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
	memset(m_BuyerAccount, 0, sizeof(m_BuyerAccount));
}
CDBCard::CDBCard(long id)
: m_ID(id),
m_lSellerID(NULL_GUID),m_lBuyerID(NULL_GUID),m_dwPrice(0),m_dwChargePrice(0),
m_dwCardState(0),m_dwCardTime(0),m_dwStartTime(0),m_dwSerialNumber(0)
{
	memset(m_CardNumber, 0, sizeof(m_CardNumber));
	memset(m_CardType, 0, sizeof(m_CardType));
	memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
	memset(m_BuyerAccount, 0, sizeof(m_BuyerAccount));
}
CDBCard::~CDBCard()
{

};
