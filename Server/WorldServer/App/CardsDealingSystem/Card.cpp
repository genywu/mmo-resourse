#include "StdAfx.h"
#include "Card.h"
#include "../../Server/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCard::CCard(void) :
_CardNumber(""), _CardPassword(""), _CardType("")
{
	_CardState		= CS_UNKNOWN;
	_CardTime		= CT_0;

	_SellerID		= NULL_GUID;
	_BuyerID		= NULL_GUID;

	_Price			= 0;	
	_ChargePrice	= 0;
}

CCard::~CCard(void)
{
}
