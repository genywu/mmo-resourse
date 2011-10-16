#include "StdAfx.h"


#include ".\Card.h"

#include "..\nets\networld\message.h"
#include "../../Server/setup/GlobeSetup.h"
#include "..\..\worldserver\game.h"

//#include "..\dbaccess\worlddb\DBCards.h"




CCard::CCard(void) :
_CardNumber(""), _CardPassword(""), _CardType("")
{
	//_CardNumber		= "";
	//_CardPassword	= "";

//	_SerialNumber	= 0;
	//_CardType		= string("");
	_CardState		= CS_UNKNOWN;
	_CardTime		= CT_0;

	_SellerID		= NULL_GUID;
	_BuyerID		= NULL_GUID;

	_Price			= 0;	
	_ChargePrice	= 0;
//	_CardTimeTick	= 0;
}

CCard::~CCard(void)
{
}

//bool CCard::Sell()
//{
//	SetCardState(CCard::eCardState::CS_SELLING);
//	SetStartTime(timeGetTime());
//	return true;
//}
//
//bool CCard::Reserve(long BuyerID)
//{
//	//SetCardState(CCard::eCardState::CS_RESERVING);
//	//SetBuyerID(BuyerID);
//	return true;
//}
//
//bool CCard::Cancel() 
//{
//	return true;
//}