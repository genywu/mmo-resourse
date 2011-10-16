
#include "../Lock.h"
#include "ObjectFactory.h"

#ifndef _TOOLS_H_
#define _TOOLS_H_


namespace XM_Tools
{

#ifndef USE_IN_SYNC_SYSTEM
	#define	AsyncLock	LockOfWinWhileSleep
	#define GET_CLASS_NAME(_TClassName)	(""#_TClassName)
#else
	#define	AsyncLock	_InvalidLock
	#define GET_CLASS_NAME(_TClassName)	(""#_TClassName)
#endif

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! 基本内存管理
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LOG_ALLOC_FREE_INFO

	//! 内存分配
	#define						__ALLOC__(_PSize)										GetMemManage<AsyncLock>().AsyncAlloc((_PSize), _FILE_AND_LINE_)
	#define						__FREE__(Param_Pointer, Param_Size)						do{\
																							if(Param_Pointer)\
																							{\
																								GetMemManage<AsyncLock>().AsyncFree(((void*)Param_Pointer), (Param_Size), _FILE_AND_LINE_);\
																								Param_Pointer = NULL;\
																							}\
																						} while(false)

	#define						__SET_OUT_INFO_SETUP__(Param_OutFun, Param_FileName)	{\
																							GetObjInfoManageLock<AsyncLock>().Lock();\
																							GetObjInfoManage().SetOutInfoSetup((Param_OutFun), (Param_FileName));\
																							GetObjInfoManageLock<AsyncLock>().UnLock();\
																							GetMemManage<AsyncLock>().SetOutInfoSetup((Param_OutFun), (Param_FileName));\
																						}

	#define						__OUT_RUNNING_INFO__(Param_Detailed)					{\
																							GetObjInfoManageLock<AsyncLock>().Lock();\
																							GetObjInfoManage().OutRunTimeObjInfo();\
																							GetObjInfoManageLock<AsyncLock>().UnLock();\
																							GetMemManage<AsyncLock>().OutRunTimeObjInfo(Param_Detailed);\
																						}
	
#else

	//! 内存分配
	#define						__ALLOC__(_PSize)										GetMemManage<AsyncLock>().AsyncAlloc((_PSize))
	#define						__FREE__(Param_Pointer, Param_Size)						do{\
																							if(Param_Pointer)\
																							{\
																								GetMemManage<AsyncLock>().AsyncFree(((void*)Param_Pointer), (Param_Size));\
																								Param_Pointer = NULL;\
																							}\
																						} while(false)

	

	#define						__SET_OUT_INFO_SETUP__(Param_OutFun, Param_FileName)

	#define						__OUT_RUNNING_INFO__(Param_Detailed)

#endif


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! 被内存池管理的对象
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//! new/delete
#ifdef LOG_ALLOC_FREE_INFO
	#define						__NEW__													new(_FILE_AND_LINE_)
#else
	#define						__NEW__													new
#endif

	#define						__SAFE_DELETE__(Param_Pointer)							if(NULL != (Param_Pointer)){delete (Param_Pointer);(Param_Pointer) = NULL;}
	#define						__SAFE_DELETE_ARRAY__(Param_Pointer)					if(NULL != (Param_Pointer)){delete[] (Param_Pointer);(Param_Pointer) = NULL;}


	//! 被内存池管理的对象基类
	class CMemObject
	{
	public:
		virtual ~CMemObject(void){}

#ifdef LOG_ALLOC_FREE_INFO
		//!
		static void* operator new(size_t Size, const char *pInfo);
		static void* operator new[](size_t Size, const char *pInfo);

#else
		//!
		static void* operator new(size_t Size);
		static void* operator new[](size_t Size);
		
#endif

		//!
		static void operator delete(void *pMem);
		static void operator delete[](void *pMem);
	};



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! 对象工厂
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LOG_ALLOC_FREE_INFO
	#define						__OBJ_ALLOC__(_TClassName)								GetFactory<_TClassName, AsyncLock>(GET_CLASS_NAME(_TClassName)).CreateObj(GET_CLASS_NAME(_TClassName), _FILE_AND_LINE_)
	#define						__OBJ_ALLOC_PVOID__(_TClassName, Param_ConstructParam)	GetFactory<_TClassName, AsyncLock>(GET_CLASS_NAME(_TClassName)).CreateObj((Param_ConstructParam), GET_CLASS_NAME(_TClassName), _FILE_AND_LINE_)
	#define						__OBJ_FREE__(_TClassName, Param_Pointer)				do{\
																							GetFactory<_TClassName, AsyncLock>(GET_CLASS_NAME(_TClassName)).ReleaseObj(dynamic_cast<_TClassName*>(Param_Pointer), GET_CLASS_NAME(_TClassName), _FILE_AND_LINE_);\
																							Param_Pointer = NULL;\
																						} while(false)
#else
	#define						__OBJ_ALLOC__(_TClassName)								GetFactory<_TClassName, AsyncLock>().CreateObj()
	#define						__OBJ_ALLOC_PVOID__(_TClassName, Param_ConstructParam)	GetFactory<_TClassName, AsyncLock>().CreateObj((Param_ConstructParam))
	#define						__OBJ_FREE__(_TClassName, Param_Pointer)				do{\
																							GetFactory<_TClassName, AsyncLock>().ReleaseObj(dynamic_cast<_TClassName*>(Param_Pointer));\
																							Param_Pointer = NULL;\
																						} while(false)
#endif
};






#endif //! _TOOLS_H_