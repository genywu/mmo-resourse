

#pragma once

//! 网络相关的公共定义
//!------------------------------------------------------------------------------------

//! 网络标记
enum eNetFlag
{
	NF_SCC_Client			= 0x1000,
	NF_SCC_Server			= 0x2000,

	NF_SUC_Client			= 0x4000,
	NF_SUC_Server			= 0x8000,

	NF_SM_Server			= 0x10000,
	NF_SM_Client			= 0x20000,
};