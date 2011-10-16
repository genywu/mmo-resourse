


/*
在此定义MemManage用到的：
	基本常量
	数据类型
	宏
	枚举

*/

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
	#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#pragma warning(disable:4996; disable:4311; disable:4312; disable:4345)

#include <map>
#include <string>
#include <Windows.h>


#ifndef _MEM_MANAGE_DEF_
#define _MEM_MANAGE_DEF_



namespace XM_Tools
{
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 宏
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef NULL
	#define						NULL							0
#endif

	//!							析构时释放系统内存
//#define							DESTRUTOR_FREE_SYSMEM

//!								分配时填充内存
//#define							FILL_MEM
//!								断言控制
//#define							ASSERT_ENABLE


#ifndef MEM_LOG_FILE
	#define MEM_LOG_FILE			"log\mem_info.log"
#endif //! MEM_LOG_FILE
	
//!								记录分配、释放信息
//#define							LOG_ALLOC_FREE_INFO

//!								使用片管理
#define							USE_TILE_MANAGE
//!								使用块管理
//#define							USE_BLOCK_MANAGE
//!								使用堆管理
//#define							USE_HEAP_MANAGE



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ASSERT_ENABLE
	#define						MM_ASSERY(bValue)				if(!(bValue))throw
	#define						FILL_VALUE						(0xbb)
#else
	#define						MM_ASSERY(bValue)
	#define						FILL_VALUE						(0)
#endif

	//!								在返回一个值前抛出异常（受控于ASSERT_ENABLE）
#define							RETURN_THROW(ReturnValue)		{MM_ASSERY(false);return (ReturnValue);}
#define							RETURN_THROW_VOID				{MM_ASSERY(false);}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LOG_ALLOC_FREE_INFO

	#define						FILE_LINE_TEMP2(x, y)			"file["x"]，line["#y"]"
	#define						FILE_LINE_TEMP(x, y)			FILE_LINE_TEMP2(x, y)
	#define						_FILE_AND_LINE_					FILE_LINE_TEMP(__FILE__, __LINE__)

	//!							用于日志记录的一个宏（将传入的类名与代码所在的文件名、行号连接成一个字符串）
	#define						LINK_OBJ_INFO(ClassName)		(#ClassName ## _FILE_AND_LINE_ )

#else

	#define						_FILE_AND_LINE_					NULL

	//!							用于日志记录的一个宏（将传入的类名与代码所在的文件名、行号连接成一个字符串）
	#define						LINK_OBJ_INFO(ClassName)		NULL

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! 数字对齐
	inline unsigned long		SNAP_TO_GRID(unsigned long uSrcNum, unsigned long uGrid)		
	{
		return (uSrcNum / uGrid + ((uSrcNum % uGrid) ? 1 : 0)) * uGrid;
	}

	//! 信息输出函数类型
	typedef	void				(*OutInfoFun)(const char *pFileName, const char *pText)	;

	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 基本定义
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	#ifndef SHRINK_SCALE
		#define					SHRINK_SCALE					0
	#endif
	//! 缓存行
	#ifndef CACHE_LINES
		#define					CACHE_LINES						64
	#endif

	//!							信息缓存大小
	const unsigned long			INFO_BUF_SIZE					= 1024 * 8;

	//!							CPU宽（byte）
	const unsigned long 		CPU_SIZE						= sizeof(long);
	//!							最小粒度（byte）（这个值最好设定为CPU的位宽，必须等于2的N次幂）
	const unsigned long			MIN_GRANULARITY					= CPU_SIZE * 2;
	//!							内存分配起始值（byte）（这个值最好设定为CPU位宽的2倍，必须等于2的N次幂）
	const unsigned long 		MIN_ALLOC_SIZE					= CPU_SIZE * 2;
	//!							最大粒度（最大内存申请数）
	const unsigned long			MAX_GRANULARITY					= 1024 * 1024 * 1024;



	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 小块内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//!							尺寸基数
	const unsigned char 		TILE_POOL_NUM					= 32;
	//!							最大块尺寸
	const unsigned long 		TILE_MAX_SIZE					= TILE_POOL_NUM * MIN_GRANULARITY;
	//!							默认进行一次空闲块添加时，增加的可使用内存
	const unsigned long 		TILE_DEFAULT_ADD_SIZE			= (1024 * 1024 * 4) >> SHRINK_SCALE;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 大块内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							块基本大小
	const unsigned long 		BLOCK_BSAE_SIZE					= TILE_MAX_SIZE * CPU_SIZE;
	//!							默认情况下准备块数，必须是CPU宽的整数倍
	const unsigned long 		BLOCK_DEFAULT_PROVIDE_COUNT		= ( MAX_GRANULARITY / BLOCK_BSAE_SIZE ) >> SHRINK_SCALE;
	//!							允许申请的最大内存
	const unsigned long			BLOCK_MAX_ALLOC_SIZE			= 1024 * 512;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 堆内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							堆的最小管理粒度（byte）
	const unsigned long			HEAP_MIN_GRANULARITY			= BLOCK_BSAE_SIZE * CPU_SIZE;
	//!							堆最大管理的内存块数
	const unsigned long			MAX_MANAGE_COUNT				= 128;
	//!							每次释放8个块
	const unsigned long			FREE_COUNT						= 8;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 对象管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							
	const unsigned long			CLASS_NAME_SIZE					= 512;


};



#endif // _MEM_MANAGE_DEF_