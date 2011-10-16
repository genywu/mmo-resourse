




#pragma once

//! 名称
////////////////////////////////////////////////////////////////////////////////////////////////////
//!							基本名称长度
const		DWORD			BASE_NAME_SIZE				= 32;
//!							一个用户组名称的长度
const		DWORD			MAX_USER_GROUP_NAME_SZIE	= BASE_NAME_SIZE;
//!							一个用户名称的长度
const		DWORD			MAX_USER_NAME_SZIE			= BASE_NAME_SIZE;
//!							一个SM名称的长度
const		DWORD			MAX_SM_NAME_SZIE			= BASE_NAME_SIZE;
//!							一个GAPP名称的长度
const		DWORD			MAX_GAPP_NAME_SZIE			= BASE_NAME_SIZE;



//! 文本
////////////////////////////////////////////////////////////////////////////////////////////////////
//!							基本文本长度
const		DWORD			BASE_TEXT_SIZE				= 1024;
//!							更新报的描述文长度
const		DWORD			PACK_README_TEXT_SIZE		= BASE_TEXT_SIZE * 8;


//! 其他
////////////////////////////////////////////////////////////////////////////////////////////////////
//!							最大网络地址长度
const		DWORD			MAX_HOST_ADDRESS_SZIE		= 256;

//!							基本密码长度
const		DWORD			BASE_PASSWORD_SIZE			= 36;
//!							保存MD5 16密码的长度
const		DWORD			MD5_16_PASSWORD_SIZE		= 16;
//!							保存MD5 32密码的长度
const		DWORD			MD5_32_PASSWORD_SIZE		= 36;

//!							数据库相关字符串配置
const		DWORD			ABOUT_DATABASE_SETUP_SIZE	= 256;

//!							当前版本
const		DWORD			CURR_VERSION				= 1002;

//!							无效的字符串
const		string			INVALID_STRING				("");

//!							一组硬件信息个数
const		DWORD			HARD_INFO_GROUP_NUM			= 512;
//!							缓存硬件信息组数
const		DWORD			HARD_INFO_CACHE_GROUP_NUM	= 8;
//!							服务器最大CPU个数
const		DWORD			MAX_CPU_NUM					=16;
