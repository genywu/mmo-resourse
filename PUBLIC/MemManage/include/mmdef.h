


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

#include <map>
#include <string>


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
#define							DESTRUTOR_FREE_SYSMEM

	//!							分配时填充内存
#define							FILL_MEM
	//!							断言控制
#define							ASSERT_ENABLE


#ifndef LOG_FILE
	#define LOG_FILE			"log\mem_info.log"
#endif //! LOG_FILE
	
//!								记录分配、释放信息
#define							LOG_ALLOC_FREE_INFO
//!								是否记录分配时所在的文件
#define							_LOG_ALLOC_FILE_INFO_
	


	
	
	


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
#ifdef _LOG_ALLOC_FILE_INFO_

	#define						FILE_LINE_TEMP2(x, y)			"所在文件["x"]，所在行数["#y"]"
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
		#define					SHRINK_SCALE					1
	#endif


	//!							CPU宽（byte）
	const unsigned long 		CPU_SIZE						= sizeof(long);
	//!							最小粒度（byte）（这个值最好设定为CPU的位宽，必须等于2的N次幂）
	const unsigned long			MIN_GRANULARITY					= CPU_SIZE;
	//!							内存分配起始值（byte）（这个值最好设定为CPU位宽的2倍，必须等于2的N次幂）
	const unsigned long 		MIN_ALLOC_SIZE					= CPU_SIZE * 2;
	//!							最大粒度（最大内存申请数）
	const unsigned long			MAX_GRANULARITY					= 1024 * 1024 * 512;



	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 小块内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//!							尺寸基数
	const unsigned char 		TILE_POOL_NUM					= 32;
	//!							最大块尺寸
	const unsigned long 		TILE_MAX_SIZE					= TILE_POOL_NUM * MIN_GRANULARITY;
	//!							默认进行一次空闲块添加时，增加的可使用内存
	const unsigned long 		TILE_DEFAULT_ADD_SIZE			= 1024 * 1024 * 16;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 大块内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							块基本大小
	const unsigned long 		BLOCK_BSAE_SIZE					= TILE_MAX_SIZE * CPU_SIZE;
	//!							默认情况下准备块数，必须是CPU宽的整数倍
	const unsigned long 		BLOCK_DEFAULT_PROVIDE_COUNT		= MAX_GRANULARITY / BLOCK_BSAE_SIZE / SHRINK_SCALE;
	//!							允许申请的最大内存
	const unsigned long			BLOCK_MAX_ALLOC_SIZE			= 1024 * 1024 * 2;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 堆内存管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							堆的最小管理粒度（byte）
	const unsigned long			HEAP_MIN_GRANULARITY			= BLOCK_BSAE_SIZE * CPU_SIZE;


	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//! 对象管理
	//!------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//!							
	const unsigned long			CLASS_NAME_SIZE					= 512;


};



#endif // _MEM_MANAGE_DEF_