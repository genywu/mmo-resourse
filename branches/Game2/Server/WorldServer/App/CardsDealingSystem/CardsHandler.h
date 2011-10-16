//
// 管理卡类：单实例类
// 主要管理卡
//
// 这里不用存盘操作，因为在CCard类里会及时进行存盘操作。
#pragma once
#include "Card.h"

class CCardsHandler
{
public:
	CCardsHandler(void);
	virtual ~CCardsHandler(void);

	bool Initialize();						// 载入，初始
	bool Uninitialize();

private:
	static CCardsHandler *instance;			//该类唯一实例句柄

//	CDBCards _DBCards;

public:
	static CCardsHandler* getInstance();	//得到唯一实例句柄
	static void delInstance();				//删除唯一实例句柄

	int InsertCard(CCard *card);
	int RemoveCard(CCard *card);


	CCard* QueryCardByCardNumber(const char *cardnumber);

	//bool QueryCardByPlayer(long playerid, vector<CCard*> &cards);
	//bool QueryCardByPlayer(const char *accmout, vector<CCard*> &cards);


	DWORD QueryChargePriceByTimeType( CCard::eCardTime cardtime);
	DWORD QueryTimeByTimeType( CCard::eCardTime cardtime);

	size_t getCardsNumByAccount(const char* account);
	size_t getCardsNum()	{return _Cards.size();}	// 当前寄售卡的数量

	DWORD GetSellingCardsNum();

	bool DBAddNew(CCard *card);				// 保存一个通过验证的新卡到数据库
	bool DBDelete(CCard *card);
	//bool DBUpdateSell(CCard *card);		//
	bool DBUpdateReserve(CCard *card);		//
	bool DBLoad();							// 服务器启动时，从数据库中载入

	bool DBSaveLargess(CCard *card);		// 交易成功后，写入赠送数据库，卖家登陆时，收取金钱

	void CardsLoop();						// 如果达到下线时间，寄售卡就下线


	void CancelAllCards();
public:
	map<string,CCard*> _Cards;				// 寄售中的卡 <卡号,CCard>

public:
	struct DenyInfo
	{
		BYTE	times;						// 次数
		DWORD	last_time;					// 最后操作时间
	};

	map<string,DenyInfo*> _DenyCards;		// 暂时无法寄售的玩家 
	map<string,DenyInfo*> _DenyAccounts;	// 暂时无法寄售的玩家 

	int Check(CCard* card);					// 根据严正返回的密码错误结果，进行检查。
	void AddDenys(CCard *card);
};
