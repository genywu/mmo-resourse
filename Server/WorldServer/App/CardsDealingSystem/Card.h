//
// 卡类：
// 卡的基本属性 包括卡的属性和状态
// 包括卡的买卖和取消
//
#pragma once

class CCard
{
	friend class CCardsHandler;  // Declare a friend class. CCardsHandler这个类用于管理CCard类
								 // CCardsHandler 是 CCard 的一个类聚
public:
	//// 卡类型
	//enum eCardType {
	//	CT_UNKNOWN = 0,			// 未知卡类型
	//	CT_10,					// 10元卡
	//	CT_30,					// 30元卡
	//	CT_MONTH,				// 包月卡（现在未开放的）
	//	CT_MOBILE				// 手机卡（现在未开放的）
	//};

	// 卡状态
	enum eCardState {
		CS_UNKNOWN = 0,			// 未知卡状态，或者非法卡状态
		CS_NORMAL,				// 正常
		CS_VALIDATED,			// 通过验证
		CS_SELLING,				// 寄售中状态
		CS_RESERVING,			// 预订状态
		CS_SELLED				// 售出状态
	};

	// 卡寄存有效时间 
	enum eCardTime {
		CT_0	= 0,
		CT_6	= 1,		// 6小时（单位：毫秒）
		CT_12,		        // 12小时
		CT_24		        // 24小时
	};

public:
	CCard(void);
	virtual ~CCard(void);

private:
	DWORD _SerialNumber;		// 序列号字符串，用于开始寄售时，对改卡操作的跟踪。GUID
	string _CardNumber;			// 卡号最大长度不会超过12位，现在是12位整的数字，以后可能出现字符
	string _CardPassword;		// 卡号最大长度不会超过10位


	string _CardType;		// 卡类型 计费服务器通过卡号进行识别
	eCardState _CardState;		// 卡状态
	eCardTime _CardTime;		// 有效寄售时间

	// 在寄售卡和购买卡时，操作未结束前，卖卡人和买卡人都必须在线。
	// 如果离线，该操作将会失败。寄售或购卡会取消。
	
	CGUID _SellerID;				// 卖卡人
	string _SellerAccount;
	
	CGUID _BuyerID;				// 买卡人
	string _BuyerAccount;
	

	
	DWORD	_StartTime;			// 卡的开始寄售时间 只有在寄售状态下才算开始。寄售计时 操过有效时间将取消寄售
	//time_t	_TimeStamp;			// 这个时间用于计算操作是否超时的，如果因为网络延迟超时，就会取消操作

	// 单卡最小金额和最大金额在全局配置文件中设置 每次交易金额应在该范围以内
	// 超出范围不会授予交易
	unsigned int _Price;		// 此卡欲出售价值（游戏币）
	unsigned int _ChargePrice;	// 手续费


public:
	void SetSerialNumber(DWORD serialnumber) { _SerialNumber = serialnumber;}	// 验证通过后，从计费服务器获得
	DWORD GetSerialNumber() {return _SerialNumber;}


	void SetCardNumber(const char *cardnumber) {_CardNumber = cardnumber;}
	const char *GetCardNumber() {return _CardNumber.data();}

	void SetCardType(const char *cardtype) {_CardType = cardtype;} // 验证通过后，从计费服务器获得
	const char *GetCardType() {return _CardType.data();}

	void SetCardState(eCardState cardstate) {_CardState = cardstate;}
	eCardState GetCardState() {return _CardState;}

	void SetCardTime(eCardTime cardtime) {_CardTime = cardtime;}
	eCardTime GetCardTime() {return _CardTime;}

	void SetSellerID(const CGUID& playerid) {_SellerID = playerid;}
	const CGUID& GetSellerID() {return _SellerID;}

	void SetSellerAccount(const char *acc) {_SellerAccount = acc;}
	const char *GetSellerAccount() {return _SellerAccount.data();}

	void SetBuyerID(const CGUID&  playerid) {_BuyerID = playerid;}
	const CGUID& GetBuyerID() {return _BuyerID;}

	void SetBuyerAccount(const char *acc) {_BuyerAccount = acc;}
	const char *GetBuyerAccount() {return _BuyerAccount.data();}

	void SetPrice(DWORD price) {_Price = price;}
	DWORD GetPrice() {return _Price;}

	void SetChargePrice(DWORD chargeprice) {_ChargePrice = chargeprice;}
	DWORD GetChargePrice() {return _ChargePrice;}

	void SetStartTime(DWORD starttime) {_StartTime = starttime;}
	DWORD GetStartTime() {return _StartTime;}
	
public:


	//bool Validate(const char *cardnumber,
	//	const char *cardpassword,
	//	DWORD sellprice,
	//	BYTE timetype,
	//	LONG playerid);			// 效验卡，发送卡号和密码
	//							// 返回卡类型， 卡状态， 是否是非法卡等信息

	//bool Sell();				// 卡被寄卖 状态：正常->寄卖
	//bool Reserve(long BuyerID);				// 卡被预定 状态：寄卖->预订
	//bool Deal();				// 卡被购买 状态：预定->售出
	//bool Cancel();				// 卡被取消 状态：寄卖->正常

};
