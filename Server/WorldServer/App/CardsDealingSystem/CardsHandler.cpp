#include "StdAfx.h"
#include "CardsHandler.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCardsHandler *CCardsHandler::instance = NULL;

// 得到唯一实例句柄
CCardsHandler* CCardsHandler::getInstance()
{
	if(!instance) {
		instance = new CCardsHandler;
	}
	return instance;
}

//删除唯一实例句柄
void CCardsHandler::delInstance()
{
	SAFE_DELETE(instance);
}

CCardsHandler::CCardsHandler(void)
{
}

CCardsHandler::~CCardsHandler(void)
{
}

bool CCardsHandler::Initialize()
{
	//return _DBCards.LoadAllCards();
	return true;
}

int CCardsHandler::InsertCard(CCard *card)
{
	if(!card) return 1;

	if( NULL_GUID == card->GetSellerID() ) return 2;	// 

	if( !card->GetCardNumber() && !strlen(card->GetCardNumber()) ) return 2;

	CCard *pCard = _Cards[card->GetCardNumber()]; // == _Cards.find();

	if(!pCard) 
	{
		_Cards[card->GetCardNumber()] = card;
		return 0;
	}

	return 2;
}

DWORD CCardsHandler::QueryChargePriceByTimeType( CCard::eCardTime cardtime )
{
	switch( cardtime )
	{
	case CCard::CT_6:
		return GlobalSetup::GetSetup()->m_dwCard6HoursChargePrice;
	case CCard::CT_12:
		return GlobalSetup::GetSetup()->m_dwCard12HoursChargePrice;
	case CCard::CT_24:
		return GlobalSetup::GetSetup()->m_dwCard24HoursChargePrice;
	default:
		return 0;
	}
}

DWORD CCardsHandler::QueryTimeByTimeType( CCard::eCardTime cardtime )
{
	switch( cardtime )
	{
	case CCard::CT_6:
		return GlobalSetup::GetSetup()->m_dwCard6Hours;
	case CCard::CT_12:
		return GlobalSetup::GetSetup()->m_dwCard12Hours;
	case CCard::CT_24:
		return GlobalSetup::GetSetup()->m_dwCard24Hours;
	default:
		return 0;
	}
}

DWORD CCardsHandler::GetSellingCardsNum()
{
	DWORD n=0;
	for( map<string,CCard*>::iterator it=_Cards.begin(); it!=_Cards.end(); it++ )
	{
		if(it->second->GetCardState() == CCard::CS_SELLING)
		{
			n++;
		}
	}
	return n;
}

int CCardsHandler::RemoveCard(CCard *card)
{
	if(!card) return 1;
	_Cards.erase( card->GetCardNumber() );
	
	return 0;
}

bool CCardsHandler::DBAddNew(CCard *card) 
{

	return false;
}
bool CCardsHandler::DBDelete(CCard *card) 
{
	return false;
}


//bool CCardsHandler::DBUpdateSell(CCard *card) 
//{
//	return _DBCards.UpdateSell(card);
//}

bool CCardsHandler::DBUpdateReserve(CCard *card) 
{

	return true;
}

bool CCardsHandler::DBSaveLargess(CCard *card)
{

	return true;
}

void CCardsHandler::CardsLoop()
{
	
}


size_t CCardsHandler::getCardsNumByAccount(const char *account)
{
	size_t n = 0;
	for( map<string,CCard*>::iterator it= _Cards.begin(); it!=_Cards.end(); ++it )
	{
		if( strcmp(it->second->GetSellerAccount(), account) == 0 )
		{
			n++;
		}
	}
	return n;
}


CCard * CCardsHandler::QueryCardByCardNumber(const char *cardnumber)
{
	map<string,CCard*>::iterator it = _Cards.find(cardnumber);

	if(it != _Cards.end())
		return it->second;
	return NULL;
}

void CCardsHandler::AddDenys(CCard *card)
{
	const char *cardnum = card->GetCardNumber();
	const char *account = card->GetSellerAccount();

	if(!cardnum) 
		return ;

	map<string,DenyInfo*>::iterator itC = _DenyCards.find(cardnum);
	if( itC == _DenyCards.end() ) // insert
	{
		// init di
		DenyInfo *di = new DenyInfo;
		di->last_time = GetCurTime();
		di->times = 1;
		// inser di into map
		_DenyCards.insert(pair<string,DenyInfo*>(cardnum,di));
	}
	else
	{
		itC->second->times++;
		itC->second->last_time = GetCurTime();
	}

	if(!account) 
		return ;

	map<string,DenyInfo*>::iterator itA = _DenyAccounts.find(account);

	if( itA == _DenyAccounts.end() ) // insert
	{
		// init di
		DenyInfo *di = new DenyInfo;
		di->last_time = GetCurTime();
		di->times = 1;
		// inser di into map
		_DenyAccounts.insert(pair<string,DenyInfo*>(account,di));
	}
	else
	{
		itA->second->times++;
		itA->second->last_time = GetCurTime();
	}

}

// 返回0成功 1玩家讲被拒绝 2卡将被拒绝
int CCardsHandler::Check(CCard *card)
{
	// 分别检查帐号好卡号是否达到操作次数

	// 是否存在
	const char *cardnum = card->GetCardNumber();
	const char *account = card->GetSellerAccount();

	if(!cardnum) 
		return 501;

	map<string,DenyInfo*>::iterator itC = _DenyCards.find(cardnum);

	if( itC != _DenyCards.end() ) // insert
	{
		if(itC->second->times >= GlobalSetup::GetSetup()->m_dwCardRetryNum)
			return 501; // 
	}

	if(!account) 
		return 502;

	map<string,DenyInfo*>::iterator itA = _DenyAccounts.find(account);

	if( itA != _DenyAccounts.end() ) // insert
	{
		if(itA->second->times >= GlobalSetup::GetSetup()->m_dwCardRetryNum)
			return 502; // 
	}

	return 0;
}
