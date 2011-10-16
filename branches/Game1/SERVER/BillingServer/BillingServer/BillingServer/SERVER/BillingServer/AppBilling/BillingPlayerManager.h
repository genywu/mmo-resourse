#pragma once

/**
* @brief	Manage online players' account info
* @auther	Fox
* @data		2008/03/10
* @class	CBillingPlayerManager
*/

#include "..\public\guid.h"

class CBillingPlayerManager
{
public:
	CBillingPlayerManager(void);
	~CBillingPlayerManager(void);

public:
	struct tagAccInfo
	{
		long	lPlayerID;
		string	strPlayerID;					// Player ID
		long	lGSID;							// GS ID

		tagAccInfo(
			long	llPlayerID,
			string	sstrPlayerID,				// Player ID
			long	llGSID						// GS ID
			)
			: lPlayerID ( llPlayerID )
			, strPlayerID ( sstrPlayerID )
			, lGSID ( llGSID )
		{
		}
	};

	struct tagIncLogNode
	{
		double	fLogTime;
		string	strBuyerID;
		string	strBuyerIP;
		DWORD	dwYuanbao;
		long	lGoodsID;
		long	lGoodsNum;
		long	lLSID;
		long	lWSID;

		tagIncLogNode(
			double	ffLogTime,
			string	sstrBuyerID,
			string	sstrBuyerIP,
			DWORD	ddwYuanbao,
			long	llGoodsID,
			long	llGoodsNum,
			long	llLSID,
			long	llWSID
			)
			: fLogTime(ffLogTime)
			, strBuyerID( sstrBuyerID )
			, strBuyerIP( sstrBuyerIP )
			, dwYuanbao( ddwYuanbao )
			, lGoodsID( llGoodsID )
			, lGoodsNum( llGoodsNum )
			, lLSID( llLSID )
			, lWSID( llWSID )
		{
		}
	};

	struct tagTradeNode
	{
		long	lType;						// Trade Type
		long	lBuyerID;
		long	lSellerID;
		string	strBuyerID;					// Buyer ID
		string	strSellerID;				// Seller ID
		string	strBuyerIP;					// Buyer IP
		string	strSellerIP;				// Seller IP
		string	strBuyerName;
		string	strSellerName;
		DWORD	dwYuanbao;					// Yuanbao sum
		long	lGoodsID;					// Goods ID
		long	lGoodsNum;					// Goods Quantity
		long	lGSID;						// GS ID
		long	lSessID;					// Session ID
		long	lPlugID;					// Plug ID
		long	lLSID;						// LoginServer ID
		long	lWSID;						// WorldServer ID
		CGUID	guidGoods;					// Goods GUID

		tagTradeNode(
			long	llType,
			long	llBuyerID,
			long	llSellerID,
			string	sstrBuyerID,			// Buyer ID
			string	sstrSellerID,			// Seller ID
			string	sstrBuyerIP,			// Buyer IP
			string	sstrSellerIP,			// Seller IP
			string	sstrBuyerName,
			string	sstrSellerName,
			DWORD	ddwYuanbao,				// Yuanbao sum
			long	llGoodsID,				// Goods ID
			long	llGoodsNum,				// Goods Quantity
			long	llGSID,					// GS ID
			long	llSID,					// Session ID
			long	llPID,					// Plug ID
			long	llLSID,					// LoginServer ID
			long	llWSID,					// WorldServer ID
			CGUID	&gGuid					// Goods GUID
			)
			: lType ( llType )
			,lBuyerID ( llBuyerID )
			, lSellerID ( llSellerID )
			, strBuyerID ( sstrBuyerID )
			, strSellerID ( sstrSellerID )
			, strBuyerIP ( sstrBuyerIP )
			, strSellerIP ( sstrSellerIP )
			, strBuyerName ( sstrBuyerName )
			, strSellerName ( sstrSellerName )
			, dwYuanbao ( ddwYuanbao )
			, lGoodsID ( llGoodsID )
			, lGoodsNum ( llGoodsNum )
			, lGSID ( llGSID )
			, lSessID ( llSID )
			, lPlugID ( llPID )
			, lLSID (llLSID)
			, lWSID (llWSID)
			, guidGoods ( gGuid )
		{
		}
	};

	///
	/// Add some a request account info into queue
	/// return the result of adding
	///
	static bool PushACQueue( const tagAccInfo &rAccInfo );

	///
	/// Check empty or not
	///
	static bool IsEmptyACQueue();

	///
	/// Add some a incrementshop log into queue
	/// return the result of adding
	///
	static bool PushILQueue( const tagIncLogNode &rIncLog );

	///
	/// Check empty or not
	///
	static bool IsEmptyILQueue();

	///
	/// Add some a request trade info into queue
	/// return the result of adding
	///
	static bool PushTRQueue( const tagTradeNode &rTradeNode );

	///
	/// Check empty or not
	///
	static bool IsEmptyTRQueue();

	///
	/// Run in Mainloop process query or charge request
	///
	bool	Run();

	void	Release();

	HANDLE	CreateThread();

	static	bool Start(bool bLogSwitch = false);
	static	bool End(bool bLogSwitch = false);

	static	bool OnLogProcess();

private:
	DWORD	m_dwThreadID;
	HANDLE	m_hThread;
	static	HANDLE s_hLogThread;
	static	list<tagAccInfo> m_AccInfoReqQueue;		// Request Account Info Queue
	static	list<tagIncLogNode> m_IncLogQueue;		// IncrementShop Log Insert Queue
	static	list<tagTradeNode> m_TradeReqQueue;		// Request Trade Info Queue
	static	CRITICAL_SECTION m_CriticalSectionAC;
	static	CRITICAL_SECTION m_CriticalSectionIL;
	static	CRITICAL_SECTION m_CriticalSectionTR;
};