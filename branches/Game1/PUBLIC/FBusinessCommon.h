///
/// @file FBusinessCommon.h
/// @brief public definitions in FBusiness(faction business) module for Client/GS.
///
#ifndef ___FBUSINESS_COMMON_H_
#define ___FBUSINESS_COMMON_H_

namespace FBusiness
{
	/// 消息操作类型
	enum S2COperType
	{
		S2C_SELL_PAGE = 0,
		S2C_BUY_PAGE,
		S2C_PRICE_CHANGED,
	};

	enum C2SOperType
	{
		C2S_PLAYER_BUY = 0,
		C2S_PLAYER_SELL,
		C2S_PRICE_CHANGED_RES,
		C2S_PLAYER_CLOSE,
	};
}

#endif
