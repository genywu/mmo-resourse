#pragma once
#include "..\myadobase.h"
#include "..\..\server\dbserver\appworld\DBcard.h"

class CMessage;

class CRSCards :
	public CMyAdoBase
{
public:
	CRSCards(void);
	~CRSCards(void);
public:
	bool LoadAllCards();
	bool AddNew(CDBCard *card);			// 添加一个新的寄售卡
	bool Delete(CDBCard *card);			// 删除
	bool UpdateSell(CDBCard *card);		// 更新成寄售状态
	bool UpdateReserve(CDBCard *card);	// 更新成寄售状态
	bool SaveLargess(CDBCard *card);	

	bool Load(CDBCard* card);
	bool SaveAll(void);
};

extern CRSCards g_RSCards;