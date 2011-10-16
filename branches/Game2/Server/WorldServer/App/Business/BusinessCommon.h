///
/// @file BusinessCommon.h
/// @brief 跑商系统公用声明
///
#pragma once

namespace Business
{
	/// 贸易点基本属性
	struct tagTradeSpotProperty
	{
		int areaId;
		int Id;
		int GsId;
		/// 所在场景的资源ID
		int RegionResID;
		/// 影响同类商品库存系数
		float fCategoryVolumeQuo;
		/// 影响所有商品库存系数
		float fAllVolumeQuo;
	};

	/// 声望等级
	enum eCreditLevel
	{
		CL_BLAND = 1,
		CL_NORMAL,
		CL_KINDLY,
		CL_RESPECT,
		CL_HONOR,
	};

	/// 声望描述
	struct tagCreditDesc
	{
		int min;
		int max;
		char desc[256];
		int level;
	};

	/// 贸易区域基本属性
	struct tagTradeAreaProperty
	{
		int Id;
		char name[256]; // even it's hard code
	};

	/// 系统全局配置
	struct tagSystemSetup
	{
		DWORD dwUpdatePriceInterval; ///< 物价波动定时器间隔
		DWORD dwUpdateBuyLimitInterval; ///< 切换场景多长时间后更新限购额
		float fEndureQuo; ///< 耐久计算系数，耐久降低值=装入商品价格*EndureQuo
		BYTE enable; ///< 是否开启商业系统(1/0)
	};

//#define LOG_FILE_NAME "business_log_ws"
//#define BUSINESS_LOG_INFO( LogInfo ) PutoutLog( LOG_FILE_NAME, LT_DEFAULT, LogInfo )
//#define BUSINESS_LOG_WARNING( LogInfo ) PutoutLog( LOG_FILE_NAME, LT_WARNING, LogInfo )
//#define BUSINESS_LOG_ERROR( LogInfo ) PutoutLog( LOG_FILE_NAME, LT_ERROR, LogInfo )
// To enable/disable arg list support
#define SUPPORT_ARG_LIST

#ifdef SUPPORT_ARG_LIST
	inline const char *FMT_STR( const char *fmt, ... )
	{
		static char buf[2056];
		va_list arg_list;
		va_start( arg_list, fmt );
		vsprintf( buf, fmt, arg_list );
		va_end( arg_list );
		return buf;
	}
#else
#define FMT_STR( info ) info
#endif
}