

#include "MemList.h"
#include "HeapPool.h"
#include "BlockPool.h"
#include "TilePool.h"


#ifndef _MEM_MANAGE_H_
#define _MEM_MANAGE_H_


namespace XM_Tools
{

	/*
	============================================================================
	============================================================================
	内存管理类
	============================================================================
	============================================================================
	*/
	template<typename _TLock>
	class CMemManage
	{
	public:
		CMemManage(void);
		~CMemManage(void);

	public:
		//!											初始化
		unsigned long								Init(void);
		//!											释放
		void										Release(void);

		//!											设置信息输出函数和输出路径
		void										SetOutInfoSetup(OutInfoFun pOutInfoFun, const char *pFileName);
		//!											运行时信息输出
		void										OutRunTimeObjInfo(void);

	public:
#ifdef _LOG_ALLOC_FILE_INFO_
		//!											得到一块内存
		void*										AsyncAlloc(unsigned long lSize, const char *pInfo);
#else
		//!											得到一块内存
		void*										AsyncAlloc(unsigned long lSize);
#endif
		//!											释放一块内存
		void										AsyncFree(void* pBuf, unsigned long lSize);


	private:
		//! 内存来源类型
		enum eMemSourceType
		{
			//! 系统C函数malloc分配
			eMST_System_malloc,
			//! 内存管理器的堆池分配
			eMST_MemManage_Heap,
			//! 内存管理器的块池分配
			eMST_MemManage_Block,
			//! 内存管理器的片池分配
			eMST_MemManage_Tile,
		};

	private:
		typedef CHeapPool<_TLock>					THeapPool;
		typedef CBlockPool<_TLock, THeapPool>		TBlockPool;
		typedef CTilePool<_TLock, THeapPool>		TTilePool;

		//!											堆池
		THeapPool									m_HeapPool;
		//!											块池
		TBlockPool									m_BlockPool;
		//!											片池
		TTilePool									m_TilePool;

#ifdef _LOG_ALLOC_FILE_INFO_
		//!											已分配内存所在信息保存
		std::map<void*, std::string>				m_mapAllocInfo;
		//!											用于m_mapAllocInfo对象
		_TLock										m_InfoLock;

#endif

		//!											输出函数
		OutInfoFun									m_OutInfoFun;
		//!											输出文件
		const char									*m_pOutFile;

	};


	/*
	============================================================================
	============================================================================
	内存管理类
	============================================================================
	============================================================================
	*/
	//!							获取内存管理对象
	template<typename _TLock>
	CMemManage<_TLock>&			GetMemManage(void);
};

#include "../cpp/MemManage.cpp"

#endif //! _MEM_MANAGE_H_