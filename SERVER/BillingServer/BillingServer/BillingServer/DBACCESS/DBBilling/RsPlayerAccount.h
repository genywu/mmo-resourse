#pragma once
//#include "..\myadobase.h"

class CRsPlayerAccount
{
public:
	CRsPlayerAccount(void);
	~CRsPlayerAccount(void);

public:
	
	/* Get User Account Info
	[getUserPoint]
	@UserID VARCHAR(8),
	@Point  INT OUTPUT,
	@Result INT OUTPUT				--	0 : Sucess  -1 : Not Found User 
	*/
	static long GetUserPoint( const char *szUserID, long &lPoint );

	/* User Buy
	[buyItemCode]
	@UserID VARCHAR(12),			--  遊戲帳號
	@ItemIndex INT,
	@Qty INT,
	@Amount INT,					--  要扣的點數
	@IP VARCHAR(20),

	@World VARCHAR(50),				--  伺服器
	@Character_Name VARCHAR(50),	--  購買者角色名稱

	@Result INT OUTPUT,	--	0：成功	-1：Not Found User	-2：點數不足	-3：取得TranCode錯誤	-4：資料處理錯誤
	@TranCode VARCHAR(500) OUTPUT,	--  交易序號
	@LastPoint INT OUTPUT			--  玩家剩餘點數
	*/
	static long BuyItemCode( const string &strUserID, long lItemIndex, long lItemNum, long lAmount, const string &strServer,
		const string &strUserIP, const string &strCharacter, char *szTranCode, long &lLastPoint );

	/*
	buyPlayerItem
	參數名稱	型態	說明
	@UIDfrom	varchar(20)	賣家遊戲帳號
	@Ipfrom	varchar(20)	賣家來源IP
	@Cnamefrom	varchar(50)	賣家角色名稱
	@UIDto	varchar(20)	買家遊戲帳號
	@Ipto	varchar(20)	買家來源IP
	@Cnameto	varchar(50)	買家角色名稱
	@WorldId	int	交易的 World ID
	@ItemIdx	int	Item Idx
	@ItemNum	int	Item Num
	@Qty	int	交易數量
	@Amount	int	應扣總金額

	參數名稱	型態	說明
	@Result	int	0：成功
	-1：查無賣家遊戲帳號
	-2：查無買家遊戲帳號
	-3：買家點數不足
	-4：取得TranCode錯誤
	-5：資料處理錯誤
	@TranCode	varchar(500)	交易序號
	@UIDfromLastPoint	int	賣家剩餘點數
	@UIDtoLastPoint	int	買家剩餘點數
	*/
	static long BuyPlayerItem( const string &strBuyerID, const string &strBuyerIP, const string &strBuyerName,
		const string &strSellerID, const string &strSellerIP, const string &strSellerName, long lGSID,
		long lItemIndex, long lItemNum, long lAmount, char *szTranCode, long &lBuyerPoint, long &lSellerPoint );
};